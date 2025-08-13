.. _reference_constr_list:

SET OF / SEQUENCE OF
====================

The ASN.1 ``SET OF`` and ``SEQUENCE OF`` types represent an **ordered** or **unordered** collection of elements, respectively.
Both contain **zero or more elements** of the same type.

- ``SEQUENCE OF`` preserves the order of elements.
- ``SET OF`` does not guarantee ordering in ASN.1 semantics, but in Python,
  both are represented in the same way using an ordered list interface for convenience.

Example ASN.1 definitions:

.. code-block:: asn1

    MySeqOfInts ::= SEQUENCE OF INTEGER
    MySetOfStrings ::= SET OF UTF8String

generate the following Python classes:

.. code-block:: python

    class MySeqOfInts(_Asn1ListType[int]):
        pass

    class MySetOfStrings(_Asn1ListType[str]):
        pass

Generated Python classes for ``SET OF`` and ``SEQUENCE OF`` behave like Python lists with type restrictions.
All elements must be of the specified ASN.1 type (or compatible Python type).

The concept of the list type is described in :class:`_Asn1ListType`.

Behavior
--------

- Both ``SET OF`` and ``SEQUENCE OF`` use the same list-like Python API.
- Python's ordering is preserved for **both**, even though ASN.1 ``SET OF`` does not define ordering.
- Elements can be iterated over directly:

.. code-block:: python

    seq = MySeqOfInts([1, 2, 3])
    for value in list(seq): # direct iterator not implemented
        print(value)

- Elements can be added via ``add()`` or ``extend()``:

.. code-block:: python

    seq.add(4)
    seq.extend([5, 6])

- Conversion to a standard Python list is simple:

.. code-block:: python

    values = list(seq)

- ``del`` can be used to remove items by index:

.. code-block:: python

    del seq[1]  # removes the second element

