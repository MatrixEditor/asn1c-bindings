import pytest

from bitarray import bitarray

from example_mod._example_mod import (
    NamedSigned8,
    Signed8,
    ExampleBoolean,
    ExampleBitString,
    ExampleOctetString,
    ExampleNull,
    ExampleObjectIdentifier,
    ExampleRelativeOid,
)


def test_named_integer():
    # Generated class: NamedSigned8
    # type: int (8 bits) represented by an enum
    # enum: first = -1, second = 0, third = 1
    # enum type: NamedSigned8.VALUES

    # The object can be instantiated without any arguments,
    # its state will invalid.
    obj = NamedSigned8()
    assert not obj.is_valid()

    # You can assign either the enum value directly or the
    # enum reference
    obj.value = -1
    assert obj.value == NamedSigned8.VALUES.NamedSigned8_first
    # use VALUES directly
    obj.value = NamedSigned8.VALUES.NamedSigned8_first
    assert obj.value == NamedSigned8.VALUES.NamedSigned8_first

    # Encoding and decoding is as simple as calling two methods. The
    # internal binding will do the rest and take care of the encoding.

    # DER encode:
    raw_data = b"\x02\x01\x01"
    obj.value = 1
    assert obj.encode() == raw_data

    # BER decode
    parsed = NamedSigned8.decode(raw_data)
    assert parsed.value == NamedSigned8.VALUES.NamedSigned8_third

    # check constrains (valid for other objects)
    assert obj.is_valid()
    obj.check_constraints()


def test_basic_integer__size_contraint():
    # Generated class: Signed8
    # type: int (8 bits)
    # constraints: -127 <= value <= 127

    # instantiation same as above, however, the value
    # is limited to -127 <= value <= 127
    obj = Signed8()
    obj.value = 127
    assert obj.is_valid()

    # repr() and str() behave different
    assert repr(obj) == "<Signed8>"
    assert str(obj) == "127"
    assert str(Signed8()) == "<Signed8>"

    # check constrains will be called under the hood when
    # is_valid is called. It is NOT called when applying a
    # new value.
    obj.value = 128
    assert not obj.is_valid()
    with pytest.raises(ValueError):
        # raises an error because the value is out of range
        obj.check_constraints()

    # Encode/Decode
    raw_data = b"\x02\x01\n"
    obj.value = 10
    assert obj.encode() == raw_data

    parsed = Signed8.decode(raw_data)
    assert parsed.value == 10

    # invalid values shouldn't be allowed
    with pytest.raises(ValueError):
        _ = Signed8(1000).encode()

    # as well as invalid raw data
    raw_data = b"\x02\x02\x03\xe8"
    with pytest.raises(ValueError):
        _ = Signed8.decode(raw_data)


def test_basic_boolean():
    # Generated class: ExampleBoolean
    # type: bool
    # constraints: value = True | False
    obj = ExampleBoolean()
    obj.value = True
    assert obj.is_valid()

    # Value will be casted to boolean first
    obj.value = 1000
    assert obj.value is True


def test_basic_bit_string():
    # Generated class: ExampleBitString
    # type: bitarray.bitarray

    # This type relies on the external library 'bitarray' for efficiency
    # and portability. You can assign any value that can be converted into
    # a bytes object. The object will return a new bitarray object
    # each time you access it.
    bit_data = bitarray(b"\x01\x02\x03")
    obj = ExampleBitString()
    obj.value = bit_data
    assert obj.is_valid()

    # conversion must return the same value
    assert obj.value == bit_data

    # encoding/decoding scheme is the same as above
    raw_data = b"\x03\x04\x00\x01\x02\x03"
    parsed = ExampleBitString.decode(raw_data)
    assert parsed.value == bit_data
    assert parsed.encode() == raw_data


def test_basic_octet_string():  # bytes
    # Generated class: ExampleOctetString
    # type: bytes
    # constraints: 1 <= size <= 32

    # This type just stores raw data of bytes object
    obj = ExampleOctetString()
    obj.value = b"\x01\x02\x03"
    assert obj.is_valid()
    assert obj.value.hex() == "010203"

    # size constaint
    obj.value = b""
    assert not obj.is_valid()

    obj.value = b"0" * 33
    assert not obj.is_valid()

    # encoding/decoding scheme is the same as above
    raw_data = b"\x04\x03\x01\x02\x03"
    parsed = ExampleOctetString.decode(raw_data)
    assert parsed.value == b"\x01\x02\x03"
    assert parsed.encode() == raw_data


def test_basic_null():
    # Generated class: ExampleNull
    # type: None
    obj = ExampleNull()
    obj.value = None
    assert obj.is_valid()

    # encoding and decoding nothing special here. This object
    # is just a wrapper for None
    raw_data = b"\x05\x00"
    parsed = ExampleNull.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.encode() == raw_data


def test_basic_object_identifier():
    # Generated class: ExampleObjectIdentifier
    # type: str
    obj = ExampleObjectIdentifier()
    obj.value = "1.2.3"
    assert obj.value == "1.2.3"

    # encoding and decoding same as for the previous types.
    raw_data = b"\x06\x02\x01\x02"
    parsed = ExampleObjectIdentifier.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.encode() == raw_data
    assert parsed.value == "0.1.2"


def test_basic_relative_oid():
    # Generated class: ExampleRelativeOid
    # type: str
    obj = ExampleRelativeOid()
    obj.value = "1.2.3"
    assert obj.value == "1.2.3"

    # encoding and decoding same as for the previous types.
    raw_data = b"\r\x03\x01\x02\x03"
    parsed = ExampleRelativeOid.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.encode() == raw_data
    assert parsed.value == "1.2.3"

    with pytest.raises(ValueError):
        # intentionally raises an error upon invalid value type
        # needed for conversion
        obj.value = None
