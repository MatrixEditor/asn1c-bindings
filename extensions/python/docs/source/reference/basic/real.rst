.. _reference_basic_real:

REAL
====

The ASN.1 ``REAL`` type represents a floating-point number, optionally including
special values such as ``PLUS-INFINITY``, ``MINUS-INFINITY``, and ``NOT-A-NUMBER`` (NaN),
depending on the encoding rules used.

In Python, the generated ``REAL`` class wraps the native ``float`` type, providing
encoding, decoding, and constraint checks compliant with ASN.1 rules.

Example ASN.1 definition:

.. code-block:: asn1

    MyReal ::= REAL

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyReal(_Asn1BasicType[float]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[float]
    :no-index:

    Represents a REAL ASN.1 type.

    .. py:method:: __init__(self, value: float | None = None) -> None
        :no-index:

        Initializes the REAL instance with an optional initial value.

        The value may be any Python object coercible to a floating-point number.

    .. py:property:: value
        :type: float
        :no-index:

        Gets or sets the floating-point value.

        Setting the value accepts any Python number type or a string representing
        a number in decimal or scientific notation.

Special Considerations
----------------------

- Some ASN.1 encoding rules allow representation of infinities and NaN, which
  are mapped directly to their Python ``float`` equivalents.
- Precision and range depend on the underlying platform's floating-point
  implementation (IEEE 754 double-precision on most systems).
