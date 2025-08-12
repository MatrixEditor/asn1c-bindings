.. _reference_basic_null:

NULL
====

The ASN.1 ``NULL`` type represents the presence of a value without carrying any
data. It is most often used in control structures, protocol markers, or to
indicate that a certain choice or option is intentionally left empty.

In Python, the generated ``NULL`` type acts as a singleton-like type that can be
instantiated but always holds the same conceptual meaning: “no value”.

.. note::
    Unlike Python's built-in ``None``, the ASN.1 ``NULL`` type is an actual
    encoded value on the wire — it is represented by a zero-length payload and
    an ASN.1 tag.

Example ASN.1 definition:

.. code-block:: asn1

    MyNull ::= NULL

would generate the following Python class:

.. code-block:: python

    class MyNull(_Asn1BasicType[None]):
        pass


Conceptual Representation
--------------------------

.. py:class:: _Asn1BasicType[None]
    :no-index:

    Represents a NULL value. This type ignores any assigned value and
    always returns ``None`` when accessed.

    .. py:method:: __init__(self) -> None
        :no-index:

        Creates a NULL instance. Since the type has no internal data,
        no parameters are accepted.

    .. py:property:: value
        :type: None
        :no-index:

        Always returns ``None``. Any attempt to set ``value`` will
        overwrite nothing — the result is still ``None``.



