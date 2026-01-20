# JER OPEN TYPE Fix - Summary

## Issue Addressed
Fixed issue #455 where the JER encoder produces non-parseable JSON for OPEN TYPE values in direct type mode.

## Problem Description
The `OPEN_TYPE_jer_get` decoder was always expecting a CHOICE-style JSON key wrapper even for direct type mode (no CHOICE wrapper). This caused "Unexpected JSON key in SEQUENCE" errors when decoding OPEN TYPE values that are direct SEQUENCE or SEQUENCE OF types without a CHOICE wrapper.

### Example of the Issue
For an OPEN TYPE field that contains a direct SEQUENCE (no CHOICE), the JSON should be:
```json
"value": {"field1": 1, "field2": "test"}
```

But the decoder was expecting:
```json
"value": {"someKey": {"field1": 1, "field2": "test"}}
```

This mismatch caused decoding to fail with "Unexpected JSON key" errors.

## Root Cause
In `/home/runner/work/asn1c/asn1c/skeletons/OPEN_TYPE_jer.c`, the `OPEN_TYPE_jer_get` function had code (lines 105-145) that always expected to parse and skip a JSON key+colon before decoding the value. This code should only execute for CHOICE wrapper mode, not for direct type mode.

## Solution
Modified `OPEN_TYPE_jer_get` to conditionally execute the key-parsing logic:
- **CHOICE wrapper mode** (`elm->type->elements_count > 0`): Parse and skip the key+colon
- **Direct type mode** (`elm->type->elements_count == 0`): Skip key-parsing, decode value directly

## Files Modified
1. `/home/runner/work/asn1c/asn1c/skeletons/OPEN_TYPE_jer.c` - Main fix
2. `/home/runner/work/asn1c/asn1c/skeletons/constr_SEQUENCE_OF_jer.c` - Added clarifying comments

## Testing
- Basic ASN.1 compiler tests pass
- Project builds successfully
- Comprehensive test documentation provided in `tests/f1ap-regression/JER_OPENTYPE_FIX_TEST.md`

## Impact
This fix resolves the critical bug where JER encoding produced valid JSON but the decoder could not parse it back, making JER unusable for certain ASN.1 structures (particularly F1AP and similar protocols with OPEN TYPE fields).

## Commits
1. Fix OPEN_TYPE_jer_get to handle direct type mode - Main fix
2. Add test documentation for JER OPEN TYPE fix - Testing docs
3. Add clarifying comment for XMLValueList in SEQUENCE_OF JER encoder - Additional documentation
4. Address code review feedback - improve documentation and comments - Final polish

## Branch
`copilot/fix-jer-encoder-issues`

## Related Issue
Fixes #455
