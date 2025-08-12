.. _reference_basic_oid:

OBJECT IDENTIFIER
=================

The ASN.1 ``OBJECT IDENTIFIER`` (OID) type uniquely identifies objects such as
standards, protocols, and algorithms using a dotted-decimal string notation.

In Python, the generated ``OBJECT IDENTIFIER`` class wraps a native
``str`` representing the dotted-decimal OID, e.g., ``"1.3.6.1.4.1"``.

Example ASN.1 definition:

.. code-block:: asn1

    MyOid ::= OBJECT IDENTIFIER

which generates a Python class similar to:

.. code-block:: python

    class MyOid(_Asn1BasicType[str]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[str]
    :no-index:

    Represents an ASN.1 OBJECT IDENTIFIER.

    .. py:method:: __init__(self, value: str | None = None) -> None
        :no-index:

        Initializes the OID with an optional dotted-decimal string.

    .. py:property:: value
        :type: str
        :no-index:

        Gets or sets the OID as a dotted-decimal string.

        The string must follow the OID syntax, e.g., numbers separated by dots.
        Validation against this syntax is typically performed.

Usage Notes
-----------

- The OID string represents a hierarchical series of integer nodes separated by dots.

- Example of a valid OID string: ``"1.2.840.113549.1.1.5"``

- Assignment accepts any valid dotted-decimal string.


.. _reference_basic_relative_oid:

RELATIVE-OID
============

The ASN.1 ``RELATIVE-OID`` type is similar to OBJECT IDENTIFIER but represents
a relative path in the OID tree, omitting the root nodes.

In Python, ``RELATIVE-OID`` is also represented as a native ``str`` containing
dot-separated integer arcs.

Example ASN.1 definition:

.. code-block:: asn1

    MyRelOid ::= RELATIVE-OID

which generates a Python class similar to:

.. code-block:: python

    class MyRelOid(_Asn1BasicType[str]):
        pass

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[str]
    :no-index:

    Represents an ASN.1 RELATIVE-OID.

    .. py:method:: __init__(self, value: str | None = None) -> None
        :no-index:

        Initializes the relative OID with an optional dot-separated string.

    .. py:property:: value
        :type: str
        :no-index:

        Gets or sets the relative OID string.

        The string consists of dot-separated integers representing arcs relative
        to some root.
