import pytest

from example_mod._example_mod import ExampleUTF8String


def test_utf8string_accepts_valid_string():
    obj = ExampleUTF8String()
    obj.value = "Hello, 世界"
    assert obj.is_valid()
    assert obj.value == "Hello, 世界"


def test_utf8string_encodes_and_decodes_correctly():
    obj = ExampleUTF8String()
    obj.value = "Test string"
    encoded = obj.encode()
    decoded = ExampleUTF8String.decode(encoded)
    assert decoded.value == "Test string"


def test_utf8string_rejects_invalid_type():
    obj = ExampleUTF8String()
    with pytest.raises(TypeError):
        obj.value = 1234


def test_utf8string_decoding_fails_with_wrong_tag():
    raw_data = b"\x04\x05Hello"  # OCTET STRING tag 0x04 instead of UTF8String
    with pytest.raises(ValueError):
        ExampleUTF8String.decode(raw_data)
