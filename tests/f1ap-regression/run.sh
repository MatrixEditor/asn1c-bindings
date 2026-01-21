#!/usr/bin/env bash
set -euo pipefail

# Test for F1AP compilation regression (issue #410)
# This test ensures that F1AP code generated with -findirect-choice and 
# -fcompound-names compiles successfully without circular include errors

# For distcheck: srcdir points to source directory, current dir is build directory
# For normal check: srcdir=. and we're in the source directory
srcdir="${srcdir:-.}"
abs_top_builddir="${abs_top_builddir:-$(cd ../.. && pwd)}"
abs_top_srcdir="${abs_top_srcdir:-$(cd ../.. && pwd)}"

# Copy source files to current directory if not already present
if [ ! -f F1AP-16.7.0.asn ]; then
  cp -p "${srcdir}/F1AP-16.7.0.asn" .
fi

ASN1C_EXE="${abs_top_builddir}/asn1c/asn1c"
SKELETONS_DIR="${abs_top_srcdir}/skeletons"

echo "Testing F1AP code generation and compilation..."
echo "srcdir=${srcdir} abs_top_builddir=${abs_top_builddir} abs_top_srcdir=${abs_top_srcdir} pwd=${PWD}"

# Generate code with the same flags that exposed the issue
${ASN1C_EXE} -S "${SKELETONS_DIR}" \
  -fcompound-names \
  -findirect-choice \
  F1AP-16.7.0.asn

# Test compilation of everything
echo "Attempt to build converter-example"
make -f converter-example.mk

echo "F1AP test PASSED: Code generated and compiled successfully"
