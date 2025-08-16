.. _reference_cmake_api:

=========
CMake API
=========

The ``asn1cpython-support`` package provides several CMake functions to assist
with generating ASN.1-based C extensions for Python. These functions encapsulate
source generation, compilation, stub handling, and installation details.

.. cmake:command:: asn1c_add_extension

    Generates C sources from ASN.1 specification files and creates a Python C
    extension module target.

    .. code-block:: cmake

        asn1c_add_extension(
            NAME <module_name>
            ASN_FILES <asn_file1> [<asn_file2> ...]
            [SUBMODULE]
            [CUSTOM_OUTPUT]
            [PY_SRC_DIR <dir>]
            [PY_STUB_DIR <dir>]
            [PY_H_DIR <dir>]
            [C_SRC_DIR <dir>]
            [C_H_DIR <dir>]
            [SK_OUT_DIR <dir>]
        )

    :param NAME:
        The name of the Python extension module target (e.g. ``_example_mod``)
        **without** the project's package name. May include dots for submodules,
        e.g., ``submod._foo``.

        .. note::

            If you intend to add the extension as a submodule within the project
            (e.g. ``mypackage.submod._extension``) the ``SUBMODULE`` option has
            to be set additionally.

    :param ASN_FILES:
        One or more ASN.1 specification files to compile.

    :param SUBMODULE:
        Indicates the extension is placed in a submodule within project's
        package.

    **Output Control Options**

    These options provide **granular control** over where generated files are
    placed. If omitted, files are written under
    ``${CMAKE_CURRENT_SOURCE_DIR}/src/generated/${NAME}`` by default.

    .. important::

      If you specify a custom output directory other than ``SK_OUT_DIR``, you
      **MUST** specify ``CUSTOM_OUTPUT`` to disable creation of the above
      mentioned directory.


    :param PY_SRC_DIR:
      Directory for generated **Python C source files**.
      Passed to ``asn1c`` as ``-out-py-src``.

    :param PY_STUB_DIR:
      Directory for generated **Python stub files** (``.pyi``).
      Passed to ``asn1c`` as ``-out-py-stub``.

    :param PY_H_DIR:
      Directory for generated **Python-specific C headers**.
      Passed to ``asn1c`` as ``-out-py-h``.

    :param C_SRC_DIR:
      Directory for generated **plain C source files**.
      Passed to ``asn1c`` as ``-out-c-src``.

    :param C_H_DIR:
      Directory for generated **plain C headers**.
      Passed to ``asn1c`` as ``-out-c-h``.

    :param SK_OUT_DIR:
      Directory for skeleton files.
      Passed to ``asn1c`` as ``-out-skeletons``.


    .. note::

      By default, a combined ``A1C_GENERATED_DIR`` is created under
      ``src/generated/${NAME}``. All generated sources are placed here unless
      more specific directories are supplied.

      If you want to fully control output placement, define:

      .. code-block:: cmake

          set(A1C_EXT_CUSTOM_OUTPUT ON)

      or pass ``-DA1C_EXT_CUSTOM_OUTPUT=ON`` to CMake, ot just set
      ``CUSTOM_OUTPUT`` when calling this function.
      In this mode, ``asn1c_add_extension`` will **not** create or manage
      ``A1C_GENERATED_DIR`` and expects explicit directory options instead.

    **Behavior**

    In its core, this function:

    * Runs the ASN.1 compiler (``asn1c``) at **CMake configure time**,
      generating sources into a dedicated ``src/generated/<module_path>/``
      directory.
    * Automatically collects all generated `.c` files afterwards.
    * Creates a Python extension module target with ``python_add_library()``.
    * Copies and renames the stub file ``py_module.pyi`` to
      ``<module_name>.pyi`` in the generated sources directory.
    * Handles installation of the compiled module and stub file, respecting
      submodule namespaces (dots in module names converted to subdirectories).

    Example:

    .. code-block:: cmake

        # Simple Module
        # - saved to: ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/${A1C_EXT_NAME}
        asn1c_add_extension(
            NAME _example_mod
            ASN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/example.asn"
        )

        # Simple Submodule
        # - saved to: ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/${A1C_EXT_NAME}
        asn1c_add_extension(
            NAME submodule._example_mod
            ASN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/example_submodule.asn"
            SUBMODULE
        )

        # Simple Module with custom output directories
        # - skeletons saved_to: ${CMAKE_CURRENT_SOURCE_DIR}/skeletons
        # - sources saved_to: ${CMAKE_CURRENT_SOURCE_DIR}/sources
        # - pyi saved_to: ${CMAKE_CURRENT_SOURCE_DIR}/sources
        asn1c_add_extension(
          NAME _example_mod
          PY_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources
          PY_STUB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources
          SK_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/skeletons
          CUSTOM_OUTPUT
          ASN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/example.asn
        )


.. cmake:command:: asn1c_generate

  Runs the ASN.1 compiler (`asn1c`) to generate the C source files and Python
  type stubs for the specified extension module. This is typically called at
  **CMake configure time** so that the generated sources are available before
  the build step.

  Even though this command does not take any input arguments, certain variables
  **must** be set: :cmake:variable:`A1C_PATH`,
  :cmake:variable:`A1C_EXT_NAME`, :cmake:variable:`A1C_EXT_BASENAME`,
  :cmake:variable:`A1C_SKELETONS_PATH`,  :cmake:variable:`A1C_GENERATED_DIR`,
  :cmake:variable:`A1C_BER`,  :cmake:variable:`A1C_OER`,
  :cmake:variable:`A1C_UPER`,  :cmake:variable:`A1C_PER`,
  :cmake:variable:`A1C_XER`, :cmake:variable:`A1C_JER`,
  :cmake:variable:`A1C_PRINT` and :cmake:variable:`A1C_SOURCE_FILES`.
  Optionally, extra compiler arguments can be set using
  :cmake:variable:`A1C_EXTRA_ARGS`.

  Example:

  .. code-block:: cmake

    # Make sure all variables are set before
    asn1c_generate()


.. cmake:command:: asn1c_install_stub

  Installs the generated Python type stub file (``.pyi``) into the correct package
  directory during ``make install``.

  :param DESTINATION_DIR:

    The install path inside the Python package (relative to the package root).

  Additionally, the following variables **must** be
  set::cmake:variable:`A1C_GENERATED_DIR` and :cmake:variable:`A1C_EXT_BASENAME`

  Example:

  .. code-block:: cmake

    # Installs the stub file "_example_mod.pyi" into the current project as a
    # simple top-level module: PROJECT._example_mod
    set(A1C_EXT_BASENAME "_example_mod")
    set(A1C_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/generated")
    asn1c_install_stub(${SKBUILD_PROJECT_NAME})


Core Variables
--------------


.. cmake:variable:: A1C_EXT_NAME

  Argument to :cmake:command:`asn1c_add_extension` (``NAME``)

  Logical name of the ASN.1 extension (used for messages, install paths, and generation directory naming).

.. cmake:variable:: A1C_EXT_SUBMODULE

  Argument to :cmake:command:`asn1c_add_extension` (``SUBMODULE``)

  Boolean flag; if set, the extension is part of a Python submodule (``foo.bar`` → ``foo/bar``).

.. cmake:variable:: A1C_EXT_BASENAME

  Derived in :cmake:command:`asn1c_add_extension`

  Base name of the extension module file (no path, e.g. ``_mod``).

.. cmake:variable:: A1C_EXT_PATH

  Derived in :cmake:command:`asn1c_add_extension`

  Filesystem path to the module's location (submodule path if ``SUBMODULE`` is set, otherwise equal to ``A1C_EXT_NAME``).

.. cmake:variable:: A1C_EXT_ASN_FILES

  Argument to :cmake:command:`asn1c_add_extension` (``ASN_FILES``)

  Optional explicit list of ``.asn`` ASN.1 schema files for this extension.

.. cmake:variable:: A1C_ASN_FILES

  Global CMake variable or environment variable

  Optional project-wide override specifying ASN.1 files if not given in ``ASN_FILES``. Can be semicolon-, comma-, or space-separated.

.. cmake:variable:: A1C_GENERATED_DIR

  Set via ``set_from_env_or_default`` in :cmake:command:`asn1c_add_extension`

  Directory where ``asn1c`` outputs generated ``.c``, ``.h``, and ``.pyi`` files. Defaults to ``src/generated/<A1C_EXT_NAME>`` in the current source directory.

.. cmake:variable:: A1C_PATH

  Environment or CMake variable

  Path to the ``asn1c`` compiler executable.

.. cmake:variable:: A1C_SKELETONS_PATH

  Environment or CMake variable

  Path to the ASN.1C skeleton files (used to generate code with proper boilerplate).

.. cmake:variable:: A1C_BER
                    A1C_OER
                    A1C_UPER
                    A1C_PER
                    A1C_XER
                    A1C_JER
                    A1C_PRINT

  Environment or CMake variables (optional)

  Encoding rules options passed to ``asn1c`` to enable specific encoders/decoders (e.g., BER, OER, PER, XER, JER).

.. cmake:variable:: A1C_EXTRA_ARGS

  Environment or CMake variable (optional)

  Additional custom arguments to pass to ``asn1c``.

