.. _reference_basic_bitstring:

BIT STRING
==========

The ASN.1 ``BIT STRING`` type represents an arbitrary-length sequence of bits.
It is used to model flags, bitmasks, and other bit-level data. These types store
their bit data internally as a little endian encoded byte array.

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
            V_flag1 = 1 << 0
            V_flag2 = 1 << 1
            V_flag3 = 1 << 2

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

        Gets or sets the bit string value.

        When setting, you can assign:

        - a bytes object, which will be converted into a bit array,
        - a ``bitarray.bitarray`` object directly.

        .. note::
            Direct mutation of the returned bitarray will NOT update the
            internal stored value unless reassigned explicitly.

Named BIT STRING types will be represented by :class:`_Asn1FlagType`.

Usage Notes
-----------

- Bit order within the underlying bit array is **LITTLE ENDIAN** aligned,
  meaning bit 0 corresponds to the least significant bit of the first byte.

- Named bit strings allow intuitive usage of individual flags:

  .. code-block:: python

      flags = MyFlags()
      flags.value = MyFlags.V_flag1 | MyFlags.V_flag3  # Set flag1 and flag3
      if flags.value & MyFlags.V_flag2:
          print("Flag 2 is set")

- The ``value`` property allows seamless conversion between raw bits
  and named flags.

- Direct modifications to the ``bitarray`` returned by ``value`` do not
  affect the stored value unless reassigned.


