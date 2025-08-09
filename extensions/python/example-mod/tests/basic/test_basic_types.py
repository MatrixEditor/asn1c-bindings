import pytest

from bitarray import bitarray

from example_mod._example_mod import (
    ExampleOctetString,
    ExampleNull,
    ExampleObjectIdentifier,
    ExampleRelativeOid,
    ExampleReal,
)


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




def test_basic_real():
    # Generated class: ExampleReal
    # type: float
    obj = ExampleReal()
    obj.value = 3.14
    assert obj.value == 3.14

    raw_data = b"\t\t\x80\xcd\t\xd7\n=p\xa3\xd7"
    parsed = ExampleReal.decode(raw_data)
    assert parsed.is_valid()
    assert parsed.value == 1.23

    with pytest.raises(ValueError):
        # intentionally raises an error upon invalid value type
        # needed for conversion
        obj.value = None
