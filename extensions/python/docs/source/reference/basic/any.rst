.. _reference_basic_any:

ANY / ANY DEFINED BY
====================

The ASN.1 ``ANY`` and ``ANY DEFINED BY`` types are flexible container types
intended to hold **arbitrary ASN.1 values**.
They differ mainly in how the contained type is determined:

- **ANY** — accepts *any* ASN.1 value, without further constraints.
- **ANY DEFINED BY** — links the actual type to another field's value
  (usually an enumerated field, OID, or similar), allowing dynamic type selection.

In modern ASN.1 usage, ``ANY`` and ``ANY DEFINED BY`` are considered **legacy
constructs** and are replaced by ``OPEN TYPE`` in the X.680 ASN.1 standard.
However, they still appear in many older ASN.1 specifications and thus remain
relevant.

Python Representation
---------------------

In Python, generated ``ANY`` and ``ANY DEFINED BY`` classes store their
contents as **raw encoded bytes**.
These bytes represent the full BER/DER/CER/… encoding of the underlying value.

The generated classes **will not automatically decode these bytes** — instead, you must
explicitly invoke the target class' decoding method to interpret the value.

Example ASN.1 definitions:

.. code-block:: asn1

    MyAny ::= ANY

    MyAnyDefinedBy ::= SEQUENCE {
        typeId OBJECT IDENTIFIER,
        value ANY DEFINED BY typeId
    }

Generated Python representations:

.. code-block:: python

    class MyAny(_BasicAsn1Type[bytes]):
        pass

    class MyAnyDefinedBy(_Asn1Type):
        typeId: str
        value: bytes  # raw encoding of the referenced type

Conceptual Representation
--------------------------

.. py:class:: _BasicAsn1Type[bytes]
    :no-index:

    Represents an ASN.1 type holding raw encoded bytes.

    .. py:method:: __init__(self, value: bytes | None = None) -> None
        :no-index:

        Initializes the object with optional raw ASN.1-encoded data.

    .. py:property:: value
        :type: bytes
        :no-index:

        Gets or sets the raw encoded bytes.

        Assigning a new value replaces the underlying stored encoding.

Decoding Contents
-----------------

To interpret the contents of an ``ANY`` or ``ANY DEFINED BY`` value,
use the decoding API of the target type:

.. code-block:: python

    # Suppose we expect the value to be a BOOLEAN
    decoded_bool = MyBoolean.ber_decode(my_any_instance.value)

    # Or with ANY DEFINED BY, decide based on typeId
    if obj.typeId == some_oid:
        decoded_value = TargetType.ber_decode(obj.value)
