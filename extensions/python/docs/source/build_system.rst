.. _build_system:

Setting up a build system
=========================


The recommended way to include the modified *asn1c* compiler into a Python
package is by using a CMake-based build system. While it is possible to use
other build systems, they are **not officially supported** and may require
significant manual integration work.

In this small tutorial, we will create a ``CMakeLists.txt``-based build system for a Python
extension module, using `hatchling <https://hatch.pypa.io/>`_ as the project builder
and `scikit-build-core <https://scikit-build-core.readthedocs.io>`_ to integrate
CMake into the Python packaging process.

.. hint::
  You may also want to take a look at the example module in the repository on GitHub
  that implements a basic extension and a submodule extension here: `example_mod <https://github.com/MatrixEditor/asn1c-bindings/tree/vlm_master/extensions/python/example-mod>`_

Overview
--------

This build setup relies on several components working together:

- **asn1cpython-support**:
  A Python package that provides helper utilities for ASN.1 compilation and
  integration. It offers a CLI interface to retrieve the installation path of
  the ``asn1c-bindings`` CMake configuration files.

- **asn1c-bindings**:
  A CMake package that exports helper functions, including
  :cmake:command:`asn1c_add_extension()`, which generates ASN.1 C sources at
  configure time, compiles them into Python extension modules, copies stub
  files, and ensures correct installation paths.

- **scikit-build-core** (or compatible PEP 517 build backend): Used as the
  Python build backend, facilitating CMake-driven builds integrated into
  standard Python packaging workflows.

Prerequisites
-------------

Before you begin, ensure the following:

* **CMake** between **3.15** and **3.27** (inclusive).
* The ASN.1 compiler toolchain (``asn1c``) is installed and accessible via your system PATH,
  or configured via environment variables or CMake defines such as ``A1C_PATH``.
* Python **3.8** or newer.
* A working installation of ``pip`` and ``hatch``.

Project Structure Example
-------------------------

A minimal project might look like this:

.. code-block::

    example-mod/
    ├── CMakeLists.txt
    ├── example.asn
    ├── src/
    │   ├── example_mod/ # Python sources go here
    │   └── generated/   # Populated by the ASN.1 compiler at configure time
    ├── pyproject.toml
    └── README.md

In this example:

* ``example.asn`` is your ASN.1 schema.
* ``src/generated`` will hold generated C and Python files during the configure step.
* ``CMakeLists.txt`` defines how to build your extension.
* ``pyproject.toml`` configures the Python overall project.

CMake Setup
-----------

CMake is a cross-platform build system that uses ``CMakeLists.txt`` files to define
build rules. In our case, we need a total of three steps to create a new
extension:

1. Finding the CMake package
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``asn1cpython-support`` package includes a helper command that tells CMake
where the ``asn1c-bindings`` package is installed. The following snippet can be
used to insert the required package into the search path of CMake:

.. code-block:: cmake

    # Retrieve the path to asn1c-bindings CMake package via the Python helper
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m asn1cpython_support --cmake-dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE asn1c-bindings_ROOT
    )
    # Load the asn1c-bindings CMake package
    find_package(asn1c-bindings CONFIG REQUIRED)

.. note::

    You must list ``asn1cpython-support`` in the ``[build-system] requires``
    section of your ``pyproject.toml``, otherwise CMake will not be able to find
    the helper. For instance:

    .. code-block:: toml

        [build-system]
        # just add the python package as a build dependency
        requires = ["hatchling", "scikit-build-core~=0.9.0", "asn1cpython-support"]


2. Adding a new extension
~~~~~~~~~~~~~~~~~~~~~~~~~

The :cmake:command:`asn1c_add_extension()` function declares a Python extension
module that will be generated and compiled automatically. This call performs
multiple actions, which include running the ``asn1c`` compiler at configure time
to generate C and Python interface files, adds them to a new Python extension
and ensures the generated ``.pyi`` type stub file is renamed and installed
correctly.

.. code-block:: cmake

    # Add an ASN.1 extension module.
    asn1c_add_extension(
        NAME _example_mod
        ASN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/example.asn
    )


Python Packaging Integration
----------------------------

When using ``scikit-build-core`` together with *hatchling*, your
``pyproject.toml`` could look like this:

.. code-block:: toml
    :caption: pyproject.toml

    [build-system]
    requires = ["hatchling", "scikit-build-core~=0.9.0", "asn1cpython-support"]
    build-backend = "hatchling.build"

    [project]
    name = "example-mod"
    version = "1.0.0"
    description = "Example Python extension built from ASN.1"
    readme = "README.md"
    dependencies = ["bitarray"]  # Needed for BIT STRING type

    [tool.hatch.build.targets.wheel.hooks.scikit-build]
    experimental = true

    # Optional: define custom compiler or skeleton paths
    # [tool.scikit-build.cmake.define]
    # A1C_PATH = "path/to/custom/asn1c"
    # A1C_SKELETONS_PATH = "path/to/skeletons"



This configuration allows you to build your package with:

.. code-block:: bash

    pip install .
    # or
    hatch build


