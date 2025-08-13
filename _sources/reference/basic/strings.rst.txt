.. _reference_basic_strings:

ASN.1 String Types
==================

ASN.1 defines a variety of string types used to represent textual data.
All generated Python classes for these string types will wrap the native
Python ``str`` type. This applies to the following ASN.1 string types:

- IA5String
- PrintableString
- VisibleString
- ISO646String
- NumericString
- UniversalString
- BMPString
- UTF8String
- GeneralString
- GraphicString
- TeletexString (T61String)
- VideotexString
- ObjectDescriptor
- UTCTime
- GeneralizedTime

Example ASN.1 definition:

.. code-block:: asn1

    MyUTF8String ::= UTF8String
    MyPrintable ::= PrintableString

which generate Python classes roughly equivalent to:

.. code-block:: python

    class MyUTF8String(_Asn1BasicType[str]):
        pass

    class MyPrintable(_Asn1BasicType[str]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[str]
    :no-index:

    Represents an ASN.1 string type.

    .. py:method:: __init__(self, value: str | None = None) -> None
        :no-index:

        Initializes the string type instance with an optional initial value.

        The value must be a Python ``str`` or ``None``.

    .. py:property:: value
        :type: str
        :no-index:

        Gets or sets the string value.

        Setting the value accepts any Python object coercible to ``str``,
        such as string literals or other string-like objects.

        .. important::
            The string is expected to conform to the ASN.1 string type's
            character repertoire and constraints; however, validation
            is performed by the constraint checking methods.

        .. admonition:: Character Sets and Constraints

            Each ASN.1 string type restricts the allowed characters differently.
            For example:

            - ``PrintableString`` allows a limited set of printable characters.
            - ``IA5String`` corresponds roughly to ASCII.
            - ``UTF8String`` supports the full Unicode set.

            The generated classes enforce these constraints via validation
            and raise exceptions if invalid data is assigned and packed.

        .. note::
            Mutating the returned string is not possible since Python strings
            are immutable. To change the value, assign a new string explicitly.

