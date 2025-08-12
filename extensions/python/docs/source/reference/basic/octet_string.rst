.. _reference_basic_octetstring:

OCTET STRING
============

The ASN.1 ``OCTET STRING`` type represents a sequence of octets (8-bit bytes).
It is typically used to store arbitrary binary data or fixed-length byte arrays.
In Python, the generated ``OCTET STRING`` class wraps the native ``bytes`` type.

Example ASN.1 definition:

.. code-block:: asn1

    MyOctetString ::= OCTET STRING

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyOctetString(_Asn1BasicType[bytes]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[bytes]
    :no-index:

    Represents an OCTET STRING ASN.1 type.

    .. py:method:: __init__(self, value: bytes | None = None) -> None
        :no-index:

        Initializes the OCTET STRING instance with an optional initial value.

        The value must be a ``bytes`` object or ``None``.

    .. py:property:: value
        :type: bytes
        :no-index:

        Gets or sets the octet string value.

        Setting the value accepts any object implementing the buffer protocol.

        .. note::
            Mutating the returned ``bytes`` object is not possible (since it
            is immutable). To update the value, assign a new bytes object
            explicitly.

