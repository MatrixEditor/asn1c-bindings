.. _reference_constr_choice:

CHOICE
======

The ASN.1 ``CHOICE`` type represents a union-like data structure where
only one of the possible fields can be set at a time.

Example ASN.1 definition:

.. code-block:: asn1

    MyChoice ::= CHOICE {
        intValue   INTEGER,
        strValue   UTF8String,
        boolValue  BOOLEAN
    }

generates the following Python class:

.. code-block:: python

    class MyChoice(_Asn1Type):
        intValue: int
        strValue: str
        boolValue: bool

        class PRESENT(enum.IntEnum):
            PR_NOTHING = 0
            PR_intValue = 1
            PR_strValue = 2
            PR_boolValue = 3

        @property
        def present(self) -> PRESENT: ...
        def __init__(
            self,
            intValue: int = ...,
            strValue: str = ...,
            boolValue: bool = ...,
        ) -> None: ...


In Python, the generated class for a ``CHOICE`` behaves similarly to a C union:

- Only **one field can hold a value at any time**.
- Setting a field automatically **clears all other fields** (sets them to ``None``).
- Accessing a field that is not currently set returns ``None`` instead of raising an exception.
- The currently selected field is tracked via a special enumeration called ``PRESENT``, accessible via the ``present`` attribute.
- An instance of a ``CHOICE`` may be created empty, representing an invalid or unset state.

Conceptual Representation
--------------------------

.. py:class:: _Asn1ChoiceType
    :no-index:

    Represents an ASN.1 CHOICE type.

    .. py:classattribute:: PRESENT
        :no-index:

        An enumeration representing all possible choices, with members
        corresponding to each field.

        - The invalid/unset state is represented by ``PR_NOTHING = 0``.
        - Fields are named as ``PR_<fieldname>``.

    .. py:attribute:: present
        :no-index:

        Holds the current active field indicator as a member of ``PRESENT``.

    .. py:method:: __init__(self, /, **members: Any) -> None
        :no-index:

        Initializes the CHOICE instance.

        - Only one field may be set via keyword arguments.
        - If multiple fields are provided or none, the object may represent
          an invalid or unset state.
        - Setting a field clears all others.

    .. py:attribute:: <field_name>
        :no-index:

        Access or set the value of the named field.

        - Setting a field clears other fields.
        - Getting an unset field returns ``None``.

.. seealso::
    :class:`_Asn1ChoiceType` concept description.

