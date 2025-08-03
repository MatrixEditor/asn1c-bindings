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

#define PyCompatLong_FromSsize_t(val) PyLong_FromSsize_t((val))
#define PyCompatLong_FromSize_t(val) PyLong_FromSize_t((val))
#define PyCompatLong_AsSsize_t(obj) PyLong_AsSsize_t(obj)
#define PyCompatLong_AsSize_t(obj) PyLong_AsSize_t(obj)

#define PyBoolCompat_Check(obj, ret)                                \
    if(!PyBool_Check(obj)) {                                        \
        PyErr_Format(PyExc_ValueError,                              \
                     "Expected a boolean but got %R instead", obj); \
        return ret;                                                 \
    }

#define PyCompatBool_FromLong(val) ((val) ? Py_True : Py_False)
#define PyCompatBool_AsLong(obj) (PyObject_IsTrue(obj))


#define PyFloatCompat_Check(obj, ret)                                         \
    if(!PyFloat_Check(obj)) {                                                 \
        PyErr_Format(PyExc_ValueError, "Expected a float but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }

#define PyCompatFloat_FromDouble(val) PyFloat_FromDouble((val))
#define PyCompatFloat_AsDouble(obj) PyFloat_AsDouble(obj)


#define PyCompatBytes_ToStringAndSize(obj, str, size) \
    _PyCompatBytes_ToStringAndSize(obj, (char **)(str), (Py_ssize_t *)(size))

#define PyCompatBytes_FromStringAndSize(str, size) \
    PyBytes_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

static inline int
_PyCompatBytes_ToStringAndSize(PyObject *pObj, char **str, Py_ssize_t *size) {
    *size = PyBytes_Size(pObj);
    *str = (char *)PyMem_RawMalloc(*size);
    if(*str == NULL) {
        return -1;
    }
    char *p = (char *)PyBytes_AsString(pObj);
    if(p == NULL) {
        PyMem_Free(*str);
        return -1;
    }
    memcpy(*str, p, *size);
    return 0;
}

#define PyCompatBitArray_FromStringAndSize(str, size) \
    _PyCompatBitArray_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

static PyObject *
_PyCompatBitArray_FromStringAndSize(const char *str, Py_ssize_t size) {
    PyObject *nResult = NULL, *nTmpBytes = NULL;
    if((nTmpBytes = PyBytes_FromStringAndSize(str, size)) == NULL) {
        goto end;
    }

    nResult = PyObject_CallOneArg(PyCompatTable->PyBitArray_Type, nTmpBytes);
end:
    Py_XDECREF(nTmpBytes);
    return nResult;
}

#define PyCompatBitArray_ToStringAndSize(obj, str, size) \
    _PyCompatBitArray_ToStringAndSize(obj, (char **)(str), (Py_ssize_t *)(size))

static int
_PyCompatBitArray_ToStringAndSize(PyObject *pObj, char **str,
                                  Py_ssize_t *size) {
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

#define PyCompatUnicode_AsUTF8AndSize(obj, size) \
    PyUnicode_AsUTF8AndSize(obj, (Py_ssize_t *)(size))

#define PyCompatUnicode_FromStringAndSize(str, size) \
    PyUnicode_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))


static inline PyObject *
PyCompatEnum_FromSsize_t(PyObject *pEnumType, Py_ssize_t value) {
    PyObject *nValue = NULL, *nResult = NULL;
    if((nValue = PyLong_FromSsize_t(value)) == NULL) {
        goto end;
    }
    nResult = PyObject_CallOneArg(pEnumType, nValue);
end:
    Py_XDECREF(nValue);
    return nResult;
}

static inline PyObject *
PyCompatEnum_FromSize_t(PyObject *pEnumType, size_t value) {
    PyObject *nValue = NULL, *nResult = NULL;
    if((nValue = PyLong_FromSize_t(value)) == NULL) {
        goto end;
    }
    nResult = PyObject_CallOneArg(pEnumType, nValue);
end:
    Py_XDECREF(nValue);
    return nResult;
}

static inline Py_ssize_t
PyCompatEnum_AsSsize_t(PyObject *pObj) {
    PyObject *nValue = NULL;
    if(PyLong_Check(pObj)) {
        return PyLong_AsSsize_t(pObj);
    }

    nValue = PyObject_GetAttrString(pObj, "value");
    if(nValue != NULL) {
        Py_ssize_t result = PyLong_AsSsize_t(nValue);
        Py_XDECREF(nValue);
        return result;
    }
    return -1;
}

static inline size_t
PyCompatEnum_AsSize_t(PyObject *pObj) {
    PyObject *nValue = NULL;
    if(PyLong_Check(pObj)) {
        return PyLong_AsSize_t(pObj);
    }

    nValue = PyObject_GetAttrString(pObj, "value");
    if(nValue != NULL) {
        size_t result = PyLong_AsSize_t(nValue);
        Py_XDECREF(nValue);
        return result;
    }
    return -1;
}

#define PyCompat_SeqItem_Get(obj, attrName, ...)                           \
    do {                                                                   \
        if((value = PyDict_GetItemString(obj, #attrName)) != NULL) {       \
            __VA_ARGS__;                                                   \
        } else {                                                           \
            PyErr_Clear();                                                 \
            if((value = PyObject_GetAttrString(obj, #attrName)) != NULL) { \
                __VA_ARGS__;                                               \
                Py_DECREF(value);                                          \
            } else                                                         \
                PyErr_Clear();                                             \
        }                                                                  \
    } while(0)

#define PyCompat_SeqItem_Set(obj, attrName, ...)                  \
    do {                                                          \
        if((value = (__VA_ARGS__)) != NULL) {                     \
            if(PyDict_SetItemString(obj, #attrName, value) < 0) { \
                goto error;                                       \
            }                                                     \
            Py_DECREF(value);                                     \
        } else {                                                  \
            goto error;                                           \
        }                                                         \
    } while(0)


#endif