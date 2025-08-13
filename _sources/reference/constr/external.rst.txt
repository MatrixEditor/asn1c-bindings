.. _reference_constr_external:

EXTERNAL
========

The ASN.1 ``EXTERNAL`` type is a **constructed** universal type used to encapsulate
data that may be defined outside the current ASN.1 module.
It typically contains an identifier describing the external data,
an optional indirect reference, and the encoded data itself.

In the ASN.1 specification, ``EXTERNAL`` is defined as a **SEQUENCE**
with a predefined structure, but in code generation it is represented as
a **custom type name** that inherits the same behavior as any generated
``SEQUENCE`` class.

Example ASN.1 definition:

.. code-block:: asn1

    MyExternalData ::= EXTERNAL

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyExternalData(_Asn1BasicType[EXTERNAL]):
        pass


Conceptual Representation
-------------------------

.. py:class:: EXTERNAL

    *Inherits from* :class:`_Asn1Type`.

    Represents a constructed ASN.1 type where fields are assigned in a
    fixed order. In the case of ``EXTERNAL``, the generated class will
    have members as specified by the ASN.1 standard for the ``EXTERNAL``
    type, such as:

    .. py:property:: direct_reference
        :type: str | None

    .. py:property:: indirect_reference
        :type: int | None

    .. py:property:: data_value_descriptor
        :type: str | None

    .. py:property:: encoding
        :type: encoding_TYPE

    .. note::
        Since ``EXTERNAL`` is implemented as a specialized ``SEQUENCE``,
        it will behave identically to any generated SEQUENCE object in Python.
        This means:

        - All values are initialized to ``None`` if optional.
        - Non-optional members have default values where applicable.
        - The object may be directly encodable even without explicitly setting values,
          depending on the default assignments.

    .. py:method:: __init__(self, /, **members: Any) -> None
        :no-index:

        Initializes the EXTERNAL instance with optional member assignments.
        Unspecified optional fields will default to ``None``.

    .. py:class:: encoding_TYPE

        Representation of the  ``encoding`` field as a CHOICE.

        .. py:property:: single_ASN1_type
            :type: bytes | None


        .. py:property:: octet_aligned
            :type: bytes | None


        .. py:property:: arbitrary
            :type: bitarray.bitarray | None

        .. seealso::
            CHOICE generation concept in: :class:`_Asn1ChoiceType`.

