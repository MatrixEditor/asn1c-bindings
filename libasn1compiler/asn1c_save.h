#ifndef ASN1C_SAVE_H
#define ASN1C_SAVE_H

#include <asn1compiler.h>

/* Check if a typename is in the PDU list (for -pdu=Type) */
int asn1c__pdu_type_lookup(const char *typename);

int asn1c_save_compiled_output(arg_t *arg, const asn1c_datadirs_t *datadirs,
                               int argc, int optc, char **argv);

#endif	/* ASN1C_SAVE_H */
