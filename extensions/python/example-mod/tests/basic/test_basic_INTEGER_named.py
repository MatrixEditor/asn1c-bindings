import pytest

from example_mod._example_mod import NamedSigned8

# Generated class: NamedSigned8
# type: int (8 bits) represented by an enum
# enum: first = -1, second = 0, third = 1
# enum type: NamedSigned8.VALUES

def test_named_INTEGER_rejects_invalid_integer():
    # Assigning an integer not mapped to any enum member won't cause an
    # exception to be raised. Only trying to convert the value back to
    # the Python representation will cause an exception.
    obj = NamedSigned8()
    x = NamedSigned8.ber_decode(b"\x02\x01\x63")
    obj.value = 99
    assert obj.is_valid()
    with pytest.raises(ValueError):
        _ = obj.value


def test_named_INTEGER_rejects_invalid_type():
    # invalid value types will raise an exception immediately
    obj = NamedSigned8()
    with pytest.raises(ValueError):
        obj.value = "invalid"
    with pytest.raises(ValueError):
        obj.value = 1.5
    with pytest.raises(ValueError):
        obj.value = [1]


# --- State Changes ---
def test_named_INTEGER_allows_switching_between_enum_values():
    # You can assign either the enum value directly or the
    # enum reference
    obj = NamedSigned8()
    obj.value = NamedSigned8.VALUES.NamedSigned8_first
    assert obj.value == NamedSigned8.VALUES.NamedSigned8_first
    obj.value = NamedSigned8.VALUES.NamedSigned8_second
    assert obj.value == NamedSigned8.VALUES.NamedSigned8_second


def test_named_INTEGER_is_valid_after_assignment():
    # - An object becomes valid after assigning a valid value.
    # - is_valid() reflects this change immediately.
    obj = NamedSigned8()
    assert not obj.is_valid()
    obj.value = 0
    assert obj.is_valid()


def test_named_INTEGER_round_trip_encoding_decoding():
    # - Encoding an object and then decoding the bytes returns
    #   an equivalent object (value-wise).
    obj = NamedSigned8()
    obj.value = NamedSigned8.VALUES.NamedSigned8_third
    encoded = obj.ber_encode()
    decoded = NamedSigned8.ber_decode(encoded)
    assert decoded.value == obj.value


@pytest.mark.parametrize(
    "enum_val,expected",
    [
        (NamedSigned8.VALUES.NamedSigned8_first, b"\x02\x01\xff"),
        (NamedSigned8.VALUES.NamedSigned8_second, b"\x02\x01\x00"),
        (NamedSigned8.VALUES.NamedSigned8_third, b"\x02\x01\x01"),
    ],
)
def test_named_INTEGER_encoding_of_each_enum_member(
    enum_val: NamedSigned8.VALUES, expected: bytes
):
    obj = NamedSigned8()
    obj.value = enum_val
    assert obj.ber_encode() == expected


def test_named_INTEGER_repr_shows_class_name():
    # repr(obj) returns only the class name in angle brackets.
    obj = NamedSigned8()
    obj.value = NamedSigned8.VALUES.NamedSigned8_second
    assert repr(obj) == "<NamedSigned8>"


def test_named_INTEGER_str_shows_enum_name_when_valid():
    # - Returns str(enum member) if valid.
    # - Returns "<NamedSigned8>" if invalid.
    obj = NamedSigned8()
    assert str(obj) == "<NamedSigned8>"
    obj.value = NamedSigned8.VALUES.NamedSigned8_second
    assert str(obj) == "0"
