.. _reference_constr_embedded_pdv:

EMBEDDED PDV
============

The ASN.1 ``EMBEDDED PDV`` type represents a structured value used for transporting
presentation data values. In the abstract syntax, it is defined as a specialized
form of the ``SEQUENCE`` type with a fixed set of components describing the
presentation context, transfer syntax, and the actual data.

In practice, the generated Python class for ``EMBEDDED PDV`` behaves exactly like a
``SEQUENCE``-derived type, since the ASN.1 standard defines it as such.
All attribute access, assignment rules, and encoding/decoding logic are identical
to any other generated sequence object.

Example ASN.1 definition:

.. code-block:: asn1

    -- From ITU-T X.680
    EMBEDDED-PDV ::= [UNIVERSAL 11] SEQUENCE {
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
        -- NOTE - The embedded-pdv type does not allow the inclusion of a
        -- data-value-descriptor value.
        --  data-value-descriptor [1] ObjectDescriptor OPTIONAL,
        data-value [2] OCTET STRING
    } -- (WITH COMPONENTS {... , data-value-descriptor ABSENT })

    MyPDV ::= EMBEDDED-PDV

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MyPDV(_Asn1BasicType[EMBEDDED_PDV]):
        # Generated members according to ASN.1 definition of EMBEDDED PDV
        pass

Python Representation
---------------------

.. py:class:: EMBEDDED_PDV
    :no-index:

    Represents a SEQUENCE-based ASN.1 type. All constraints, encoding rules,
    and attribute semantics of SEQUENCE apply.

    .. py:method:: __init__(self, /, **members: Any) -> None
        :no-index:

        Initializes the object with keyword arguments for each member.

        All members follow the assignment rules for SEQUENCE types — optional
        fields default to ``None``, while required fields are initialized with
        their default ASN.1 values.

    .. py:attribute:: <member>
        :no-index:
        :type: Any

        Each defined component of the EMBEDDED PDV structure is accessible
        as an attribute. Modifying these attributes updates the parent object
        directly, as is the case for SEQUENCE fields represented by generated classes.

Special Notes
-------------

- Even though ``EMBEDDED PDV`` is a *universal* ASN.1 type with its own tag
  ([Tag: 11]), it is internally realized as a ``SEQUENCE``. This means that in
  Python, its behavior is indistinguishable from any other generated sequence type.

- **Not officially implemented:** This universal type is not natively available
  in the library and must be explicitly defined in your ASN.1 module before it can
  be used. You must declare it manually using the following definition:

  .. code-block:: asn1

      EMBEDDED-PDV ::= [UNIVERSAL 11] SEQUENCE {
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
          -- NOTE – The embedded-pdv type does not allow the inclusion of a
          -- data-value-descriptor value.
          --  data-value-descriptor [1] ObjectDescriptor OPTIONAL,
          data-value [2] OCTET STRING
      } -- (WITH COMPONENTS {... , data-value-descriptor ABSENT })

  After declaring this type, you **must** use the `EMBEDDED-PDV` identifier
  exactly as shown in your ASN.1 schema.

- Because the underlying representation is a sequence object, the same encoding
  constraints and decoding behavior apply.

