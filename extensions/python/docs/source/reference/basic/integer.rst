.. _reference_basic_int:

INTEGER
=======

The ASN.1 ``INTEGER`` type represents an arbitrary-precision signed integer value.

In Python, the generated ``INTEGER`` class wraps the native ``int`` type, providing
encoding, decoding, and constraint checks compliant with ASN.1 rules.

Example ASN.1 definition:

.. code-block:: asn1

    MyInteger ::= INTEGER

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyInteger(_Asn1BasicType[int]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[int]

    Represents an INTEGER ASN.1 type.

    .. py:method:: __init__(self, value: int | None = None) -> None

        Initializes the INTEGER instance with an optional initial value.

    .. py:property:: value
        :type: int

        Gets or sets the integer value.

        Setting the value accepts any Python object coercible to int.


Named INTEGER values (Enumerations)
-----------------------------------

ASN.1 also supports named INTEGER values using the ``ENUMERATED`` type, where
each integer constant is given a symbolic name.

Example ASN.1 enumeration:

.. code-block:: asn1

    MyEnum ::= ENUMERATED {
        red(0),
        green(1),
        blue(2)
    }

This generates a Python class similar to:

.. code-block:: python

    class MyEnum(_Asn1EnumType):
        class VALUES(enum.IntEnum):
            V_red = 0
            V_green = 1
            V_blue = 2

Represented via :class:`_Asn1EnumType`.