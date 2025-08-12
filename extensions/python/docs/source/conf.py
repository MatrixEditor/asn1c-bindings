# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "asn1-cpython"
copyright = "2025, MatrixEditor @ github, mouse07410 @ github, Lev Walkin <vlm@lionet.info>, ..."
author = "MatrixEditor @ github, mouse07410 @ github, Lev Walkin <vlm@lionet.info>, ..."
release = "0.9.27"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "sphinx.ext.todo",
    "sphinx.ext.viewcode",
    "sphinx_copybutton",
    "sphinxcontrib.moderncmakedomain"
]

# The master toctree document.
master_doc = "index"

# The suffix of source filenames.
source_suffix = ".rst"

templates_path = ["_templates"]
exclude_patterns = []

language = "en"

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This patterns also effect to html_static_path and html_extra_path
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = "sphinx"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_title = "asn1-cpython"
html_theme = "furo"
html_static_path = ["_static"]
html_css_files = ["theme.css"]
