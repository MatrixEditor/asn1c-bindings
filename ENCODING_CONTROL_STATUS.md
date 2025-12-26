# ENCODING-CONTROL Support Status

## Current Implementation (Phases 1-3)

The asn1c compiler currently provides basic recognition of ENCODING-CONTROL directives as specified in ASN.1 standards (X.693 Annex G, X.696).

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

4. **Test Coverage**: Tests verify that ENCODING-CONTROL sections are accepted without warnings

### What's Not Yet Implemented (Phases 4-6)

The following advanced features require more extensive changes and are planned for future implementation:

#### Phase 4: Body Parsing
- Parse individual encoding instructions within the ENCODING-CONTROL body
- Grammar rules for:
  ```asn1
  ENCODING-CONTROL XER
      fieldName OCTET STRING ::= hexadecimal
      otherField OCTET STRING ::= base64
  END
  ```
- Store encoding instructions for later linkage

#### Phase 5: Type Linking  
- Create `libasn1compiler/asn1c_encoding.c` module
- Implement `asn1c_apply_encoding_controls()` to link directives to types
- Apply encoding preferences from ENCODING-CONTROL to matching type definitions

#### Phase 6: Custom Encoder Generation
- Generate custom XER encoders based on encoding controls
- Emit type-specific encoding functions when encoding controls are present
- Support hexadecimal, base64, and UTF-8 encoding formats

## Implementation Notes

### Why Phases 4-6 Are Deferred

1. **Lexer Complexity**: The current `encoding_control` lexer state consumes the body as opaque text. Proper parsing would require:
   - New lexer tokens for encoding instructions
   - Modified state machine to handle `::=` and format names within the body
   - Careful handling of END keyword detection

2. **Grammar Changes**: Adding grammar rules for encoding instructions requires:
   - New non-terminals for instruction lists
   - Type reference parsing within encoding control context
   - Format name recognition and validation

3. **Compiler Integration**: Linking and code generation requires:
   - New compilation phase to apply encoding controls
   - Modified code generator to emit custom encoders
   - Runtime skeleton support for additional encoding formats

### Minimal Implementation Strategy

For users who need ENCODING-CONTROL support now, the recommended approach is:

1. Use the existing recognition to document encoding preferences in ASN.1
2. Manually implement custom encoders in generated code if needed
3. Comment encoding directives to guide manual implementation

### Future Work

When implementing Phases 4-6, consider:

1. **Standards Compliance**: Follow X.693 Annex G and X.696 specifications
2. **Backward Compatibility**: Ensure existing ASN.1 modules compile unchanged
3. **Test Coverage**: Add comprehensive tests for all encoding formats
4. **Documentation**: Update user guide with encoding control examples

## Testing

Current tests:
- `tests/tests-asn1c-compiler/174-encoding-control-OK.asn1` - Basic recognition
- `tests/tests-asn1c-compiler/175-encoding-control-body-OK.asn1` - Body with comments

To run tests:
```bash
cd tests/tests-asn1c-compiler
./check-parsing.sh
```

## References

- X.693: ASN.1 encoding rules: XML Encoding Rules (XER)
- X.693 Annex G: ENCODING-CONTROL notation
- X.696: ASN.1 encoding rules: OER specification
