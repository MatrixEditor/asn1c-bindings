#ifndef ASN1C_SAVE_H
#define ASN1C_SAVE_H

#import <asn1compiler.h>

int asn1c_save_compiled_output(arg_t *arg, const asn1c_datadirs_t *datadirs, int argc,
                               int optc, char **argv);

#endif /* ASN1C_SAVE_H */
