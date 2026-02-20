/*-
 * Copyright (c) 2025 Contributors. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef _CBOR_SUPPORT_H_
#define _CBOR_SUPPORT_H_

#include <asn_application.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CBOR major types (RFC 7049 section 2.1)
 */
#define CBOR_MAJOR_UINT     0  /* major type 0: unsigned integer */
#define CBOR_MAJOR_NEGINT   1  /* major type 1: negative integer */
#define CBOR_MAJOR_BYTES    2  /* major type 2: byte string */
#define CBOR_MAJOR_TEXT     3  /* major type 3: text string */
#define CBOR_MAJOR_ARRAY    4  /* major type 4: array */
#define CBOR_MAJOR_MAP      5  /* major type 5: map */
#define CBOR_MAJOR_TAG      6  /* major type 6: tagged item */
#define CBOR_MAJOR_SIMPLE   7  /* major type 7: float/simple value */

/*
 * CBOR additional info values for 1/2/4/8-byte argument lengths
 */
#define CBOR_AI_1BYTE       24  /* 1-byte argument follows */
#define CBOR_AI_2BYTE       25  /* 2-byte argument follows */
#define CBOR_AI_4BYTE       26  /* 4-byte argument follows */
#define CBOR_AI_8BYTE       27  /* 8-byte argument follows */

/*
 * CBOR simple values
 */
#define CBOR_SV_FALSE       0xF4  /* simple value: false */
#define CBOR_SV_TRUE        0xF5  /* simple value: true */
#define CBOR_SV_NULL        0xF6  /* simple value: null */
#define CBOR_SV_UNDEF       0xF7  /* simple value: undefined */

/*
 * CBOR tags (RFC 7049 section 2.4)
 */
#define CBOR_TAG_POSINT_BIGNUM  2   /* positive bignum */
#define CBOR_TAG_NEGINT_BIGNUM  3   /* negative bignum */

/*
 * Encode a CBOR item header (major type + argument) into the stream.
 * Uses shortest canonical encoding.
 * Returns number of bytes written (>0), or -1 on callback error.
 */
ssize_t cbor_encode_head(uint8_t major, uint64_t argument,
                         asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Decode a CBOR item header from buffer.
 * Returns number of bytes consumed (>0), or -1 on error/truncation.
 * On success, sets *major_out and *argument_out.
 */
ssize_t cbor_decode_head(const uint8_t *buf, size_t size,
                         uint8_t *major_out, uint64_t *argument_out);

/*
 * Encode a CBOR unsigned integer (canonical: shortest encoding).
 */
ssize_t cbor_encode_uint(uint64_t value,
                         asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR negative integer.
 * Encodes -(value+1). Canonical shortest encoding.
 */
ssize_t cbor_encode_negint(uint64_t value,
                           asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR byte string (header + data).
 */
ssize_t cbor_encode_bytes(const void *data, size_t len,
                          asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR text string (header + data).
 */
ssize_t cbor_encode_text(const char *str, size_t len,
                         asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR tag.
 */
ssize_t cbor_encode_tag(uint64_t tag,
                        asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR simple value (true, false, null, float64, etc.).
 * For float64, use cbor_encode_float64() instead.
 */
ssize_t cbor_encode_simple(uint8_t simple_val,
                           asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Encode a CBOR IEEE 754 double (major type 7, additional info 27).
 */
ssize_t cbor_encode_float64(double value,
                            asn_app_consume_bytes_f *cb, void *app_key);

/*
 * Decode a CBOR IEEE 754 double from buffer (must start with 0xFB).
 * Returns number of bytes consumed (9), or -1 on error/truncation.
 */
ssize_t cbor_decode_float64(const uint8_t *buf, size_t size, double *value_out);

/*
 * Skip a complete CBOR data item starting at buf[0].
 * Handles all major types recursively (arrays, maps, tags, integers,
 * byte/text strings, floats, simple values).
 * Returns total bytes consumed, or -1 on error/truncation.
 */
ssize_t cbor_skip_item(const uint8_t *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif  /* _CBOR_SUPPORT_H_ */
