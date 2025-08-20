.. _reference_basic_bitstring:

BIT STRING
==========

The ASN.1 ``BIT STRING`` type represents an arbitrary-length sequence of bits.
It is used to model flags, bitmasks, and other bit-level data. These types store
their bit data internally as a little-endian encoded byte array.

Named BIT STRING types are special variants where each bit is assigned a
meaningful name, represented as members of an ``enum.IntFlag`` class called
``VALUES`` in the generated Python class.

Example ASN.1 definitions:

.. code-block:: asn1

    MyBitString ::= BIT STRING

    MyFlags ::= BIT STRING {
        flag1(0),
        flag2(1),
        flag3(2)
    }

These generate Python classes similar to:

.. code-block:: python

    class MyBitString(_Asn1BasicType[bitarray.bitarray]):
        pass

    class MyFlags(_BasicAsn1FlagType):
        class VALUES(enum.IntFlag):
            V_flag1 = 0
            V_flag2 = 1
            V_flag3 = 2

Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[bitarray]
    :no-index:

    Represents a basic ASN.1 BIT STRING.

    .. py:method:: __init__(self, value: bytes | bitarray.bitarray | None = None) -> None
        :no-index:

        Initializes the BIT STRING instance with optional initial bits.

        The value can be:

        - a bytes object representing the raw bits,
        - a ``bitarray.bitarray`` instance,
        - or ``None`` to initialize an empty BIT STRING.

    .. py:property:: value
        :type: bitarray.bitarray
        :no-index:

        Gets or sets the bit string value in **little-endian** form.

        When setting, you can assign:

        - a bytes object, which will be converted into a bit array,
        - a ``bitarray.bitarray`` object directly.

    .. py:property:: value_BE
        :type: bitarray.bitarray
        :no-index:

        Gets or sets the bit string value in **big-endian** form.

        Supported for **all top-level and named BIT STRING types**.

Named BIT STRING types will be represented by :class:`_Asn1FlagType`.

Usage Notes
-----------

- Bit order within the underlying bit array is **little-endian** aligned by
  default, meaning bit 0 corresponds to the least significant bit of the
  first byte.

- Named bit strings allow intuitive usage of individual flags:

  .. code-block:: python

      flags = MyFlags()
      # Keep in mind that in-place operations are not possible
      flags_value = flags.value
      flags_value[MyFlags.V_flag1] = 1

      flags.value = flags_value
      if flags.value[MyFlags.V_flag2]:
          print("Flag 2 is set")

- The ``value`` property allows seamless conversion between raw bits
  and named flags.

- Direct modifications to the ``bitarray`` returned by ``value`` or ``value_BE`` do not
  affect the stored value unless reassigned.

.. note:: Endian conversion rules:

    - **Top-level and named BIT STRING types** support both :code:`.value`
      (little-endian) and :code:`.value_BE` (big-endian).
    - **Embedded unnamed BIT STRINGs** (e.g., inline fields inside ``SEQUENCE``
      or ``SET`` without a type alias) only support little-endian access via
      :code:`.value`.

    .. code-block:: asn1

        MyFlags ::= BIT STRING { read(0), write(1) }

        MySeq ::= SEQUENCE {
            f1 BIT STRING,         -- only little-endian
            f2 MyFlags             -- supports little-endian and big-endian
        }
