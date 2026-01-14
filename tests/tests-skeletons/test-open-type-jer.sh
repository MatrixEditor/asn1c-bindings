#!/bin/bash
#
# Test script for JER Open Type encoding
# Validates that Open Type values are encoded without type name wrapper
# per ITU-T X.697 Clause 41
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WORKDIR="${SCRIPT_DIR}/test-open-type-jer-workdir"
ASN1C="${SCRIPT_DIR}/../../asn1c/asn1c"

# Clean up from previous runs
rm -rf "${WORKDIR}"
mkdir -p "${WORKDIR}"
cd "${WORKDIR}"

# Generate C code from ASN.1 schema
echo "Generating C code from ASN.1 schema..."
"${ASN1C}" -fcompound-names -findirect-choice -gen-JER \
    "${SCRIPT_DIR}/test-open-type-jer.asn1" > /dev/null

# Create test program
cat > test_program.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "TestFrame.h"
#include "TestMessage.h"
#include "jer_encoder.h"

static int
write_to_string(const void *buffer, size_t size, void *app_key) {
    char **str = (char **)app_key;
    size_t current_len = *str ? strlen(*str) : 0;
    char *new_str = realloc(*str, current_len + size + 1);
    if(!new_str) return -1;
    memcpy(new_str + current_len, buffer, size);
    new_str[current_len + size] = '\0';
    *str = new_str;
    return 0;
}

int main() {
    TestFrame_t *frame = NULL;
    TestMessage_t *msg = NULL;
    asn_enc_rval_t er;
    char *output = NULL;
    int result = 0;
    
    /* Allocate TestFrame */
    frame = calloc(1, sizeof(TestFrame_t));
    assert(frame != NULL);
    frame->msgId = 42;
    
    /* Allocate TestMessage */
    msg = calloc(1, sizeof(TestMessage_t));
    assert(msg != NULL);
    msg->msgCount = 100;
    OCTET_STRING_fromBuf(&msg->msgData, "TestData", 8);
    
    /* Set Open Type value using CHOICE structure */
    frame->value.present = TestFrame__value_PR_TestMessage;
    frame->value.choice.TestMessage = msg;
    
    /* Encode to JER */
    er = jer_encode(&asn_DEF_TestFrame, frame, 0, write_to_string, &output);
    
    if(er.encoded == -1) {
        fprintf(stderr, "ERROR: Failed to encode\n");
        result = 1;
        goto cleanup;
    }
    
    printf("%s\n", output);
    
    /* Verify that output does NOT contain "TestMessage" wrapper */
    if(strstr(output, "\"TestMessage\"")) {
        fprintf(stderr, "FAIL: Output incorrectly contains 'TestMessage' wrapper\n");
        fprintf(stderr, "This violates ITU-T X.697 Clause 41\n");
        result = 1;
        goto cleanup;
    }
    
    /* Verify that output contains the expected fields */
    if(!strstr(output, "\"msgCount\"") || !strstr(output, "\"msgData\"")) {
        fprintf(stderr, "FAIL: Output missing expected fields\n");
        result = 1;
        goto cleanup;
    }
    
    fprintf(stderr, "PASS: Open Type encoded without type wrapper\n");
    
cleanup:
    if(output) free(output);
    if(frame) ASN_STRUCT_FREE(asn_DEF_TestFrame, frame);
    
    return result;
}
EOF

# Build the test program
echo "Building test program..."
make -f converter-example.mk > /dev/null || {
    echo "ERROR: Failed to build library"
    exit 1
}

cc -DASN_PDU_COLLECTION -I. -o test_program test_program.c libasncodec.a -lm > /dev/null || {
    echo "ERROR: Failed to compile test program"
    exit 1
}

# Run the test
echo "Running test..."
./test_program || {
    echo "ERROR: Test failed"
    exit 1
}

echo "Test passed successfully!"
cd ..
rm -rf "${WORKDIR}"
exit 0
