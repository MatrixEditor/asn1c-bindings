/*-
 * Copyright (c) 2025 Contributors. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <BOOLEAN.h>
#include <cbor_encoder.h>
#include <cbor_decoder.h>
#include <cbor_support.h>

asn_enc_rval_t
BOOLEAN_encode_cbor(const asn_TYPE_descriptor_t *td, const void *sptr,
                    asn_app_consume_bytes_f *cb, void *app_key) {
    const BOOLEAN_t *st = (const BOOLEAN_t *)sptr;
    asn_enc_rval_t er = {0, 0, 0};
    ssize_t ret;

    (void)td;

    if(!st) ASN__ENCODE_FAILED;

    ret = cbor_encode_simple(*st ? CBOR_SV_TRUE : CBOR_SV_FALSE, cb, app_key);
    if(ret < 0) ASN__ENCODE_FAILED;
    er.encoded = ret;
    ASN__ENCODED_OK(er);
}

asn_dec_rval_t
BOOLEAN_decode_cbor(const asn_codec_ctx_t *opt_codec_ctx,
                    const asn_TYPE_descriptor_t *td,
                    void **sptr, const void *buf_ptr, size_t size) {
    BOOLEAN_t *st = (BOOLEAN_t *)*sptr;
    const uint8_t *buf = (const uint8_t *)buf_ptr;
    asn_dec_rval_t rval = {RC_FAIL, 0};

    (void)opt_codec_ctx;
    (void)td;

    if(!st) {
        st = (BOOLEAN_t *)CALLOC(1, sizeof(*st));
        if(!st) ASN__DECODE_FAILED;
        *sptr = st;
    }

    if(size < 1) ASN__DECODE_FAILED;

    switch(buf[0]) {
    case CBOR_SV_FALSE:
        *st = 0;
        break;
    case CBOR_SV_TRUE:
        *st = 1;
        break;
    default:
        ASN__DECODE_FAILED;
    }

    rval.consumed = 1;
    rval.code = RC_OK;
    return rval;
}
