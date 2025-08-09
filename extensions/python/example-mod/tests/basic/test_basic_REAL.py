import pytest
import math

from example_mod._example_mod import ExampleReal


def test_real_accepts_valid_float():
    # Generated class: ExampleReal
    # type: float
    obj = ExampleReal()
    obj.value = 3.14
    assert obj.value == 3.14


def test_real_decodes_correctly():
    raw_data = b"\t\t\x80\xcd\t\xd7\n=p\xa3\xd7"
    parsed = ExampleReal.ber_decode(raw_data)
    assert parsed.is_valid()
    assert math.isclose(parsed.value, 1.23, rel_tol=1e-9)


def test_real_encodes_correctly():
    obj = ExampleReal()
    obj.value = 1.23
    encoded = obj.ber_encode()
    decoded = ExampleReal.ber_decode(encoded)
    assert decoded.value == 1.23


def test_real_rejects_invalid_type():
    obj = ExampleReal()
    with pytest.raises(ValueError):
        obj.value = None

    with pytest.raises(ValueError):
        obj.value = "not-a-float"


def test_real_decoding_fails_with_wrong_tag():
    raw_data = b"\x02\x01\x01"  # INTEGER encoding
    with pytest.raises(ValueError):
        ExampleReal.ber_decode(raw_data)


def test_real_supports_special_float_values():
    obj = ExampleReal()

    obj.value = float("inf")
    assert math.isinf(obj.value) and obj.value > 0

    obj.value = float("-inf")
    assert math.isinf(obj.value) and obj.value < 0

    obj.value = float("nan")
    assert math.isnan(obj.value)
