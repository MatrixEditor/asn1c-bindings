# ENCODING-CONTROL Support Status

## Current Implementation (All Phases Complete! ✓)

The asn1c compiler now provides complete support for ENCODING-CONTROL directives as specified in ASN.1 standards (X.693 Annex G, X.696), including body parsing, type linking, and custom XER encoder generation.

### What Works Now

1. **Recognition**: The parser recognizes ENCODING-CONTROL sections and reports them without errors
   ```asn1
   ENCODING-CONTROL XER
   END
   ```

2. **Data Structure**: The ASN.1 expression structure (`asn1p_expr.h`) includes fields for storing encoding control information:
   - `encoding_type`: Type of encoding (hexadecimal, base64, utf8, etc.)
   - `encoding_reference`: Reference name (e.g., "XER")

3. **Error Reporting**: Recognition messages include filename and line number for debugging

4. **Test Coverage**: Comprehensive tests verify all phases of ENCODING-CONTROL support

5. **Phase 4: Body Parsing** ✓ - Parse individual encoding instructions within the ENCODING-CONTROL body:
   ```asn1
   ENCODING-CONTROL XER
       fieldName OCTET STRING ::= hexadecimal
       otherField OCTET STRING ::= utf8
   END
   ```

6. **Phase 5: Type Linking** ✓ - Encoding controls are automatically linked to type definitions:
   - `libasn1compiler/asn1c_encoding.c` - Full implementation
   - `libasn1compiler/asn1c_encoding.h` - API definition
   - `asn1c_apply_encoding_controls()` matches directives to types by name

7. **Phase 6: Custom Encoder Generation** ✓ - Generate custom XER encoders based on encoding controls:
   - Custom `TypeName_encode_xer()` functions
   - Custom `TypeName_decode_xer()` functions
   - Custom `asn_OP_TypeName` operation structures
   - Support for hexadecimal, utf8, and base64 (default) encoding formats

## Example Usage

### Input ASN.1 Module

```asn1
TestModule DEFINITIONS AUTOMATIC TAGS ::= BEGIN

Message ::= SEQUENCE {
    binaryData BinaryData,
    textData   TextData
}

BinaryData ::= OCTET STRING
TextData   ::= OCTET STRING

ENCODING-CONTROL XER
    BinaryData OCTET STRING ::= hexadecimal
    TextData   OCTET STRING ::= utf8
END

END
```

### Compilation

```bash
asn1c -no-gen-example TestModule.asn1
```

Output:
```
NOTE: ENCODING-CONTROL XER at TestModule.asn1:12 with 2 directive(s)
NOTE: Applied 2 encoding control directive(s) in module TestModule
```

### Generated Code Highlights

For `BinaryData` (hexadecimal encoding):
```c
/* Custom XER encoder per ENCODING-CONTROL directive */
static asn_enc_rval_t
BinaryData_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr,
    int ilevel, enum xer_encoder_flags_e flags,
    asn_app_consume_bytes_f *cb, void *app_key) {
    /* Hexadecimal encoding per ENCODING-CONTROL */
    const char * const h2c = "0123456789ABCDEF";
    char *hexbuf = (char *)MALLOC(st->size * 2 + 1);
    for(i = 0; i < st->size; i++) {
        hexbuf[i*2] = h2c[(st->buf[i] >> 4) & 0x0F];
        hexbuf[i*2 + 1] = h2c[st->buf[i] & 0x0F];
    }
    ...
}

/*
 * Custom operation structure per ENCODING-CONTROL directive:
 * Format: hexadecimal
 * Reference: XER
 */
asn_TYPE_operation_t asn_OP_BinaryData = {
    OCTET_STRING_free,
    OCTET_STRING_print,
    OCTET_STRING_compare,
    OCTET_STRING_copy,
    OCTET_STRING_decode_ber,
    OCTET_STRING_encode_der,
    BinaryData_decode_xer,  /* Custom per ENCODING-CONTROL */
    BinaryData_encode_xer,  /* Custom per ENCODING-CONTROL */
    ...
};
```

For `TextData` (UTF-8 encoding):
```c
/* Custom XER encoder per ENCODING-CONTROL directive */
static asn_enc_rval_t
TextData_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr,
    int ilevel, enum xer_encoder_flags_e flags,
    asn_app_consume_bytes_f *cb, void *app_key) {
    /* UTF-8 text encoding per ENCODING-CONTROL */
    return OCTET_STRING_encode_xer_utf8(td, sptr, ilevel, flags, cb, app_key);
}
```

### Runtime Behavior

When encoding data with XER:
- `BinaryData` containing `{0xDE, 0xAD, 0xBE, 0xEF}` encodes as `<binaryData>DEADBEEF</binaryData>`
- `TextData` containing "Hello World" encodes as `<textData>Hello World</textData>`
- Types without encoding controls use default Base64 encoding

## Implementation Details

### Code Generation Strategy

1. **Detection**: `type_needs_custom_xer_encoder()` checks if a type has encoding controls
2. **Emission**: `emit_custom_xer_encoder()` and `emit_custom_xer_decoder()` generate custom functions
3. **Operation Structure**: `emit_custom_operation_structure()` creates custom `asn_TYPE_operation_t`
4. **Integration**: `emit_type_DEF()` references custom operations for types with controls

### Encoding Formats

| Format      | XER Output Example       | Use Case                    |
|-------------|--------------------------|------------------------------|
| hexadecimal | `<data>DEADBEEF</data>` | Binary data, hashes, keys    |
| utf8        | `<data>Hello</data>`     | Text strings, human-readable |
| base64      | `<data>3q2+7w==</data>` | Default, compact binary      |

### Files Modified

**Code Generation:**
- `libasn1compiler/asn1c_C.c` - Custom encoder/decoder generation

**Runtime Support:**
- Existing skeleton functions in `skeletons/OCTET_STRING_xer.c`:
  - `OCTET_STRING_encode_xer_utf8()` - UTF-8 encoding
  - `OCTET_STRING_decode_xer_hex()` - Hexadecimal decoding
  - `OCTET_STRING_decode_xer_utf8()` - UTF-8 decoding

**Tests:**
- `tests/tests-asn1c-compiler/174-encoding-control-OK.asn1` - Basic recognition
- `tests/tests-asn1c-compiler/175-encoding-control-body-OK.asn1` - Body with comments
- `tests/tests-asn1c-compiler/176-encoding-control-applied-OK.asn1` - Type linking
- `tests/tests-asn1c-compiler/177-encoding-control-codegen-OK.asn1` - Code generation

## Testing

Run the compiler test suite:
```bash
cd tests/tests-asn1c-compiler
./check-parsing.sh
```

All ENCODING-CONTROL tests should pass:
- ✅ 174: Basic ENCODING-CONTROL recognition
- ✅ 175: ENCODING-CONTROL with body directives
- ✅ 176: Encoding controls linked to types
- ✅ 177: Custom XER encoders generated

## Limitations and Future Work

### Current Limitations

1. **OCTET STRING Only**: Custom encoders are currently only generated for OCTET STRING types
2. **XER Only**: Only XER encoding is customizable (BER, PER, OER, JER use defaults)
3. **Three Formats**: Only hexadecimal, utf8, and base64 formats are supported

### Future Enhancements

1. **Additional Types**: Extend to BIT STRING and other string types
2. **More Formats**: Support binary, decimal, and other X.693 formats
3. **Other Encodings**: Apply controls to PER, OER, and JER encodings
4. **Validation**: Add constraint checking for encoding format compatibility

## References

- X.693: ASN.1 encoding rules: XML Encoding Rules (XER)
- X.693 Annex G: ENCODING-CONTROL notation
- X.696: ASN.1 encoding rules: OER specification
