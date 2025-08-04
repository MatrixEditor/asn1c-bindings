#ifndef _PyApplication_H_
#define _PyApplication_H_

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "asn_application.h"

typedef struct {
    PyObject* str__write;
    PyObject* str__getvalue;
    PyObject* str__prepare;
    PyObject* str__oid_sep;

    PyObject* PyBytesIO_Type;
    PyObject* PyBitArray_Type;
    PyObject* PyEnum_Type;
    PyObject* PyEnumMeta_Type;
} PyCompatTable_t;

extern PyCompatTable_t* PyCompatTable;

int PyCompat_Init(void);
void PyCompat_Clear(void);

static inline PyObject*
PyCompat_CheckConstaints(const asn_TYPE_descriptor_t* pTypeDescriptor,
                         const void* pValue) {
    char errbuf[1024];
    size_t errbuf_size = sizeof(errbuf);

    int res =
        asn_check_constraints(pTypeDescriptor, pValue, errbuf, &errbuf_size);
    return res ? PyUnicode_FromStringAndSize(errbuf, errbuf_size) : Py_None;
}

static int
PyCompat_WriteToStream(const void* pValue, size_t size, void* pIO) {
    PyObject *nBytesIO = (PyObject*)pIO, *nBytes = NULL;
    int result = 0;

    if((nBytes = PyBytes_FromStringAndSize((const char*)pValue, size))
       == NULL) {
        goto error;
    }
    if(!PyObject_CallMethodOneArg(nBytesIO, PyCompatTable->str__write,
                                  nBytes)) {
        goto error;
    } else {
        goto success;
    }

error:
    result = -1;

success:
    Py_XDECREF(nBytes);
    return result;
}

static PyObject*
PyCompat_Encode_DER(const asn_TYPE_descriptor_t* pTypeDescriptor,
                    const void* pValue) {
    PyObject *nStream = NULL, *nResult = NULL;
    asn_enc_rval_t rval;
    if((nStream = PyObject_CallNoArgs(PyCompatTable->PyBytesIO_Type)) == NULL) {
        goto end;
    }

    rval = der_encode(pTypeDescriptor, pValue, PyCompat_WriteToStream, nStream);
    if(rval.encoded < 0) {
        goto end;
    }

    nResult = PyObject_CallMethodNoArgs(nStream, PyCompatTable->str__getvalue);
end:
    Py_XDECREF(nStream);
    return nResult;
}


#define PyCompat_DEF_STRUCT(name)        \
    typedef struct _##name##_Py {        \
        PyObject_HEAD name##_t ob_value; \
        int s_valid;                     \
    } PyAsn##name##Object;

#define PyCompat_DEF_TYPE(name) extern PyTypeObject PyAsn##name##_Type;

#define PyCompat_QuickCheck(typeDef, self) \
    (asn_check_constraints(&typeDef, &self->ob_value, NULL, NULL))

/* Type implementation macros */
#define PY_IMPL_GENERIC_NEW(name)                                           \
    static PyObject* PyAsn##name##__new(PyTypeObject* type, PyObject* args, \
                                        PyObject* kwds) {                   \
        PyAsn##name##Object* self =                                         \
            (PyAsn##name##Object*)type->tp_alloc(type, 0);                  \
        if(self) {                                                          \
            ASN_STRUCT_RESET(asn_DEF_##name, &self->ob_value);              \
            self->s_valid = 0;                                              \
        }                                                                   \
        return (PyObject*)self;                                             \
    }

#define PY_IMPL_GENERIC_DEALLOC(name)                               \
    static void PyAsn##name##__dealloc(PyAsn##name##Object* self) { \
        ASN_STRUCT_RESET(asn_DEF_##name, &self->ob_value);          \
        self->s_valid = 0;                                          \
        Py_TYPE(self)->tp_free((PyObject*)self);                    \
    }

#define PY_IMPL_GENERIC_REPR(name)                                    \
    static PyObject* PyAsn##name##__repr(PyAsn##name##Object* self) { \
        return PyUnicode_FromString(("<" #name ">"));                 \
    }

#define PY_IMPL_GENERIC_STR(name)                                        \
    static PyObject* PyAsn##name##__str(PyAsn##name##Object* self) {     \
        PyObject *nValue = NULL, *nResult = NULL;                        \
        if(!self->s_valid) {                                             \
            return PyUnicode_FromString(("<" #name ">"));                \
        }                                                                \
        if((nValue = PyAsn##name##_ToPython(&self->ob_value)) == NULL) { \
            return NULL;                                                 \
        }                                                                \
        nResult = PyObject_Str(nValue);                                  \
        Py_DECREF(nValue);                                               \
        return nResult;                                                  \
    }


#define PY_IMPL_GENERIC_CHECK_CONSTRAINTS(name)                         \
    static PyObject* PyAsn##name##__check_constraints(                  \
        PyAsn##name##Object* self, PyObject* Py_UNUSED(ignored)) {      \
        PyObject* nResult =                                             \
            PyCompat_CheckConstaints(&asn_DEF_##name, &self->ob_value); \
        if(!nResult) {                                                  \
            return NULL;                                                \
        }                                                               \
        if(Py_IsNone(nResult)) {                                        \
            return Py_None;                                             \
        }                                                               \
        PyErr_SetObject(PyExc_ValueError, nResult);                     \
        Py_DECREF(nResult);                                             \
        return NULL;                                                    \
    }

#define PY_IMPL_GENERIC_IS_VALID(name)                                    \
    static PyObject* PyAsn##name##__is_valid(PyAsn##name##Object* self) { \
        if(!self->s_valid) return Py_False;                               \
        if(!PyAsn##name##__check_constraints(self, NULL)) {               \
            PyErr_Clear();                                                \
            return Py_False;                                              \
        }                                                                 \
        return Py_True;                                                   \
    }


#define PY_IMPL_GENERIC_ENCODE(name)                                       \
    static PyObject* PyAsn##name##__encode(PyAsn##name##Object* self,      \
                                           PyObject* Py_UNUSED(ignored)) { \
        if(!self->s_valid) {                                               \
            PyErr_SetString(PyExc_ValueError,                              \
                            "ASN.1 object does not contain valid data");   \
            return NULL;                                                   \
        }                                                                  \
        if(!PyAsn##name##__check_constraints(self, NULL)) {                \
            return NULL;                                                   \
        }                                                                  \
        return PyCompat_Encode_DER(&asn_DEF_##name, &self->ob_value);      \
    }

#define PY_IMPL_GENERIC_DECODE(name)                                        \
    static PyObject* PyAsn##name##__decode(PyTypeObject* Py_UNUSED(type),   \
                                           PyObject* args) {                \
        Py_buffer view;                                                     \
        PyAsn##name##Object* self = NULL;                                   \
        name##_t* value = NULL;                                             \
        asn_dec_rval_t rval;                                                \
        if(PyArg_ParseTuple(args, "y*", &view) < 0) return NULL;            \
        self = (PyAsn##name##Object*)PyObject_CallNoArgs(                   \
            (PyObject*)&PyAsn##name##_Type);                                \
        if(self == NULL) {                                                  \
            return NULL;                                                    \
        }                                                                   \
        value = &self->ob_value;                                            \
        rval = ber_decode(NULL, &asn_DEF_##name, (void**)&value,            \
                          (const void*)view.buf, view.len);                 \
        self->s_valid = rval.code == RC_OK;                                 \
        if(rval.code != RC_OK) {                                            \
            PyErr_Format(PyExc_ValueError,                                  \
                         ("Failed to decode " #name " from data! code=%d"), \
                         (int)rval.code);                                   \
            Py_CLEAR(self);                                                 \
        }                                                                   \
        if(!PyAsn##name##__check_constraints(self, NULL)) {                 \
            Py_CLEAR(self);                                                 \
        }                                                                   \
        return (PyObject*)self;                                             \
    }


#define PY_IMPL_MEMBER_GETSET(typeName, memberName, memberType, attr) \
    static PyObject* PyAsn##typeName##__get_##memberName(             \
        PyAsn##typeName##Object* self) {                              \
        if(!self->s_valid) {                                          \
            return Py_None;                                           \
        }                                                             \
        return PyAsn##memberType##_ToPython(&self->attr);             \
    }                                                                 \
    static int PyAsn##typeName##__set_##memberName(                   \
        PyAsn##typeName##Object* self, PyObject* value) {             \
        int res = PyAsn##memberType##_FromPython(value, &self->attr); \
        self->s_valid = res == 0;                                     \
        return res;                                                   \
    }

#define PY_IMPL_METHODDEF_ITEM(typeName, name, flags) \
    {#name, (PyCFunction)PyAsn##typeName##__##name, (flags), NULL}

#define PY_IMPL_GETSET_ITEM(typeName, itemName)            \
    {#itemName, (getter)PyAsn##typeName##__get_##itemName, \
     (setter)PyAsn##typeName##__set_##itemName, NULL, NULL}

#define PY_IMPL_GENERIC_INIT(typeName)                                     \
    static int PyAsn##typeName##__init(PyAsn##typeName##Object* self,      \
                                       PyObject* args, PyObject* kwds) {   \
        static char* kwlist[] = {"value", NULL};                           \
        PyObject* value = NULL;                                            \
        if(!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value)) \
            return -1;                                                     \
        if(value) {                                                        \
            if(PyAsn##typeName##_FromPython(value, &self->ob_value) < 0)   \
                return -1;                                                 \
            self->s_valid = 1;                                             \
        }                                                                  \
        return 0;                                                          \
    }


#define PY_IMPL_MOD_ADD_OBJECT(mod, typeName)                                 \
    Py_INCREF(&PyAsn##typeName##_Type);                                       \
    if(PyModule_AddObject(mod, #typeName, (PyObject*)&PyAsn##typeName##_Type) \
       < 0) {                                                                 \
        return -1;                                                            \
    }


#define PY_IMPL_NEW_ENUM(typeName, target, ret, ...)                          \
    do {                                                                      \
        PyObject *nName = NULL, *nBases = NULL, *nNamespace = NULL,           \
                 *nTmpName = NULL, *nTmpValue = NULL;                         \
        if((nName = PyUnicode_FromString((#typeName ".VALUES"))) == NULL) {   \
            return (ret);                                                     \
        }                                                                     \
        nBases = Py_BuildValue("(O)", (PyObject*)PyCompatTable->PyEnum_Type); \
        if(nBases == NULL) {                                                  \
            goto end;                                                         \
        }                                                                     \
        nNamespace = PyObject_CallMethodObjArgs(                              \
            PyCompatTable->PyEnumMeta_Type, PyCompatTable->str__prepare,      \
            nName, nBases, NULL);                                             \
        if(nNamespace == NULL) {                                              \
            goto end;                                                         \
        }                                                                     \
        __VA_ARGS__;                                                          \
        target = PyObject_CallFunctionObjArgs(                                \
            PyCompatTable->PyEnumMeta_Type, nName, nBases, nNamespace, NULL); \
    end:                                                                      \
        Py_XDECREF(nName);                                                    \
        Py_XDECREF(nBases);                                                   \
        Py_XDECREF(nNamespace);                                               \
        Py_XDECREF(nTmpName);                                                 \
        Py_XDECREF(nTmpValue);                                                \
        if(!target) {                                                         \
            return (ret);                                                     \
        }                                                                     \
    } while(0)

#define PY_IMPL_ENUM_VALUE(name, value, isSigned)                             \
    do {                                                                      \
        if((nTmpName = PyUnicode_FromString(#value)) == NULL) {               \
            goto end;                                                         \
        }                                                                     \
        if((isSigned)) {                                                      \
            if((nTmpValue = PyLong_FromSsize_t((Py_ssize_t)value)) == NULL) { \
                goto end;                                                     \
            }                                                                 \
        } else {                                                              \
            if((nTmpValue = PyLong_FromSize_t((size_t)value)) == NULL) {      \
                goto end;                                                     \
            }                                                                 \
        }                                                                     \
        if(PyObject_SetItem(nNamespace, nTmpName, nTmpValue) < 0) {           \
            goto end;                                                         \
        }                                                                     \
        Py_CLEAR(nTmpName);                                                   \
        Py_CLEAR(nTmpValue);                                                  \
    } while(0)

#define PY_IMPL_ASSIGN_ENUM(typeName)                                  \
    if(PyDict_SetItemString((PyObject*)PyAsn##typeName##_Type.tp_dict, \
                            "VALUES", PyAsnEnum##typeName##_Type)      \
       < 0) {                                                          \
        return -1;                                                     \
    }


#endif