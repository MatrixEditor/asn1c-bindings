#!/usr/bin/env bash
set -Eeuo pipefail

# Local-only fuzzer runner for occasional deep checks.
#
# Examples:
#   scripts/run-local-fuzzer-check.sh
#   CPUS=12 FUZZ_TIME=60 scripts/run-local-fuzzer-check.sh
#   BUILD=/tmp/asn1c-fuzz-build RUN_ROOT=/tmp/asn1c-fuzz-runs scripts/run-local-fuzzer-check.sh
#
# Results are written under .tmp.fuzzer-runs/<timestamp>/ by default.

CPUS="${CPUS:-12}"
FUZZ_TIME="${FUZZ_TIME:-60}"
CC="${CC:-clang}"
CXX="${CXX:-clang++}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="${ROOT:-$(git -C "${SCRIPT_DIR}" rev-parse --show-toplevel)}"
BUILD="${BUILD:-${ROOT}/.tmp.fuzzer-build}"
RUN_ROOT="${RUN_ROOT:-${ROOT}/.tmp.fuzzer-runs}"
RUN_ID="${RUN_ID:-$(date +%Y%m%d-%H%M%S)}"
LOGDIR="${RUN_ROOT}/${RUN_ID}"

mkdir -p "${BUILD}" "${LOGDIR}"
exec > >(tee "${LOGDIR}/driver.log") 2>&1

echo "root:      ${ROOT}"
echo "build:     ${BUILD}"
echo "logs:      ${LOGDIR}"
echo "cpus:      ${CPUS}"
echo "fuzz time: ${FUZZ_TIME}s per libFuzzer target"
echo "compiler:  ${CC} / ${CXX}"

if [[ ! -x "${ROOT}/configure" ]]; then
    echo "Missing ${ROOT}/configure. Run autoreconf -ivf in the source tree first."
    exit 2
fi

echo
echo "== configure =="
(
    cd "${BUILD}"
    "${ROOT}/configure" CC="${CC}" CXX="${CXX}" --enable-test-fuzzer
) >"${LOGDIR}/configure.log" 2>&1

if ! grep -q "LIBFUZZER_CFLAGS='.*-fsanitize=fuzzer" "${BUILD}/config.log"; then
    echo "configure did not enable libFuzzer; see ${LOGDIR}/configure.log"
    exit 2
fi

echo "== build =="
make -C "${BUILD}" -j "${CPUS}" >"${LOGDIR}/build.log" 2>&1

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
for bundle in "${ROOT}"/tests/tests-randomized/bundles/*bundle.txt; do
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
