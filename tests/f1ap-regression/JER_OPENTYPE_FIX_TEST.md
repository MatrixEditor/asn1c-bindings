# JER OPEN TYPE Fix Test

This document describes how to test the fix for the JER encoder/decoder inconsistency
issue where OPEN TYPE values in direct type mode (no CHOICE wrapper) were causing
"Unexpected JSON key in SEQUENCE" errors during decoding.

## Problem Description

The OPEN_TYPE_jer_get decoder was always expecting a CHOICE-style key wrapper even
for direct type mode (no CHOICE wrapper). This caused decoding failures when the OPEN
TYPE value was a direct SEQUENCE or SEQUENCE OF without a CHOICE wrapper.

## Fix Applied

Modified `/home/runner/work/asn1c/asn1c/skeletons/OPEN_TYPE_jer.c` in the `OPEN_TYPE_jer_get` function
to only parse and skip the key+colon wrapper when in CHOICE wrapper mode 
(`elm->type->elements_count > 0`). For direct type mode, it now directly decodes the
value without expecting a key wrapper.

## How to Test

### Automated Test (Recommended)

1. Build the project:
   ```bash
   cd /home/runner/work/asn1c/asn1c
   make clean
   make
   ```

2. Run the test suite:
   ```bash
   make check
   ```

### Manual Test with F1AP

1. Generate F1AP code:
   ```bash
   cd /home/runner/work/asn1c/asn1c/tests/f1ap-regression
   ../../asn1c/asn1c -S ../../skeletons -fcompound-names -findirect-choice F1AP-16.7.0.asn
   make -f converter-example.mk
   ```

2. Create a test JER file (test.jer):
   ```json
   {
       "initiatingMessage": {
           "procedureCode": 0,
           "criticality": "reject",
           "value": {
               "protocolIEs": []
           }
       }
   }
   ```

3. Test decoding (should succeed without "Unexpected JSON key" error):
   ```bash
   ./converter-example -p F1AP-PDU -ijer -otext test.jer
   ```

Before the fix, this would fail with:
```
Unexpected JSON key in SEQUENCE [[
    ]
```

After the fix, it should decode successfully.

## Related Issue

This fix addresses the issue reported in mouse07410/asn1c#455 where JER encoding
produced valid JSON but the decoder could not parse it back due to the inconsistency
in OPEN TYPE handling.
