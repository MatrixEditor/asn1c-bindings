/*-
 * Copyright (c) 2025 Contributors. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <cbor_support.h>
#include <string.h>

/*
 * Encode a CBOR item header: major type (0-7) and argument.
 * Uses the canonical shortest encoding per RFC 7049.
 * Returns total bytes written, or -1 on callback error.
 */
ssize_t
cbor_encode_head(uint8_t major, uint64_t argument,
                 asn_app_consume_bytes_f *cb, void *app_key) {
    uint8_t buf[9];
    size_t len;
    uint8_t prefix = (major & 0x07) << 5;

    if(argument <= 23) {
        buf[0] = prefix | (uint8_t)argument;
        len = 1;
    } else if(argument <= 0xFF) {
        buf[0] = prefix | CBOR_AI_1BYTE;
        buf[1] = (uint8_t)argument;
        len = 2;
    } else if(argument <= 0xFFFF) {
        buf[0] = prefix | CBOR_AI_2BYTE;
        buf[1] = (uint8_t)(argument >> 8);
        buf[2] = (uint8_t)(argument);
        len = 3;
    } else if(argument <= 0xFFFFFFFF) {
        buf[0] = prefix | CBOR_AI_4BYTE;
        buf[1] = (uint8_t)(argument >> 24);
        buf[2] = (uint8_t)(argument >> 16);
        buf[3] = (uint8_t)(argument >> 8);
        buf[4] = (uint8_t)(argument);
        len = 5;
    } else {
        buf[0] = prefix | CBOR_AI_8BYTE;
        buf[1] = (uint8_t)(argument >> 56);
        buf[2] = (uint8_t)(argument >> 48);
        buf[3] = (uint8_t)(argument >> 40);
        buf[4] = (uint8_t)(argument >> 32);
        buf[5] = (uint8_t)(argument >> 24);
        buf[6] = (uint8_t)(argument >> 16);
        buf[7] = (uint8_t)(argument >> 8);
        buf[8] = (uint8_t)(argument);
        len = 9;
    }

    if(cb(buf, len, app_key) < 0)
        return -1;
    return (ssize_t)len;
}

/*
 * Decode a CBOR item header from buffer.
 * Returns bytes consumed (1..9), or -1 on error/truncation.
 */
ssize_t
cbor_decode_head(const uint8_t *buf, size_t size,
                 uint8_t *major_out, uint64_t *argument_out) {
    if(size < 1) return -1;

    uint8_t first = buf[0];
    uint8_t major = first >> 5;
    uint8_t info  = first & 0x1F;

    *major_out = major;

    if(info <= 23) {
        *argument_out = info;
        return 1;
    } else if(info == CBOR_AI_1BYTE) {
        if(size < 2) return -1;
        *argument_out = buf[1];
        return 2;
    } else if(info == CBOR_AI_2BYTE) {
        if(size < 3) return -1;
        *argument_out = ((uint64_t)buf[1] << 8) | buf[2];
        return 3;
    } else if(info == CBOR_AI_4BYTE) {
        if(size < 5) return -1;
        *argument_out = ((uint64_t)buf[1] << 24) | ((uint64_t)buf[2] << 16)
                      | ((uint64_t)buf[3] << 8)  | (uint64_t)buf[4];
        return 5;
    } else if(info == CBOR_AI_8BYTE) {
        if(size < 9) return -1;
        *argument_out = ((uint64_t)buf[1] << 56) | ((uint64_t)buf[2] << 48)
                      | ((uint64_t)buf[3] << 40) | ((uint64_t)buf[4] << 32)
                      | ((uint64_t)buf[5] << 24) | ((uint64_t)buf[6] << 16)
                      | ((uint64_t)buf[7] << 8)  | (uint64_t)buf[8];
        return 9;
    } else {
        /* Reserved or break code */
        return -1;
    }
}

ssize_t
cbor_encode_uint(uint64_t value, asn_app_consume_bytes_f *cb, void *app_key) {
    return cbor_encode_head(CBOR_MAJOR_UINT, value, cb, app_key);
}

ssize_t
cbor_encode_negint(uint64_t value, asn_app_consume_bytes_f *cb, void *app_key) {
    return cbor_encode_head(CBOR_MAJOR_NEGINT, value, cb, app_key);
}

ssize_t
cbor_encode_bytes(const void *data, size_t len,
                  asn_app_consume_bytes_f *cb, void *app_key) {
    ssize_t hlen = cbor_encode_head(CBOR_MAJOR_BYTES, len, cb, app_key);
    if(hlen < 0) return -1;
    if(len > 0) {
        if(cb(data, len, app_key) < 0) return -1;
    }
    return hlen + (ssize_t)len;
}

ssize_t
cbor_encode_text(const char *str, size_t len,
                 asn_app_consume_bytes_f *cb, void *app_key) {
    ssize_t hlen = cbor_encode_head(CBOR_MAJOR_TEXT, len, cb, app_key);
    if(hlen < 0) return -1;
    if(len > 0) {
        if(cb(str, len, app_key) < 0) return -1;
    }
    return hlen + (ssize_t)len;
}

ssize_t
cbor_encode_tag(uint64_t tag, asn_app_consume_bytes_f *cb, void *app_key) {
    return cbor_encode_head(CBOR_MAJOR_TAG, tag, cb, app_key);
}

ssize_t
cbor_encode_simple(uint8_t simple_val,
                   asn_app_consume_bytes_f *cb, void *app_key) {
    /* Simple values 0..19 and 32..255: encode as single byte with major 7 */
    uint8_t buf = simple_val;
    if(cb(&buf, 1, app_key) < 0) return -1;
    return 1;
}

ssize_t
cbor_encode_float64(double value, asn_app_consume_bytes_f *cb, void *app_key) {
    uint8_t buf[9];
    uint64_t bits;

    buf[0] = (CBOR_MAJOR_SIMPLE << 5) | CBOR_AI_8BYTE;  /* 0xFB */
    memcpy(&bits, &value, 8);
    buf[1] = (uint8_t)(bits >> 56);
    buf[2] = (uint8_t)(bits >> 48);
    buf[3] = (uint8_t)(bits >> 40);
    buf[4] = (uint8_t)(bits >> 32);
    buf[5] = (uint8_t)(bits >> 24);
    buf[6] = (uint8_t)(bits >> 16);
    buf[7] = (uint8_t)(bits >> 8);
    buf[8] = (uint8_t)(bits);

    if(cb(buf, 9, app_key) < 0) return -1;
    return 9;
}

ssize_t
cbor_decode_float64(const uint8_t *buf, size_t size, double *value_out) {
    uint64_t bits;

    if(size < 9) return -1;
    if(buf[0] != 0xFB) return -1;  /* Must be 0xFB for float64 */

    bits = ((uint64_t)buf[1] << 56) | ((uint64_t)buf[2] << 48)
         | ((uint64_t)buf[3] << 40) | ((uint64_t)buf[4] << 32)
         | ((uint64_t)buf[5] << 24) | ((uint64_t)buf[6] << 16)
         | ((uint64_t)buf[7] << 8)  | (uint64_t)buf[8];
    memcpy(value_out, &bits, 8);
    return 9;
}
