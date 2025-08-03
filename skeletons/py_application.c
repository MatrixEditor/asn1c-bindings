#include "py_application.h"

/*global state storing extra Python objects*/
PyCompatTable_t *PyCompatTable = NULL;

void
PyCompat_Clear(void) {
    Py_CLEAR(PyCompatTable->PyBytesIO_Type);
    Py_CLEAR(PyCompatTable->PyBitArray_Type);
    Py_CLEAR(PyCompatTable->PyEnum_Type);
    Py_CLEAR(PyCompatTable->PyEnumMeta_Type);
    Py_CLEAR(PyCompatTable->str__getvalue);
    Py_CLEAR(PyCompatTable->str__write);
    Py_CLEAR(PyCompatTable->str__prepare);
    PyMem_RawFree(PyCompatTable);
    PyCompatTable = NULL;
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
    PyCompatTable = PyMem_RawMalloc(sizeof(PyCompatTable_t));
    if(PyCompatTable == NULL) {
        return -1;
    }

    _CACHED_STRING(PyCompatTable, str__getvalue, "getvalue", error);
    _CACHED_STRING(PyCompatTable, str__write, "write", error);
    _CACHED_STRING(PyCompatTable, str__prepare, "__prepare__", error);

    nTmpModule = PyImport_ImportModule("io");
    if(!nTmpModule) {
        goto error;
    }
    _IMPORT_ATTR(nTmpModule, "BytesIO", PyCompatTable->PyBytesIO_Type);
    Py_CLEAR(nTmpModule);

    nTmpModule = PyImport_ImportModule("bitarray");
    if(!nTmpModule) {
        goto error;
    }
    _IMPORT_ATTR(nTmpModule, "bitarray", PyCompatTable->PyBitArray_Type);
    Py_CLEAR(nTmpModule);

    nTmpModule = PyImport_ImportModule("enum");
    if(!nTmpModule) {
        goto error;
    }
    _IMPORT_ATTR(nTmpModule, "Enum", PyCompatTable->PyEnum_Type);
    _IMPORT_ATTR(nTmpModule, "EnumType", PyCompatTable->PyEnumMeta_Type);
    Py_CLEAR(nTmpModule);

    return PyCompatTable->PyBytesIO_Type ? 0 : -1;

error:
    return -1;

#undef _CACHED_STRING
#undef _IMPORT_ATTR
}