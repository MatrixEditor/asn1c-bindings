.. _reference_overview:

.. role:: text-req
.. role:: text-opt
.. role:: text-dep

ASN.1 Overview
==============

The ASN.1 family of standards define a number of ways to encode data,
including byte-oriented (e.g., BER), bit-oriented (e.g., PER),
and textual (e.g., XER). Some encoding variants (e.g., DER) are just stricter
variants of the more general encodings (e.g., BER).

This variant of the ``asn1c`` compiler provides automatic Python bindings for:

- Basic ASN.1 types (INTEGER, BOOLEAN, REAL, etc.)
- Enumerated types
- Named bit string (flags) types
- CHOICE types (unions)
- SEQUENCE types (structs)
- SET types (special structs) and
- SEQUENCE OF / SET OF (collections)

.. contents:: Conceptual ASN.1 Types
    :class: this-will-duplicate-information-and-it-is-still-useful-here
    :local:


ASN.1 Base Type
---------------

Each generated class follows a schema that can be represented by the following
conceptual base class.

.. warning::
    This class although not present in the actual module file can be used
    as the base class for all generated ASN.1 types, due to the shared interface.


.. py:class:: _Asn1Type

    Each type alias, enumerated or constructed type WILL generate its
    own class. Each class will implement the methods described in this
    base class **(TYPING ONLY!)**.

    There are several terms used to annotate each function. Their meaning is as
    follows:

    - :text-req:`Required:` These methods MUST be impleented by the subclass
    - :text-opt:`Optional:` Only some types implement these methods
    - :text-dep:`Configurable:` Methods annotated with this term are implemented
      based on compiler options.


    .. py:method:: __repr__(self) -> str

        :text-req:`Required.`

        Returns a string with the class name, without the internal value.


    .. py:method:: __str__(self) -> str

        :text-opt:`Optional.`

        Returns the value of the ASN.1 object, if set, as a human-readable
        string. This method MAY be implemented by subtypes. Refer to each
        ASN.1 documentation to see whether the type implements this behavior.

        .. note::
            This method will simply call :code:`value.__str__()` after
            converting it internally and if supported.

    **Validation and Constraint Checking**

    All generated types implement validation methods to check ASN.1
    constraints as defined in the schema.

    .. py:method:: is_valid(self) -> bool

        :text-req:`Required.`

        Checks whether the current value satisfies all constraints without
        raising an exception. Returns ``True`` if valid, ``False`` otherwise.


    .. py:method:: check_constraints(self) -> None

        :text-req:`Required.`

        Validates the current value against ASN.1 constraints and raises
        a ``ValueError`` if any are violated.


    Encoding and decoding methods are generated only if the corresponding
    encoding rule was enabled at compile time.

    **Basic Encoding Rules (BER)**

    The Basic Encoding Rules (X.690) describe the most widely used (by the ASN.1
    community) way to encode and decode a given structure in a
    machine-independent way.

    .. py:method:: ber_encode(self) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in BER format.

    .. py:staticmethod:: ber_decode(data: bytes | Buffer) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes BER-encoded bytes into a new instance.


    **Canonical Encoding Rules (CER)**

    The Canonical Encoding Rules are a restricted form of BER that ensures
    a unique encoding for any given value.

    .. warning::
        The current implementation uses the BER encoder under the hood and no validation is performed.

    .. py:method:: cer_encode(self) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in CER format.

    .. py:staticmethod:: cer_decode(data: bytes | Buffer) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes CER-encoded bytes into a new instance.


    **Distinguished Encoding Rules (DER)**

    The Distinguished Encoding Rules are another restricted subset of BER
    (canonical BER), similar to CER, but optimized for certain cryptographic
    applications.

    .. py:method:: der_encode(self) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in DER format.

    .. py:staticmethod:: der_decode(data: bytes | Buffer) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes DER-encoded bytes into a new instance.


    **XML Encoding Rules (XER)**

    The XML Encoding Rules (X.693) define how ASN.1 structures are represented
    in XML. XER provides a human-readable form of the data and may be
    canonicalized to ensure deterministic output.

    .. py:method:: xer_encode(self, /, *, canonical: bool = ...) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in XER format. If ``canonical`` is ``True``,
        produces canonical XML form.

    .. py:staticmethod:: xer_decode(data: bytes | Buffer, /, *, canonical: bool = ...) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes XER-encoded bytes into a new instance. If ``canonical`` is
        ``True``, enforces canonical XML parsing.


    **JSON Encoding Rules (JER)**

    The JSON Encoding Rules (X.697) map ASN.1 structures to JSON format for use
    in web-based APIs and systems. Output may be either compact (minified)
    or human-readable.

    .. py:method:: jer_encode(self, /, *, minified: bool = ...) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in JSON format. If ``minified`` is ``True``,
        outputs a compact single-line JSON representation.

    .. py:staticmethod:: jer_decode(data: bytes | Buffer, /, *, minified: bool = ...) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes JSON-formatted bytes into a new instance. If ``minified`` is
        ``True``, expects compact JSON input.


    **Octet Encoding Rules (OER)**

    The Octet Encoding Rules (X.696) provide a compact binary representation
    optimized for environments with limited resources.

    .. py:method:: oer_encode(self, /, *, canonical: bool = ...) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in OER format. If ``canonical`` is ``True``,
        produces canonical octet output.

    .. py:staticmethod:: oer_decode(data: bytes | Buffer, /, *, canonical: bool = ...) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes OER-encoded bytes into a new instance.

    **Packed Encoding Rules (PER)**

    The Packed Encoding Rules (X.691) are a highly efficient binary encoding
    that omits most length and type metadata. Data may be *aligned*
    (byte-padded) or *unaligned* (bit-packed).

    .. py:method:: per_encode(self, /, *, canonical: bool = ..., aligned: bool = ...) -> bytes

        :text-dep:`Configurable.`

        Encodes the value in PER format.
        If ``aligned`` is ``True``, aligns data to byte boundaries.
        If ``canonical`` is ``True``, enforces canonical PER output.

    .. py:staticmethod:: per_decode(data: bytes | Buffer, /, *, canonical: bool = ..., aligned: bool = ...) -> _Asn1Type

        :text-dep:`Configurable.`

        Decodes PER-encoded bytes into a new instance.
        If ``aligned`` is ``True``, expects byte-aligned PER input.


    **Non-standard textual representation**

    Some types support an additional non-standard, human-oriented
    textual form, primarily for debugging or logging.

    .. py:method:: to_text(self) -> bytes

        :text-dep:`Configurable.`

        Returns a textual representation of the value.


ASN.1 Conceptual BASIC Type
---------------------------

The Abstract Syntax Notation One (ASN.1) standard defines a set of *basic types*
such as ``INTEGER``, ``BOOLEAN``, ``REAL``, ``OCTET STRING``, and others.
These basic types are mapped to native Python types where possible for ease of
use. A high-level conversion matrix is given below:

+----------------------------+---------------------+
| ASN.1 Type                 | Python Equivalent   |
+============================+=====================+
| :code:`INTEGER`            | int                 |
+----------------------------+---------------------+
| :code:`BOOLEAN`            | bool                |
+----------------------------+---------------------+
| :code:`REAL`               | float               |
+----------------------------+---------------------+
| :code:`OCTET STRING`       | bytes               |
+----------------------------+---------------------+
| :code:`UTF8String`         | str                 |
+----------------------------+---------------------+
| :code:`BIT STRING`         | bitarray (external) |
+----------------------------+---------------------+
| :code:`NULL`               | None                |
+----------------------------+---------------------+
| :code:`OID`                | str                 |
+----------------------------+---------------------+

Each new **basic** type definition in an ASN.1 module will generate a
corresponding Python class. For example, defining a new ``INTEGER`` type:

.. code-block:: asn1

    MyInteger ::= INTEGER

will generate a Python class:

.. code-block:: python

    class MyInteger(_Asn1BasicType[int]):
        pass

.. important::
    When a basic type is used *inside* a ``SEQUENCE``, ``CHOICE``,
    or other constructed type **with** or **without** being given its own type name,
    it is *inlined*, meaning the type wrapper won't be used and conversion from the
    value type is incorporated directly.

All generated Python classes — excluding those with named values (e.g.
``ENUMERATED`` types) — follow the conceptual API defined by the
``_Asn1BasicType`` class below.

.. py:class:: _Asn1BasicType[_PY_T]

    *Inherits from* :class:`_Asn1Type`.

    A conceptual base class for all ASN.1 basic types. Each basic type stores
    its underlying value in a property called ``value``.

    .. py:method:: __init__(self, value: _PY_T | None = None) -> None

        :text-req:`Required.`

        Initializes a new ASN.1 basic type instance.
        If ``value`` is provided, it is assigned to the ``value`` property
        after validation and (if necessary) conversion to the appropriate
        internal representation.

        If ``value`` is ``None``, the instance is created in an
        *unset* state.

        .. note::
            Initialization with a Python type other than the target type
            is allowed if the type supports automatic conversion.
            For example, an ``OCTET STRING`` type may accept both ``bytes`` and
            ``bytearray`` at initialization.

    .. py:property:: value
        :type: _PY_T

        :text-req:`Required.`

        The current value of the ASN.1 object, represented as the mapped
        Python type.

        This property may be reassigned after initialization, and assignments
        will be validated against the ASN.1 type conversion constraints. Type
        constrains defined in the ASN.1 file **won't** be checked after the
        assignment.

        .. note::
            Directly setting a value that violates the type (e.g., assigning
            a string to an INTEGER type) will raise a ``TypeError`` or
            ``ValueError``.


ASN.1 Conceptual Enumerated Type
--------------------------------

The ASN.1 ``ENUMERATED`` type (or named ``INTEGER``) defines a finite set of
named integer values. In the generated Python bindings, these are represented as
classes that:

- Contain an embedded ``VALUES`` enumeration (a subclass of ``enum.IntEnum``).
- Store their current value as one of these ``VALUES`` members (or its integer equivalent).
- Provide both integer and enum-based semantics for assignment and comparison.

For example:

.. code-block:: asn1

    MyEnum ::= ENUMERATED {
        firstOption (0),
        secondOption(1),
        thirdOption (2)
    }

will generate a Python class:

.. code-block:: python

    class MyEnum(_BasicAsn1EnumType):
        class VALUES(enum.IntEnum):
            V_firstOption  = 0
            V_secondOption = 1
            V_thirdOption  = 2

.. note::
    Unlike simple ``INTEGER`` aliases, ``ENUMERATED`` and **name** ``INTEGER`` types **always**
    produce a dedicated Python class with its own ``VALUES`` inner enumeration.


All generated Python ENUMERATED classes conform to the following conceptual API:

.. py:class:: _Asn1EnumType

    *Inherits from* :class:`_Asn1Type`.

    Conceptual base class for all ASN.1 ``ENUMERATED`` types.

    .. py:class:: VALUES(enum.IntEnum)

        :text-req:`Required.`

        Inner enumeration containing **all** named values defined in the
        ASN.1 type. Each member is prefixed with ``V_`` to avoid name clashes
        with class-level constants or methods.

        Example:

        .. code-block:: python

            class VALUES(enum.IntEnum):
                V_firstOption = 0
                V_secondOption = 1
                V_thirdOption = 2


    .. py:property:: value
        :type: _BasicAsn1EnumType.VALUES

        :text-req:`Required.`

        Holds the current value of the enumeration as a ``VALUES`` member.

        This property accepts assignment using either:

        - A member of ``VALUES`` (e.g., ``MyEnum.VALUES.V_firstOption``)
        - An integer corresponding to a valid ``VALUES`` member

        .. warning::
            Assigning an integer that does not correspond to any member of
            ``VALUES`` will raise a ``ValueError`` when accessing the value,
            **NOT** when assigning it.


    .. py:method:: __init__(self, value: _BasicAsn1EnumType.VALUES | int | None = None) -> None

        :text-req:`Required.`

        Initializes a new ENUMERATED instance with the given value.
        If ``None`` is provided, the instance is created in an *unset* state.


ASN.1 Conceptual Named BIT STRING Type
--------------------------------------

The ASN.1 ``BIT STRING`` type can be defined with named bit positions, often
used to represent a set of boolean flags. In the generated Python bindings,
these named ``BIT STRING`` types are represented by classes that:

- Contain an embedded ``VALUES`` enumeration (a subclass of ``enum.IntFlag``).
- Store their current value as a ``VALUES`` member, allowing multiple flags
  to be combined using bitwise operators.
- Internally represent their value using a **little-endian aligned**
  :py:mod:`bitarray.bitarray`.

For example:

.. code-block:: asn1

    MyFlags ::= BIT STRING {
        read(0),
        write(1),
        execute(2)
    }

will generate a Python class:

.. code-block:: python

    class MyFlags(_BasicAsn1FlagType):
        class VALUES(enum.IntFlag):
            V_read = 1 << 0
            V_write = 1 << 1
            V_execute = 1 << 2

.. note::
    ``IntFlag`` members behave like integers and support bitwise operations.
    This allows intuitive manipulation of multiple flag values. Be aware that
    inplace bitwise operations **will not** change the underlying value unless
    explicitly assigned afterwards.

All generated Python named BIT STRING classes conform to the following conceptual API:

.. py:class:: _Asn1FlagType

    *Inherits from* :class:`_Asn1Type`.

    Conceptual base class for all ASN.1 named ``BIT STRING`` types.

    .. py:class:: VALUES(enum.IntFlag)

        :text-req:`Required.`

        Inner enumeration containing **all** named flag values defined in the
        ASN.1 type. Each member is prefixed with ``V_`` to avoid name clashes.

        Each flag's value is a power-of-two integer corresponding to its
        bit position in the BIT STRING.

    .. py:property:: value
        :type: _BasicAsn1FlagType.VALUES

        :text-req:`Required.`

        Holds the current set of active flags as a ``VALUES`` member.

        This property accepts assignment using:

        - A ``VALUES`` member or a bitwise combination of members
        - An integer bitmask corresponding to the flags
        - A :class:`bytes` object containing the encoded BIT STRING
        - A :class:`bitarray.bitarray` object (must be little-endian aligned)

        .. warning::
            Assigning a value containing bits that are not defined in
            ``VALUES`` will not raise an error, but those bits will be preserved
            and treated as *unnamed* flags.

    .. py:method:: __init__(self, value: _BasicAsn1FlagType.VALUES | int | bytes | bitarray.bitarray | None = None) -> None

        :text-req:`Required.`

        Initializes a new BIT STRING instance with the given value.
        If ``None`` is provided, the instance is created in an *unset* state.


ASN.1 Conceptual CHOICE Type
----------------------------

The ASN.1 ``CHOICE`` type represents a union-like structure where **only one** of
several possible fields can be set at any given time. This constraint is enforced
in the generated Python classes to ensure data integrity and correct encoding.

Each CHOICE class exposes an internal enumeration ``PRESENT`` that tracks which
field is currently active.

.. py:class:: _Asn1ChoiceType

    *Inherits from* :class:`_Asn1Type`.

    .. py:class:: PRESENT(enum.IntEnum)

        This enumeration contains all possible states corresponding to each member
        of the CHOICE. By convention, members are named ``PR_<member_name>``.
        The default state ``PR_NOTHING`` indicates that no field is currently set.


    .. py:method:: __init__(self, /, **members: Any) -> None

        :text-req:`Required.`

        Initializes a new CHOICE instance by setting exactly **one** field using
        keyword arguments. For example:

        .. code-block:: python

            choice = MyChoice(field1=value1)

        Attempting to set multiple fields simultaneously is disallowed.

    .. attribute:: present
        :type: _BasicAsn1ChoiceType.PRESENT

        :text-req:`Required.`

        Reflects the currently active field of the CHOICE. Reading this property
        returns the corresponding ``PRESENT`` enum member, indicating which
        field is set.


.. admonition:: Behavioral Notes
    :class: note

    - Only one field can be set at a time. Setting a new field automatically clears
      any previously set field.

    - Accessing an unset field returns ``None`` instead of raising an exception.

    - The CHOICE type behaves conceptually like a C union, but with Pythonic
      safety checks and clear state tracking.


ASN.1 Conceptual List Type
--------------------------

The :class:`_Asn1ListType` class is the conceptual base for all generated Python
classes that represent ASN.1 ``SEQUENCE OF`` and ``SET OF`` types.

These ASN.1 types hold an ordered or unordered collection of elements of the
same type:

- ``SEQUENCE OF`` preserves the element order.
- ``SET OF`` does not guarantee ordering in ASN.1 semantics (although Python
  will typically maintain insertion order internally).

In Python, the generated classes behave like mutable sequences (similar to
lists) and implement part of Python's *mapping protocol*, allowing index-based
access and assignment. To convert such an ASN.1 list to a native Python list,
use ``list(my_obj)``.

Example ASN.1 definition:

.. code-block:: asn1

    MySequenceOfIntegers ::= SEQUENCE OF INTEGER

which generates a Python class roughly equivalent to:

.. code-block:: python

    class MySequenceOfIntegers(_Asn1ListType[int]):
        pass

Each generated Python class conforms to the conceptual list type specified
below. However, there are some constrains when using this type:

- **Deletion** is supported with ``del my_obj[index]``, but there is no direct
  ``remove(value)`` method.
- Values assigned to the collection are converted to the target ASN.1 Python
  type automatically (see the :ref:`conversion model <reference_conversion_model>`).
- Mutating the returned Python element directly **will not** update the
  internal ASN.1 representation unless explicitly re-assigned.

.. py:class:: _Asn1ListType[_PY_T]

    *Inherits from* :class:`_Asn1Type`.

    Represents a SEQUENCE OF or SET OF ASN.1 type containing elements
    of type ``_PY_T``.

    .. py:method:: __init__(self, values: Iterable[_PY_T] | None = None) -> None

        :text-req:`Required.`

        Initializes the collection with an optional iterable of elements.


    .. py:method:: __len__(self) -> int

        :text-req:`Required.`

        Returns the number of elements in the collection.


    .. py:method:: __getitem__(self, index: int) -> _PY_T

        :text-req:`Required.`

        Retrieves the element at the specified index.


    .. py:method:: __setitem__(self, index: int, value: _PY_T) -> None

        :text-req:`Required.`

        Replaces the element at the specified index with ``value``.


    .. py:method:: __delitem__(self, index: int) -> None

        :text-req:`Required.`

        Removes the element at the specified index.


    .. py:method:: add(self, value: _PY_T) -> None

        :text-req:`Required.`

        Appends a single element to the end of the collection.


    .. py:method:: extend(self, values: Iterable[_PY_T]) -> None

        :text-req:`Required.`

        Appends multiple elements from the iterable ``values``.


    .. py:method:: clear(self) -> None

        :text-req:`Required.`

        Removes all elements from the collection.



