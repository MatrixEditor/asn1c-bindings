.. _reference_object_model:

ASN.1 Object Model
------------------

Each generated Python class representing an ASN.1 type acts as a **wrapper** around
an internal native (C-extension) representation. To provide a natural Python interface,
accessing or assigning the value of an ASN.1 object involves conversion between
the native format and native Python types.

**Reading the Value**

When you access the ``value`` property of an ASN.1 object, the underlying native
value is converted **on-demand** to the corresponding Python object, for example:

- ASN.1 ``INTEGER`` converts to a Python ``int``
- ASN.1 ``BOOLEAN`` converts to a Python ``bool``
- ASN.1 ``OCTET STRING`` converts to Python ``bytes``
- ASN.1 named ``BIT STRING`` converts to an ``enum.IntFlag`` or
  :class:`bitarray.bitarray`, depending on the type

This conversion produces a **new** Python object representing the current state.

**Writing to the Value**

Assigning to the ``value`` property performs the inverse conversion: the provided
Python object is converted back into the native ASN.1 internal format.

**Important Note**

Modifying the Python object obtained from ``value`` does **not** affect the
underlying ASN.1 object unless the modified object is explicitly assigned back
to the ``value`` property.

For example:

.. code-block:: python

    # Retrieve a Python integer representing the ASN.1 INTEGER value
    py_int = asn1_integer.value

    # Modifying py_int has no effect on asn1_integer internally
    py_int += 1
    assert asn1_integer.value != py_int

    # To update the ASN.1 object, assign the modified value explicitly
    asn1_integer.value = py_int



