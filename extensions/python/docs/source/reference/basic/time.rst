.. _reference_basic_time_types:

TIME
====

ASN.1 defines several universal types for representing time and date information:
``TIME``, ``DATE``, ``TIME-OF-DAY``, ``DATE-TIME``, and ``DURATION``.

While each has a distinct semantic meaning, their **underlying ASN.1 encoding
is based on strings**, and they are **not natively implemented** in the library.
This means that users **must define them manually** in their ASN.1 module
before use.

A typical set of ASN.1 definitions for these types might be:

.. code-block:: asn1

    TIME ::= [UNIVERSAL 14] VisibleString
    DATE ::= [UNIVERSAL 31] IMPLICIT TIME
    TIME-OF-DAY ::= [UNIVERSAL 32] IMPLICIT TIME
    DATE-TIME ::= [UNIVERSAL 33] IMPLICIT TIME
    DURATION ::= [UNIVERSAL 34] IMPLICIT TIME

The above definitions map the time-related types to a ``VisibleString`` base
type (or to ``TIME`` in the case of derived types). This allows them to be
handled as textual data while still using the correct ASN.1 universal tags.

Python Representation
---------------------

Since these types are defined as string-based in ASN.1, their generated Python
classes will wrap a native :class:`str` value.

Example generated Python classes might look like:

.. code-block:: python

    class TIME(_Asn1BasicType[str]):
        pass

    class DATE(_Asn1BasicType[str]):
        pass

    class TIME_OF_DAY(_Asn1BasicType[str]):
        pass

    class DATE_TIME(_Asn1BasicType[str]):
        pass

    class DURATION(_Asn1BasicType[str]):
        pass

Conceptual Representation
--------------------------

All of these types follow the same general pattern:

.. py:class:: _Asn1BasicType[str]
    :no-index:

    Represents an ASN.1 string-based time type.

    .. py:method:: __init__(self, value: str | None = None) -> None
        :no-index:

        Initializes the instance with an optional string value.

    .. py:property:: value
        :type: str
        :no-index:

        Gets or sets the string value.

        The value should conform to the expected format for the specific time
        type (e.g., ISO 8601 format for ``DATE-TIME``), but no automatic format
        validation is performed unless implemented by the user.
