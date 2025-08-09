#ifndef _PyConvert_H_
#define _PyConvert_H_

#include <Python.h>
#include "py_application.h"

#define PyCompat_ArgCheck(obj, ret) \
    if(!obj) {                      \
        PyErr_BadArgument();        \
        return ret;                 \
    }

#define PyCompatLong_Check(obj, ret)                                 \
    PyCompat_ArgCheck(obj, ret);                                     \
    if(!PyLong_Check(obj) || Py_IsNone(obj)) {                       \
        PyErr_Format(PyExc_ValueError,                               \
                     "Expected an integer but got %R instead", obj); \
        return ret;                                                  \
    }

#define PyCompatLong_FromSsize_t(val) PyLong_FromSsize_t((Py_ssize_t)(val))
#define PyCompatLong_FromSize_t(val) PyLong_FromSize_t((size_t)(val))
#define PyCompatLong_AsSsize_t(obj) PyLong_AsSsize_t(obj)
#define PyCompatLong_AsSize_t(obj) PyLong_AsSize_t(obj)

static inline int
PyCompatLong_FromObject(PyObject *pObj, void *val, int is_signed) {
    PyCompatLong_Check(pObj, -1);
    if(is_signed) {
        *((long *)val) = PyLong_AsLong(pObj);
    } else {
        *((unsigned long *)val) = PyLong_AsUnsignedLong(pObj);
    }
    return 0;
}

static inline PyObject *
PyCompatLong_AsObject(void *val, int is_signed) {
    if(is_signed) {
        return PyLong_FromLong((*(long *)val));
    } else {
        return PyLong_FromUnsignedLong((*(unsigned long *)val));
    }
}

#define PyCompatBool_Check(obj, ret)                                \
    PyCompat_ArgCheck(obj, ret);                                    \
    if(!PyBool_Check(obj) || Py_IsNone(obj)) {                      \
        PyErr_Format(PyExc_ValueError,                              \
                     "Expected a boolean but got %R instead", obj); \
        return ret;                                                 \
    }

#define PyCompatBool_FromLong(val) ((val) ? Py_True : Py_False)
#define PyCompatBool_AsLong(obj) (PyObject_IsTrue(obj))

static inline int
PyCompatBool_FromObject(PyObject *pObj, unsigned *val) {
    *val = PyObject_IsTrue(pObj);
    return *val == -1 ? -1 : 0;
}

#define PyCompatNull_AsLong(obj) (0)
#define PyCompatNull_FromLong(val) Py_None

static inline int
PyCompatNull_FromObject(PyObject *pObj, int *val) {
    *val = 0;
    return 0;
}

#define PyCompatFloat_Check(obj, ret)                                         \
    PyCompat_ArgCheck(obj, ret);                                              \
    if(!PyFloat_Check(obj) || Py_IsNone(obj)) {                               \
        PyErr_Format(PyExc_ValueError, "Expected a float but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }

#define PyCompatFloat_FromDouble(val) PyFloat_FromDouble((double)(val))
#define PyCompatFloat_AsDouble(obj) PyFloat_AsDouble(obj)

static inline int
PyCompatFloat_FromObject(PyObject *pObj, void *val, int is_float) {
    PyCompatFloat_Check(pObj, -1);
    if(is_float) {
        *((float *)val) = (float)PyFloat_AS_DOUBLE(pObj);
    } else {
        *((double *)val) = PyFloat_AS_DOUBLE(pObj);
    }
    return 0;
}

static inline PyObject *
PyCompatFloat_AsObject(void *val, int is_float) {
    if(is_float) {
        return PyFloat_FromDouble((double)(*(float *)val));
    } else {
        return PyFloat_FromDouble((double)(*(double *)val));
    }
}


#define PyCompatUnicode_Check(obj, ret)                             \
    PyCompat_ArgCheck(obj, ret);                                    \
    if(!PyUnicode_Check(obj) || Py_IsNone(obj)) {                   \
        PyErr_Format(PyExc_ValueError,                              \
                     "Expected a string but got %R instead.", obj); \
        return ret;                                                 \
    }

#define PyCompatBytes_ToStringAndSize(obj, str, size) \
    _PyCompatBytes_ToStringAndSize(obj, (char **)(str), (Py_ssize_t *)(size))

#define PyCompatBytes_FromStringAndSize(str, size) \
    PyBytes_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

#define PyCompatBytes_Check(obj, ret)                                         \
    PyCompat_ArgCheck(obj, ret);                                              \
    if(!PyBytes_Check(obj) || Py_IsNone(obj)) {                               \
        PyErr_Format(PyExc_ValueError, "Expected a bytes but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }

static inline int
_PyCompatBytes_ToStringAndSize(PyObject *pObj, char **str, Py_ssize_t *size) {
    PyCompatBytes_Check(pObj, -1);
    *size = PyBytes_Size(pObj);
    if(*str) {
        PyMem_Free(*str);
    }
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

static inline PyObject *
PyCompatBitArray_New(PyObject *pBytesObj) {
    PyObject *nArgs = NULL, *nKwargs = NULL, *nResult = NULL;

    if((nArgs = PyTuple_New(pBytesObj ? 1 : 0)) && (nKwargs = PyDict_New())) {
        /* PyTuple_SetItem:
         * This function “steals” a reference to o and discards a reference to
         * an item already in the tuple at the affected position.
         */
        if(!pBytesObj || PyTuple_SetItem(nArgs, 0, Py_NewRef(pBytesObj)) == 0) {
            if(PyDict_SetItem(nKwargs, PyCompatTable->str__endian,
                              PyCompatTable->str__little)
               == 0) {
                nResult = PyObject_Call(
                    (PyObject *)PyCompatTable->PyBitArray_Type, nArgs, nKwargs);
            }
        }
    }
    Py_XDECREF(nArgs);
    Py_XDECREF(nKwargs);
    return nResult;
}

#define PyCompatBitArray_FromStringAndSize(str, size) \
    _PyCompatBitArray_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

static PyObject *
_PyCompatBitArray_FromStringAndSize(const char *str, Py_ssize_t size) {
    PyObject *nResult = NULL, *nTmpBytes = NULL;
    if((nTmpBytes = PyBytes_FromStringAndSize(str, size)) == NULL) {
        goto end;
    }

    nResult = PyCompatBitArray_New(nTmpBytes);
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
    if(!PyBytes_CheckExact(pObj)) {
        if((nTmpBytes =
                PyObject_CallMethodNoArgs(pObj, PyCompatTable->str__to_bytes))
           == NULL) {
            PyErr_Clear();
            PyErr_SetString(PyExc_ValueError,
                            "Expected a bytes or bitarray object!");
            goto error;
        }
    } else {
        nTmpBytes = Py_NewRef(pObj);
    }
    result = PyCompatBytes_ToStringAndSize(nTmpBytes, str, size);
    goto end;

error:
    result = -1;

end:
    Py_XDECREF(nTmpBytes);
    return result;
}

static int
PyCompatBitArray_FromObject(PyObject *pObj, char **str, Py_ssize_t *size) {
    PyObject *nValue = NULL, *nBitArray = NULL, *nArgs = NULL, *nKwargs = NULL;
    int result = -1;

    if(PyObject_IsInstance(pObj, PyCompatTable->PyBitArray_Type)) {
        result = _PyCompatBitArray_ToStringAndSize(nBitArray, str, size);
    } else {
        // the object MUST be an integer
        if((nValue = PyObject_CallOneArg((PyObject *)(&PyLong_Type), pObj))
           == NULL) {
            return -1;
        }

        if((nArgs = Py_BuildValue("(O)", nValue))
           && (nKwargs = Py_BuildValue("{OO}", PyCompatTable->str__endian,
                                       PyCompatTable->str__little))) {
            nBitArray = PyObject_Call(PyCompatTable->PyBitArray_FromLong, nArgs,
                                      nKwargs);
            if(nBitArray) {
                result =
                    _PyCompatBitArray_ToStringAndSize(nBitArray, str, size);
            }
        }
    }

    Py_XDECREF(nArgs);
    Py_XDECREF(nKwargs);
    Py_XDECREF(nValue);
    Py_XDECREF(nBitArray);
    return result;
}

static PyObject *
PyCompatBitArray_AsLong(const char *str, Py_ssize_t size) {
    PyObject *nResult = NULL, *nBitArray = NULL;

    if((nBitArray = _PyCompatBitArray_FromStringAndSize(str, size)) != NULL) {
        nResult =
            PyObject_CallOneArg(PyCompatTable->PyBitArray_AsLong, nBitArray);
    }
    Py_XDECREF(nBitArray);
    return nResult;
}

#define PyCompatUnicode_AsUTF8AndSize(obj, size) \
    _PyCompatUnicode_AsUTF8AndSize(obj, (Py_ssize_t *)(size))

static inline char *
_PyCompatUnicode_AsUTF8AndSize(PyObject *pObj, Py_ssize_t *size) {
    const char *str = NULL;

    str = PyUnicode_AsUTF8AndSize(pObj, size);
    return str ? strdup(str) : NULL;
}

#define PyCompatUnicode_FromStringAndSize(str, size) \
    PyUnicode_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

#define PyCompatUnicode_AsUTF8(obj, str, size) \
    PyUnicode_AsUTF8((PyObject *)(obj), (char **)(str), (Py_ssize_t *)(size))

static inline int
_PyCompatUnicode_AsUTF8(PyObject *pObj, char **str, Py_ssize_t *size) {
    PyCompatUnicode_Check(pObj, -1);
    if(*str) {
        PyMem_Free(*str);
    }

    *str = _PyCompatUnicode_AsUTF8AndSize(pObj, size);
    return *str == NULL ? -1 : 0;
}

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
    PyErr_Clear();
    PyErr_SetString(PyExc_ValueError, "Invalid enum value");
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
    PyErr_Clear();
    PyErr_SetString(PyExc_ValueError, "Invalid enum value");
    return -1;
}

static inline int
PyCompatEnum_FromObject(PyObject *pObj, void *dst, int is_signed) {
    if(is_signed) {
        *(Py_ssize_t *)dst = PyCompatEnum_AsSsize_t(pObj);
    } else {
        *(size_t *)dst = PyCompatEnum_AsSize_t(pObj);
    }
    return PyErr_Occurred() != NULL ? -1 : 0;
}

static inline PyObject *
PyCompatEnum_AsObject(PyObject *pEnumType, void *src, int is_signed) {
    if(is_signed) {
        return PyCompatEnum_FromSsize_t(pEnumType, *(Py_ssize_t *)src);
    } else {
        return PyCompatEnum_FromSize_t(pEnumType, *(size_t *)src);
    }
}

#define PyCompatFlag_AsObject(pEnumType, str, size)                      \
    _PyCompatFlag_AsObject((PyObject *)(pEnumType), (const char *)(str), \
                           (Py_ssize_t)(size))

static inline PyObject *
_PyCompatFlag_AsObject(PyObject *pEnumType, const char *str, Py_ssize_t size) {
    PyObject *nValue = NULL, *nResult = NULL;
    if((nValue = PyCompatBitArray_AsLong(str, size)) != NULL) {
        nResult = PyObject_CallOneArg(pEnumType, nValue);
    }
    Py_XDECREF(nValue);
    return nResult;
}

#define PyCompatFlag_FromObject(value, str, size) \
    PyCompatBitArray_FromObject((value), (char **)(str), (Py_ssize_t *)(size))


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


#define PyCompat_GenericGetAttr(obj, attrName, value)                  \
    do {                                                               \
        if((value = PyObject_GetAttrString(obj, #attrName)) == NULL) { \
            PyErr_Clear();                                             \
            value = PyMapping_GetItemString(obj, #attrName);           \
        }                                                              \
    } while(0)

#define PyCompatAsnType_New(typeName)                \
    (PyAsn##typeName##Object *)(PyObject_CallNoArgs( \
        (PyObject *)&PyAsn##typeName##_Type))

#define PyCompatCHOICE_New(typeName) PyCompatAsnType_New(typeName)

#endif