.. _reference_basic_boolean:

BOOLEAN
=======

The ASN.1 ``BOOLEAN`` type represents a truth value, indicating either
``TRUE`` or ``FALSE``.

In Python, the generated ``BOOLEAN`` class wraps the native ``bool`` type, providing
encoding, decoding, and constraint checks compliant with ASN.1 rules.

Example ASN.1 definition:

.. code-block:: asn1

    MyBoolean ::= BOOLEAN

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyBoolean(_Asn1BasicType[bool]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[bool]
    :no-index:

    Represents a Boolean ASN.1 type.

    .. py:method:: __init__(self, value: bool | None = None) -> None
        :no-index:

        Initializes the BOOLEAN instance with an optional initial value.

    .. py:property:: value
        :type: bool
        :no-index:

        Gets or sets the Boolean value.

        Setting the value accepts any Python object coercible to bool,
        e.g., ``0``, ``1``, ``True``, ``False``.



