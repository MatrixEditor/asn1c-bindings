import pytest

from example_mod._example_mod import ExampleObjectIdentifier, ExampleRelativeOid


def test_OBJECT_IDENTIFIER_accepts_valid_string():
    # Generated class: ExampleObjectIdentifier
    # type: str
    obj = ExampleObjectIdentifier()
    obj.value = "1.2.3"
    assert obj.value == "1.2.3"


def test_OBJECT_IDENTIFIER_decodes_correctly():
    raw_data = b"\x06\x02\x01\x02"
    parsed = ExampleObjectIdentifier.ber_decode(raw_data)
    assert parsed.is_valid()
    assert parsed.value == "0.1.2"  # This is how ASN.1 encodes OIDs


def test_OBJECT_IDENTIFIER_encodes_correctly():
    obj = ExampleObjectIdentifier()
    obj.value = "0.1.2"
    assert obj.ber_encode() == b"\x06\x02\x01\x02"


def test_OBJECT_IDENTIFIER_rejects_invalid_type():
    obj = ExampleObjectIdentifier()
    with pytest.raises(ValueError):
        obj.value = 12345


def test_OBJECT_IDENTIFIER_decoding_fails_with_wrong_tag():
    raw_data = b"\x04\x02\x01\x02"  # OCTET STRING tag instead of OID
    with pytest.raises(ValueError):
        ExampleObjectIdentifier.ber_decode(raw_data)


# -- Relative OID --


def test_relativeoid_accepts_valid_string():
    # Generated class: ExampleRelativeOid
    # type: str
    obj = ExampleRelativeOid("1.2.3")
    assert obj.value == "1.2.3"


def test_relativeoid_decodes_correctly():
    raw_data = b"\r\x03\x01\x02\x03"
    parsed = ExampleRelativeOid.ber_decode(raw_data)
    assert parsed.is_valid()
    assert parsed.value == "1.2.3"


def test_relativeoid_encodes_correctly():
    obj = ExampleRelativeOid()
    obj.value = "1.2.3"
    assert obj.ber_encode() == b"\r\x03\x01\x02\x03"


def test_relativeoid_rejects_invalid_type():
    obj = ExampleRelativeOid()
    with pytest.raises((TypeError, ValueError)):
        obj.value = None


def test_relativeoid_decoding_fails_with_wrong_tag():
    raw_data = b"\x06\x03\x01\x02\x03"  # OID tag instead of RELATIVE-OID
    with pytest.raises(ValueError):
        ExampleRelativeOid.ber_decode(raw_data)
