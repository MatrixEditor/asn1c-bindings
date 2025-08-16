#ifndef _ASN1C_PYIMPORT_H_
#define _ASN1C_PYIMPORT_H_

#include "asn1c_internal.h"

typedef struct asn1c_pyimportdef_s {
    char *module_name;
    char *pyimport_path;

    TQ_ENTRY(struct asn1c_pyimportdef_s) next;
} asn1c_pyimportdef_t;

typedef struct asn1c_pyimports_s {
    TQ_HEAD(asn1c_pyimportdef_t) imports;
} asn1c_pyimports_t;

asn1c_pyimports_t *asn1c_pyimports_new(void);
void asn1c_pyimports_free(asn1c_pyimports_t *pyimports);

#endif /* _ASN1C_PYIMPORT_H_ */