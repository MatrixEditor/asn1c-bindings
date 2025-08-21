.. _reference_basic_bitstring:

BIT STRING
==========

The ASN.1 ``BIT STRING`` type represents an arbitrary-length sequence of bits.
It is used to model flags, bitmasks, and other bit-level data. In the generated
Python bindings, **all** ``BIT STRING`` types implement type behavior described
in :class:`_Asn1BitStrType`.

Named ``BIT STRING`` types additionally expose named boolean attributes for
convenient flag access.

Example ASN.1 definitions:

.. code-block:: asn1

    ExampleBitString ::= BIT STRING

    ExampleNamedBitString ::= BIT STRING {
        zero(0),
        one(1),
        two(2)
    }

These generate Python classes similar to:

.. code-block:: python

    class ExampleBitString(_Asn1BitStrType):
        pass

    class ExampleNamedBitString(_Asn1BitStrType):
        V_zero: bool  # bit 0
        V_one: bool   # bit 1
        V_two: bool   # bit 2

Conceptual Representation
--------------------------

.. py:class:: _Asn1BitStrType

    Base class for all ASN.1 ``BIT STRING`` types.

    .. py:method:: __init__(self, size: int = ...) -> None

        Initializes a ``BIT STRING`` with the given number of **bytes**.

    .. py:property:: value
        :type: bitarray.bitarray | bytes

        Gets or sets the raw value of the ``BIT STRING``.

        - Assignment accepts either a :class:`bytes` object or a
          :class:`bitarray.bitarray`.
        - The returned object can be modified directly, but must be reassigned
          to take effect.

    .. py:method:: clear() -> None

        Clears all bits (sets them to ``0``).

    .. py:method:: set(bit: int, flag: bool) -> None

        Sets the given ``bit`` position to ``True`` or ``False``.

    .. py:method:: get(bit: int) -> bool

        Returns the boolean state of the given ``bit`` position.

    .. py:method:: size() -> int

        Returns the current number of **bytes** in the ``BIT STRING``.

    .. py:method:: resize(size: int) -> None

        Adjusts the number of **bits** in the ``BIT STRING``.



Usage Notes
-----------

- Unlike integer masks, bits can be manipulated directly using
  :func:`_Asn1BitStrType.set` and :func:`_Asn1BitStrType.get`.

  .. code-block:: python

      bs = ExampleBitString(size=8)
      bs.set(3, True)   # set bit 3
      print(bs.get(3))  # True

- Named ``BIT STRING`` types provide boolean attributes for each flag:

  .. code-block:: python

      flags = ExampleNamedBitString()
      flags.V_one = True
      if flags.V_one:
          print("Flag 'one' is set")

- For very small ``BIT STRING`` types (≤ 1 byte):
  If they are encoded using two bytes of space, the queried bit will always be
  positioned on the **last octet internally**.