struct asn_TYPE_descriptor_s {
    const char *name;
};
typedef struct asn_TYPE_descriptor_s asn_TYPE_descriptor_t;

static asn_TYPE_descriptor_t asn_DEF_test;

asn_TYPE_descriptor_t asn_DEF_test_1 = { .name = "test" };

void init(void) {
    asn_DEF_test = asn_DEF_test_1;
}
