.. asn1cpython documentation master file, created by
   sphinx-quickstart on Sun Aug 10 22:57:15 2025.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

ASN.1 Compiler for C(Python)
============================

This project was started by `vlm <https://github.com/vlm/asn1c>`_ and then updated
for recent bug fixes by `mouse07410 <https://github.com/mouse07410/asn1c>`_. Any
updates from these projects will be merged into this one.

About
-----

ASN.1 to C compiler takes the ASN.1 module files (example) and generates
the C++ compatible C source code. That code can be used to serialize
the native C structures into compact and unambiguous BER/OER/PER/XER/JER-based
data files, and deserialize the files back.

.. note::
   This version of the ASN.1 compiler includes functionality to generate
   Python bindings automatically - no coding involved.

Various ASN.1 based formats are widely used in the industry,
such as to encode the X.509 certificates employed in the HTTPS handshake,
to exchange control data between mobile phones and cellular networks,
to perform car-to-car communication in intelligent transportation networks.

The ASN.1 family of standards is large and complex, and no open source
compiler supports it in its entirety.
The asn1c is arguably the most evolved open source ASN.1 compiler. Additionally,
using this project, the *asn1c* compiler is now capable of generating Python
bindings.


A simple Example
----------------

Lets consider the following ASN.1 definition using basic types only:

.. code-block:: asn1

   ExampleModule DEFINITIONS AUTOMATIC TAGS ::= BEGIN

      ExampleSequence ::= SEQUENCE {
         foo      INTEGER,
         bar      REAL,
         baz      BOOLEAN
      }

   END


After compiling the extension module e.g. into (``example_mod._example_mod``), the
type above can be used as a generic Python class:

.. code-block:: python

   from example_mod._example_mod import ExampleSequence

   seq = ExampleSequence() # kw-only, all values are optional
   assert seq.foo == 0     # all elements will be initialiazed with zero
   seq.foo = 100           # assignment works just like that
   assert seq.foo == 100

   # to encode or decode using a specific rules, just use the codec name
   # as a prefix: (e.g. BER -> obj.ber_encode, ExampleSequence.ber_decode)
   data = seq.ber_encode()
   parsed = ExampleSequence.ber_decode(data)
   assert parsed.foo == seq.foo


Supported Types
---------------

- Basic Types: ``NULL``, ``INTEGER`` (*named*), ``BOOLEAN``, ``BIT STRING``
  (*named*), ``OCTET STRING``, ``OBJECT IDENTIFIER,``, ``RELATIVE-OID``,
  ``REAL``, ``ENUMERATED``, ``UTF8String`` and variants,

.. note::
   ``UTCTIME`` is currently implemented as raw bytes

- Constructed Types: ``SEQUENCE``, ``CHOICE``, ``SET``, ``SEQUENCE OF``, ``SET
  OF`` (anonymous types are supported too, e.g. ``SET OF SET``)


.. toctree::
   :maxdepth: 2
   :caption: Basics

   installation
   build_system


.. toctree::
   :maxdepth: 2
   :caption: Reference:

   reference/index.rst
