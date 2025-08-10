#ifndef ASN1_COMPILED_PY_OUTPUT_H
#define ASN1_COMPILED_PY_OUTPUT_H

#include "asn1c_out.h"

#define PY_GEN_DEFAULT_INCLUDE()                                          \
    do {                                                                  \
        if((arg->flags & A1C_INCLUDES_QUOTED)) {                          \
            GEN_POS_INCLUDE(OT_PY_TYPE_INCLUDES, "\"py_application.h\""); \
        } else {                                                          \
            GEN_POS_INCLUDE(OT_PY_TYPE_INCLUDES, "<py_application.h>");   \
        }                                                                 \
    } while(0)

#define PY_GEN_BEGIN_FUNC() \
    OUT("{\n");             \
    INDENT(+1)

#define PY_GEN_END_FUNC() \
    INDENT(-1);           \
    OUT("}\n")

#define PY_GEN_TYPE_MOD_INIT(typeName)                     \
    REDIR(OT_PY_TYPE_MOD_DECLS);                           \
    OUT("int PyAsn%s_ModSetupTypes(void);\n", typeName);   \
    OUT("void PyAsn%s_ModClear(PyObject *);\n", typeName); \
    OUT("int PyAsn%s_ModInit(PyObject *);\n", typeName);

#define PY_GEN_ASNTYPE_FROMPY_INLINE(name)                                    \
    OUT("static inline int PyAsn%s_FromPython(PyObject *pObj, %s_t *pDst)\n", \
        name, name);                                                          \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_FROMPY(name) \
    OUT("int PyAsn%s_FromPython(PyObject *pObj, %s_t *pDst);\n", name, name)

#define PY_GEN_ASNTYPE_FROMPY_IMPL(name)                                     \
    OUT("int PyAsn%s_FromPython(PyObject *value, %s_t *dst)\n", name, name); \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_TOPY_INLINE(name)                                 \
    OUT("static inline PyObject *PyAsn%s_ToPython(%s_t *pSrc, PyObject " \
        "*parent)\n",                                                    \
        name, name);                                                     \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_TOPY(name)                                            \
    OUT("PyObject *PyAsn%s_ToPython(%s_t *pSrc, PyObject *parent);\n", name, \
        name)

#define PY_GEN_DEF_TYPE(name)                \
    OUT("PyCompat_DEF_STRUCT(%s);\n", name); \
    OUT("PyCompat_DEF_TYPE(%s);\n", name)

#define PY_GEN_TYPE_NEW(name) OUT("PY_IMPL_GENERIC_NEW(%s);\n", name)

#define PY_GEN_TYPE_DEALLOC(name) OUT("PY_IMPL_GENERIC_DEALLOC(%s);\n", name)

#define PY_GEN_TYPE_REPR(name) OUT("PY_IMPL_REPR(%s);\n", name)

#define PY_GEN_TYPE_STR(name) OUT("PY_IMPL_GENERIC_STR(%s);\n", name)

#define PY_GEN_TYPE_INIT(name) OUT("PY_IMPL_GENERIC_INIT(%s);\n", name)

#define PY_GEN_TYPE_CHECK_CONSTRAINTS(name) \
    OUT("PY_IMPL_GENERIC_CHECK_CONSTRAINTS(%s);\n", name)

#define PY_GEN_TYPE_IS_VALID(name) OUT("PY_IMPL_GENERIC_IS_VALID(%s);\n", name)

#define PY_GEN_TYPE_ENCODE(name) OUT("PY_IMPL_GENERIC_ENCODE(%s);\n", name)

#define PY_GEN_TYPE_DECODE(name) OUT("PY_IMPL_GENERIC_DECODE(%s);\n", name)

#define PY_GEN_TYPE_DEFAULT_MEMBER(name) \
    OUT("PY_IMPL_MEMBER_GETSET(%s, value, %s, self->ob_value);\n", name, name)

#define PY_GEN_TYPE_METHODS_PARSER_KW(typeName, funcName)          \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, " #funcName                    \
        "_encode, METH_VARARGS | METH_KEYWORDS),\n",               \
        typeName);                                                 \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, " #funcName                    \
        "_decode, METH_VARARGS | METH_KEYWORDS | METH_STATIC),\n", \
        typeName);

#define PY_GEN_TYPE_METHODS_PARSER_NOARGS(typeName, funcName)               \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, " #funcName "_encode, METH_NOARGS),\n", \
        typeName);                                                          \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, " #funcName                             \
        "_decode, METH_VARARGS | METH_STATIC),\n",                          \
        typeName);

#define PY_GEN_TYPE_DEFAULT_METHODS(type_id)                              \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, is_valid, METH_NOARGS),\n", type_id); \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, check_constraints, METH_NOARGS),\n",  \
        type_id);                                                         \
    if(arg->flags & A1C_GEN_BER) {                                        \
        PY_GEN_TYPE_METHODS_PARSER_NOARGS(type_id, ber);                  \
        PY_GEN_TYPE_METHODS_PARSER_NOARGS(type_id, der);                  \
        PY_GEN_TYPE_METHODS_PARSER_NOARGS(type_id, cer);                  \
    }                                                                     \
    if(arg->flags & A1C_GEN_XER) {                                        \
        PY_GEN_TYPE_METHODS_PARSER_KW(type_id, xer);                      \
    }                                                                     \
    if(arg->flags & (A1C_GEN_APER | A1C_GEN_UPER)) {                      \
        PY_GEN_TYPE_METHODS_PARSER_KW(type_id, per);                      \
    }                                                                     \
    if(arg->flags & (A1C_GEN_PRINT)) {                                    \
        OUT("{\"to_text\", (PyCFunction)PyAsn%s__plain_encode, "          \
            "(METH_NOARGS), NULL},\n",                                    \
            type_id);                                                     \
    }                                                                     \
    if(arg->flags & (A1C_GEN_OER)) {                                      \
        PY_GEN_TYPE_METHODS_PARSER_KW(type_id, oer);                      \
    }                                                                     \
    if(arg->flags & (A1C_GEN_JER)) {                                      \
        PY_GEN_TYPE_METHODS_PARSER_KW(type_id, jer);                      \
    }

#define PY_GEN_TYPE_METHODS_END()                  \
    OUT("{NULL, NULL, 0, NULL} /* sentinel */\n"); \
    INDENT(-1);                                    \
    OUT("};\n");

#define PY_GEN_TYPE_METHODS_BEGIN(name)                      \
    OUT("static PyMethodDef PyAsn%s_methods[] = {\n", name); \
    INDENT(+1)

#define PY_GEN_TYPE_ATTRS_BEGIN(name)                       \
    OUT("static PyGetSetDef PyAsn%s_getset[] = {\n", name); \
    INDENT(+1)


#define PY_GEN_TYPE_ATTRS_END()                             \
    OUT("{NULL, NULL, NULL, NULL, NULL} /* sentinel */\n"); \
    INDENT(-1);                                             \
    OUT("};\n");

#define PY_GEN_TYPE_ATTR(typeName, attrName) \
    OUT(("PY_IMPL_GETSET_ITEM(%s, " #attrName "),\n"), typeName)

#define PY_GEN_TYPE_ATTRSTR(typeName, attrName) \
    OUT(("PY_IMPL_GETSET_ITEM(%s, %s),\n"), typeName, attrName)

#define PY_GEN_TYPE_PARSERS(typeName)                                  \
    if(arg->flags & A1C_GEN_BER) {                                     \
        /*        E        D  */                                       \
        /* BER:  ber <--> ber */                                       \
        OUT("PY_IMPL_DECODE_BER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_SPECIFIC(%s, ber, ATS_BER);\n", typeName); \
        /* DER:  der <--> ber */                                       \
        OUT("PY_IMPL_DECODE_DER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_SPECIFIC(%s, der, ATS_DER);\n", typeName); \
        /* CER:  ber <--> cer */                                       \
        OUT("PY_IMPL_DECODE_CER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_SPECIFIC(%s, cer, ATS_BER);\n", typeName); \
    }                                                                  \
    if(arg->flags & A1C_GEN_XER) {                                     \
        OUT("PY_IMPL_DECODE_XER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_XER(%s);\n", typeName);                    \
    }                                                                  \
    if(arg->flags & (A1C_GEN_APER | A1C_GEN_UPER)) {                   \
        OUT("PY_IMPL_DECODE_PER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_PER(%s);\n", typeName);                    \
    }                                                                  \
    if(arg->flags & (A1C_GEN_PRINT)) {                                 \
        OUT("PY_IMPL_ENCODE_SPECIFIC(%s, plain, "                      \
            "ATS_NONSTANDARD_PLAINTEXT);\n",                           \
            typeName);                                                 \
    }                                                                  \
    if(arg->flags & (A1C_GEN_OER)) {                                   \
        OUT("PY_IMPL_DECODE_OER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_OER(%s);\n", typeName);                    \
    }                                                                  \
    if(arg->flags & (A1C_GEN_JER)) {                                   \
        OUT("PY_IMPL_DECODE_JER(%s);\n", typeName);                    \
        OUT("PY_IMPL_ENCODE_JER(%s);\n", typeName);                    \
    }

#define PY_GEN_CLASS_BEGIN(modName, typeName)                 \
    PY_GEN_CLASS_BEGIN_INTERNAL(modName, typeName, typeName); \
    PY_GEN_CLASS_DOC("ASN.1 %s type", typeName)

#define PY_GEN_CLASS_DOC(docfmt, ...) \
    OUT(".tp_doc = \"" docfmt "\",\n", __VA_ARGS__);


#define PY_GEN_CLASS_BEGIN_INTERNAL(modName, typeName, pyTypeName)  \
    OUT("PyTypeObject PyAsn%s_Type = {\n", typeName);               \
    INDENT(+1);                                                     \
    OUT("PyVarObject_HEAD_INIT(NULL, 0)\n");                        \
    OUT(".tp_name = \"%s.%s\",\n", (modName), pyTypeName);          \
    OUT(".tp_basicsize = sizeof(PyAsn%sObject),\n", typeName);      \
    OUT(".tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,\n"); \
    OUT(".tp_new = (newfunc)PyAsn%s__new,\n", typeName);            \
    OUT(".tp_repr = (reprfunc)PyAsn%s__repr,\n", typeName);         \
    OUT(".tp_dealloc = (destructor)PyAsn%s__dealloc,\n", typeName); \
    OUT(".tp_init = (initproc)PyAsn%s__init,\n", typeName);         \
    OUT(".tp_getset = PyAsn%s_getset,\n", typeName);                \
    OUT(".tp_methods = PyAsn%s_methods,\n", typeName)


#define PY_GEN_CLASS_END() \
    INDENT(-1);            \
    OUT("};\n")

#define PY_GEN_BASIC_CLASS(modName, typeName)             \
    REDIR(OT_PY_IMPL_CODE);                               \
    PY_GEN_TYPE_NEW(typeName);                            \
    PY_GEN_TYPE_INIT(typeName);                           \
    PY_GEN_TYPE_DEALLOC(typeName);                        \
    PY_GEN_TYPE_REPR(typeName);                           \
    PY_GEN_TYPE_STR(typeName);                            \
    PY_GEN_TYPE_CHECK_CONSTRAINTS(typeName);              \
    PY_GEN_TYPE_IS_VALID(typeName);                       \
    PY_GEN_TYPE_ENCODE(typeName);                         \
    PY_GEN_TYPE_DECODE(typeName);                         \
    PY_GEN_TYPE_PARSERS(typeName);                        \
    PY_GEN_TYPE_DEFAULT_MEMBER(typeName);                 \
    /*methods*/                                           \
    REDIR(OT_PY_IMPL_METHODS);                            \
    PY_GEN_TYPE_METHODS_BEGIN(typeName);                  \
    PY_GEN_TYPE_DEFAULT_METHODS(typeName);                \
    PY_GEN_TYPE_METHODS_END();                            \
    /*attributes*/                                        \
    REDIR(OT_PY_IMPL_ATTRS);                              \
    PY_GEN_TYPE_ATTRS_BEGIN(typeName);                    \
    PY_GEN_TYPE_ATTR(typeName, value);                    \
    PY_GEN_TYPE_ATTRS_END();                              \
    /*type def*/                                          \
    REDIR(OT_PY_IMPL_CLASS);                              \
    PY_GEN_CLASS_BEGIN((modName), typeName);              \
    OUT(".tp_str = (reprfunc)PyAsn%s__str,\n", typeName); \
    PY_GEN_CLASS_END();

#define PY_GEN_MOD_SETUP_SINGLE(typeName) \
    OUT("if (PyType_Ready(&PyAsn%s_Type) < 0) return -1;\n", typeName);

#define PY_GEN_MOD_ADD_OBJECT(typeName) \
    OUT("PY_IMPL_MOD_ADD_OBJECT(mod, %s);\n", typeName);

#define PY_GEN_MOD_INIT_SINGLE(typeName) PY_GEN_MOD_ADD_OBJECT(typeName);

#define PY_GEN_MOD_BASIC(typeName)               \
    REDIR(OT_PY_IMPL_CODE_MOD_SETUP);            \
    INDENTED(PY_GEN_MOD_SETUP_SINGLE(typeName)); \
    REDIR(OT_PY_IMPL_CODE_MOD_INIT);             \
    INDENTED(PY_GEN_MOD_INIT_SINGLE(typeName))

#define PY_GEN_MODULE_ADD_TYPE(typeName)                                       \
    do {                                                                       \
        compiler_streams_t *cs = arg->pytarget, *saved_cs = NULL;              \
        saved_cs = arg->target;                                                \
        arg->target = cs;                                                      \
        REDIR(OT_PY_IMPL_MOD_SETUP_TYPES);                                     \
        INDENTED(                                                              \
            OUT("if (PyAsn%s_ModSetupTypes() < 0) return NULL;\n", typeName)); \
        REDIR(OT_PY_IMPL_MOD_CLEAR);                                           \
        INDENTED(OUT("PyAsn%s_ModClear(m);\n", typeName));                     \
        REDIR(OT_PY_IMPL_MOD_INIT);                                            \
        INDENTED(OUT("if (PyAsn%s_ModInit(nModule) < 0) return NULL;\n",       \
                     typeName));                                               \
        REDIR(OT_PY_IMPL_MOD_INCLUDES);                                        \
        OUT("#include \"%s_Py.h\"\n", typeName);                               \
        arg->target = saved_cs;                                                \
    } while(0)


#define PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName,          \
                             type_def_path)                                   \
    OUT("PY_IMPL_CHOICE_GETATTR(%s, %s, %s);\n", (typeName),                  \
        (targetEnumTypeName), (attrName));                                    \
    if(arg->embed > 1) {                                                      \
        OUT("PY_IMPL_CHOICE_GENERIC_SETATTR(%s, %s, %s, *%s);\n", (typeName), \
            (targetEnumTypeName), (attrName), (type_def_path));               \
    } else {                                                                  \
        OUT("PY_IMPL_CHOICE_SETATTR(%s, %s, %s);\n", (typeName),              \
            (targetEnumTypeName), (attrName));                                \
    }

#define PY_GEN_CHOICE_TYPEREF_GETSET(                                      \
    typeName, targetTypeName, targetEnumTypeName, attrName, type_def_path) \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                          \
        "PyAsn%s_FromPython(value, &dst->choice.%s));\n",                  \
        (typeName), (targetEnumTypeName), (attrName), (targetTypeName),    \
        (attrName));                                                       \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                \
        "PyAsn%s_ToPython((%s_t *)&src->choice.%s, parent));\n",           \
        (typeName), (attrName), (targetTypeName), (targetTypeName),        \
        (attrName));                                                       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)

#define PY_GEN_CHOICE_INTEGER_GETSET(typeName, targetEnumTypeName, attrName,   \
                                     isSigned, type_def_path)                  \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                              \
        "PyCompatLong_FromObject(value, (void *)&dst->choice.%s, %d));\n",     \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (isSigned)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatLong_AsObject((void *)&src->choice.%s, %d));\n",              \
        (typeName), (attrName), (attrName), (isSigned));                       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_BOOLEAN_GETSET(typeName, targetEnumTypeName, attrName, \
                                     type_def_path)                          \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s,"                             \
        "PyCompatBool_FromObject(value, &dst->choice.%s));\n",               \
        (typeName), (targetEnumTypeName), (attrName), (attrName));           \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                  \
        "PyCompatBool_FromLong(src->choice.%s));\n",                         \
        (typeName), (attrName), (attrName));                                 \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_BYTES_GETSET(typeName, targetEnumTypeName, attrName,     \
                                   type_def_path)                              \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                              \
        "PyCompatBytes_ToStringAndSize(value, &dst->choice.%s.buf, "           \
        "&dst->choice.%s.size));\n",                                           \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (attrName)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatBytes_FromStringAndSize(src->choice.%s.buf, "                 \
        "src->choice.%s.size));\n",                                            \
        (typeName), (attrName), (attrName), (attrName));                       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_REAL_GETSET(typeName, targetEnumTypeName, attrName,   \
                                  is_float32, type_def_path)                \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                           \
        "PyCompatFloat_FromObject(value, (void *)&dst->choice.%s, %d));\n", \
        (typeName), (targetEnumTypeName), (attrName), (attrName),           \
        (is_float32));                                                      \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                 \
        "PyCompatFloat_AsObject((void *)&src->choice.%s, %d));\n",          \
        (typeName), (attrName), (attrName), (is_float32));                  \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_NULL_GETSET(typeName, targetEnumTypeName, attrName, \
                                  type_def_path)                          \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                         \
        "PyCompatNull_FromObject(value, &dst->choice.%s));\n",            \
        (typeName), (targetEnumTypeName), (attrName), (attrName));        \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, Py_None);\n", (typeName),       \
        (attrName));                                                      \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_BITSTRING_GETSET(typeName, targetEnumTypeName, attrName, \
                                       type_def_path)                          \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                              \
        "PyCompatBitArray_ToStringAndSize(value, &dst->choice.%s.buf, "        \
        "&dst->choice.%s.size));\n",                                           \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (attrName)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatBitArray_FromStringAndSize(src->choice.%s.buf, "              \
        "src->choice.%s.size));\n",                                            \
        (typeName), (attrName), (attrName), (attrName));                       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_NAMED_BITSTRING_GETSET(                                  \
    typeName, targetEnumTypeName, attrName, enumTypeName, type_def_path)       \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                              \
        "PyCompatFlag_FromObject(value, &dst->choice.%s.buf, "                 \
        "&dst->choice.%s.size));\n",                                           \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (attrName)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatFlag_AsObject(PyAsnEnum%s_Type, src->choice.%s.buf, "         \
        "src->choice.%s.size));\n",                                            \
        (typeName), (attrName), (enumTypeName), (attrName), (attrName));       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_OID_GETSET(typeName, targetEnumTypeName, attrName, \
                                 type_def_path)                          \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                        \
        "PyCompatOID_FromUnicode(value, &dst->choice.%s));\n",           \
        (typeName), (targetEnumTypeName), (attrName), (attrName));       \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                              \
        "PyCompatOID_AsUTF8String(&src->choice.%s));\n",                 \
        (typeName), (attrName), (attrName));                             \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_RELATIVE_OID_GETSET(typeName, targetEnumTypeName, \
                                          attrName, type_def_path)      \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s,"                        \
        "PyCompatRelativeOID_FromUnicode(value, &dst->choice.%s));\n",  \
        (typeName), (targetEnumTypeName), (attrName), (attrName));      \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                             \
        "PyCompatRelativeOID_AsUTF8String(&src->choice.%s));\n",        \
        (typeName), (attrName), (attrName));                            \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)


#define PY_GEN_CHOICE_STRING_GETSET(typeName, targetEnumTypeName, attrName,    \
                                    type_def_path)                             \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s,%s, "                               \
        "PyCompatUnicode_AsUTF8(value, &dst->choice.%s.buf, "                  \
        "&dst->choice.%s.size));\n",                                           \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (attrName)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatUnicode_FromStringAndSize(src->choice.%s.buf, "               \
        "src->choice.%s.size));\n",                                            \
        (typeName), (attrName), (attrName), (attrName));                       \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)

#define PY_GEN_CHOICE_ENUM_GETSET(typeName, targetEnumTypeName, attrName,      \
                                  enumName, isSigned, type_def_path)           \
    OUT("PY_IMPL_CHOICE_ATTR_FROMPY(%s, %s, %s, "                              \
        "PyCompatEnum_FromObject(value, "                                      \
        "(void *)&dst->choice.%s, %d));\n",                                    \
        (typeName), (targetEnumTypeName), (attrName), (attrName), (isSigned)); \
    OUT("PY_IMPL_CHOICE_ATTR_TOPY(%s, %s, "                                    \
        "PyCompatEnum_AsObject(PyAsnEnum%s_Type, (void *)&src->choice.%s, "    \
        "%d));\n",                                                             \
        (typeName), (attrName), (enumName), (attrName), (isSigned));           \
    PY_GEN_CHOICE_GETSET(typeName, targetEnumTypeName, attrName, type_def_path)

/* SEQUENCE */
#define PY_GEN_SEQ_GETSET(typeName, attrName, optional)               \
    OUT("PY_IMPL_SEQ_%sGETATTR(%s, %s);\n", (optional) ? "OPT_" : "", \
        typeName, attrName);                                          \
    OUT("PY_IMPL_SEQ_%sSETATTR(%s, %s);\n", (optional) ? "OPT_" : "", \
        typeName, attrName);

#define PY_GEN_SEQ_TYPEREF_CONV(typeName, targetTypeName, attrName, optional, \
                                indirect)                                     \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                  \
        "PyAsn%s_FromPython(value, (%s_t *)target));\n",                      \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName),    \
        (targetTypeName), (targetTypeName));                                  \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                    \
        "PyAsn%s_ToPython((%s_t *)target, parent));\n",                       \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName),    \
        (targetTypeName), (targetTypeName));

#define PY_GEN_SEQ_TYPEREF_GETSET(typeName, targetTypeName, attrName,     \
                                  optional, indirect)                     \
    PY_GEN_SEQ_TYPEREF_CONV(typeName, targetTypeName, attrName, optional, \
                            indirect);                                    \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_BOOLEAN_CONV(typeName, attrName, optional, indirect)     \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                \
        "PyCompatBool_FromObject(value, (unsigned int *)target));\n",       \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName)); \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                  \
        "PyCompatBool_FromLong(*(BOOLEAN_t *)target));\n",                  \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_BOOLEAN_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_BOOLEAN_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_INTEGER_CONV(typeName, attrName, optional, indirect,    \
                                isSigned)                                  \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                               \
        "PyCompatLong_FromObject(value, target, %d));\n",                  \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (isSigned));                                                       \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                 \
        "PyCompatLong_AsObject(target, %d));\n",                           \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (isSigned));

#define PY_GEN_SEQ_INTEGER_GETSET(typeName, attrName, optional, indirect,      \
                                  isSigned)                                    \
    PY_GEN_SEQ_INTEGER_CONV(typeName, attrName, optional, indirect, isSigned); \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_BYTES_CONV(typeName, attrName, optional, indirect)       \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                \
        "PyCompatBytes_ToStringAndSize(value, &((ASN__PRIMITIVE_TYPE_t "    \
        "*)target)->buf, &((ASN__PRIMITIVE_TYPE_t *)target)->size));\n",    \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName)); \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                  \
        "PyCompatBytes_FromStringAndSize(((ASN__PRIMITIVE_TYPE_t "          \
        "*)target)->buf, ((ASN__PRIMITIVE_TYPE_t *)target)->size));\n",     \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_BYTES_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_BYTES_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)


#define PY_GEN_SEQ_REAL_CONV(typeName, attrName, optional, indirect,       \
                             is_float32)                                   \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                               \
        "PyCompatFloat_FromObject(value, target, %d));\n",                 \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (is_float32));                                                     \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                 \
        "PyCompatFloat_AsObject(target, %d));\n",                          \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (is_float32));

#define PY_GEN_SEQ_REAL_GETSET(typeName, attrName, optional, indirect,        \
                               is_float32)                                    \
    PY_GEN_SEQ_REAL_CONV(typeName, attrName, optional, indirect, is_float32); \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)


#define PY_GEN_SEQ_NULL_CONV(typeName, attrName, optional, indirect)        \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                \
        "PyCompatNull_FromObject(value, (int *)target));\n",                \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName)); \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, Py_None);\n",                      \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_NULL_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_NULL_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)


#define PY_GEN_SEQ_OID_CONV(typeName, attrName, optional, indirect)          \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                 \
        "PyCompatOID_FromUnicode(value, (OBJECT_IDENTIFIER_t *)target));\n", \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));  \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                   \
        "PyCompatOID_AsUTF8String((OBJECT_IDENTIFIER_t *)target));\n",       \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_OID_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_OID_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)


#define PY_GEN_SEQ_RELATIVE_OID_CONV(typeName, attrName, optional, indirect) \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                 \
        "PyCompatRelativeOID_FromUnicode(value, (RELATIVE_OID_t "            \
        "*)target));\n",                                                     \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));  \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                   \
        "PyCompatRelativeOID_AsUTF8String((RELATIVE_OID_t *)target));\n",    \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_RELATIVE_OID_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_RELATIVE_OID_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_STRING_CONV(typeName, attrName, optional, indirect)        \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                  \
        "PyCompatUnicode_AsUTF8(value, &((OCTET_STRING_t *)(target))->buf, "  \
        "&((OCTET_STRING_t *)(target))->size);\n",                            \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));   \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                    \
        "PyCompatUnicode_FromStringAndSize(((OCTET_STRING_t *)target)->buf, " \
        "((OCTET_STRING_t *)target)->size);\n",                               \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_STRING_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_STRING_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)


#define PY_GEN_SEQ_ENUM_CONV(typeName, attrName, enumTypeName, isSigned,   \
                             optional, indirect)                           \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                               \
        "PyCompatEnum_FromObject(value, target, %d));\n",                  \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (isSigned));                                                       \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                 \
        "PyCompatEnum_AsObject(PyAsnEnum%s_Type, target, %d));\n",         \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName), \
        (enumTypeName), (isSigned));

#define PY_GEN_SEQ_ENUM_GETSET(typeName, attrName, enumTypeName, isSigned,     \
                               optional, indirect)                             \
    PY_GEN_SEQ_ENUM_CONV(typeName, attrName, enumTypeName, isSigned, optional, \
                         indirect);                                            \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_BITSTR_CONV(typeName, attrName, optional, indirect)       \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                 \
        "PyCompatBitArray_ToStringAndSize(value, &((BIT_STRING_t "           \
        "*)target)->buf, &((BIT_STRING_t *)target)->size));\n",              \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));  \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                   \
        "PyCompatBitArray_FromStringAndSize(((BIT_STRING_t *)target)->buf, " \
        "((BIT_STRING_t *)target)->size));\n",                               \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName));

#define PY_GEN_SEQ_BITSTR_GETSET(typeName, attrName, optional, indirect) \
    PY_GEN_SEQ_BITSTR_CONV(typeName, attrName, optional, indirect);      \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

#define PY_GEN_SEQ_NAMED_BITSTR_CONV(typeName, attrName, enumTypeName,      \
                                     optional, indirect)                    \
    OUT("PY_IMPL_SEQ_ATTR%s_FROMPY(%s, %s, "                                \
        "PyCompatFlag_FromObject(value, &((BIT_STRING_t "                   \
        "*)target)->buf, &((BIT_STRING_t *)target)->size));\n",             \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName)); \
    OUT("PY_IMPL_SEQ_ATTR%s_TOPY(%s, %s, "                                  \
        "PyCompatFlag_AsObject(PyAsnEnum%s_Type, ((BIT_STRING_t "           \
        "*)target)->buf, "                                                  \
        "((BIT_STRING_t *)target)->size));\n",                              \
        (optional || indirect ? "_INDIRECT" : ""), (typeName), (attrName),  \
        (enumTypeName));

#define PY_GEN_SEQ_NAMED_BITSTR_GETSET(typeName, attrName, enumTypeName,     \
                                       optional, indirect)                   \
    PY_GEN_SEQ_NAMED_BITSTR_CONV(typeName, attrName, enumTypeName, optional, \
                                 indirect);                                  \
    PY_GEN_SEQ_GETSET(typeName, attrName, optional)

/* SET */
#define PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)     \
    OUT("PY_IMPL_SET_%sGETATTR(%s, %s, %s);\n", (optional) ? "OPT_" : "", \
        typeName, enumTypeName, attrName);                                \
    OUT("PY_IMPL_SET_%sSETATTR(%s, %s, %s);\n", (optional) ? "OPT_" : "", \
        typeName, enumTypeName, attrName);

#define PY_GEN_SET_NAMED_BITSTR_GETSET(typeName, typeEnumTypeName, attrName, \
                                       enumTypeName, optional, indirect)     \
    PY_GEN_SEQ_NAMED_BITSTR_CONV(typeName, attrName, enumTypeName, optional, \
                                 indirect);                                  \
    PY_GEN_SET_GETSET(typeName, typeEnumTypeName, attrName, optional)

#define PY_GEN_SET_BITSTR_GETSET(typeName, enumTypeName, attrName, optional, \
                                 indirect)                                   \
    PY_GEN_SEQ_BITSTR_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_ENUM_GETSET(typeName, typeEnumTypeName, attrName,           \
                               enumTypeName, isSigned, optional, indirect)     \
    PY_GEN_SEQ_ENUM_CONV(typeName, attrName, enumTypeName, isSigned, optional, \
                         indirect);                                            \
    PY_GEN_SET_GETSET(typeName, typeEnumTypeName, attrName, optional)

#define PY_GEN_SET_STRING_GETSET(typeName, enumTypeName, attrName, optional, \
                                 indirect)                                   \
    PY_GEN_SEQ_STRING_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_RELATIVE_OID_GETSET(typeName, enumTypeName, attrName,  \
                                       optional, indirect)                \
    PY_GEN_SEQ_RELATIVE_OID_CONV(typeName, attrName, optional, indirect); \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_OID_GETSET(typeName, enumTypeName, attrName, optional, \
                              indirect)                                   \
    PY_GEN_SEQ_OID_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_NULL_GETSET(typeName, enumTypeName, attrName, optional, \
                               indirect)                                   \
    PY_GEN_SEQ_NULL_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_REAL_GETSET(typeName, enumTypeName, attrName, optional,    \
                               indirect, is_float32)                          \
    PY_GEN_SEQ_REAL_CONV(typeName, attrName, optional, indirect, is_float32); \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_BYTES_GETSET(typeName, enumTypeName, attrName, optional, \
                                indirect)                                   \
    PY_GEN_SEQ_BYTES_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_INTEGER_GETSET(typeName, enumTypeName, attrName, optional,  \
                                  indirect, isSigned)                          \
    PY_GEN_SEQ_INTEGER_CONV(typeName, attrName, optional, indirect, isSigned); \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_BOOLEAN_GETSET(typeName, enumTypeName, attrName, optional, \
                                  indirect)                                   \
    PY_GEN_SEQ_BOOLEAN_CONV(typeName, attrName, optional, indirect);          \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

#define PY_GEN_SET_TYPEREF_GETSET(typeName, enumTypeName, targetTypeName, \
                                  attrName, optional, indirect)           \
    PY_GEN_SEQ_TYPEREF_CONV(typeName, targetTypeName, attrName, optional, \
                            indirect);                                    \
    PY_GEN_SET_GETSET(typeName, enumTypeName, attrName, optional)

/* SET OF / SEQUENCE OF */
#define PY_GEN_SEQ_OF_TYPEREF_BASIC(seqTypeName, typeName, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_FROMPY(%s, %s_t);\n", seqTypeName, typeName);        \
    if((constrParentPath) != NULL) {                                         \
        OUT("PY_IMPL_SEQ_OF_GENERIC_SETITEM(%s, %s_t, "                      \
            "*PY_IMPL_SEQ_OF_COMPONENT_TYPE(%s));\n",                        \
            seqTypeName, typeName, constrParentPath);                        \
    } else {                                                                 \
        OUT("PY_IMPL_SEQ_OF_SETITEM(%s, %s_t);\n", seqTypeName, typeName);   \
    }                                                                        \
    OUT("PY_IMPL_SEQ_OF_ADD(%s, %s_t);\n", seqTypeName, typeName);

#define PY_GEN_SEQ_OF_BASIC(seqTypeName, typeName, constrParentPath)     \
    OUT("PY_IMPL_SEQ_OF_FROMPY(%s, %s);\n", seqTypeName, typeName);      \
    if((constrParentPath) != NULL) {                                     \
        OUT("PY_IMPL_SEQ_OF_GENERIC_SETITEM(%s, %s, "                    \
            "*PY_IMPL_SEQ_OF_COMPONENT_TYPE(%s));\n",                    \
            seqTypeName, typeName, constrParentPath);                    \
    } else {                                                             \
        OUT("PY_IMPL_SEQ_OF_SETITEM(%s, %s);\n", seqTypeName, typeName); \
    }                                                                    \
    OUT("PY_IMPL_SEQ_OF_ADD(%s, %s);\n", seqTypeName, typeName);

#define PY_GEN_SEQ_OF_BASICSTR(seqTypeName, typeName, constrParentPath)   \
    OUT("PY_IMPL_SEQ_OF_FROMPY(%s, " #typeName ");\n", seqTypeName);      \
    if((constrParentPath) != NULL) {                                      \
        OUT("PY_IMPL_SEQ_OF_GENERIC_SETITEM(%s, " #typeName               \
            ", *PY_IMPL_SEQ_OF_COMPONENT_TYPE(%s));\n",                   \
            seqTypeName, constrParentPath);                               \
    } else {                                                              \
        OUT("PY_IMPL_SEQ_OF_SETITEM(%s, " #typeName ");\n", seqTypeName); \
    }                                                                     \
    OUT("PY_IMPL_SEQ_OF_ADD(%s, " #typeName ");\n", seqTypeName);

#define PY_GEN_SEQ_OF_TYPEREF_GETSET(seqTypeName, refTypeName,       \
                                     constrParentPath)               \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, %s_t, PyAsn%s_ToPython((%s_t " \
        "*)src, parent));\n",                                        \
        seqTypeName, refTypeName, refTypeName, refTypeName);         \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, %s_t, PyAsn%s_FromPython("   \
        "value, (%s_t *)target));\n",                                \
        seqTypeName, refTypeName, refTypeName, refTypeName);         \
    PY_GEN_SEQ_OF_TYPEREF_BASIC(seqTypeName, refTypeName, constrParentPath)

#define PY_GEN_SEQ_OF_INTEGER_GETSET(seqTypeName, isSigned, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, %s, "                                 \
        "PyCompatLong_FromObject(value, target, %d));\n",                     \
        (seqTypeName), (isSigned ? "Py_ssize_t" : "unsigned long"),           \
        (isSigned));                                                          \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, %s, "                                   \
        "PyCompatLong_AsObject(src, %d));\n",                                 \
        (seqTypeName), (isSigned ? "Py_ssize_t" : "unsigned long"),           \
        (isSigned));                                                          \
    PY_GEN_SEQ_OF_BASIC(seqTypeName,                                          \
                        (isSigned ? "Py_ssize_t" : "unsigned long"),          \
                        constrParentPath)

#define PY_GEN_SEQ_OF_BOOLEAN_GETSET(seqTypeName, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, bool, "                     \
        "PyCompatBool_FromObject(value, target));\n",               \
        (seqTypeName));                                             \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, bool, "                       \
        "PyCompatBool_AsObject(src));\n",                           \
        (seqTypeName));                                             \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, bool, constrParentPath)

#define PY_GEN_SEQ_OF_BYTES_GETSET(seqTypeName, constrParentPath)   \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, OCTET_STRING_t, "           \
        "PyCompatBytes_ToStringAndSize(value, &target->buf, "       \
        "&target->size));\n",                                       \
        (seqTypeName));                                             \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, OCTET_STRING_t, "             \
        "PyCompatBytes_FromStringAndSize(src->buf, src->size));\n", \
        (seqTypeName));                                             \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, OCTET_STRING_t, constrParentPath)

#define PY_GEN_SEQ_OF_REAL_GETSET(seqTypeName, is_float32, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, %s, "                                \
        "PyCompatFloat_FromObject(value, (void *)target, %d));\n",           \
        (seqTypeName), (is_float32 ? "float" : "double"), (is_float32));     \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, %s, "                                  \
        "PyCompatFloat_AsObject((void *)src, %d));\n",                       \
        (seqTypeName), (is_float32 ? "float" : "double"), (is_float32));     \
    PY_GEN_SEQ_OF_BASIC(seqTypeName, (is_float32 ? "float" : "double"),      \
                        constrParentPath)

#define PY_GEN_SEQ_OF_NULL_GETSET(seqTypeName, constrParentPath)          \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, NULL_t, "                         \
        "PyCompatNull_FromObject(value, target));\n",                     \
        seqTypeName);                                                     \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, NULL_t, Py_None);\n", seqTypeName); \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, NULL_t, constrParentPath)

#define PY_GEN_SEQ_OF_OID_GETSET(seqTypeName, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, OBJECT_IDENTIFIER_t, "  \
        "PyCompatOID_FromUnicode(value, target));\n",           \
        (seqTypeName));                                         \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, OBJECT_IDENTIFIER_t, "    \
        "PyCompatOID_AsUTF8String(src));\n",                    \
        (seqTypeName));                                         \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, OBJECT_IDENTIFIER_t, constrParentPath)

#define PY_GEN_SEQ_OF_RELATIVE_OID_GETSET(seqTypeName, constrParentPath) \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, RELATIVE_OID_t, "                \
        "PyCompatRelativeOID_FromUnicode(value, target));\n",            \
        (seqTypeName));                                                  \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, RELATIVE_OID_t, "                  \
        "PyCompatRelativeOID_AsUTF8String(src));\n",                     \
        (seqTypeName));                                                  \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, RELATIVE_OID_t, constrParentPath)

#define PY_GEN_SEQ_OF_BITSTRING_GETSET(seqTypeName, constrParentPath)  \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, BIT_STRING_t, "                \
        "PyCompatBitArray_ToStringAndSize(value, &target->buf, "       \
        "&target->size));\n",                                          \
        (seqTypeName));                                                \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, BIT_STRING_t, "                  \
        "PyCompatBitArray_FromStringAndSize(src->buf, src->size));\n", \
        (seqTypeName));                                                \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, BIT_STRING_t, constrParentPath)

#define PY_GEN_SEQ_OF_STRING_GETSET(seqTypeName, constrParentPath)        \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, OCTET_STRING_t, "                 \
        "PyCompatUnicode_AsUTF8(value, &target->buf, &target->size));\n", \
        (seqTypeName));                                                   \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, OCTET_STRING_t, "                   \
        "PyCompatUnicode_FromStringAndSize(src->buf, src->size));\n",     \
        (seqTypeName));                                                   \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, OCTET_STRING_t, constrParentPath)

#define PY_GEN_SEQ_OF_ENUM_GETSET(seqTypeName, enumTypeName, is_signed,        \
                                  constrParentPath)                            \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, %s, PyCompatEnum_FromObject(value, "   \
        "(void*)target, %d));\n",                                              \
        seqTypeName, (is_signed ? "Py_ssize_t" : "unsigned long"), is_signed); \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, %s, "                                    \
        "PyCompatEnum_AsObject(PyAsnEnum%s_Type, (void*)src, %d));\n",         \
        seqTypeName, (is_signed ? "Py_ssize_t" : "unsigned long"),             \
        enumTypeName, is_signed);                                              \
    PY_GEN_SEQ_OF_BASIC(seqTypeName,                                           \
                        (is_signed ? "Py_ssize_t" : "unsigned long"),          \
                        constrParentPath)

#define PY_GEN_SEQ_OF_NAMED_BITSTR_GETSET(seqTypeName, enumTypeName,        \
                                          constrParentPath)                 \
    OUT("PY_IMPL_SEQ_OF_ITEM_FROMPY(%s, BIT_STRING_t, "                     \
        "PyCompatFlag_FromObject(value, &target->buf, "                     \
        "&target->size));\n",                                               \
        (seqTypeName));                                                     \
    OUT("PY_IMPL_SEQ_OF_ITEM_TOPY(%s, BIT_STRING_t, "                       \
        "PyCompatFlag_AsObject(PyAsnEnum%s_Type, src->buf, src->size));\n", \
        (seqTypeName), enumTypeName);                                       \
    PY_GEN_SEQ_OF_BASICSTR(seqTypeName, BIT_STRING_t, constrParentPath)

#endif