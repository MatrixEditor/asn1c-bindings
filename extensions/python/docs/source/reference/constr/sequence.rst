.. _reference_constr_sequence:

SEQUENCE
========

The ASN.1 ``SEQUENCE`` type represents an ordered collection of fields,
similar to a record or a struct in other programming languages.

Each field in a ``SEQUENCE`` can be mandatory or optional, and may have
default values. The fields can be of any ASN.1 type, including other
constructed types.

.. attention::
    Default values are still in development!

Example ASN.1 definition:

.. code-block:: asn1

    Person ::= SEQUENCE {
        name      UTF8String,
        age       INTEGER OPTIONAL,
        married   BOOLEAN DEFAULT FALSE
    }

generates the following Python class:

.. code-block:: python

    class Person(_Asn1Type):
        name: str
        age: int | None
        married: bool

        def __init__(
            self,
            name: str = ...,
            age: int | None = ...,
            married: bool = ...,
        ) -> None:

In Python, each ASN.1 ``SEQUENCE`` generates a class that behaves like a C-style
struct with named attributes for each field.

- All mandatory fields are initialized to their default value or a zero-equivalent.
- Optional fields are initialized as ``None``.
- Setting one field does not affect others.
- Access to each field is through a Python attribute with the same name.
- Encoding and decoding methods follow the enabled ASN.1 encoding rules.

Conceptual Representation
--------------------------

.. py:class:: _Asn1SequenceType
    :no-index:

    Represents an ASN.1 SEQUENCE.

    .. py:method:: __init__(self, *, field1: Any = ..., field2: Any = ..., ...) -> None
        :no-index:

        Initializes a new SEQUENCE instance.

        - Optional fields default to ``None`` if not provided.
        - Non-optional fields are initialized to their default or zero-equivalent value.
        - Fields are passed as keyword arguments only.

    .. py:property:: field_name
        :no-index:

        Access or set the value of the field named ``field_name``.

        - Setting the field updates its internal ASN.1 value.
        - Getting the field returns the current value or ``None`` if optional and unset.

Behavior
--------

- The SEQUENCE class acts like a typed container, allowing access by attribute name.
- Fields are initialized automatically, enabling encoding even without explicit
  field initialization, as the underlying binding initializes values to zero.
- Optional fields left unset will be omitted during encoding.
- Default values are applied automatically unless explicitly overridden.

Example Usage in Python:

.. code-block:: python

    person = Person(name="Alice")
    print(person.age)        # None, as age is optional and unset
    person.married = True    # Override default FALSE value
    encoded = person.ber_encode()

