import pytest
from bitarray import bitarray

from example_mod._example_mod import ExampleChoice, ExampleSequence, ExampleSet


# -- SEQUENCE
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
    obj.sBitStr = bitarray("01001100")
    obj.snBitStr.V_namedA = True
    obj.sOid = "1.2.3.4"
    obj.sRelOid = "1.2.3.4.5"
    obj.sChoice.sicInt = 1
    obj.sRefChoice.cBoolean = True
    # NOTE: we can also set the value directly here
    # obj.sOctetStr = b"\x01\x02\x03"
    obj.sOctetStr.value = b"\x01\x02\x03"
    raw_data = obj.ber_encode()

    parsed = ExampleSequence.ber_decode(raw_data)
    assert parsed.is_valid()
    assert parsed.sBitStr.value == bitarray("01001100")
    assert parsed.ber_encode() == raw_data
    assert parsed.snBitStr.V_namedA is True
