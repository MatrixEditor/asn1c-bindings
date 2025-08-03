#include "py_application.h"

/*global state storing extra Python objects*/
PyCompatTable_t *PyCompat_Table = NULL;

void
PyCompat_Clear(void) {
    Py_CLEAR(PyCompat_Table->PyBytesIO_Type);
    Py_CLEAR(PyCompat_Table->str__getvalue);
    Py_CLEAR(PyCompat_Table->str__write);
    PyMem_FREE(PyCompat_Table);
    PyCompat_Table = NULL;
}

int
PyCompat_Init(void) {
#define _CACHED_STRING(state, attr, str, label) \
    if(((state)->attr = PyUnicode_InternFromString((str))) == NULL) goto label

#define _IMPORT_ATTR(ext_mod, attr, target)                             \
    if((target = PyObject_GetAttrString((ext_mod), (attr)), !target)) { \
        goto error;                                                     \
    }

    PyObject *nTmpModule = NULL;
    PyCompat_Table = PyMem_New(PyCompatTable_t, 1);
    if(PyCompat_Table == NULL) {
        return -1;
    }

    _CACHED_STRING(PyCompat_Table, str__getvalue, "getvalue", error);
    _CACHED_STRING(PyCompat_Table, str__write, "write", error);

    nTmpModule = PyImport_ImportModule("io");
    if(!nTmpModule) {
        goto error;
    }
    _IMPORT_ATTR(PyCompat_Table->PyBytesIO_Type, "BytesIO",
                 PyCompat_Table->PyBytesIO_Type);
    Py_CLEAR(nTmpModule);

    nTmpModule = PyImport_ImportModule("bitarray");
    if(!nTmpModule) {
        goto error;
    }
    _IMPORT_ATTR(PyCompat_Table->PyBitArray_Type, "bitarray",
                 PyCompat_Table->PyBitArray_Type);
    Py_CLEAR(nTmpModule);

    return PyCompat_Table->PyBytesIO_Type ? 0 : -1;

error:
    return -1;

#undef _CACHED_STRING
#undef _IMPORT_ATTR
}