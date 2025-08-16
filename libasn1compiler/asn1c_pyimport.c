#include "asn1c_pyimport.h"

asn1c_pyimports_t *asn1c_pyimports_new(void) {
    asn1c_pyimports_t *pyimports = calloc(1, sizeof(asn1c_pyimports_t));
    if (pyimports) {
        TQ_INIT(&(pyimports->imports));
    }
    return pyimports;
}

void asn1c_pyimports_free(asn1c_pyimports_t *pyimports) {
    asn1c_pyimportdef_t *def;
    if (pyimports) {
        while ((def = TQ_REMOVE(&(pyimports->imports), next))) {
            free(def->module_name);
            free(def->pyimport_path);
            free(def);
        }
    }
}