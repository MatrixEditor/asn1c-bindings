/*
 * Regression test for anonymous SEQUENCE OF unsigned INTEGER member
 * descriptor specifics and BER/DER behavior.
 */
#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SeqOfUInt32.h>

static void
append_value(SeqOfUInt32_t *seq, unsigned long value) {
    unsigned long *member = calloc(1, sizeof(*member));
    assert(member);
    *member = value;
    assert(ASN_SEQUENCE_ADD(&seq->list, member) == 0);
}

static void
check_success_roundtrip(void) {
    SeqOfUInt32_t in;
    SeqOfUInt32_t *out = NULL;
    asn_enc_rval_t erv;
    asn_dec_rval_t drv;
    uint8_t encoded[32];
    static const uint8_t expected[] = {
        0x30, 0x0a, 0x02, 0x01, 0x00, 0x02, 0x05, 0x00, 0xff, 0xff, 0xff, 0xff
    };

    memset(&in, 0, sizeof(in));
    append_value(&in, 0UL);
    append_value(&in, 4294967295UL);

    erv = der_encode_to_buffer(&asn_DEF_SeqOfUInt32, &in, encoded, sizeof(encoded));
    assert(erv.encoded == (ssize_t)sizeof(expected));
    assert(memcmp(encoded, expected, sizeof(expected)) == 0);

    drv = ber_decode(0, &asn_DEF_SeqOfUInt32, (void **)&out, expected, sizeof(expected));
    assert(drv.code == RC_OK);
    assert(drv.consumed == sizeof(expected));
    assert(out);
    assert(out->list.count == 2);
    assert(*out->list.array[0] == 0UL);
    assert(*out->list.array[1] == 4294967295UL);

    ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, out);
    ASN_STRUCT_RESET(asn_DEF_SeqOfUInt32, &in);
}

static void
check_encode_failure_small_buffer(void) {
    SeqOfUInt32_t in;
    asn_enc_rval_t erv;
    uint8_t encoded[4];

    memset(&in, 0, sizeof(in));
    append_value(&in, 0UL);
    append_value(&in, 4294967295UL);

    erv = der_encode_to_buffer(&asn_DEF_SeqOfUInt32, &in, encoded, sizeof(encoded));
    assert(erv.encoded == -1);

    ASN_STRUCT_RESET(asn_DEF_SeqOfUInt32, &in);
}

static void
check_decode_failure_bad_element_tag(void) {
    SeqOfUInt32_t *out = NULL;
    asn_dec_rval_t drv;
    static const uint8_t malformed[] = {
        0x30, 0x03, 0x05, 0x01, 0x00
    };

    drv = ber_decode(0, &asn_DEF_SeqOfUInt32, (void **)&out, malformed, sizeof(malformed));
    assert(drv.code == RC_FAIL);
    ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, out);
}

int
main(void) {
    check_success_roundtrip();
    check_encode_failure_small_buffer();
    check_decode_failure_bad_element_tag();
    return 0;
}
