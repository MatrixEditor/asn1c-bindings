.. _reference_constr_set:

SET
===

The ASN.1 ``SET`` type represents an unordered collection of fields,
similar to ``SEQUENCE``, but without guaranteed ordering.

Like ``SEQUENCE``, each field can be mandatory or optional, and
may have default values. However, encoding rules for ``SET`` allow
fields to appear in any order.

.. attention::
    Default values are still in development!

Example ASN.1 definition:

.. code-block:: asn1

    Preferences ::= SET {
        color           UTF8String OPTIONAL,
        notifications   BOOLEAN DEFAULT TRUE,
        timezone        INTEGER
    }

generates the following Python class:

.. code-block:: python

    class Preferences(_Asn1Type):
        color: str | None
        notifications: bool
        timezone: int

        def __init__(
            self,
            color: str | None = ...,
            notifications: bool = ...,
            timezone: int = ...
        ) -> None: ...

In Python, each ASN.1 ``SET`` generates a class similar to a ``SEQUENCE`` class,
but with the following key differences:

- **Explicit Assignment Required**: All fields **must** be explicitly assigned
  valid values before encoding; internally, the class tracks which fields have been set.
- **Default Return of None**: Accessing any unset field returns ``None`` by default,
  regardless of whether it is optional or has a default value.
- The order of fields does not affect the encoding.

Conceptual Representation
--------------------------

.. py:class:: _Asn1SetType
    :no-index:

    Represents an ASN.1 SET.

    .. py:method:: __init__(self, *, field1: Any = ..., field2: Any = ..., ...) -> None
        :no-index:

        Initializes a new SET instance.

        - All fields are initialized as unset (``None``).
        - Fields must be explicitly assigned before encoding.
        - Fields are passed as keyword arguments only.

    .. py:attribute:: field_name
        :no-index:

        Access or set the value of the field named ``field_name``.

        - Getting a field returns ``None`` if it has not been explicitly assigned.
        - Setting a field marks it as assigned and stores the given value.

Example Usage in Python:

.. code-block:: python

    prefs = Preferences()
    prefs.color = "blue"
    prefs.timezone = 5
    print(prefs.notifications)  # None, since not assigned explicitly
    encoded = prefs.ber_encode()

