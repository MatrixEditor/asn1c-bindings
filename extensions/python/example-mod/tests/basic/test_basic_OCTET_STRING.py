import pytest

from example_mod._example_mod import ExampleOctetString


def test_OCTET_STRING_accepts_valid_bytes_and_is_valid():
    # ASN.1 OCTET STRING type that:
    #    - Stores raw bytes
    #    - Requires 1 <= size <= 32
    obj = ExampleOctetString()
    obj.value = b"\x01\x02\x03"
    assert obj.is_valid()
    assert obj.value.hex() == "010203"


def test_OCTET_STRING_decodes_correctly():
    raw_data = b"\x04\x03\x01\x02\x03"
    parsed = ExampleOctetString.ber_decode(raw_data)
    assert parsed.value == b"\x01\x02\x03"


def test_OCTET_STRING_encodes_correctly():
    """
    Verifies that encoding an ExampleOctetString with a valid bytes value:
    - Produces the expected DER-encoded data.
    """
    obj = ExampleOctetString()
    obj.value = b"\x01\x02\x03"
    assert obj.ber_encode() == b"\x04\x03\x01\x02\x03"


def test_OCTET_STRING_rejects_empty_bytes():
    # Assigning empty bytes violates the minimum size constraint:
    #    - is_valid() should return False.
    obj = ExampleOctetString()
    obj.value = b""
    assert not obj.is_valid()


def test_OCTET_STRING_rejects_bytes_longer_than_max_size():
    obj = ExampleOctetString()
    obj.value = b"0" * 33
    assert not obj.is_valid()


def test_OCTET_STRING_encoding_fails_if_invalid_size():
    obj = ExampleOctetString()
    obj.value = b"0" * 33  # invalid length
    with pytest.raises(ValueError):
        obj.ber_encode()


def test_OCTET_STRING_rejects_non_bytes_type():
    obj = ExampleOctetString()
    with pytest.raises(ValueError):
        obj.value = "string"
    with pytest.raises(ValueError):
        obj.value = 12345


# --- Decoding Errors ---


def test_OCTET_STRING_decoding_fails_with_truncated_data():
    raw_data = b"\x04\x03\x01\x02"  # missing last byte
    with pytest.raises(ValueError):
        ExampleOctetString.ber_decode(raw_data)


def test_OCTET_STRING_decoding_fails_with_wrong_tag():
    # Tag 0x02 = INTEGER, not OCTET STRING
    raw_data = b"\x02\x03\x01\x02\x03"
    with pytest.raises(ValueError):
        ExampleOctetString.ber_decode(raw_data)
