import pytest
from bitarray import bitarray

from example_mod._example_mod import ExampleChoice, ExampleSequence


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


def test_constr_seq_parse():
    obj = ExampleSequence()
    obj.sBitStr = b"2"
    obj.snBitStr = 2
    obj.sOid = "1.2.3.4"
    obj.sRelOid = "1.2.3.4.5"
    raw_data = obj.ber_encode()

    parsed = ExampleSequence.ber_decode(raw_data)
    assert parsed.is_valid()
    assert parsed.sBitStr == bitarray("01001100")
    assert parsed.ber_encode() == raw_data
