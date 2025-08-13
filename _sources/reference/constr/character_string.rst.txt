.. _reference_constr_character_string:

CHARACTER STRING
================

The ASN.1 ``CHARACTER STRING`` type is a general-purpose string type defined
in the ASN.1 standard. Unlike specific string encodings such as ``UTF8String``
or ``PrintableString``, ``CHARACTER STRING`` is intended to carry an arbitrary
string whose encoding and structure can be determined by additional
information elements.

Internally, the ASN.1 standard defines ``CHARACTER STRING`` as a **SEQUENCE**-based
type containing an optional data value descriptor and the actual string data.

In Python, the generated ``CHARACTER STRING`` class behaves exactly like a
``SEQUENCE``-derived object — each of its components is accessible as a class
attribute, and assignment follows the same rules as for any SEQUENCE type.

.. admonition:: Implementation note

  This universal type is rarely used in modern ASN.1 applications and is **not**
  defined by default. If required, you need to manually include the
  definition in your ASN.1 schema.

  .. code-block:: asn1

      -- From ITU-T X.680

      CHARACTER-STRING ::= [UNIVERSAL 29] SEQUENCE {
        identification [0] CHOICE {
            syntaxes [0] SEQUENCE {
              abstract [0] OBJECT IDENTIFIER,
              transfer [1] OBJECT IDENTIFIER
            },
            syntax [1] OBJECT IDENTIFIER,
            presentation-context-id [2] INTEGER,
            context-negotiation [3] SEQUENCE {
              presentation-context-id [0] INTEGER,
              transfer-syntax [1] OBJECT IDENTIFIER
            },
            transfer-syntax [4] OBJECT IDENTIFIER,
            fixed [5] NULL
        },
        -- NOTE - The unrestricted character string typ does not allow the
        -- inclusion of a data-value-descriptor value.
        --  data-value-descriptor [1] ObjectDescriptor OPTIONAL,
        string-value [2] OCTET STRING
      } -- (WITH COMPONENTS {... , data-value-descriptor ABSENT })



Python Representation
--------------------------

.. py:class:: CHARACTER_STRING
    :no-index:

    *Inherits from* :class:`_Asn1Type`.

    Represents a SEQUENCE-based ASN.1 type. All constraints, encoding rules,
    and attribute semantics of SEQUENCE apply.


    .. py:method:: __init__(self, /, **members: Any) -> None
        :no-index:

        Initializes the object with keyword arguments for each member.

        Optional fields default to ``None``, while required fields are
        initialized with their default ASN.1 values.

    Each defined component of the CHARACTER STRING structure is accessible
    as an attribute. Modifying these attributes updates the parent object
    directly.

    .. py:property:: identification
        :type: CHARACTER_STRING.identification_TYPE

    .. py:property:: string_value
        :type: bytes

    .. py:class:: identification_TYPE

        *Inherits from* :class:`_Asn1ChoiceType`.

