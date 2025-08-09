import pytest

from example_mod._example_mod import Signed8


# Generated class: Signed8
# type: int (8 bits)
# constraints: -127 <= value <= 127

def test_INTEGER_allows_values_within_range():
    # Signed8 represents a signed 8-bit integer with constraints:
    #   -127 <= value <= 127
    obj = Signed8()
    obj.value = 127
    assert obj.is_valid()


def test_INTEGER_rejects_values_outside_range():
    # Verifies that values outside the allowed range (-127..127)
    # are considered invalid.
    #
    # - Assigning a value beyond the limit does not immediately raise an error.
    # - is_valid() detects the constraint violation.
    # - check_constraints() explicitly raises ValueError if the value is out of
    #   range.
    obj = Signed8()
    obj.value = 128  # out of range
    assert not obj.is_valid()
    with pytest.raises(ValueError):
        obj.check_constraints()


def test_INTEGER_init_with_positional():
    # Instantiating Signed8 with positional arguments
    obj = Signed8(127)
    assert obj.value == 127


def test_INTEGER_init_with_kwargs():
    # Instantiating Signed8 with keyword arguments
    obj = Signed8(value=127)
    assert obj.value == 127


# --- Representation Tests ---
def test_INTEGER_repr_and_str_behavior():
    # - repr(obj) shows the class name only.
    # - str(obj) shows the numeric value when set.
    # - str(Signed8()) shows "<Signed8>" when no value is set.
    obj = Signed8()
    obj.value = 127
    assert repr(obj) == "<Signed8>"
    assert str(obj) == "127"
    assert str(Signed8()) == "<Signed8>"


# --- Encoding Tests ---
def test_INTEGER_encodes_correctly():
    obj = Signed8(10)
    assert obj.encode() == b"\x02\x01\n"


def test_INTEGER_raises_on_encoding_invalid_value():
    # Encoding a value outside the allowed range (-127..127) raises ValueError.
    with pytest.raises(ValueError):
        Signed8(1000).encode()


# --- Decoding Tests ---
def test_INTEGER_decodes_correctly():
    raw_data = b"\x02\x01\n"
    parsed = Signed8.decode(raw_data)
    assert parsed.value == 10


def test_INTEGER_raises_on_decoding_invalid_data():
    raw_data = b"\x02\x02\x03\xe8"  # encoded value 1000 (out of range)
    with pytest.raises(ValueError):
        Signed8.decode(raw_data)
