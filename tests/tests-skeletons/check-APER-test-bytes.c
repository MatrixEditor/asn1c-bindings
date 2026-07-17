/*
 * Quick helper to compute APER wire bytes for NativeEnumerated extension cases.
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include <NativeEnumerated.h>
#include <per_support.h>
#include <aper_support.h>

/* base: E ::= ENUMERATED { ee1(0), ee2(1), ... } */
static const asn_INTEGER_enum_map_t base_value2enum[] = {
    { 0, 3, "ee1" },
    { 1, 3, "ee2" }
};
static const unsigned int base_enum2value[] = { 0, 1 };
static const asn_INTEGER_specifics_t base_specs = {
    base_value2enum, base_enum2value, 2, 3, 1, 0, 0
};

static const asn_per_constraints_t ext_constraints = {
    { APC_CONSTRAINED | APC_EXTENSIBLE, 1, 1, 0, 1 },
    { APC_UNCONSTRAINED, -1, -1, 0, 0 },
    0, 0
};

static void encode_marker(long marker, const char *label) {
    asn_TYPE_descriptor_t td = asn_DEF_NativeEnumerated;
    asn_per_outp_t po;
    asn_enc_rval_t er;
    size_t i, nbytes;
    uint8_t buf[16];

    td.specifics = &base_specs;
    memset(&po, 0, sizeof(po));
    po.buffer = po.tmpspace;
    po.nbits = 8 * sizeof(po.tmpspace);

    er = NativeEnumerated_encode_aper(&td, &ext_constraints, &marker, &po);
    if(er.encoded < 0) {
        printf("%s (marker=%ld): ENCODE FAILED\n", label, marker);
        return;
    }
    nbytes = (po.buffer - po.tmpspace) + (po.nboff ? 1 : 0);
    printf("%s (marker=%ld): %zu bytes:", label, marker, nbytes);
    for(i = 0; i < nbytes; i++) printf(" 0x%02x", po.tmpspace[i]);
    printf("\n");
}

static void decode_wire(const uint8_t *wire, size_t nbytes, const char *label) {
    asn_TYPE_descriptor_t td = asn_DEF_NativeEnumerated;
    asn_per_data_t pd;
    asn_dec_rval_t rv;
    long value = -1;
    long *vp = &value;

    td.specifics = &base_specs;
    memset(&pd, 0, sizeof(pd));
    pd.buffer = wire;
    pd.nbits = 8 * nbytes;

    rv = NativeEnumerated_decode_aper(NULL, &td, &ext_constraints, (void**)&vp, &pd);
    printf("%s: code=%d value=%ld\n", label, (int)rv.code, value);
}

int main(void) {
    /* Test encoding of markers (unknown extension values) */
    encode_marker(LONG_MAX,     "index 0");
    encode_marker(LONG_MAX - 1, "index 1");
    encode_marker(LONG_MAX - 3, "index 3");
    encode_marker(LONG_MAX - 63, "index 63");
    encode_marker(LONG_MAX - 64, "index 64");
    encode_marker(LONG_MAX - 65535, "index 65535");

    /* Test decoding of known wire bytes */
    {
        static const uint8_t w0[] = { 0x80 }; /* ext + nsnnwn(0) */
        static const uint8_t w1[] = { 0x81 }; /* ext + nsnnwn(1) */
        static const uint8_t w3[] = { 0x83 }; /* ext + nsnnwn(3) */
        decode_wire(w0, 1, "decode 0x80 (index 0)");
        decode_wire(w1, 1, "decode 0x81 (index 1)");
        decode_wire(w3, 1, "decode 0x83 (index 3)");
    }
    return 0;
}
