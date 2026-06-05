/*
 * Tests for the updated XER OCTET STRING hex/Base64 handling (issue #538).
 *
 * Covers:
 *  17a. Default OCTET_STRING XER encoder (OCTET_STRING_encode_xer) emits
 *       contiguous upper-case hex for XER_F_BASIC and XER_F_CANONICAL.
 *  17b. XER_F_BASIC|XER_F_BASE64 emits Base64; XER_F_CANONICAL|XER_F_BASE64
 *       emits hex (canonical overrides XER_F_BASE64).
 *  17c. Regression #538: "0x6B" treated as Base64, not hex (no 0x heuristic).
 *  17d. Liberal hex: lower-case digits, internal whitespace, H'aAbB'.
 *  17e. Odd-length pure-hex-alphabet string decoded as Base64.
 *  17f. Garbage input → RC_FAIL.
 *  17g. Chunked auto-decode: format pinned by first non-ws chunk.
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <OCTET_STRING.h>
#include <xer_encoder.h>

/* ------------------------------------------------------------------ */
/* Encode helpers                                                       */
/* ------------------------------------------------------------------ */

static char enc_buf[8192];
static size_t enc_off;

static int
collect(const void *buf, size_t size, void *key) {
    (void)key;
    assert(enc_off + size < sizeof(enc_buf));
    memcpy(enc_buf + enc_off, buf, size);
    enc_off += size;
    return 0;
}

static const char *
encode_os(const uint8_t *data, size_t len, enum xer_encoder_flags_e flags) {
    OCTET_STRING_t os;
    asn_enc_rval_t er;

    memset(&os, 0, sizeof(os));
    os.buf = (uint8_t *)data;
    os.size = len;

    enc_off = 0;
    memset(enc_buf, 0, sizeof(enc_buf));

    er = OCTET_STRING_encode_xer(&asn_DEF_OCTET_STRING, &os,
                                 0, flags, collect, NULL);
    assert(er.encoded >= 0);
    enc_buf[enc_off] = '\0';
    return enc_buf;
}

/* Decode a complete "<tag>VALUE</tag>" XML fragment via _decode_xer_auto. */
static int
decode_auto(const char *xml, uint8_t **out_buf, size_t *out_size) {
    OCTET_STRING_t *st = NULL;
    asn_dec_rval_t dr;

    dr = OCTET_STRING_decode_xer_auto(NULL, &asn_DEF_OCTET_STRING,
                                      (void **)&st, "tag",
                                      xml, strlen(xml));
    if(dr.code != RC_OK) {
        if(st) ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, st);
        return -1;
    }
    *out_buf  = st->buf;
    *out_size = st->size;
    /* Transfer ownership of buf; free the shell */
    st->buf = NULL;
    ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, st);
    return 0;
}

/* ------------------------------------------------------------------ */
/* 17a: Default encoder emits contiguous upper-case hex                 */
/* ------------------------------------------------------------------ */
static void
test_17a_default_encoder_hex(void) {
    static const uint8_t data[] = {0xD3, 0x1E, 0x81};
    const char *got;

    printf("17a: Default encoder emits upper-case hex\n");

    /* BASIC-XER */
    got = encode_os(data, sizeof(data), XER_F_BASIC);
    printf("     XER_F_BASIC   -> \"%s\"\n", got);
    assert(strcmp(got, "D31E81") == 0);

    /* CANONICAL-XER */
    got = encode_os(data, sizeof(data), XER_F_CANONICAL);
    printf("     XER_F_CANONICAL -> \"%s\"\n", got);
    assert(strcmp(got, "D31E81") == 0);

    /* Auto-decode of the hex output must give back the original bytes */
    {
        uint8_t *out; size_t out_sz;
        assert(decode_auto("<tag>D31E81</tag>", &out, &out_sz) == 0);
        assert(out_sz == sizeof(data));
        assert(memcmp(out, data, sizeof(data)) == 0);
        free(out);
    }
    printf("     Round-trip: OK\n");
}

/* ------------------------------------------------------------------ */
/* 17b: XER_F_BASE64 flag; canonical overrides                          */
/* ------------------------------------------------------------------ */
static void
test_17b_base64_flag(void) {
    static const uint8_t data[] = {0xD3, 0x1E, 0x81};
    const char *got;

    printf("17b: XER_F_BASE64 flag and canonical override\n");

    /* BASIC + BASE64 should give Base64 output */
    got = encode_os(data, sizeof(data), XER_F_BASIC | XER_F_BASE64);
    printf("     XER_F_BASIC|XER_F_BASE64   -> \"%s\"\n", got);
    assert(strcmp(got, "0x6B") == 0);  /* D3 1E 81 → Base64 "0x6B" */

    /* CANONICAL + BASE64 → canonical wins, so hex output */
    got = encode_os(data, sizeof(data), XER_F_CANONICAL | XER_F_BASE64);
    printf("     XER_F_CANONICAL|XER_F_BASE64 -> \"%s\"\n", got);
    assert(strcmp(got, "D31E81") == 0);
}

/* ------------------------------------------------------------------ */
/* 17c: Regression #538 — "0x6B" is Base64, not hex-with-0x-prefix    */
/* ------------------------------------------------------------------ */
static void
test_17c_regression_538(void) {
    /* {0xD3, 0x1E, 0x81} → Base64 "0x6B"
     * The old code had a "0x"/"0X" heuristic that would have treated this as
     * hex 0x6B = {0x6B}, which is wrong.  Correct: parse as Base64. */
    static const uint8_t expected[] = {0xD3, 0x1E, 0x81};
    uint8_t *out; size_t out_sz;

    printf("17c: \"0x6B\" decoded as Base64 (regression #538)\n");
    assert(decode_auto("<tag>0x6B</tag>", &out, &out_sz) == 0);
    printf("     got %zu byte(s): ", out_sz);
    for(size_t i = 0; i < out_sz; i++) printf("%02X ", out[i]);
    printf("\n");
    assert(out_sz == sizeof(expected));
    assert(memcmp(out, expected, sizeof(expected)) == 0);
    free(out);

    /* Likewise "0X12" */
    printf("     \"0X12\" also treated as Base64 (not hex)\n");
    assert(decode_auto("<tag>0X12</tag>", &out, &out_sz) == 0);
    /* Base64 "0X12" → {0xD1, 0x7D, 0xA6} — just verify it is NOT {0x12} */
    assert(out_sz != 1 || out[0] != 0x12);
    free(out);
}

/* ------------------------------------------------------------------ */
/* 17d: Liberal hex — lower-case, internal whitespace, H' prefix       */
/* ------------------------------------------------------------------ */
static void
test_17d_liberal_hex(void) {
    static const uint8_t expected_aabb[] = {0xAA, 0xBB};
    uint8_t *out; size_t out_sz;

    printf("17d: Liberal hex decoding\n");

    /* Lower-case digits */
    assert(decode_auto("<tag>aabb</tag>", &out, &out_sz) == 0);
    assert(out_sz == 2 && memcmp(out, expected_aabb, 2) == 0);
    free(out);
    printf("     lower-case \"aabb\": OK\n");

    /* Internal whitespace */
    assert(decode_auto("<tag>AA BB</tag>", &out, &out_sz) == 0);
    assert(out_sz == 2 && memcmp(out, expected_aabb, 2) == 0);
    free(out);
    printf("     whitespace \"AA BB\": OK\n");

    /* H' prefix (nonstandard) */
    assert(decode_auto("<tag>H'aAbB'</tag>", &out, &out_sz) == 0);
    assert(out_sz == 2 && memcmp(out, expected_aabb, 2) == 0);
    free(out);
    printf("     H'aAbB': OK\n");
}

/* ------------------------------------------------------------------ */
/* 17e: Odd-length pure-hex-alphabet string decoded as Base64          */
/* ------------------------------------------------------------------ */
static void
test_17e_odd_length_as_base64(void) {
    /* "ABC" has 3 characters, all in hex alphabet, but odd count → Base64.
     * Base64 "ABC" = 0x00 0x10 0x83 padded (partial group of 3). */
    uint8_t *out; size_t out_sz;

    printf("17e: Odd-length hex-alphabet string decoded as Base64\n");
    assert(decode_auto("<tag>ABC</tag>", &out, &out_sz) == 0);
    printf("     \"ABC\" → %zu byte(s)\n", out_sz);
    /* Must NOT be decoded as if it were 3 hex nibbles (invalid hex anyway). */
    /* It should decode as Base64 and produce 2 bytes (3 chars ≈ 2.25 bytes
     * rounded down to 2).  We just verify the decode succeeds and is ≠ error. */
    assert(out_sz >= 1);
    free(out);
}

/* ------------------------------------------------------------------ */
/* 17f: Garbage input → RC_FAIL                                         */
/* ------------------------------------------------------------------ */
static void
test_17f_garbage(void) {
    OCTET_STRING_t *st = NULL;
    asn_dec_rval_t dr;

    printf("17f: Garbage input rejected\n");

    dr = OCTET_STRING_decode_xer_auto(NULL, &asn_DEF_OCTET_STRING,
                                      (void **)&st, "tag",
                                      "<tag>P!Q</tag>", 14);
    if(st) ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, st);
    assert(dr.code != RC_OK);
    printf("     \"P!Q\" correctly rejected (code=%d)\n", dr.code);
}

/* ------------------------------------------------------------------ */
/* 17g: Chunked auto-decode — format pinned by first chunk             */
/* ------------------------------------------------------------------ */
static void
test_17g_chunked(void) {
    /*
     * "AABB" is an ambiguous body (pure hex alphabet, even count).
     * Split it across two chunks: "AA" then "BB".
     * The first chunk "AA" classifies as AMBIGUOUS_HEX, pins to hex.
     * The second chunk "BB" must use the pinned hex converter.
     * Result: {0xAA, 0xBB}.
     */
    static const uint8_t expected[] = {0xAA, 0xBB};
    static const char xml[] = "<tag>AABB</tag>";
    OCTET_STRING_t *st = NULL;
    asn_dec_rval_t dr;
    size_t consumed = 0;

    printf("17g: Chunked decode — format pinned by first chunk\n");

    /* Feed byte-by-byte to force the chunked path */
    while(consumed < strlen(xml)) {
        dr = OCTET_STRING_decode_xer_auto(NULL, &asn_DEF_OCTET_STRING,
                                          (void **)&st, "tag",
                                          xml + consumed, 1);
        if(dr.code == RC_OK) {
            consumed += dr.consumed;
            break;
        } else if(dr.code == RC_WMORE) {
            consumed += dr.consumed ? dr.consumed : 1;
        } else {
            printf("  ERROR: Decode failed at byte %zu, code=%d\n",
                   consumed, dr.code);
            if(st) ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, st);
            assert(0);
        }
    }

    assert(dr.code == RC_OK);
    assert(st != NULL);
    printf("     Result: %zu byte(s): ", st->size);
    for(size_t i = 0; i < st->size; i++) printf("%02X ", st->buf[i]);
    printf("\n");
    assert(st->size == sizeof(expected));
    assert(memcmp(st->buf, expected, sizeof(expected)) == 0);
    ASN_STRUCT_FREE(asn_DEF_OCTET_STRING, st);
    printf("     Chunked hex round-trip: OK\n");
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int
main(void) {
    printf("=== XER OCTET STRING auto-detect tests (issue #538) ===\n\n");

    test_17a_default_encoder_hex();
    test_17b_base64_flag();
    test_17c_regression_538();
    test_17d_liberal_hex();
    test_17e_odd_length_as_base64();
    test_17f_garbage();
    test_17g_chunked();

    printf("\n=== All tests passed ===\n");
    return 0;
}
