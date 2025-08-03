## Setup C sources

```bash
asn1c -M exmaple_mod._example_mod -D ./src/generated -no-gen-example -gen-python -no-gen-autotools  example.asn
```

## Install Extension

```bash
pip install -v .
```

## Examples:

Examples are placed in the test folder.