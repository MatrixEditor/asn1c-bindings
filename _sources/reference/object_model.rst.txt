.. _reference_object_model:

ASN.1 Object Model
------------------

Each generated Python class representing an ASN.1 type acts as a **wrapper** around
an internal native (C-extension) representation. To provide a natural Python interface,
accessing or assigning the value of an ASN.1 object involves conversion between
the native format and native Python types.

Reading the Value
~~~~~~~~~~~~~~~~~

When you access the ``value`` property of an ASN.1 object, the underlying native
value is converted **on-demand** to the corresponding Python object, for example:

- ASN.1 ``INTEGER`` converts to a Python ``int``
- ASN.1 ``BOOLEAN`` converts to a Python ``bool``
- ASN.1 ``OCTET STRING`` converts to Python ``bytes``
- ASN.1 ``BIT STRING`` always generates a new class optionally with named members

This conversion produces a **new** Python object representing the current state.

Writing to the Value
~~~~~~~~~~~~~~~~~~~~

Assigning to the ``value`` property performs the inverse conversion: the provided
Python object is converted back into the native ASN.1 internal format.

.. admonition:: Important Note
  :class: note

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


.. important::
  When an attribute of an ASN.1 object is itself represented by another **generated class**
  (e.g., a nested ``SEQUENCE``, ``SET``, or a user-defined type wrapping a basic ASN.1 type),
  accessing that attribute does **not** return a copy of the value, but a direct reference to
  the contained object.


  This means that **modifying the returned object will also modify the parent**.
  Unlike attributes that map directly to Python native types (``int``, ``str``, ``bool``, etc.)
  — which require explicitly reassigning the attribute to update the encoded value —
  changes made to generated-class attributes are immediately reflected in the parent
  object's internal state.

  Example:

  .. code-block:: python

      person = Person()
      person.name = NameType(given="John", family="Smith")

      # Access the nested NameType object
      n = person.name

      # Modifying the nested object updates the parent directly
      n.given = "Jonathan"

      assert person.name.given == "Jonathan"  # True

  This behavior is intentional and allows in-place updates of complex ASN.1 structures
  without requiring reassignment of the entire sub-object.