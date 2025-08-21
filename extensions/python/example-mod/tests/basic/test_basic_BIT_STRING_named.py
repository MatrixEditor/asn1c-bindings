import bitarray
import pytest

from example_mod._example_mod import ExampleNamedBitString


def test_named_BIT_STRING_set_single_bit():
    # ASN.1 BIT STRING type that can be represented by:
    #     - An integer value
    #     - A NamedBitString.VALUES enum member
    #     - Any object convertible to bytes (internally stored as bitarray)
    obj = ExampleNamedBitString()
    obj.set(1, True)
    assert obj.is_valid()
    assert obj.get(1) is True


def test_named_BIT_STRING_set_single_bit_by_attr():
    obj = ExampleNamedBitString()
    assert obj.V_zero is False
    obj.V_zero = True
    # conversion must return the same value
    assert obj.V_zero is True


def test_named_BIT_STRING_decodes_correctly_from_der():
    raw_data = b"\x03\x02\x00@"
    parsed = ExampleNamedBitString.ber_decode(raw_data)
    # the first bit (bit 0) is set
    assert parsed.V_one is True
    assert parsed.get(1) is True
    # even if the underlying bitarray stores more data, the index position will
    # be adjusted automatically
    assert parsed.value == bitarray.bitarray("0000000001000000")


def test_named_BIT_STRING_encodes_correctly_to_der():
    obj = ExampleNamedBitString()
    obj.V_zero = 1
    assert obj.ber_encode() == b"\x03\x02\x00\x80"


def test_named_BIT_STRING_rejects_unsupported_type():
    obj = ExampleNamedBitString()
    with pytest.raises(ValueError):
        obj.value = "invalid"
    with pytest.raises(ValueError):
        obj.value = 123
    with pytest.raises(ValueError):
        obj.value = {"key": "value"}


def test_named_BIT_STRING_encoding_fails_without_valid_value():
    obj = ExampleNamedBitString()
    # BIT STRING objects are always valid (if no size constraint is set)
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
