import pytest

from example_mod._example_mod import ExampleBoolean

# Generated class: ExampleBoolean
# type: bool
# constraints: value = True | False


def test_BOOLEAN_accepts_true_and_is_valid():
    # ExampleBoolean represents the  ASN.1 BOOLEAN type that only allows True or False.
    obj = ExampleBoolean()
    obj.value = True
    assert obj.is_valid()


# --- Automatic Type Casting ---


def test_BOOLEAN_casts_non_boolean_to_bool():
    # - Automatically casts the value to Python's bool type.
    obj = ExampleBoolean()
    obj.value = 1000
    assert obj.value is True
    assert isinstance(obj.value, bool)
