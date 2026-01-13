typedef struct asn_TYPE_descriptor_s asn_TYPE_descriptor_t;
struct asn_TYPE_descriptor_s {
    const char *name;
};

#if 0
extern asn_TYPE_descriptor_t asn_DEF_messageClassExtension __attribute__((weak, alias("asn_DEF_messageClassExtension_4")));
#else
static asn_TYPE_descriptor_t asn_DEF_messageClassExtension_4;
static asn_TYPE_descriptor_t asn_DEF_messageClassExtension;
__attribute__((constructor)) static void asn_DEF_messageClassExtension_4_alias_init(void) {
asn_DEF_messageClassExtension = asn_DEF_messageClassExtension_4;
}
#endif

asn_TYPE_descriptor_t asn_DEF_messageClassExtension_4 = {
    .name = "messageClassExtension"
};
