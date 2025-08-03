#ifndef _PyConvert_H_
#define _PyConvert_H_

#include <Python.h>
#include "py_application.h"

#define PyLongCompat_Check(obj, ret)                                 \
    if(!PyLong_Check(obj)) {                                         \
        PyErr_Format(PyExc_ValueError,                               \
                     "Expected an integer but got %R instead", obj); \
        return ret;                                                  \
    }

#define PyBoolCompat_Check(obj, ret)                                \
    if(!PyBool_Check(obj)) {                                        \
        PyErr_Format(PyExc_ValueError,                              \
                     "Expected a boolean but got %R instead", obj); \
        return ret;                                                 \
    }

#define PyFloatCompat_Check(obj, ret)                                         \
    if(!PyFloat_Check(obj)) {                                                 \
        PyErr_Format(PyExc_ValueError, "Expected a float but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }


static inline int
PyCompatBytes_ToStringAndSize(PyObject *pObj, char **str, Py_ssize_t *size) {
    *size = PyBytes_Size(pObj);
    *str = (char *)PyMem_Malloc(*size);
    if(*str == NULL) {
        return -1;
    }

    memcpy(*str, PyBytes_AsString(pObj), *size);
    return 0;
}

static PyObject *
PyCompatBitArray_FromStringAndSize(const char *str, Py_ssize_t size) {
    PyObject *nResult = NULL, *nTmpBytes = NULL;
    if((nTmpBytes = PyBytes_FromStringAndSize(str, size)) == NULL) {
        goto end;
    }

    nResult = PyObject_CallOneArg(PyCompatTable->PyBitArray_Type, nTmpBytes);
end:
    Py_XDECREF(nTmpBytes);
    return nResult;
}

/* Equal to
def to_bytes(obj: bitarray) -> bytes:
    return bytes(obj)
*/
static int
PyCompatBitArray_ToStringAndSize(PyObject *pObj, char **str, Py_ssize_t *size) {
    PyObject *nTmpBytes = NULL;
    int result = 0;
    if((nTmpBytes = PyBytes_FromObject(pObj)) == NULL) {
        goto error;
    }

    result = PyCompatBytes_ToStringAndSize(nTmpBytes, str, size);
    goto end;

error:
    result = -1;

end:
    Py_XDECREF(nTmpBytes);
    return result;
}


#endif