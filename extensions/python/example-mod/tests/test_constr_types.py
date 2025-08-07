import pytest

from example_mod._example_mod import ExampleChoice, ExampleSequence


# -- CHOICE
# Each generated CHOICE class will behave like a C-union. If one sets the value
# of one of the fields, the other fields will be set to None (seemingly).
# - If a field is NOT set, it WILL NOT thrown an exception and simply return None.
# - The current "state"/"present" value type is registered via a custom enumeration
#   named "PRESENT" and exposed via the attribute "present".
def test_constr_choice_init():
    # Initialization can be done using keyword arguments:
    obj = ExampleChoice(foo=b"...")
    # Upon successful initialization, is_valid will return True, indicating
    # a value has been set (may be invalid to the ASN.1 schema).
    assert obj.is_valid()
    assert obj.foo == b"..."
    assert obj.bar is None
    assert obj.baz is None

    # We could also just create an empty CHOICE object, which will be in
    # an invalid state.
    obj = ExampleChoice()
    assert not obj.is_valid()
    # The constraints will be called, however, because the value is not
    # set, the check will fail.
    with pytest.raises(ValueError):
        obj.check_constraints()


def test_constr_choice_parse():
    # Encoding and decoding behave the same way, because the implementation
    # is type-agnostic and uses the asn1c runtime under the hood.
    obj = ExampleChoice(foo=b"...")
    raw_data = obj.encode()
    assert raw_data == b"\x80\x03..."

    # Decoding will return a new object in a valid state
    parsed = ExampleChoice.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.foo == b"..."

    # NOTE: Because enumerated types do not enforce any constraints,
    # the decoded value is not guaranteed to be valid for enumerations.
    obj.baz = 4  # invalid value
    raw_data = obj.encode()
    assert raw_data == b"\x82\x01\x04"

    # The invalid value will raise an exception when we try to convert
    # it back to the Python representation.
    parsed = ExampleChoice.decode(raw_data)
    with pytest.raises(ValueError):
        _ = parsed.baz


def test_constr_choice_inner_enum():
    # Inner enum types will generate a new enum class for each member. The newly
    # created class will be stored as a class attribute named using the
    # following schema:
    #
    #   class_name := <member_name> '_VALUES'
    obj = ExampleChoice()
    obj.cEnum = ExampleChoice.cEnum_VALUES.cEnum_ceA
    assert obj.cEnum == ExampleChoice.cEnum_VALUES.cEnum_ceA


# -- SEQUENCE / SET
# Each generated SEQUENCE class will behave like a C-struct.
#
# VERY IMPORTANT NOTE: Because the internal binding sets all values to zero in
# the newly allocated value, the object may be able to encode directly.
def test_constr_seq_init():
    # Generated type: ExampleSequence
    obj = ExampleSequence()
    # Internal state points to valid, but not all constraints are met
    assert not obj.is_valid()
    # By default, all values are set to None if optional and return a
    # default value if not optional.
    assert obj.sInt == 0

    # optional values return None if not set
    assert obj.sBoolOpt is None
    obj.sBoolOpt = False
    assert obj.sBoolOpt is False

    # Raw-Data: created with sInt=200 and sBoolOpt=True
    raw_data = b"0\r\x80\x02\x00\xc8\x81\x01\x00\x82\x01\xff\x83\x01\x00"
    parsed = ExampleSequence.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.sInt == 200
    assert parsed.sBoolOpt is True
    assert parsed.encode() == raw_data
