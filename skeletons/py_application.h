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
    PyObject* str__endian;
    PyObject* str__little;
    PyObject* str__to_bytes;

    PyObject* PyBytesIO_Type;
    PyObject* PyBitArray_Type;
    PyObject* PyIntEnum_Type;
    PyObject* PyEnumMeta_Type;
    PyObject* PyIntFlag_Type;

    PyObject* PyBitArray_AsLong;
    PyObject* PyBitArray_FromLong;
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

    if ((nBytes = PyBytes_FromStringAndSize((const char*)pValue, size))
        == NULL) {
        goto error;
    }
    if (!PyObject_CallMethodOneArg(nBytesIO, PyCompatTable->str__write,
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
    if ((nStream = PyObject_CallNoArgs(PyCompatTable->PyBytesIO_Type))
        == NULL) {
        goto end;
    }

    rval = der_encode(pTypeDescriptor, pValue, PyCompat_WriteToStream, nStream);
    if (rval.encoded < 0) {
        PyErr_Format(PyExc_ValueError, "Failed to encode %s",
                     rval.failed_type->name);
        goto end;
    }

    nResult = PyObject_CallMethodNoArgs(nStream, PyCompatTable->str__getvalue);
end:
    Py_XDECREF(nStream);
    return nResult;
}

/*
 * Struct definition for basic types.
 *
 * Each class will store a reference to the actual value (native
 * representation), an internal state and a reference to the parent
 * object (if any). The flag is used to determine whether the object
 * is in a valid state (e.g. contains valid data). The parent object
 * MAY be NULL and should point to the parent object IF this instance
 * currently stores a reference to a child object.
 *      - ob_value - native representation of the value
 *      - s_valid - internal state
 *      - ob_parent - parent (optional)
 */
#define PyCompat_DEF_STRUCT(name)         \
    typedef struct _##name##_Py {         \
        PyObject_HEAD name##_t* ob_value; \
        int s_valid;                      \
        PyObject* ob_parent;              \
    } PyAsn##name##Object;


#define PyCompat_DEF_ENUM(name)               \
    typedef PyObject PyAsnEnum##name##Object; \
    extern PyObject* PyAsnEnum##name##_Type;

#define PyCompat_DEF_ANON_STRUCT(name, structName) \
    typedef struct _##name##_Py {                  \
        PyObject_HEAD struct structName* ob_value; \
        int s_valid;                               \
        PyObject* ob_parent;                       \
    } PyAsn##name##Object;                         \
    typedef struct structName name##_t;

#define PyCompat_DEF_TYPE(name) extern PyTypeObject PyAsn##name##_Type;

#define PyCompat_QuickCheck(typeDef, self) \
    (asn_check_constraints(&typeDef, self->ob_value, NULL, NULL))

/* Type implementation macros */
#define PY_IMPL_MALLOC(qtypeName) (qtypeName*)PyMem_RawMalloc(sizeof(qtypeName))
#define PY_IMPL_MALLOC_CHECK(target, qtypeName, ret) \
    do {                                             \
        target = PY_IMPL_MALLOC(qtypeName);          \
        if (!(target)) {                             \
            PyErr_NoMemory();                        \
            return ret;                              \
        }                                            \
    } while (0)

#define PY_IMPL_FREE(value) PyMem_RawFree((void*)(value))

#define PY_IMPL_XFREE(value)               \
    do {                                   \
        if (value) {                       \
            PyMem_RawFree((void*)(value)); \
        }                                  \
    } while (0)

#define PY_IMPL_XCLEAR(value)    \
    do {                         \
        if ((value)) {           \
            PY_IMPL_FREE(value); \
            value = NULL;        \
        }                        \
    } while (0)


#define PY_IMPL_GENERIC_NEW(name)                                           \
    static PyObject* PyAsn##name##__new(PyTypeObject* type, PyObject* args, \
                                        PyObject* kwds) {                   \
        PyAsn##name##Object* self =                                         \
            (PyAsn##name##Object*)type->tp_alloc(type, 0);                  \
        if (self) {                                                         \
            self->ob_value = NULL;                                          \
            self->s_valid = 0;                                              \
            self->ob_parent = NULL;                                         \
            self->ob_value = PY_IMPL_MALLOC(name##_t);                      \
            if (!self->ob_value) {                                          \
                Py_CLEAR(self);                                             \
            } else {                                                        \
                memset(self->ob_value, 0, sizeof(name##_t));                \
            }                                                               \
        }                                                                   \
        return (PyObject*)self;                                             \
    }

#define PY_IMPL_GENERIC_DEALLOC(name) PY_IMPL_DEALLOC(name, asn_DEF_##name)

#define PY_IMPL_DEALLOC(name, type_DEF)                                 \
    static void PyAsn##name##__dealloc(PyAsn##name##Object* self) {     \
        if (self->ob_parent) {                                          \
            if (Py_REFCNT(self->ob_parent) < 1) {                       \
                PyErr_SetString(PyExc_MemoryError,                      \
                                "UAF: parent object already deleted!"); \
                return;                                                 \
            }                                                           \
            Py_DECREF(self->ob_parent);                                 \
            self->ob_value = NULL;                                      \
        } else {                                                        \
            if (self->ob_value != NULL) {                               \
                ASN_STRUCT_RESET((type_DEF), self->ob_value);           \
                PyMem_RawFree(self->ob_value);                          \
            }                                                           \
            self->ob_value = NULL;                                      \
            self->ob_parent = NULL;                                     \
        }                                                               \
        Py_TYPE(self)->tp_free((PyObject*)self);                        \
    }

#define PY_IMPL_GENERIC_REPR(name)                                    \
    static PyObject* PyAsn##name##__repr(PyAsn##name##Object* self) { \
        return PyUnicode_FromString(("<" #name ">"));                 \
    }

#define PY_IMPL_GENERIC_STR(name)                                              \
    static PyObject* PyAsn##name##__str(PyAsn##name##Object* self) {           \
        PyObject *nValue = NULL, *nResult = NULL;                              \
        if (!self->s_valid) {                                                  \
            return PyUnicode_FromString(("<" #name ">"));                      \
        }                                                                      \
        if ((nValue = PyAsn##name##_ToPython(self->ob_value, NULL)) == NULL) { \
            return NULL;                                                       \
        }                                                                      \
        nResult = PyObject_Str(nValue);                                        \
        Py_DECREF(nValue);                                                     \
        return nResult;                                                        \
    }


#define PY_IMPL_GENERIC_CHECK_CONSTRAINTS(name) \
    PY_IMPL_CHECK_CONSTRAINTS(name, (&asn_DEF_##name))

#define PY_IMPL_CHECK_CONSTRAINTS(name, type_DEF)                              \
    static PyObject* PyAsn##name##__check_constraints(                         \
        PyAsn##name##Object* self, PyObject* Py_UNUSED(ignored)) {             \
        PyObject* nResult =                                                    \
            PyCompat_CheckConstaints((type_DEF), (const void*)self->ob_value); \
        if (!nResult) {                                                        \
            return NULL;                                                       \
        }                                                                      \
        if (Py_IsNone(nResult)) {                                              \
            return Py_None;                                                    \
        }                                                                      \
        PyErr_SetObject(PyExc_ValueError, nResult);                            \
        Py_DECREF(nResult);                                                    \
        return NULL;                                                           \
    }

#define PY_IMPL_GENERIC_IS_VALID(name)                                    \
    static PyObject* PyAsn##name##__is_valid(PyAsn##name##Object* self) { \
        if (!self->s_valid) return Py_False;                              \
        if (!PyAsn##name##__check_constraints(self, NULL)) {              \
            PyErr_Clear();                                                \
            return Py_False;                                              \
        }                                                                 \
        return Py_True;                                                   \
    }


#define PY_IMPL_GENERIC_ENCODE(name) PY_IMPL_ENCODE(name, &asn_DEF_##name)

#define PY_IMPL_ENCODE(name, type_DEF)                                     \
    static PyObject* PyAsn##name##__encode(PyAsn##name##Object* self,      \
                                           PyObject* Py_UNUSED(ignored)) { \
        if (!self->s_valid) {                                              \
            PyErr_SetString(PyExc_ValueError,                              \
                            "ASN.1 object does not contain valid data");   \
            return NULL;                                                   \
        }                                                                  \
        if (!PyAsn##name##__check_constraints(self, NULL)) {               \
            return NULL;                                                   \
        }                                                                  \
        return PyCompat_Encode_DER((type_DEF), self->ob_value);            \
    }

#define PY_IMPL_GENERIC_DECODE(name) PY_IMPL_DECODE(name, &asn_DEF_##name)

#define PY_IMPL_DECODE(name, type_DEF)                                     \
    static PyObject* PyAsn##name##__decode(PyTypeObject* Py_UNUSED(type),  \
                                           PyObject* args) {               \
        Py_buffer view;                                                    \
        PyAsn##name##Object* self = NULL;                                  \
        asn_dec_rval_t rval;                                               \
        if (PyArg_ParseTuple(args, "y*", &view) < 0) return NULL;          \
        self = (PyAsn##name##Object*)PyObject_CallNoArgs(                  \
            (PyObject*)&PyAsn##name##_Type);                               \
        if (self == NULL) {                                                \
            return NULL;                                                   \
        }                                                                  \
        rval = ber_decode(NULL, (type_DEF), (void**)&self->ob_value,       \
                          (const void*)view.buf, view.len);                \
        self->s_valid = rval.code == RC_OK;                                \
        switch (rval.code) {                                               \
        case RC_WMORE:                                                     \
            PyErr_SetString(PyExc_ValueError, "Failed to decode " #name    \
                                              " from data! "               \
                                              "(need more data)");         \
            Py_CLEAR(self);                                                \
            break;                                                         \
        case RC_OK:                                                        \
            break;                                                         \
        default:                                                           \
            PyErr_Format(                                                  \
                PyExc_ValueError,                                          \
                ("Failed to decode " #name " at byte %ld: Invalid data!"), \
                rval.consumed);                                            \
            Py_CLEAR(self);                                                \
            break;                                                         \
        };                                                                 \
        if (self && !PyAsn##name##__check_constraints(self, NULL)) {       \
            Py_CLEAR(self);                                                \
        }                                                                  \
        return (PyObject*)self;                                            \
    }


#define PY_IMPL_MEMBER_GETSET(typeName, memberName, memberType, attr) \
    static PyObject* PyAsn##typeName##__get_##memberName(             \
        PyAsn##typeName##Object* self) {                              \
        if (!self->s_valid) {                                         \
            return Py_None;                                           \
        }                                                             \
        return PyAsn##memberType##_ToPython((attr), (PyObject*)self); \
    }                                                                 \
    static int PyAsn##typeName##__set_##memberName(                   \
        PyAsn##typeName##Object* self, PyObject* value) {             \
        int res = PyAsn##memberType##_FromPython(value, (attr));      \
        self->s_valid = res == 0;                                     \
        return res;                                                   \
    }

#define PY_IMPL_METHODDEF_ITEM(typeName, name, flags) \
    {#name, (PyCFunction)PyAsn##typeName##__##name, (flags), NULL}

#define PY_IMPL_GETSET_ITEM(typeName, itemName)            \
    {#itemName, (getter)PyAsn##typeName##__get_##itemName, \
     (setter)PyAsn##typeName##__set_##itemName, NULL, NULL}

#define PY_IMPL_GENERIC_INIT(typeName)                                      \
    static int PyAsn##typeName##__init(PyAsn##typeName##Object* self,       \
                                       PyObject* args, PyObject* kwds) {    \
        static char* kwlist[] = {"value", NULL};                            \
        PyObject* value = NULL;                                             \
        if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value)) \
            return -1;                                                      \
        if (value) {                                                        \
            if (PyAsn##typeName##_FromPython(value, self->ob_value) < 0)    \
                return -1;                                                  \
            self->s_valid = 1;                                              \
        }                                                                   \
        return 0;                                                           \
    }


#define PY_IMPL_MOD_ADD_OBJECT(mod, typeName)                                  \
    Py_INCREF(&PyAsn##typeName##_Type);                                        \
    if (PyModule_AddObject(mod, #typeName, (PyObject*)&PyAsn##typeName##_Type) \
        < 0) {                                                                 \
        return -1;                                                             \
    }


#define PY_IMPL_NEW_ENUM(typeName, target, ret, ...) \
    PY_IMPL_NEW_ENUM_TYPE(PyIntEnum_Type, typeName, target, ret, __VA_ARGS__)

#define PY_IMPL_NEW_ENUM_TYPE(enumType, typeName, target, ret, ...)          \
    do {                                                                     \
        PyObject *nName = NULL, *nBases = NULL, *nNamespace = NULL,          \
                 *nTmpName = NULL, *nTmpValue = NULL;                        \
        int result = 0;                                                      \
        if ((nName = PyUnicode_FromString((#typeName))) == NULL) {           \
            return (ret);                                                    \
        }                                                                    \
        nBases = Py_BuildValue("(O)", (PyObject*)PyCompatTable->enumType);   \
        if (nBases) {                                                        \
            nNamespace = PyObject_CallMethodObjArgs(                         \
                PyCompatTable->PyEnumMeta_Type, PyCompatTable->str__prepare, \
                nName, nBases, NULL);                                        \
            if (nNamespace) {                                                \
                __VA_ARGS__;                                                 \
                if (result >= 0) {                                           \
                    target = PyObject_CallFunctionObjArgs(                   \
                        PyCompatTable->PyEnumMeta_Type, nName, nBases,       \
                        nNamespace, NULL);                                   \
                }                                                            \
            }                                                                \
        }                                                                    \
        Py_XDECREF(nName);                                                   \
        Py_XDECREF(nBases);                                                  \
        Py_XDECREF(nNamespace);                                              \
        Py_XDECREF(nTmpName);                                                \
        Py_XDECREF(nTmpValue);                                               \
        if (!target) {                                                       \
            return (ret);                                                    \
        }                                                                    \
    } while (0)

#define PY_IMPL_ENUM_VALUE(name, value, isSigned)                          \
    do {                                                                   \
        if (result >= 0) {                                                 \
            result = -1;                                                   \
            if ((nTmpName = PyUnicode_FromString(#name)) != NULL) {        \
                if ((isSigned)) {                                          \
                    nTmpValue = PyLong_FromSsize_t((Py_ssize_t)value);     \
                } else {                                                   \
                    nTmpValue = PyLong_FromSize_t((size_t)value);          \
                }                                                          \
                if (nTmpValue) {                                           \
                    result =                                               \
                        PyObject_SetItem(nNamespace, nTmpName, nTmpValue); \
                }                                                          \
            }                                                              \
        }                                                                  \
        Py_CLEAR(nTmpName);                                                \
        Py_CLEAR(nTmpValue);                                               \
    } while (0)

#define PY_IMPL_ASSIGN_ENUM(typeName) \
    PY_IMPL_ASSIGN_ENUM_DIRECT(typeName, typeName, VALUES)

#define PY_IMPL_ASSIGN_ENUM_DIRECT(typeName, enumTypeName, attrName)    \
    if (PyDict_SetItemString((PyObject*)PyAsn##typeName##_Type.tp_dict, \
                             #attrName, PyAsnEnum##enumTypeName##_Type) \
        < 0) {                                                          \
        return -1;                                                      \
    }

#define PY_IMPL_CHOICE_ATTR_FROMPY(typeName, attrName, ...)       \
    static inline int PyAsn##typeName##__##attrName##_FromPython( \
        PyObject* value, typeName##_t* dst) {                     \
        if (!value || Py_IsNone(value)) {                         \
            dst->present = ExampleChoice_PR_NOTHING;              \
            return 0;                                             \
        }                                                         \
        if ((__VA_ARGS__) < 0) return -1;                         \
        dst->present = ExampleChoice_PR_##attrName;               \
        return 0;                                                 \
    }

#define PY_IMPL_CHOICE_SETATTR(typeName, attrName)                             \
    static int PyAsn##typeName##__set_##attrName(                              \
        PyAsn##typeName##Object* self, PyObject* value,                        \
        void* Py_UNUSED(arg)) {                                                \
        ASN_STRUCT_RESET(asn_DEF_##typeName, self->ob_value);                  \
        self->ob_value->present = typeName##_PR_NOTHING;                       \
        int result =                                                           \
            PyAsn##typeName##__##attrName##_FromPython(value, self->ob_value); \
        self->s_valid = result != -1;                                          \
        if (result < 0) {                                                      \
            return -1;                                                         \
        }                                                                      \
        self->ob_value->present = typeName##_PR_##attrName;                    \
        return 0;                                                              \
    }

#define PY_IMPL_CHOICE_ATTR_TOPY(typeName, attrName, topyfunc)        \
    static inline PyObject* PyAsn##typeName##__##attrName##_ToPython( \
        const typeName##_t* src, PyObject* parent) {                  \
        return (topyfunc);                                            \
    }


#define PY_IMPL_CHOICE_GETATTR(typeName, attrName)                        \
    static PyObject* PyAsn##typeName##__get_##attrName(                   \
        PyAsn##typeName##Object* self, void* Py_UNUSED(arg)) {            \
        if (self->ob_value->present != ExampleChoice_PR_##attrName)       \
            return Py_None;                                               \
        return PyAsn##typeName##__##attrName##_ToPython(self->ob_value,   \
                                                        (PyObject*)self); \
    }

#define PY_IMPL_GENERIC_FIELDNAMES(typeName)                                   \
    static const char** PyAsn##typeName##__field_names(void) {                 \
        static const char** _##typeName##__names = NULL;                       \
        if (!_##typeName##__names) {                                           \
            _##typeName##__names = PyMem_RawCalloc(                            \
                asn_DEF_##typeName.elements_count + 1, sizeof(char*));         \
            for (int i = 0; i < asn_DEF_##typeName.elements_count; i++) {      \
                _##typeName##__names[i] = asn_DEF_##typeName.elements[i].name; \
            }                                                                  \
            _##typeName##__names[asn_DEF_##typeName.elements_count] = NULL;    \
        }                                                                      \
        return _##typeName##__names;                                           \
    }

#define PY_IMPL_INIT_KWONLY(typeName, args, kwargs)                       \
    if ((args) && (PyTuple_Size(args) > 0)) {                             \
        PyErr_SetString(PyExc_TypeError,                                  \
                        (#typeName ": unexpected positional arguments")); \
        return -1;                                                        \
    }                                                                     \
    if (!kwargs || !PyDict_Size(kwargs)) {                                \
        return 0;                                                         \
    }

#define PY_IMPL_CHOICE_INIT_ATTR(typeName, attrName, srcObj, tmpValue)    \
    PyCompat_GenericGetAttr((srcObj), attrName, (tmpValue));              \
    if ((tmpValue)) {                                                     \
        if (PyAsn##typeName##__##attrName##_FromPython((tmpValue), (dst)) \
            < 0) {                                                        \
            Py_DECREF((tmpValue));                                        \
            return -1;                                                    \
        }                                                                 \
    } else                                                                \
        PyErr_Clear();


#define PY_IMPL_CHOICE_INIT(typeName)                                      \
    static int PyAsn##typeName##__init(PyAsn##typeName##Object* self,      \
                                       PyObject* args, PyObject* kwargs) { \
        PY_IMPL_INIT_KWONLY(typeName, args, kwargs);                       \
        if (PyAsn##typeName##_FromPython(kwargs, self->ob_value) < 0)      \
            return -1;                                                     \
        self->s_valid = self->ob_value->present != typeName##_PR_NOTHING;  \
        return 0;                                                          \
    }

#define PY_IMPL_CHOICE_TOPY(typeName)                                        \
    PyObject* PyAsn##typeName##_ToPython(typeName##_t* src,                  \
                                         PyObject* parent) {                 \
        PyAsn##typeName##Object* self = PyCompatCHOICE_New(typeName);        \
        if (!parent) {                                                       \
            if (asn_copy(&asn_DEF_##typeName, (void**)&self->ob_value, src)  \
                < 0) {                                                       \
                Py_DECREF(self);                                             \
                return NULL;                                                 \
            }                                                                \
        } else {                                                             \
            self->ob_value = (typeName##_t*)src;                             \
            self->ob_parent = Py_NewRef(parent);                             \
        }                                                                    \
        self->s_valid = self->ob_value->present != ExampleChoice_PR_NOTHING; \
        return (PyObject*)self;                                              \
    }

#define PY_IMPL_CHOICE_PRESENT_ATTR(typeName)                             \
    static PyObject* PyAsn##typeName##__get_present(                      \
        PyAsn##typeName##Object* self, void* Py_UNUSED(arg)) {            \
        return PyCompatEnum_AsObject(PyAsnEnum##typeName##_PRESENT_Type,  \
                                     (void*)&self->ob_value->present, 0); \
    }

#define PY_IMPL_SEQ_ATTR_FREE(typeName, attrName, ...)      \
    static inline int PyAsn##typeName##__##attrName##_Free( \
        typeName##_t* dst) {                                \
        if (dst->attrName) {                                \
            __VA_ARGS__;                                    \
        }                                                   \
        return 0;                                           \
    }

#define PY_IMPL_SEQ_ATTR_GENERIC_FREE(typeName, attrName) \
    PY_IMPL_SEQ_ATTR_FREE(typeName, attrName, PyMem_RawFree(dst->attrName))


#define PY_IMPL_SEQ_ATTR_GENERIC_NEW(typeName, attrName, attrType)          \
    static inline attrType* PyAsn##typeName##__##attrName##_New(void) {     \
        static const size_t _##typeName##__##attrName##_size =              \
            sizeof(attrType);                                               \
        void* newValue = PyMem_RawMalloc(_##typeName##__##attrName##_size); \
        memset(newValue, 0, _##typeName##__##attrName##_size);              \
        return (attrType*)newValue;                                         \
    }

#define PY_IMPL_SEQ_ATTR_INDIRECT_FROMPY(typeName, attrName, ...) \
    static inline int PyAsn##typeName##__##attrName##_FromPython( \
        PyObject* value, typeName##_t* dst) {                     \
        void* target = NULL;                                      \
        if (PyAsn##typeName##__##attrName##_Free(dst) < 0) {      \
            return -1;                                            \
        }                                                         \
        dst->attrName = PyAsn##typeName##__##attrName##_New();    \
        if (!dst->attrName) {                                     \
            return -1;                                            \
        }                                                         \
        target = dst->attrName;                                   \
        return __VA_ARGS__;                                       \
    }

#define PY_IMPL_SEQ_ATTR_FROMPY(typeName, attrName, ...)          \
    static inline int PyAsn##typeName##__##attrName##_FromPython( \
        PyObject* value, typeName##_t* dst) {                     \
        void* target = (void*)&dst->attrName;                     \
        if (value == NULL) return 0;                              \
        return __VA_ARGS__;                                       \
    }

#define PY_IMPL_SEQ_ATTR_INDIRECT_TOPY(typeName, attrName, ...)       \
    static inline PyObject* PyAsn##typeName##__##attrName##_ToPython( \
        typeName##_t* src, PyObject* parent) {                        \
        void* target = (void*)src->attrName;                          \
        if (!target) {                                                \
            Py_RETURN_NONE;                                           \
        };                                                            \
        return __VA_ARGS__;                                           \
    }

#define PY_IMPL_SEQ_ATTR_TOPY(typeName, attrName, ...)                \
    static inline PyObject* PyAsn##typeName##__##attrName##_ToPython( \
        typeName##_t* src, PyObject* parent) {                        \
        void* target = (void*)&src->attrName;                         \
        return __VA_ARGS__;                                           \
    }


#define PY_IMPL_SEQ_OPT_GETATTR(typeName, attrName)                       \
    static PyObject* PyAsn##typeName##__get_##attrName(                   \
        PyAsn##typeName##Object* self, void* Py_UNUSED(closure)) {        \
        if (!(self->ob_value->attrName)) {                                \
            Py_RETURN_NONE;                                               \
        }                                                                 \
        return PyAsn##typeName##__##attrName##_ToPython(self->ob_value,   \
                                                        (PyObject*)self); \
    }

#define PY_IMPL_SEQ_GETATTR(typeName, attrName)                           \
    static PyObject* PyAsn##typeName##__get_##attrName(                   \
        PyAsn##typeName##Object* self, void* Py_UNUSED(closure)) {        \
        return PyAsn##typeName##__##attrName##_ToPython(self->ob_value,   \
                                                        (PyObject*)self); \
    }

#define PY_IMPL_SEQ_SETATTR(typeName, attrName)                            \
    static int PyAsn##typeName##__set_##attrName(                          \
        PyAsn##typeName##Object* self, PyObject* value,                    \
        void* Py_UNUSED(closure)) {                                        \
        return PyAsn##typeName##__##attrName##_FromPython(value,           \
                                                          self->ob_value); \
    }

#define PY_IMPL_SEQ_OPT_SETATTR(typeName, attrName)                            \
    static int PyAsn##typeName##__set_##attrName(                              \
        PyAsn##typeName##Object* self, PyObject* value,                        \
        void* Py_UNUSED(closure)) {                                            \
        if (value == NULL || Py_IsNone(value)) {                               \
            self->ob_value->attrName = NULL;                                   \
            return 0;                                                          \
        } else {                                                               \
            return PyAsn##typeName##__##attrName##_FromPython(value,           \
                                                              self->ob_value); \
        }                                                                      \
    }


#define PY_IMPL_SEQ_GENERIC_TOPY(typeName)

#define PY_IMPL_SEQ_TOPY(typeName, type_DEF)                              \
    PyObject* PyAsn##typeName##_ToPython(typeName##_t* src,               \
                                         PyObject* parent) {              \
        PyAsn##typeName##Object* self = PyCompatAsnType_New(typeName);    \
        if (!parent) {                                                    \
            if (asn_copy((type_DEF), (void**)&self->ob_value, src) < 0) { \
                Py_DECREF(self);                                          \
                return NULL;                                              \
            }                                                             \
        } else {                                                          \
            self->ob_value = (typeName##_t*)src;                          \
            self->ob_parent = Py_NewRef(parent);                          \
        }                                                                 \
        self->s_valid = 1;                                                \
        return (PyObject*)self;                                           \
    }

#define PY_IMPL_SEQ_INIT_ATTR(typeName, attrName)                            \
    if (result == 0) {                                                       \
        PyCompat_GenericGetAttr(pObj, attrName, tmp);                        \
        if (tmp) {                                                           \
            if (PyAsn##typeName##__##attrName##_FromPython(tmp, pDst) < 0) { \
                result = -1;                                                 \
            }                                                                \
        } else                                                               \
            PyErr_Clear();                                                   \
    }

#define PY_IMPL_SEQ_FROMPY(typeName, ...)                                  \
    int PyAsn##typeName##_FromPython(PyObject* pObj, typeName##_t* pDst) { \
        PyObject* tmp = NULL;                                              \
        int result = 0;                                                    \
        if (pObj != NULL) {                                                \
            __VA_ARGS__;                                                   \
        }                                                                  \
        Py_XDECREF(tmp);                                                   \
        return result;                                                     \
    }

#define PY_IMPL_SEQ_INIT(typeName)                                         \
    static int PyAsn##typeName##__init(PyAsn##typeName##Object* self,      \
                                       PyObject* args, PyObject* kwargs) { \
        self->s_valid = 1;                                                 \
        PY_IMPL_INIT_KWONLY(typeName, args, kwargs);                       \
        if (PyAsn##typeName##_FromPython(kwargs, self->ob_value) < 0) {    \
            return -1;                                                     \
        }                                                                  \
        return 0;                                                          \
    }

#define PY_IMPL_MOD_ASSIGN_OBJECT(typeName, attrName, obj)              \
    if (PyDict_SetItemString((PyObject*)PyAsn##typeName##_Type.tp_dict, \
                             #attrName, (PyObject*)(obj))               \
        < 0) {                                                          \
        return -1;                                                      \
    }

#define PY_IMPL_MOD_ASSIGN_INNER(innerTypeName, attrName, obj)    \
    if (PyDict_SetItemString(                                     \
            (PyObject*)((PyAsn##innerTypeName##_Type))->tp_dict), \
        #attrName, (obj))                                         \
        < 0) {                                                    \
            return -1;                                            \
        }


/* inner anonymous types */
#define PY_IMPL_SEQ_ANON_ATTR_FROMPY(typeName, attrName, attr, innerTypeName) \
    static inline int PyAsn##typeName##__##attrName##_FromPython(             \
        PyObject* value, typeName##_t* src) {                                 \
        return PyAsn##innerTypeName##_FromPython(value, (attr));              \
    }

#define PY_IMPL_SEQ_ANON_ATTR_TOPY(typeName, attrName, attr, innerTypeName) \
    static inline PyObject* PyAsn##typeName##__##attrName##_ToPython(       \
        typeName##_t* src, PyObject* parent) {                              \
        return PyAsn##innerTypeName##_ToPython((attr), parent);             \
    }

#define PY_IMPL_SEQ_INNER_GETATTR(typeName, attrName, attr, innerTypeName) \
    static PyObject* PyAsn##typeName##__get_##attrName(                    \
        PyAsn##typeName##Object* self, void* Py_UNUSED(closure)) {         \
        return PyAsn##innerTypeName##_ToPython((attr), (PyObject*)self);   \
    }

#define PY_IMPL_SEQ_INNER_OPT_GETATTR(typeName, attrName, innerTypeName) \
    static PyObject* PyAsn##typeName##__get_##attrName(                  \
        PyAsn##typeName##Object* self, void* Py_UNUSED(closure)) {       \
        if (self->ob_value->attrName == NULL) {                          \
            Py_RETURN_NONE;                                              \
        }                                                                \
        return PyAsn##innerTypeName##_ToPython(self->ob_value->attrName, \
                                               (PyObject*)self);         \
    }

#define PY_IMPL_SEQ_INNER_SETATTR(typeName, attrName, attr, innerTypeName) \
    static int PyAsn##typeName##__set_##attrName(                          \
        PyAsn##typeName##Object* self, PyObject* value,                    \
        void* Py_UNUSED(closure)) {                                        \
        return PyAsn##innerTypeName##_FromPython(value,                    \
                                                 &self->ob_value->attr);   \
    }

#define PY_IMPL_SEQ_INNER_OPT_SETATTR(typeName, attrName, attr, innerTypeName, \
                                      type_DEF)                                \
    static int PyAsn##typeName##__set_##attrName(                              \
        PyAsn##typeName##Object* self, PyObject* value,                        \
        void* Py_UNUSED(closure)) {                                            \
        if (value == Py_None) {                                                \
            if (self->ob_value->attrName != NULL) {                            \
                ASN_STRUCT_RESET((type_DEF), self->ob_value->attr);            \
                PyMem_RawFree(self->ob_value->attr);                           \
            }                                                                  \
            self->ob_value->attrName = NULL;                                   \
            return 0;                                                          \
        }                                                                      \
        return PyAsn##innerTypeName##_FromPython(value, self->ob_value->attr); \
    }


#endif