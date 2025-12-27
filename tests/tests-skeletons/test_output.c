#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <asn_internal.h>
#include <xer_encoder.h>
#include <INTEGER.h>
#include <IA5String.h>
#include <constr_CHOICE.h>
#include <constr_SEQUENCE.h>

typedef struct TestChoice {
    enum {
        TestChoice_PR_NOTHING,
        TestChoice_PR_intValue,
        TestChoice_PR_strValue
    } present;
    union TestChoice_u {
        INTEGER_t intValue;
        IA5String_t strValue;
    } choice;
} TestChoice_t;

typedef struct TestContainer {
    TestChoice_t myChoice;
} TestContainer_t;

static asn_TYPE_member_t asn_MBR_TestChoice_1[] = {
    { .flags = 0, .name = "intValue", .type = &asn_DEF_INTEGER, .memb_offset = offsetof(TestChoice_t, choice.intValue), },
    { .flags = 0, .name = "strValue", .type = &asn_DEF_IA5String, .memb_offset = offsetof(TestChoice_t, choice.strValue), },
};
static const asn_TYPE_tag2member_t asn_MAP_TestChoice_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 0 },
    { (ASN_TAG_CLASS_UNIVERSAL | (22 << 2)), 1, 0, 0 }
};
static asn_CHOICE_specifics_t asn_SPC_TestChoice_specs_1 = {
    .struct_size = sizeof(TestChoice_t), .ctx_offset = 0,
    .pres_offset = offsetof(TestChoice_t, present),
    .pres_size = sizeof(((TestChoice_t *)0)->present),
    .tag2el = asn_MAP_TestChoice_tag2el_1, .tag2el_count = 2,
    .to_canonical_order = NULL, .from_canonical_order = NULL, .ext_start = -1
};
static const ber_tlv_tag_t asn_DEF_TestChoice_tags_1[] = { (ASN_TAG_CLASS_CONTEXT | (0 << 2)) };
asn_TYPE_descriptor_t asn_DEF_TestChoice = {
    .name = "TestChoice", .xml_tag = "TestChoice", .op = &asn_OP_CHOICE,
    .tags = asn_DEF_TestChoice_tags_1, .tags_count = 1,
    .elements = asn_MBR_TestChoice_1, .elements_count = 2,
    .specifics = &asn_SPC_TestChoice_specs_1
};

static asn_TYPE_member_t asn_MBR_TestContainer_1[] = {
    { .flags = 0, .name = "myChoice", .type = &asn_DEF_TestChoice, .memb_offset = offsetof(TestContainer_t, myChoice), },
};
static const ber_tlv_tag_t asn_DEF_TestContainer_tags_1[] = { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)) };
static const asn_TYPE_tag2member_t asn_MAP_TestContainer_tag2el_1[] = { { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } };
static asn_SEQUENCE_specifics_t asn_SPC_TestContainer_specs_1 = {
    .struct_size = sizeof(TestContainer_t), .ctx_offset = 0,
    .tag2el = asn_MAP_TestContainer_tag2el_1, .tag2el_count = 1,
};
asn_TYPE_descriptor_t asn_DEF_TestContainer = {
    .name = "TestContainer", .xml_tag = "TestContainer", .op = &asn_OP_SEQUENCE,
    .tags = asn_DEF_TestContainer_tags_1, .tags_count = 1,
    .elements = asn_MBR_TestContainer_1, .elements_count = 1,
    .specifics = &asn_SPC_TestContainer_specs_1
};

int main() {
    TestContainer_t container;
    memset(&container, 0, sizeof(container));
    container.myChoice.present = TestChoice_PR_intValue;
    asn_long2INTEGER(&container.myChoice.choice.intValue, 42);
    xer_fprint(stdout, &asn_DEF_TestContainer, &container);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &container.myChoice.choice.intValue);
    return 0;
}
