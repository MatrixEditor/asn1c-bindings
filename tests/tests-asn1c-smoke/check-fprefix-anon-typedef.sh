#!/usr/bin/env bash

set -ex
set -o pipefail

top_builddir=${top_builddir:-../..}
top_srcdir=${top_srcdir:-../..}
srcdir=$(cd "$(dirname "$0")" && pwd)

testdir=test-fprefix-anon-typedef
cleanup() {
    rm -rf "$testdir"
}
trap cleanup EXIT

rm -rf "$testdir"
mkdir "$testdir"
cd "$testdir"

cp "${srcdir}/data/fprefix-anon-typedef.asn" test.asn

prefix=S1AP_
mkdir "$prefix"

../"${top_builddir}"/asn1c/asn1c -S ../"${top_srcdir}"/skeletons -flink-skeletons \
    -fprefix="$prefix" -D "$prefix" test.asn

# The anonymous typedef alias in FWD-DEFS must carry the -fprefix.
# With -fprefix=S1AP_ the name must be "S1AP_InitiatingMessage__value",
# not the unprefixed "InitiatingMessage__value" or the bare field name "value".
grep -F "${prefix}InitiatingMessage__value" "${prefix}/${prefix}InitiatingMessage.h"
grep -F "} value;" "${prefix}/${prefix}InitiatingMessage.h" && exit 1

"${CC:-cc}" -c -I"${prefix}" -I../"${top_srcdir}"/skeletons \
    "${prefix}/${prefix}InitiatingMessage.c" -o "${prefix}.o"

test -f "${prefix}.o"
