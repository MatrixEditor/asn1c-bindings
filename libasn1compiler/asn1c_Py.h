#ifndef ASN1_COMPILER_LANDUAGE_PY_H
#define ASN1_COMPILER_LANDUAGE_PY_H

#include "asn1c_internal.h"
#include "asn1c_lang.h"

int asn1c_lang_Py_type_SEQUENCE(arg_t *);
int asn1c_lang_Py_type_CHOICE(arg_t *);
int asn1c_lang_Py_type_SIMPLE_TYPE(arg_t *);
int asn1c_lang_Py_type_SEQ_OF(arg_t *);

// int asn1c_lang_PyStub_type_SEQUENCE(arg_t *);
// int asn1c_lang_PyStub_type_CHOICE(arg_t *);
// int asn1c_lang_PyStub_type_SIMPLE_TYPE(arg_t *);

#endif /* ASN1_COMPILER_LANDUAGE_PY_H */