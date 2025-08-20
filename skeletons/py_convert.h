/*
 * Copyright (c) 2025 MatrixEditor @ github
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef _PyConvert_H_
#define _PyConvert_H_

#include <Python.h>
#include <py_application.h>

#define PyCompat_ArgCheck(obj, ret) \
    if (!obj) {                     \
        PyErr_BadArgument();        \
        return ret;                 \
    }

#define PyCompatLong_Check(obj, ret)                                 \
    PyCompat_ArgCheck(obj, ret);                                     \
    if (!PyLong_Check(obj) || Py_IsNone(obj)) {                      \
        PyErr_Format(PyExc_ValueError,                               \
                     "Expected an integer but got %R instead", obj); \
        return ret;                                                  \
    }

#define PyCompatLong_FromSsize_t(val) PyLong_FromSsize_t((Py_ssize_t)(val))
#define PyCompatLong_FromSize_t(val) PyLong_FromSize_t((size_t)(val))
#define PyCompatLong_AsSsize_t(obj) PyLong_AsSsize_t(obj)
#define PyCompatLong_AsSize_t(obj) PyLong_AsSize_t(obj)

static inline int PyCompatLong_FromObject(PyObject *pObj, void *val,
                                          int is_signed) {
    PyCompatLong_Check(pObj, -1);
    if (is_signed) {
        *((long *)val) = PyLong_AsLong(pObj);
    } else {
        *((unsigned long *)val) = PyLong_AsUnsignedLong(pObj);
    }
    return 0;
}

static inline PyObject *PyCompatLong_AsObject(void *val, int is_signed) {
    if (is_signed) {
        return PyLong_FromLong((*(long *)val));
    } else {
        return PyLong_FromUnsignedLong((*(unsigned long *)val));
    }
}

#define PyCompatBool_Check(obj, ret)                                \
    PyCompat_ArgCheck(obj, ret);                                    \
    if (!PyBool_Check(obj) || Py_IsNone(obj)) {                     \
        PyErr_Format(PyExc_ValueError,                              \
                     "Expected a boolean but got %R instead", obj); \
        return ret;                                                 \
    }

#define PyCompatBool_FromLong(val) ((val) ? Py_True : Py_False)
#define PyCompatBool_AsLong(obj) (PyObject_IsTrue(obj))

static inline int PyCompatBool_FromObject(PyObject *pObj, unsigned *val) {
    int tmp = PyObject_IsTrue(pObj);
    if (tmp < 0) return -1;
    *val = (unsigned)tmp;
    return 0;
}

#define PyCompatNull_AsLong(obj) (0)
#define PyCompatNull_FromLong(val) Py_None

static inline int PyCompatNull_FromObject(PyObject *pObj, int *val) {
    *val = 0;
    return 0;
}

#define PyCompatFloat_Check(obj, ret)                                         \
    PyCompat_ArgCheck(obj, ret);                                              \
    if (!PyFloat_Check(obj) || Py_IsNone(obj)) {                              \
        PyErr_Format(PyExc_ValueError, "Expected a float but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }

#define PyCompatFloat_FromDouble(val) PyFloat_FromDouble((double)(val))
#define PyCompatFloat_AsDouble(obj) PyFloat_AsDouble(obj)

static inline int PyCompatFloat_FromObject(PyObject *pObj, void *val,
                                           int is_float) {
    PyCompatFloat_Check(pObj, -1);
    if (is_float) {
        *((float *)val) = (float)PyFloat_AS_DOUBLE(pObj);
    } else {
        *((double *)val) = PyFloat_AS_DOUBLE(pObj);
    }
    return 0;
}

static inline PyObject *PyCompatFloat_AsObject(void *val, int is_float) {
    if (is_float) {
        return PyFloat_FromDouble((double)(*(float *)val));
    } else {
        return PyFloat_FromDouble((double)(*(double *)val));
    }
}

#define PyCompatUnicode_Check(obj, ret)                             \
    PyCompat_ArgCheck(obj, ret);                                    \
    if (!PyUnicode_Check(obj) || Py_IsNone(obj)) {                  \
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
    if (!PyBytes_Check(obj) || Py_IsNone(obj)) {                              \
        PyErr_Format(PyExc_ValueError, "Expected a bytes but got %R instead", \
                     obj);                                                    \
        return ret;                                                           \
    }

static inline int _PyCompatBytes_ToStringAndSize(PyObject *pObj, char **str,
                                                 Py_ssize_t *size) {
    Py_buffer view;
    char *p = NULL;
    int result = 0;

    PyCompat_ArgCheck(pObj, -1);
    if (!PyObject_CheckBuffer(pObj)) {
        PyErr_Format(PyExc_ValueError,
                     "Expected a buffer-like but got %R instead", pObj);
        return -1;
    }

    if (PyObject_GetBuffer(pObj, &view, PyBUF_FULL_RO) < 0) return -1;

    if (*str) {
        PyMem_Free(*str);
        *str = NULL;
    }
    *size = view.len;
    *str = (char *)PyMem_RawMalloc(view.len);
    if (*str == NULL) {
        result = -1;
        goto end;
    }
    memcpy(*str, view.buf, view.len);

end:
    PyBuffer_Release(&view);
    return result;
}

#define PyCompatBitArray_LITTLE_ENDIAN 1
#define PyCompatBitArray_BIG_ENDIAN 0

static inline PyObject *PyCompatBitArray_New(PyObject *pBytesObj, int little) {
    PyObject *nArgs = NULL, *nKwargs = NULL, *nResult = NULL;

    if ((nArgs = PyTuple_New(pBytesObj ? 1 : 0)) && (nKwargs = PyDict_New())) {
        /* PyTuple_SetItem:
         * This function “steals” a reference to o and discards a reference to
         * an item already in the tuple at the affected position.
         */
        if (!pBytesObj ||
            PyTuple_SetItem(nArgs, 0, Py_NewRef(pBytesObj)) == 0) {
            if (PyDict_SetItem(nKwargs, PyCompatTable->str__endian,
                               little ? PyCompatTable->str__little
                                      : PyCompatTable->str__big) == 0) {
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
    PyCompatBitArray_FromStringAndSize_Endian(str, size, 1)

#define PyCompatBitArray_FromStringAndSize_Endian(str, size, littleEndian) \
    _PyCompatBitArray_FromStringAndSize((const char *)(str),               \
                                        (Py_ssize_t)(size), (littleEndian))

static PyObject *_PyCompatBitArray_FromStringAndSize(const char *str,
                                                     Py_ssize_t size,
                                                     int littleEndian) {
    PyObject *nResult = NULL, *nTmpBytes = NULL;
    if ((nTmpBytes = PyBytes_FromStringAndSize(str, size)) == NULL) {
        goto end;
    }

    nResult = PyCompatBitArray_New(nTmpBytes, littleEndian);
end:
    Py_XDECREF(nTmpBytes);
    return nResult;
}

#define PyCompatBitArray_ToStringAndSize(obj, str, size) \
    _PyCompatBitArray_ToStringAndSize(obj, (char **)(str), (Py_ssize_t *)(size))

static int _PyCompatBitArray_ToStringAndSize(PyObject *pObj, char **str,
                                             Py_ssize_t *size) {
    PyObject *nTmpBytes = NULL;
    int result = 0;
    if (PyObject_TypeCheck(pObj,
                           (PyTypeObject *)PyCompatTable->PyBitArray_Type)) {
        if ((nTmpBytes = PyObject_CallMethodNoArgs(
                 pObj, PyCompatTable->str__to_bytes)) == NULL) {
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

#define PyCompatBitArray_FromObject(obj, str, size, little) \
    _PyCompatBitArray_FromObject(obj, (char **)(str), (Py_ssize_t *)(size), \
                                 (little))

static int _PyCompatBitArray_FromObject(PyObject *pObj, char **str,
                                       Py_ssize_t *size, int little) {
    PyObject *nValue = NULL, *nBitArray = NULL, *nArgs = NULL, *nKwargs = NULL;
    int result = -1;

    if (PyObject_IsInstance(pObj, PyCompatTable->PyBitArray_Type) ||
        PyObject_CheckBuffer(pObj)) {
        result = _PyCompatBitArray_ToStringAndSize(pObj, str, size);
    } else {
        // the object MUST be an integer
        if ((nValue = PyObject_CallOneArg((PyObject *)(&PyLong_Type), pObj)) ==
            NULL) {
            return -1;
        }

        if ((nArgs = Py_BuildValue("(O)", nValue)) &&
            (nKwargs = Py_BuildValue("{OO}", PyCompatTable->str__endian,
                                     little ? PyCompatTable->str__little
                                            : PyCompatTable->str__big))) {
            nBitArray = PyObject_Call(PyCompatTable->PyBitArray_FromLong, nArgs,
                                      nKwargs);
            if (nBitArray) {
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

static PyObject *PyCompatBitArray_AsLong(const char *str, Py_ssize_t size,
                                         int little) {
    PyObject *nResult = NULL, *nBitArray = NULL;

    if ((nBitArray = _PyCompatBitArray_FromStringAndSize(str, size, little)) !=
        NULL) {
        nResult =
            PyObject_CallOneArg(PyCompatTable->PyBitArray_AsLong, nBitArray);
    }
    Py_XDECREF(nBitArray);
    return nResult;
}

#define PyCompatUnicode_AsUTF8AndSize(obj, size) \
    _PyCompatUnicode_AsUTF8AndSize(obj, (Py_ssize_t *)(size))

static inline char *_PyCompatUnicode_AsUTF8AndSize(PyObject *pObj,
                                                   Py_ssize_t *size) {
    const char *tmp = NULL;
    char *str = NULL;
    tmp = PyUnicode_AsUTF8AndSize(pObj, size);
    if (tmp != NULL) {
        str = (char *)PyMem_RawMalloc(*size);
        if (str != NULL) {
            memcpy(str, tmp, *size);
        }
    }
    return str;
}

#define PyCompatUnicode_FromStringAndSize(str, size) \
    PyUnicode_FromStringAndSize((const char *)(str), (Py_ssize_t)(size))

#define PyCompatUnicode_AsUTF8(obj, str, size)                 \
    _PyCompatUnicode_AsUTF8((PyObject *)(obj), (char **)(str), \
                            (Py_ssize_t *)(size))

static inline int _PyCompatUnicode_AsUTF8(PyObject *pObj, char **str,
                                          Py_ssize_t *size) {
    PyCompatUnicode_Check(pObj, -1);
    if (*str) {
        PyMem_Free(*str);
        *str = NULL;
    }

    *str = (char *)_PyCompatUnicode_AsUTF8AndSize(pObj, size);
    if (!*str) {
        return -1;
    }
    return *str == NULL ? -1 : 0;
}

static inline PyObject *PyCompatEnum_FromSsize_t(PyObject *pEnumType,
                                                 Py_ssize_t value) {
    PyObject *nValue = NULL, *nResult = NULL;
    PyCompat_ArgCheck(pEnumType, NULL);

    if ((nValue = PyLong_FromSsize_t(value)) == NULL) {
        goto end;
    }
    nResult = PyObject_CallOneArg(pEnumType, nValue);
end:
    Py_XDECREF(nValue);
    return nResult;
}

static inline PyObject *PyCompatEnum_FromSize_t(PyObject *pEnumType,
                                                size_t value) {
    PyObject *nValue = NULL, *nResult = NULL;
    if ((nValue = PyLong_FromSize_t(value)) == NULL) {
        goto end;
    }
    nResult = PyObject_CallOneArg(pEnumType, nValue);
end:
    Py_XDECREF(nValue);
    return nResult;
}

static inline Py_ssize_t PyCompatEnum_AsSsize_t(PyObject *pObj) {
    PyObject *nValue = NULL;
    if (PyLong_Check(pObj)) {
        return PyLong_AsSsize_t(pObj);
    }

    nValue = PyObject_GetAttrString(pObj, "value");
    if (nValue != NULL) {
        Py_ssize_t result = PyLong_AsSsize_t(nValue);
        Py_XDECREF(nValue);
        return result;
    }
    if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
        PyErr_Clear();
        PyErr_SetString(PyExc_ValueError, "Invalid enum value");
    }
    return -1;
}

static inline size_t PyCompatEnum_AsSize_t(PyObject *pObj) {
    PyObject *nValue = NULL;
    if (PyLong_Check(pObj)) {
        return PyLong_AsSize_t(pObj);
    }

    nValue = PyObject_GetAttrString(pObj, "value");
    if (nValue != NULL) {
        size_t result = PyLong_AsSize_t(nValue);
        Py_XDECREF(nValue);
        return result;
    }
    if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
        PyErr_Clear();
        PyErr_SetString(PyExc_ValueError, "Invalid enum value");
    }
    return -1;
}

static inline int PyCompatEnum_FromObject(PyObject *pObj, void *dst,
                                          int is_signed) {
    if (is_signed) {
        *(Py_ssize_t *)dst = PyCompatEnum_AsSsize_t(pObj);
    } else {
        *(size_t *)dst = PyCompatEnum_AsSize_t(pObj);
    }
    return PyErr_Occurred() != NULL ? -1 : 0;
}

static inline PyObject *PyCompatEnum_AsObject(PyObject *pEnumType, void *src,
                                              int is_signed) {
    if (is_signed) {
        return PyCompatEnum_FromSsize_t(pEnumType, *(Py_ssize_t *)src);
    } else {
        return PyCompatEnum_FromSize_t(pEnumType, *(size_t *)src);
    }
}

#define PyCompatFlag_AsObject(pEnumType, str, size, littleEndian)        \
    _PyCompatFlag_AsObject((PyObject *)(pEnumType), (const char *)(str), \
                           (Py_ssize_t)(size), (littleEndian))

static inline PyObject *_PyCompatFlag_AsObject(PyObject *pEnumType,
                                               const char *str, Py_ssize_t size,
                                               int little) {
    PyObject *nValue = NULL, *nResult = NULL;
    PyCompat_ArgCheck(pEnumType, NULL);
    if ((size > 0) && (str == NULL)) {
        PyErr_SetString(PyExc_ValueError,
                        "Flag2Obj: NULL buffer with positive size");
    }

    if ((nValue = PyCompatBitArray_AsLong(str, size, little)) != NULL) {
        nResult = PyObject_CallOneArg(pEnumType, nValue);
    }
    Py_XDECREF(nValue);
    return nResult;
}

#define PyCompatFlag_FromObject(value, str, size, littleEndian)                \
    PyCompatBitArray_FromObject((value), (char **)(str), (Py_ssize_t *)(size), \
                                (littleEndian))

#define PyCompat_GenericGetAttr(obj, attrName, value)                   \
    do {                                                                \
        if ((value = PyObject_GetAttrString(obj, #attrName)) == NULL) { \
            PyErr_Clear();                                              \
            value = PyMapping_GetItemString(obj, #attrName);            \
        }                                                               \
    } while (0)

#define PyCompatAsnType_New(typeName)                \
    (PyAsn##typeName##Object *)(PyObject_CallNoArgs( \
        (PyObject *)&PyAsn##typeName##_Type))

#define PyCompatCHOICE_New(typeName) PyCompatAsnType_New(typeName)

static inline PyObject *PyCompatAsnType_FromParent(PyTypeObject *type,
                                                   PyObject *parent,
                                                   void *value) {
    PyCompatAsnObject_t *obj = NULL;
    if (value == NULL || type == NULL || parent == NULL) {
        PyErr_BadArgument();
        return NULL;
    }

    obj = (PyCompatAsnObject_t *)PyObject_CallNoArgs((PyObject *)type);
    if (obj == NULL) {
        return NULL;
    }

    obj->ob_value = value;
    obj->ob_parent = Py_NewRef(parent);
    obj->s_valid = 1;
    return (PyObject *)obj;
}
#endif