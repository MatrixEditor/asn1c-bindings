#!/usr/bin/env bash

set -ex
set -o pipefail

top_builddir=${top_builddir:-../..}
top_srcdir=${top_srcdir:-../..}

testdir=test-Time
rm -rf "$testdir"
mkdir "$testdir"
cd "$testdir"

cat > test.asn <<'EOF'
Module DEFINITIONS ::= BEGIN
Validity ::= SEQUENCE {
  notBefore Time,
  notAfter Time
}
Time ::= CHOICE {
  utcTime UTCTime,
  generalTime GeneralizedTime
}
END
EOF

../"${top_builddir}"/asn1c/asn1c -flink-skeletons \
    -S ../"${top_srcdir}"/skeletons test.asn

test -f asn1c_time.h
test -f asn1c_time.c
test ! -f Time.h
test ! -f Time.c
grep -R "\"asn1c_time.h\"" ./*.h

cd ..
rm -rf "$testdir"
