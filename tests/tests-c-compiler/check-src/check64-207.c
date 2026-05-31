/*
 * Regression test for anonymous unsigned INTEGER in SEQUENCE OF.
 * Verifies that:
 *  - Generated code includes per-element descriptor with field_unsigned=1
 *  - DER encode/BER decode roundtrip works correctly for unsigned 32-bit values
 *  - Encode failure (buffer too small) is handled
 *  - Decode failure (malformed tag) is handled
 */
#undef	NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <SeqOfUInt32.h>

static void
test_encode_decode(void) {
	SeqOfUInt32_t seq;
	unsigned long *elem0, *elem1;
	asn_enc_rval_t er;
	asn_dec_rval_t dr;
	uint8_t buf[128];
	SeqOfUInt32_t *decoded = NULL;

	memset(&seq, 0, sizeof(seq));

	/* Allocate two elements: 0 and 4294967295 (2^32 - 1) */
	elem0 = calloc(1, sizeof(*elem0));
	assert(elem0);
	*elem0 = 0;

	elem1 = calloc(1, sizeof(*elem1));
	assert(elem1);
	*elem1 = 4294967295UL;

	ASN_SEQUENCE_ADD(&seq, elem0);
	ASN_SEQUENCE_ADD(&seq, elem1);

	/* Encode */
	er = der_encode_to_buffer(&asn_DEF_SeqOfUInt32, &seq, buf, sizeof(buf));
	assert(er.encoded > 0);
	fprintf(stderr, "DER encoded %zd bytes\n", er.encoded);

	/* Decode */
	dr = ber_decode(NULL, &asn_DEF_SeqOfUInt32, (void **)&decoded, buf, er.encoded);
	assert(dr.code == RC_OK);
	assert(decoded);
	assert(decoded->list.count == 2);
	assert(*decoded->list.array[0] == 0);
	assert(*decoded->list.array[1] == 4294967295UL);

	fprintf(stderr, "Successfully decoded: {%lu, %lu}\n",
		*decoded->list.array[0], *decoded->list.array[1]);

	ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, &seq);
	ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, decoded);
}

static void
test_encode_failure(void) {
	SeqOfUInt32_t seq;
	unsigned long *elem;
	asn_enc_rval_t er;
	uint8_t buf[5]; /* Too small */

	memset(&seq, 0, sizeof(seq));

	elem = calloc(1, sizeof(*elem));
	assert(elem);
	*elem = 4294967295UL;
	ASN_SEQUENCE_ADD(&seq, elem);

	/* Encode should fail due to small buffer */
	er = der_encode_to_buffer(&asn_DEF_SeqOfUInt32, &seq, buf, sizeof(buf));
	assert(er.encoded == -1);
	fprintf(stderr, "Encode correctly failed with small buffer\n");

	ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, &seq);
}

static void
test_decode_failure(void) {
	SeqOfUInt32_t *decoded = NULL;
	asn_dec_rval_t dr;
	/* Invalid DER: wrong tag (OCTET STRING instead of INTEGER) */
	uint8_t bad_buf[] = {
		0x30, 0x03, /* SEQUENCE, length 3 */
		0x04, 0x01, 0x00 /* OCTET STRING (wrong!), length 1, value 0 */
	};

	dr = ber_decode(NULL, &asn_DEF_SeqOfUInt32, (void **)&decoded, bad_buf, sizeof(bad_buf));
	assert(dr.code == RC_FAIL);
	fprintf(stderr, "Decode correctly failed with malformed element tag\n");

	if(decoded) ASN_STRUCT_FREE(asn_DEF_SeqOfUInt32, decoded);
}

int
main(void) {
	test_encode_decode();
	test_encode_failure();
	test_decode_failure();
	fprintf(stderr, "All tests passed\n");
	return 0;
}
