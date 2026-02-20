/*-
 * Copyright (c) 2025 Contributors. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Test suite for CBOR (Concise Binary Object Representation) codec.
 * Tests round-trip encoding/decoding for key ASN.1 types.
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include <asn_application.h>
#include <INTEGER.h>
#include <NativeInteger.h>
#include <OCTET_STRING.h>
#include <BIT_STRING.h>
#include <cbor_encoder.h>
#include <cbor_decoder.h>
#include <cbor_support.h>

/* ------------------------------------------------------------------ */
/* Buffer accumulator for encoding output                               */
/* ------------------------------------------------------------------ */
struct buffer_acc {
    uint8_t *data;
    size_t   len;
    size_t   cap;
};

static int
buf_append(const void *bytes, size_t sz, void *key) {
    struct buffer_acc *acc = (struct buffer_acc *)key;
    if(acc->len + sz > acc->cap) {
        size_t new_cap = (acc->cap == 0) ? 64 : acc->cap * 2;
        while(new_cap < acc->len + sz) new_cap *= 2;
        uint8_t *p = (uint8_t *)realloc(acc->data, new_cap);
        if(!p) return -1;
        acc->data = p;
        acc->cap  = new_cap;
    }
    memcpy(acc->data + acc->len, bytes, sz);
    acc->len += sz;
    return 0;
}

static void
buf_free(struct buffer_acc *acc) {
    free(acc->data);
    acc->data = NULL;
    acc->len = acc->cap = 0;
}

/* ------------------------------------------------------------------ */
/* INTEGER round-trip helper                                            */
/* ------------------------------------------------------------------ */
static void
test_integer_roundtrip(intmax_t val, const char *label) {
    INTEGER_t orig, *decoded = NULL;
    struct buffer_acc enc;
    asn_enc_rval_t er;
    asn_dec_rval_t dr;
    intmax_t result;

    memset(&orig, 0, sizeof(orig));
    memset(&enc, 0, sizeof(enc));

    if(asn_imax2INTEGER(&orig, val)) {
        fprintf(stderr, "FAIL: asn_imax2INTEGER(%s)\n", label);
        exit(1);
    }

    er = cbor_encode(&asn_DEF_INTEGER, &orig, buf_append, &enc);
    if(er.encoded < 0) {
        fprintf(stderr, "FAIL: encode %s\n", label);
        exit(1);
    }

    dr = cbor_decode(NULL, &asn_DEF_INTEGER, (void **)&decoded,
                     enc.data, enc.len);
    if(dr.code != RC_OK || !decoded) {
        fprintf(stderr, "FAIL: decode %s (code=%d)\n", label, dr.code);
        exit(1);
    }

    if(asn_INTEGER2imax(decoded, &result)) {
        fprintf(stderr, "FAIL: asn_INTEGER2imax for %s\n", label);
        exit(1);
    }

    if(result != val) {
        fprintf(stderr, "FAIL: round-trip mismatch %s: got %jd, want %jd\n",
                label, result, val);
        exit(1);
    }

    ASN_STRUCT_FREE(asn_DEF_INTEGER, decoded);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &orig);
    buf_free(&enc);
    printf("  ✓ INTEGER round-trip: %s (%jd)\n", label, val);
}

static void
test_integer_cbor_edge_cases(void) {
    printf("test_integer_cbor_edge_cases\n");

    test_integer_roundtrip(0,             "zero");
    test_integer_roundtrip(1,             "one");
    test_integer_roundtrip(-1,            "minus_one");
    test_integer_roundtrip(23,            "23_1byte_boundary");
    test_integer_roundtrip(24,            "24_2byte_boundary");
    test_integer_roundtrip(127,           "127");
    test_integer_roundtrip(-127,          "minus_127");
    test_integer_roundtrip(-128,          "INT8_MIN");
    test_integer_roundtrip(255,           "UINT8_MAX");
    test_integer_roundtrip(256,           "256");
    test_integer_roundtrip(-256,          "minus_256");
    test_integer_roundtrip(32767,         "INT16_MAX");
    test_integer_roundtrip(-32768,        "INT16_MIN");
    test_integer_roundtrip(65535,         "UINT16_MAX");
    test_integer_roundtrip(65536,         "65536");
    test_integer_roundtrip(-65536,        "minus_65536");
    test_integer_roundtrip(2147483647LL,  "INT32_MAX");
    test_integer_roundtrip(-2147483648LL, "INT32_MIN");
    test_integer_roundtrip(4294967295LL,  "UINT32_MAX");
    test_integer_roundtrip(INT64_MAX,     "INT64_MAX");
    test_integer_roundtrip(INT64_MIN,     "INT64_MIN");

    /* Verify CBOR negint decoding for large values.
     * argument == 2^63 means value = -(2^63+1) which doesn't fit int64_t.
     * Construct the CBOR manually and verify decode succeeds. */
    {
        /* CBOR negint with argument = INT64_MAX + 1 = 0x8000000000000000 */
        uint8_t cbor_neg2_63[9] = {
            0x3B,  /* major=1 (negint), additional=27 (8-byte) */
            0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        /* value = -(0x8000000000000000 + 1) = -0x8000000000000001 */
        INTEGER_t *decoded = NULL;
        asn_dec_rval_t dr = cbor_decode(NULL, &asn_DEF_INTEGER,
                                        (void **)&decoded,
                                        cbor_neg2_63, sizeof(cbor_neg2_63));
        if(dr.code != RC_OK || !decoded) {
            fprintf(stderr, "FAIL: decode negint 2^63 (code=%d)\n", dr.code);
            exit(1);
        }
        printf("  ✓ INTEGER decode: CBOR negint argument=2^63 (bignum path)\n");
        ASN_STRUCT_FREE(asn_DEF_INTEGER, decoded);
    }

    {
        /* CBOR negint with argument = UINT64_MAX = 0xFFFFFFFFFFFFFFFF */
        uint8_t cbor_neg_u64max[9] = {
            0x3B,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
        /* value = -(UINT64_MAX + 1) = -2^64 */
        INTEGER_t *decoded = NULL;
        asn_dec_rval_t dr = cbor_decode(NULL, &asn_DEF_INTEGER,
                                        (void **)&decoded,
                                        cbor_neg_u64max, sizeof(cbor_neg_u64max));
        if(dr.code != RC_OK || !decoded) {
            fprintf(stderr, "FAIL: decode negint UINT64_MAX (code=%d)\n", dr.code);
            exit(1);
        }
        printf("  ✓ INTEGER decode: CBOR negint argument=UINT64_MAX (bignum -2^64)\n");
        ASN_STRUCT_FREE(asn_DEF_INTEGER, decoded);
    }

    printf("PASSED: test_integer_cbor_edge_cases\n\n");
}

/* ------------------------------------------------------------------ */
/* NativeInteger round-trip                                             */
/* ------------------------------------------------------------------ */
static void
test_native_integer_roundtrip(long val, const char *label) {
    long orig = val, result = 0;
    long *decoded = NULL;
    struct buffer_acc enc;
    asn_enc_rval_t er;
    asn_dec_rval_t dr;

    memset(&enc, 0, sizeof(enc));

    er = cbor_encode(&asn_DEF_NativeInteger, &orig, buf_append, &enc);
    if(er.encoded < 0) {
        fprintf(stderr, "FAIL: NativeInteger encode %s\n", label);
        exit(1);
    }

    dr = cbor_decode(NULL, &asn_DEF_NativeInteger, (void **)&decoded,
                     enc.data, enc.len);
    if(dr.code != RC_OK || !decoded) {
        fprintf(stderr, "FAIL: NativeInteger decode %s (code=%d)\n", label, dr.code);
        exit(1);
    }
    result = *decoded;
    free(decoded);
    buf_free(&enc);

    if(result != val) {
        fprintf(stderr, "FAIL: NativeInteger mismatch %s: got %ld, want %ld\n",
                label, result, val);
        exit(1);
    }
    printf("  ✓ NativeInteger round-trip: %s (%ld)\n", label, val);
}

static void
test_native_integer_cbor_edge_cases(void) {
    printf("test_native_integer_cbor_edge_cases\n");

    test_native_integer_roundtrip(0,           "zero");
    test_native_integer_roundtrip(1,           "one");
    test_native_integer_roundtrip(-1,          "minus_one");
    test_native_integer_roundtrip(23,          "23");
    test_native_integer_roundtrip(24,          "24");
    test_native_integer_roundtrip(127,         "127");
    test_native_integer_roundtrip(-127,        "minus_127");
    test_native_integer_roundtrip(-128,        "INT8_MIN");
    test_native_integer_roundtrip(255,         "UINT8_MAX");
    test_native_integer_roundtrip(256,         "256");
    test_native_integer_roundtrip(-256,        "minus_256");
    test_native_integer_roundtrip(32767,       "INT16_MAX");
    test_native_integer_roundtrip(-32768,      "INT16_MIN");
    test_native_integer_roundtrip(65535,       "UINT16_MAX");
    test_native_integer_roundtrip(65536,       "65536");
    test_native_integer_roundtrip(-65536,      "minus_65536");
    test_native_integer_roundtrip(2147483647L, "INT32_MAX");
    test_native_integer_roundtrip(-2147483648L, "INT32_MIN");
    test_native_integer_roundtrip(LONG_MAX,    "LONG_MAX");
    test_native_integer_roundtrip(LONG_MIN,    "LONG_MIN");

    printf("PASSED: test_native_integer_cbor_edge_cases\n\n");
}

/* ------------------------------------------------------------------ */
/* BIT STRING round-trip                                                */
/* ------------------------------------------------------------------ */
static void
test_bit_string_roundtrip(const uint8_t *bits, int nbytes, int bits_unused,
                          const char *label) {
    BIT_STRING_t orig, *decoded = NULL;
    struct buffer_acc enc;
    asn_enc_rval_t er;
    asn_dec_rval_t dr;

    memset(&orig, 0, sizeof(orig));
    memset(&enc, 0, sizeof(enc));

    if(nbytes > 0) {
        orig.buf = (uint8_t *)malloc(nbytes);
        assert(orig.buf);
        memcpy(orig.buf, bits, nbytes);
    }
    orig.size = nbytes;
    orig.bits_unused = bits_unused;

    er = cbor_encode(&asn_DEF_BIT_STRING, &orig, buf_append, &enc);
    if(er.encoded < 0) {
        fprintf(stderr, "FAIL: BIT_STRING encode %s\n", label);
        exit(1);
    }

    dr = cbor_decode(NULL, &asn_DEF_BIT_STRING, (void **)&decoded,
                     enc.data, enc.len);
    if(dr.code != RC_OK || !decoded) {
        fprintf(stderr, "FAIL: BIT_STRING decode %s (code=%d)\n", label, dr.code);
        exit(1);
    }

    if(decoded->size != nbytes || decoded->bits_unused != bits_unused) {
        fprintf(stderr, "FAIL: BIT_STRING mismatch %s: "
                "size=%d (want %d), unused=%d (want %d)\n",
                label, decoded->size, nbytes,
                decoded->bits_unused, bits_unused);
        exit(1);
    }
    if(nbytes > 0 && memcmp(decoded->buf, bits, nbytes) != 0) {
        fprintf(stderr, "FAIL: BIT_STRING data mismatch %s\n", label);
        exit(1);
    }

    ASN_STRUCT_FREE(asn_DEF_BIT_STRING, decoded);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BIT_STRING, &orig);
    buf_free(&enc);
    printf("  ✓ BIT_STRING round-trip: %s\n", label);
}

static void
test_bit_string_cbor_edge_cases(void) {
    printf("test_bit_string_cbor_edge_cases\n");

    /* 0 bits: size=0, bits_unused=0 */
    test_bit_string_roundtrip(NULL, 0, 0, "0_bits");

    /* 8 bits (1 byte): bits_unused=0 */
    {
        uint8_t data[1] = {0xA5};
        test_bit_string_roundtrip(data, 1, 0, "8_bits");
    }

    /* 1 bit: size=1, bits_unused=7 */
    {
        uint8_t data[1] = {0x80};
        test_bit_string_roundtrip(data, 1, 7, "1_bit");
    }

    /* 9 bits: size=2, bits_unused=7 */
    {
        uint8_t data[2] = {0xFF, 0x80};
        test_bit_string_roundtrip(data, 2, 7, "9_bits");
    }

    /* 16 bits: size=2, bits_unused=0 */
    {
        uint8_t data[2] = {0xDE, 0xAD};
        test_bit_string_roundtrip(data, 2, 0, "16_bits");
    }

    printf("PASSED: test_bit_string_cbor_edge_cases\n\n");
}

/* ------------------------------------------------------------------ */
/* OCTET STRING round-trip                                              */
/* ------------------------------------------------------------------ */
static void
test_octet_string_roundtrip(const uint8_t *data, size_t len, const char *label) {
    OCTET_STRING_t orig, *decoded = NULL;
    struct buffer_acc enc;
    asn_enc_rval_t er;
    asn_dec_rval_t dr;

    memset(&orig, 0, sizeof(orig));
    memset(&enc, 0, sizeof(enc));

    if(len > 0) {
        orig.buf = (uint8_t *)malloc(len);
        assert(orig.buf);
        memcpy(orig.buf, data, len);
    }
    orig.size = (int)len;

    er = cbor_encode(&asn_DEF_OCTET_STRING, &orig, buf_append, &enc);
    if(er.encoded < 0) {
        fprintf(stderr, "FAIL: OCTET_STRING encode %s\n", label);
        exit(1);
    }

    dr = cbor_decode(NULL, &asn_DEF_OCTET_STRING, (void **)&decoded,
                     enc.data, enc.len);
    if(dr.code != RC_OK || !decoded) {
        fprintf(stderr, "FAIL: OCTET_STRING decode %s (code=%d)\n", label, dr.code);
        exit(1);
    }

    if((size_t)decoded->size != len) {
        fprintf(stderr, "FAIL: OCTET_STRING size mismatch %s: "
                "got %d, want %zu\n", label, decoded->size, len);
        exit(1);
    }
    if(len > 0 && memcmp(decoded->buf, data, len) != 0) {
        fprintf(stderr, "FAIL: OCTET_STRING data mismatch %s\n", label);
        exit(1);
    }

    ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, decoded);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_OCTET_STRING, &orig);
    buf_free(&enc);
    printf("  ✓ OCTET_STRING round-trip: %s (len=%zu)\n", label, len);
}

static void
test_octet_string_cbor_edge_cases(void) {
    printf("test_octet_string_cbor_edge_cases\n");

    /* Empty */
    test_octet_string_roundtrip(NULL, 0, "empty");

    /* Single byte */
    {
        uint8_t d[1] = {0x42};
        test_octet_string_roundtrip(d, 1, "single_byte");
    }

    /* 23 bytes (last with 1-byte CBOR header) */
    {
        uint8_t d[23];
        for(int i = 0; i < 23; i++) d[i] = (uint8_t)i;
        test_octet_string_roundtrip(d, 23, "23_bytes");
    }

    /* 24 bytes (first with 2-byte CBOR header) */
    {
        uint8_t d[24];
        for(int i = 0; i < 24; i++) d[i] = (uint8_t)(i + 1);
        test_octet_string_roundtrip(d, 24, "24_bytes");
    }

    /* 100 bytes */
    {
        uint8_t d[100];
        for(int i = 0; i < 100; i++) d[i] = (uint8_t)(i * 2);
        test_octet_string_roundtrip(d, 100, "100_bytes");
    }

    printf("PASSED: test_octet_string_cbor_edge_cases\n\n");
}

/* ------------------------------------------------------------------ */
/* Generic asn_encode / asn_decode dispatch via ATS_CBOR                */
/* ------------------------------------------------------------------ */
static void
test_asn_encode_decode_cbor_dispatch(void) {
    printf("test_asn_encode_decode_cbor_dispatch\n");

    /* Use INTEGER round-trip through the generic API */
    {
        INTEGER_t orig, *decoded = NULL;
        struct buffer_acc enc;
        asn_enc_rval_t er;
        asn_dec_rval_t dr;
        intmax_t result;

        memset(&orig, 0, sizeof(orig));
        memset(&enc, 0, sizeof(enc));

        if(asn_imax2INTEGER(&orig, 12345)) {
            fprintf(stderr, "FAIL: asn_imax2INTEGER\n");
            exit(1);
        }

        er = asn_encode(NULL, ATS_CBOR, &asn_DEF_INTEGER, &orig,
                        buf_append, &enc);
        if(er.encoded < 0) {
            fprintf(stderr, "FAIL: asn_encode(ATS_CBOR, INTEGER)\n");
            exit(1);
        }

        dr = asn_decode(NULL, ATS_CBOR, &asn_DEF_INTEGER,
                        (void **)&decoded, enc.data, enc.len);
        if(dr.code != RC_OK || !decoded) {
            fprintf(stderr, "FAIL: asn_decode(ATS_CBOR, INTEGER) code=%d\n",
                    dr.code);
            exit(1);
        }

        if(asn_INTEGER2imax(decoded, &result) || result != 12345) {
            fprintf(stderr, "FAIL: dispatch round-trip mismatch: %jd\n", result);
            exit(1);
        }

        ASN_STRUCT_FREE(asn_DEF_INTEGER, decoded);
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &orig);
        buf_free(&enc);
        printf("  ✓ asn_encode/asn_decode dispatch: ATS_CBOR INTEGER\n");
    }

    /* Use OCTET STRING round-trip through the generic API */
    {
        OCTET_STRING_t orig, *decoded = NULL;
        struct buffer_acc enc;
        asn_enc_rval_t er;
        asn_dec_rval_t dr;
        static const uint8_t payload[] = {0xDE, 0xAD, 0xBE, 0xEF};

        memset(&orig, 0, sizeof(orig));
        memset(&enc, 0, sizeof(enc));

        orig.buf = (uint8_t *)malloc(sizeof(payload));
        assert(orig.buf);
        memcpy(orig.buf, payload, sizeof(payload));
        orig.size = sizeof(payload);

        er = asn_encode(NULL, ATS_CBOR, &asn_DEF_OCTET_STRING, &orig,
                        buf_append, &enc);
        if(er.encoded < 0) {
            fprintf(stderr, "FAIL: asn_encode(ATS_CBOR, OCTET_STRING)\n");
            exit(1);
        }

        dr = asn_decode(NULL, ATS_CBOR, &asn_DEF_OCTET_STRING,
                        (void **)&decoded, enc.data, enc.len);
        if(dr.code != RC_OK || !decoded) {
            fprintf(stderr, "FAIL: asn_decode(ATS_CBOR, OCTET_STRING) code=%d\n",
                    dr.code);
            exit(1);
        }

        if((size_t)decoded->size != sizeof(payload)
           || memcmp(decoded->buf, payload, sizeof(payload)) != 0) {
            fprintf(stderr, "FAIL: dispatch OCTET_STRING data mismatch\n");
            exit(1);
        }

        ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, decoded);
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_OCTET_STRING, &orig);
        buf_free(&enc);
        printf("  ✓ asn_encode/asn_decode dispatch: ATS_CBOR OCTET_STRING\n");
    }

    printf("PASSED: test_asn_encode_decode_cbor_dispatch\n\n");
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int
main(void) {
    printf("=== CBOR Codec Tests ===\n\n");

    test_integer_cbor_edge_cases();
    test_native_integer_cbor_edge_cases();
    test_bit_string_cbor_edge_cases();
    test_octet_string_cbor_edge_cases();
    test_asn_encode_decode_cbor_dispatch();

    printf("=== ALL CBOR TESTS PASSED ===\n");
    return 0;
}
