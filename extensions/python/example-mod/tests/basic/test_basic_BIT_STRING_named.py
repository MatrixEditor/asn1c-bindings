import pytest

from bitarray import bitarray
from example_mod._example_mod import ExampleNamedBitString


def test_named_BIT_STRING_accepts_integer_and_is_valid():
    # ASN.1 BIT STRING type that can be represented by:
    #     - An integer value
    #     - A NamedBitString.VALUES enum member
    #     - Any object convertible to bytes (internally stored as bitarray)
    obj = ExampleNamedBitString()
    obj.value = 1
    assert obj.is_valid()
    assert obj.value == 1


def test_named_BIT_STRING_returns_same_integer_value():
    obj = ExampleNamedBitString()
    obj.value = 1
    # conversion must return the same value
    assert obj.value == 1
    # Although, the object is returned as an instance of enum.IntFlag,
    # the value is still an integer.
    assert isinstance(obj.value, int)


def test_named_BIT_STRING_decodes_correctly_from_der():
    raw_data = b"\x03\x02\x00\x01"
    parsed = ExampleNamedBitString.ber_decode(raw_data)
    assert parsed.value == 1


def test_named_BIT_STRING_encodes_correctly_to_der():
    obj = ExampleNamedBitString()
    obj.value = 1
    assert obj.ber_encode() == b"\x03\x02\x00\x01"


def test_named_BIT_STRING_invalid_integer_value():
    # Assigning an integer not mapped to any NamedBitString.VALUES WILL NOT raise an
    # exception. The enum.IntFlag instance will only store the integer value.
    obj = ExampleNamedBitString()
    obj.value = 999
    assert obj.is_valid()
    assert obj.value == 999


def test_named_BIT_STRING_rejects_unsupported_type():
    obj = ExampleNamedBitString()
    with pytest.raises(ValueError):
        obj.value = "invalid"
    with pytest.raises(TypeError):
        obj.value = {"key": "value"}


def test_named_BIT_STRING_encoding_fails_without_valid_value():
    obj = ExampleNamedBitString()
    with pytest.raises(ValueError):
        obj.ber_encode()


def test_named_BIT_STRING_decoding_fails_with_malformed_data():
    # Missing a byte for the bitstring content
    raw_data = b"\x03\x02\x00"
    with pytest.raises(ValueError):
        ExampleNamedBitString.ber_decode(raw_data)


def test_named_BIT_STRING_decoding_fails_with_wrong_tag():
    # Tag 0x04 is OCTET STRING, not BIT STRING
    raw_data = b"\x04\x02\x00\x01"
    with pytest.raises(ValueError):
        ExampleNamedBitString.ber_decode(raw_data)
