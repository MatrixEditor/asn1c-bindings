#!/usr/bin/env bash
set -Eeuo pipefail

# Local-only fuzzer runner for occasional deep checks.
#
# Examples:
#   scripts/run-local-fuzzer-check.sh
#   CPUS=12 FUZZ_TIME=60 scripts/run-local-fuzzer-check.sh
#   BUILD=/tmp/asn1c-fuzz-build RUN_ROOT=/tmp/asn1c-fuzz-runs scripts/run-local-fuzzer-check.sh
#
# Results are written under /tmp/asn1c-fuzzer-local/runs/<timestamp>/ by default.
# A clean source snapshot and build tree are kept under /tmp/asn1c-fuzzer-local/
# by default, so the active checkout can remain configured in-tree.

CPUS="${CPUS:-12}"
FUZZ_TIME="${FUZZ_TIME:-60}"
CC_WAS_SET=0
CXX_WAS_SET=0
if [[ -n "${CC+x}" ]]; then
    CC_WAS_SET=1
fi
if [[ -n "${CXX+x}" ]]; then
    CXX_WAS_SET=1
fi
CC="${CC:-}"
CXX="${CXX:-}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="${ROOT:-$(git -C "${SCRIPT_DIR}" rev-parse --show-toplevel)}"
WORK_ROOT="${WORK_ROOT:-/tmp/asn1c-fuzzer-local}"
SOURCE="${SOURCE:-${WORK_ROOT}/source}"
BUILD="${BUILD:-${WORK_ROOT}/build}"
RUN_ROOT="${RUN_ROOT:-${WORK_ROOT}/runs}"
RUN_ID="${RUN_ID:-$(date +%Y%m%d-%H%M%S)}"
LOGDIR="${RUN_ROOT}/${RUN_ID}"
CLEAN_BUILD="${CLEAN_BUILD:-1}"

can_link_libfuzzer() {
    local cc="$1"
    local probe_dir probe_src probe_bin probe_log

    if ! command -v "${cc}" >/dev/null 2>&1; then
        return 1
    fi

    probe_dir="$(mktemp -d "${TMPDIR:-/tmp}/asn1c-fuzzer-probe.XXXXXX")"
    probe_src="${probe_dir}/probe.c"
    probe_bin="${probe_dir}/probe"
    probe_log="${probe_dir}/probe.log"

    printf '%s\n' \
        '#include <stddef.h>' \
        'int LLVMFuzzerTestOneInput(const unsigned char *Data, size_t Size) {' \
        '    (void)Data;' \
        '    (void)Size;' \
        '    return 0;' \
        '}' >"${probe_src}"

    if "${cc}" -fsanitize=fuzzer "${probe_src}" -o "${probe_bin}" >"${probe_log}" 2>&1; then
        rm -rf "${probe_dir}"
        return 0
    fi

    rm -rf "${probe_dir}"
    return 1
}

matching_clangxx_for() {
    local cc="$1"
    local cc_dir cc_base cxx_base cxx_path

    cc_dir="$(dirname "${cc}")"
    cc_base="$(basename "${cc}")"
    cxx_base="${cc_base/clang/clang++}"
    cxx_path="${cc_dir}/${cxx_base}"

    if [[ "${cc_base}" != "${cxx_base}" && -x "${cxx_path}" ]]; then
        printf '%s\n' "${cxx_path}"
    else
        printf '%s\n' "clang++"
    fi
}

select_fuzzer_compiler() {
    local candidates=()
    local pair candidate_cc candidate_cxx requested_cxx

    if [[ "${CC_WAS_SET}" -eq 1 ]]; then
        if [[ "${CXX_WAS_SET}" -eq 0 ]]; then
            requested_cxx="$(matching_clangxx_for "${CC}")"
        else
            requested_cxx="${CXX}"
        fi
        candidates+=("${CC}:${requested_cxx}")
    elif [[ "${CXX_WAS_SET}" -eq 1 ]]; then
        candidates+=("clang:${CXX}")
    else
        candidates+=("clang:clang++")
    fi

    candidates+=("/opt/local/bin/clang-mp-22:/opt/local/bin/clang++-mp-22")

    for pair in "${candidates[@]}"; do
        candidate_cc="${pair%%:*}"
        candidate_cxx="${pair#*:}"
        if can_link_libfuzzer "${candidate_cc}" && command -v "${candidate_cxx}" >/dev/null 2>&1; then
            CC="${candidate_cc}"
            CXX="${candidate_cxx}"
            return
        fi
    done

    echo "Could not find a Clang that can link a -fsanitize=fuzzer test program."
    echo "On this host Apple clang may not ship libclang_rt.fuzzer_osx.a."
    echo "Try installing LLVM and rerun with CC=/path/to/clang CXX=/path/to/clang++."
    exit 2
}

select_fuzzer_compiler

mkdir -p "${WORK_ROOT}" "${BUILD}" "${LOGDIR}"
exec > >(tee "${LOGDIR}/driver.log") 2>&1

echo "root:      ${ROOT}"
echo "work root: ${WORK_ROOT}"
echo "source:    ${SOURCE}"
echo "build:     ${BUILD}"
echo "logs:      ${LOGDIR}"
echo "cpus:      ${CPUS}"
echo "fuzz time: ${FUZZ_TIME}s per libFuzzer target"
echo "compiler:  ${CC} / ${CXX}"

if [[ "${SOURCE}" == "${ROOT}" && -e "${ROOT}/config.status" ]]; then
    echo "The active checkout is already configured in-tree."
    echo "Use the default SOURCE=${WORK_ROOT}/source, or run make distclean first."
    exit 2
fi

echo
echo "== prepare source snapshot =="
rm -rf "${SOURCE}"
mkdir -p "${SOURCE}"
(
    cd "${ROOT}"
    git ls-files -z | while IFS= read -r -d '' path; do
        mkdir -p "${SOURCE}/$(dirname "${path}")"
        if [[ -L "${ROOT}/${path}" ]]; then
            cp -Pp "${ROOT}/${path}" "${SOURCE}/${path}"
        else
            cp -p "${ROOT}/${path}" "${SOURCE}/${path}"
        fi
    done
) >"${LOGDIR}/source-copy.log" 2>&1 || {
    echo "source snapshot failed; tail of ${LOGDIR}/source-copy.log:"
    tail -80 "${LOGDIR}/source-copy.log"
    exit 2
}

(
    cd "${SOURCE}"
    autoreconf -ivf
) >"${LOGDIR}/autoreconf.log" 2>&1 || {
    echo "autoreconf failed; tail of ${LOGDIR}/autoreconf.log:"
    tail -80 "${LOGDIR}/autoreconf.log"
    exit 2
}

if [[ "${CLEAN_BUILD}" == "1" ]]; then
    rm -rf "${BUILD}"
fi
mkdir -p "${BUILD}"

echo
echo "== configure =="
(
    cd "${BUILD}"
    "${SOURCE}/configure" CC="${CC}" CXX="${CXX}" --enable-test-fuzzer
) >"${LOGDIR}/configure.log" 2>&1 || {
    echo "configure failed; tail of ${LOGDIR}/configure.log:"
    tail -80 "${LOGDIR}/configure.log"
    exit 2
}

if ! grep -q "LIBFUZZER_CFLAGS='.*-fsanitize=fuzzer" "${BUILD}/config.log"; then
    echo "configure did not enable libFuzzer; see ${LOGDIR}/configure.log"
    exit 2
fi

echo "== build =="
make -C "${BUILD}" -j "${CPUS}" >"${LOGDIR}/build.log" 2>&1 || {
    echo "build failed; tail of ${LOGDIR}/build.log:"
    tail -80 "${LOGDIR}/build.log"
    exit 2
}

failures=0

run_check() {
    local name="$1"
    shift
    local logfile="${LOGDIR}/${name}.log"

    echo
    echo "== ${name} =="
    if "$@" >"${logfile}" 2>&1; then
        echo "PASS ${name}"
    else
        echo "FAIL ${name}; see ${logfile}"
        failures=$((failures + 1))
    fi
}

# Run everything except tests-randomized through its ordinary Automake entry
# points. tests-randomized is split below so its large bundles can use all CPUs.
for dir in \
    libasn1common \
    libasn1parser \
    libasn1fix \
    libasn1print \
    libasn1compiler \
    asn1-tools \
    asn1c \
    skeletons \
    examples \
    doc
do
    run_check "${dir//\//-}" \
        make -C "${BUILD}/${dir}" -j "${CPUS}" check FUZZ_TIME="${FUZZ_TIME}"
done

for dir in \
    tests-asn1c-compiler \
    tests-skeletons \
    tests-asn1c-smoke \
    tests-c-compiler \
    ioc-v2x \
    ieee1609-ioc \
    f1ap-regression
do
    run_check "tests-${dir}" \
        make -C "${BUILD}/tests/${dir}" -j "${CPUS}" check FUZZ_TIME="${FUZZ_TIME}"
done

echo
echo "== prepare split randomized cases =="
RND_BUILD="${BUILD}/tests/tests-randomized"
CASE_DIR="${RND_BUILD}/.tmp.fuzzcases"
rm -rf "${CASE_DIR}"
find "${RND_BUILD}" -maxdepth 1 -type d -name '.tmp.fuzzcase-*' -exec rm -rf {} +
mkdir -p "${CASE_DIR}"

case_tests=()
case_no=0
for bundle in "${SOURCE}"/tests/tests-randomized/bundles/*bundle.txt; do
    base="$(basename "${bundle}" .txt)"
    line_no=0
    while IFS= read -r line || [[ -n "${line}" ]]; do
        line_no=$((line_no + 1))
        line_without_comment="${line%%--*}"
        if [[ ! "${line_without_comment}" =~ [A-Za-z] ]]; then
            continue
        fi

        printf -v case_name 'fuzzcase-%04d-%s-L%d.test' \
            "${case_no}" "${base}" "${line_no}"
        printf '%s\n' "${line}" >"${CASE_DIR}/${case_name}"
        case_tests+=(".tmp.fuzzcases/${case_name}")
        case_no=$((case_no + 1))
    done < "${bundle}"
done

printf '%s\n' "${case_tests[@]}" >"${LOGDIR}/randomized-tests.list"
echo "created ${#case_tests[@]} one-line randomized test cases"

run_check "tests-randomized-split" \
    make -C "${RND_BUILD}" -j "${CPUS}" check \
        FUZZ_TIME="${FUZZ_TIME}" \
        TESTS="${case_tests[*]}"

summary="${LOGDIR}/problem-summary.txt"
{
    echo "Run ID: ${RUN_ID}"
    echo "Root: ${ROOT}"
    echo "Build: ${BUILD}"
    echo "FUZZ_TIME: ${FUZZ_TIME}"
    echo "CPUS: ${CPUS}"
    echo
    echo "Problem-pattern matches:"
    grep -R -n -E \
        'ERROR: AddressSanitizer|ERROR: UndefinedBehaviorSanitizer|AddressSanitizer:|UndefinedBehaviorSanitizer:|runtime error:|SUMMARY:|ERROR: libFuzzer|Test unit written|LeakSanitizer|FAIL:|ERROR:' \
        "${LOGDIR}" || true
    echo
    echo "Fuzzer artifacts:"
    find "${BUILD}" \( \
        -name 'crash-*' -o \
        -name 'leak-*' -o \
        -name 'oom-*' -o \
        -name 'timeout-*' \
    \) -print || true
} >"${summary}"

tarball="${RUN_ROOT}/${RUN_ID}.tar.gz"
tar -C "${RUN_ROOT}" -czf "${tarball}" "${RUN_ID}"

echo
echo "summary: ${summary}"
echo "archive: ${tarball}"

if [[ "${failures}" -ne 0 ]]; then
    echo "completed with ${failures} failing check group(s)"
    exit 1
fi

echo "completed successfully"
