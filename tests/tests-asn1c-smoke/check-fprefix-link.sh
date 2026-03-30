#!/usr/bin/env bash

set -ex
set -o pipefail

top_builddir=${top_builddir:-../..}
top_srcdir=${top_srcdir:-../..}
srcdir=$(cd "$(dirname "$0")" && pwd)

testdir=test-fprefix-link
cleanup() {
    rm -rf "$testdir"
}
trap cleanup EXIT

rm -rf "$testdir"
mkdir "$testdir"
cd "$testdir"

cp "${srcdir}/data/fprefix-link.asn" test.asn

for prefix in A_ B_; do
    mkdir "$prefix"
    ../"${top_builddir}"/asn1c/asn1c -S ../"${top_srcdir}"/skeletons -flink-skeletons \
        -fprefix="$prefix" -D "$prefix" test.asn

    grep -F "asn_MBR_${prefix}Cause_" "${prefix}/${prefix}Cause.c"
    grep -F "asn_MBR_Cause_" "${prefix}/${prefix}Cause.c" && exit 1

    "${CC:-cc}" -c -I"${prefix}" -I../"${top_srcdir}"/skeletons \
        "${prefix}/${prefix}Cause.c" -o "${prefix}.o"
done

"${CC:-cc}" -r A_.o B_.o -o combined.o

test -f combined.o
