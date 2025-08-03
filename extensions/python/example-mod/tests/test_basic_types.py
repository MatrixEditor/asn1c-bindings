import pytest

from example_mod._example_mod import NamedSigned8, Signed8


def test_named_integer():
    # Generated class: NamedSigned8
    # type: signed integer (8 bits)
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
    # type: signed integer (8 bits)
    # constraints: -127 <= value <= 127

    # instantiation same as above, however, the value
    # is limited to -127 <= value <= 127
    obj = Signed8()
    obj.value = 127
    assert obj.is_valid()

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
    raw_data = b'\x02\x02\x03\xe8'
    with pytest.raises(ValueError):
        _ = Signed8.decode(raw_data)
