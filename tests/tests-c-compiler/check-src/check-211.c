#undef NDEBUG
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <T.h>

static void
decode_ber_once(const uint8_t *buf, size_t size, int expect_ok) {
    T_t *decoded = NULL;
    asn_dec_rval_t rv;

    rv = ber_decode(0, &asn_DEF_T, (void **)&decoded, buf, size);
    if(expect_ok) {
        assert(rv.code == RC_OK);
        assert(decoded);
    } else {
        assert(rv.code != RC_OK);
    }
    ASN_STRUCT_FREE(asn_DEF_T, decoded);
}

static void
fill_defined(T_t *t) {
    memset(t, 0, sizeof(*t));
    t->id = 1;
    t->value.present = value_PR_Payload;
    t->value.choice.Payload = 1;
}

static void
decode_oer_valid_and_truncated(void) {
    uint8_t buf[32];
    T_t source;
    T_t *decoded = NULL;
    asn_enc_rval_t er;
    asn_dec_rval_t rv;
    ssize_t encoded;
    size_t cut;
    int saw_failure = 0;

    fill_defined(&source);

    er = oer_encode_to_buffer(&asn_DEF_T, 0, &source, buf, sizeof(buf));
    assert(er.encoded > 0);
    encoded = er.encoded;

    rv = oer_decode(0, &asn_DEF_T, (void **)&decoded, buf, encoded);
    assert(rv.code == RC_OK);
    assert(decoded);
    assert(decoded->id == 1);
    assert(decoded->value.present == value_PR_Payload);
    assert(decoded->value.choice.Payload != 0);
    ASN_STRUCT_FREE(asn_DEF_T, decoded);

    /*
     * Truncated OER input exercises OPEN TYPE cleanup after the selected
     * BOOLEAN decoder has started, where the wrapper must be reset as a
     * CHOICE rather than as the selected primitive type.
     */
    for(cut = 1; cut < (size_t)encoded; cut++) {
        decoded = NULL;
        rv = oer_decode(0, &asn_DEF_T, (void **)&decoded, buf, cut);
        if(rv.code == RC_OK) {
            ASN_STRUCT_FREE(asn_DEF_T, decoded);
            continue;
        }
        saw_failure = 1;
        ASN_STRUCT_FREE(asn_DEF_T, decoded);
    }
    assert(saw_failure);

    ASN_STRUCT_RESET(asn_DEF_T, &source);
}

int
main(void) {
    /*
     * id 0 selects an information object set row with no &Type.  The decoder
     * must fail cleanly instead of dereferencing a NULL type descriptor.
     */
    static const uint8_t typeless_ber[] = {
        0x30, 0x07, 0x80, 0x01, 0x00, 0xa1, 0x02, 0x30, 0x00
    };

    /*
     * id 1 selects the later row that does define Payload.  The generated
     * selector must use the compact OPEN TYPE CHOICE presence index.
     */
    static const uint8_t defined_ber[] = {
        0x30, 0x08, 0x80, 0x01, 0x01, 0xa1, 0x03, 0x01, 0x01, 0xff
    };

    /*
     * The inner BOOLEAN is truncated.  Failure cleanup must reset the OPEN
     * TYPE wrapper, not size that reset from the selected primitive type.
     */
    static const uint8_t truncated_ber[] = {
        0x30, 0x80, 0x80, 0x01, 0x01, 0x81
    };

    decode_ber_once(typeless_ber, sizeof(typeless_ber), 0);
    decode_ber_once(defined_ber, sizeof(defined_ber), 1);
    decode_ber_once(truncated_ber, sizeof(truncated_ber), 0);
    decode_oer_valid_and_truncated();

    printf("OK: OPEN TYPE typeless row regressions decoded without crashing.\n");
    return 0;
}
