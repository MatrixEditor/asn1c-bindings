import pytest

from bitarray import bitarray
from example_mod._example_mod import ExampleBitString


# Generated class: ExampleBitString
# type: bitarray.bitarray

# This type relies on the external library 'bitarray' for efficiency
# and portability. You can assign any value that can be converted into
# a bytes object. The object will return a new bitarray object
# each time you access it.

# NOTE: the bitarray object ALWAYS uses little endian to represent its data.


def test_BIT_STRING_accepts_valid_bitarray():
    # ASN.1 BIT STRING type backed by the 'bitarray' library.
    bit_data = bitarray(b"\x01\x02\x03", endian="little")
    obj = ExampleBitString()
    obj.value = bit_data
    assert obj.is_valid()


def test_BIT_STRING_rejects_non_convertible_types():
    obj = ExampleBitString()
    with pytest.raises(ValueError):
        obj.value = "not a bitarray"
    with pytest.raises(TypeError):
        obj.value = {"a": 1}  # dict


def test_BIT_STRING_returns_equivalent_bitarray():
    # - Returns a bitarray equivalent to the one assigned.
    # - The equality check works even if it's a new object instance internally.
    bit_data = bitarray(b"\x01\x02\x03", endian="little")
    obj = ExampleBitString()
    obj.value = bit_data
    assert obj.value == bit_data
    assert isinstance(obj.value, bitarray)


def test_BIT_STRING_decodes_correctly():
    bit_data = bitarray(b"\x01\x02\x03", endian="little")
    raw_data = b"\x03\x04\x00\x01\x02\x03"
    parsed = ExampleBitString.ber_decode(raw_data)
    assert parsed.value == bit_data


def test_BIT_STRING_encodes_correctly():
    bit_data = bitarray(b"\x01\x02\x03", endian="little")
    raw_data = b"\x03\x04\x00\x01\x02\x03"
    obj = ExampleBitString()
    obj.value = bit_data
    assert obj.ber_encode() == raw_data
