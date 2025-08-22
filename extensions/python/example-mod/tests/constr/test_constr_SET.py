import pytest

from example_mod._example_mod import ExampleSet, ExampleSequence


# -- SET
# A set will be a combination of a SEQUENCE and CHOICE in the C extension.
# Whether a variable has been set or not is determined by the internal
# state. Therefore, values MUST be set manually.
def _make_setobj(s_int=42, si_bool=True):
    obj = ExampleSet()
    obj.sInt = s_int
    obj.setSeq = ExampleSet.setSeq_TYPE()
    obj.setSeq.siBool = si_bool
    return obj


def test_constr_set_initial_attr():
    obj = ExampleSet()
    assert obj.sInt is None
    assert obj.setSeq is None


@pytest.mark.parametrize(
    "encode,decode,is_defined",
    [
        ("ber_encode", "ber_decode", True),
        ("der_encode", "der_decode", True),
        ("cer_encode", "cer_decode", True),
        ("xer_encode", "xer_decode", True),
        ("per_encode", "per_decode", False),
        ("jer_encode", "jer_decode", True),
    ],
)
def test_constr_set_encode_decode(encode, decode, is_defined):
    original = _make_setobj(123, False)
    encode_func = getattr(original, encode)
    if is_defined:
        data = encode_func()

        decode_func = getattr(ExampleSet, decode)
        decoded = decode_func(data)

        assert isinstance(decoded, ExampleSet)
        assert decoded.sInt == original.sInt
        assert isinstance(decoded.setSeq, ExampleSet.setSeq_TYPE)
        assert decoded.setSeq.siBool == original.setSeq.siBool
    else:
        with pytest.raises(NotImplementedError):
            encode_func()


def test_constr_set_independent_instances():
    obj1 = _make_setobj(1, True)
    obj2 = _make_setobj(2, False)
    assert obj1.sInt != obj2.sInt
    assert obj1.setSeq.siBool != obj2.setSeq.siBool


def test_constr_set_field_types():
    obj = _make_setobj()
    assert isinstance(obj.sInt, int)
    assert isinstance(obj.setSeq.siBool, bool)


@pytest.mark.parametrize("value", [0, -1, 99999])
def test_constr_set_integer_values(value):
    obj = _make_setobj(s_int=value)
    assert obj.sInt == value


def test_constr_set_copy():
    set_obj = ExampleSet()
    set_obj.sInt = 42
    assert set_obj.sInt == 42

    seq_obj = ExampleSequence()
    seq_obj.sSet = set_obj
    assert seq_obj.sSet.sInt == 42
