import pytest

from example_mod._example_mod import ExampleChoice


# -- CHOICE
# Each generated CHOICE class will behave like a C-union. If one sets the value
# of one of the fields, the other fields will be set to None (seemingly).
# If a field is NOT set, it WILL NOT thrown an exception and simply return None.
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
