#ifndef ASN1_COMPILED_PY_OUTPUT_H
#define ASN1_COMPILED_PY_OUTPUT_H

#include "asn1c_out.h"

#define PY_GEN_BEGIN_FUNC() \
    OUT("{\n");             \
    INDENT(+1)

#define PY_GEN_END_FUNC() \
    INDENT(-1);           \
    OUT("}\n")

#define PY_GEN_ASNTYPE_FROMPY_INLINE(name)                                    \
    OUT("static inline int PyAsn%s_FromPython(PyObject *pObj, %s_t *pDst)\n", \
        name, name);                                                          \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_FROMPY(name) \
    OUT("int PyAsn%s_FromPython(PyObject *pObj, %s_t *pDst);\n", name, name)

#define PY_GEN_ASNTYPE_TOPY_INLINE(name)                                       \
    OUT("static inline PyObject *PyAsn%s_ToPython(%s_t *pSrc)\n", name, name); \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_TOPY(name) \
    OUT("PyObject *PyAsn%s_ToPython(%s_t *pSrc);\n", name, name)

#define PY_GEN_TYPE_NEW(name) OUT("PY_IMPL_GENERIC_NEW(%s);\n", name)

#define PY_GEN_TYPE_DEALLOC(name) OUT("PY_IMPL_GENERIC_DEALLOC(%s);\n", name)

#define PY_GEN_TYPE_REPR(name) OUT("PY_IMPL_GENERIC_REPR(%s);\n", name)

#define PY_GEN_TYPE_INIT(name) OUT("PY_IMPL_GENERIC_INIT(%s);\n", name)

#define PY_GEN_TYPE_CHECK_CONSTRAINTS(name) \
    OUT("PY_IMPL_GENERIC_CHECK_CONSTRAINTS(%s);\n", name)

#define PY_GEN_TYPE_IS_VALID(name) OUT("PY_IMPL_GENERIC_IS_VALID(%s);\n", name)

#define PY_GEN_TYPE_ENCODE(name) OUT("PY_IMPL_GENERIC_ENCODE(%s);\n", name)

#define PY_GEN_TYPE_DECODE(name) OUT("PY_IMPL_GENERIC_DECODE(%s);\n", name)

#define PY_GEN_TYPE_DEFAULT_MEMBER(name) \
    OUT("PY_IMPL_MEMBER_GETSET(%s, value, %s, ob_value);\n", name, name)

#define PY_GEN_TYPE_DEFAULT_METHODS(name)                                    \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, is_valid, METH_NOARGS),\n", name);       \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, check_constraints, METH_NOARGS),\n",     \
        name);                                                               \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, encode, METH_NOARGS),\n", name);         \
    OUT("PY_IMPL_METHODDEF_ITEM(%s, decode, METH_VARARGS | METH_STATIC),\n", \
        name);

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

#define PY_GEN_CLASS_BEGIN(modName, typeName)                       \
    OUT("PyTypeObject PyAsn%s_Type = {\n", typeName);               \
    INDENT(+1);                                                     \
    OUT("PyVarObject_HEAD_INIT(NULL, 0)\n");                        \
    OUT(".tp_name = \"%s.%s\",\n", (modName), typeName);            \
    OUT(".tp_basicsize = sizeof(PyAsn%sObject),\n", typeName);      \
    OUT(".tp_doc = \"ASN.1 %s type\",\n", typeName);                \
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

#define PY_GEN_BASIC_CLASS(modName, typeName) \
    REDIR(OT_PY_IMPL_CODE);                   \
    PY_GEN_TYPE_NEW(typeName);                \
    PY_GEN_TYPE_INIT(typeName);               \
    PY_GEN_TYPE_DEALLOC(typeName);            \
    PY_GEN_TYPE_REPR(typeName);               \
    PY_GEN_TYPE_CHECK_CONSTRAINTS(typeName);  \
    PY_GEN_TYPE_IS_VALID(typeName);           \
    PY_GEN_TYPE_ENCODE(typeName);             \
    PY_GEN_TYPE_DECODE(typeName);             \
    PY_GEN_TYPE_DEFAULT_MEMBER(typeName);     \
    /*methods*/                               \
    REDIR(OT_PY_IMPL_METHODS);                \
    PY_GEN_TYPE_METHODS_BEGIN(typeName);      \
    PY_GEN_TYPE_DEFAULT_METHODS(typeName);    \
    PY_GEN_TYPE_METHODS_END();                \
    /*attributes*/                            \
    REDIR(OT_PY_IMPL_ATTRS);                  \
    PY_GEN_TYPE_ATTRS_BEGIN(typeName);        \
    PY_GEN_TYPE_ATTR(typeName, value);        \
    PY_GEN_TYPE_ATTRS_END();                  \
    /*type def*/                              \
    REDIR(OT_PY_IMPL_CLASS);                  \
    PY_GEN_CLASS_BEGIN((modName), typeName);  \
    PY_GEN_CLASS_END();

#define PY_GEN_MOD_SETUP_SINGLE(typeName) \
    OUT("if (PyType_Ready(&PyAsn%s_Type) < 0) return -1;\n", typeName);

#define PY_GEN_MOD_ADD_OBJECT(typeName) \
    OUT("PY_IMPL_MOD_ADD_OBJECT(mod, %s);\n", typeName);

#define PY_GEN_MOD_INIT_SINGLE(typeName) PY_GEN_MOD_ADD_OBJECT(typeName);

#define PY_GEN_MOD_BASIC(typeName)     \
    INDENT(+1);                        \
    REDIR(OT_PY_IMPL_CODE_MOD_SETUP);  \
    PY_GEN_MOD_SETUP_SINGLE(typeName); \
    REDIR(OT_PY_IMPL_CODE_MOD_INIT);   \
    PY_GEN_MOD_INIT_SINGLE(typeName);  \
    INDENT(-1)

#define PY_GEN_MODULE_ADD_TYPE(typeName)                               \
    REDIR(OT_PY_IMPL_MOD_SETUP_TYPES);                                 \
    INDENT(+1);                                                        \
    OUT("if (PyAsn%s_ModSetupTypes() < 0) return NULL;\n", typeName);  \
    INDENT(-1);                                                        \
    REDIR(OT_PY_IMPL_MOD_CLEAR);                                       \
    INDENT(+1);                                                        \
    OUT("PyAsn%s_ModClear(m);\n", typeName);                           \
    INDENT(-1);                                                        \
    REDIR(OT_PY_IMPL_MOD_INIT);                                        \
    INDENT(+1);                                                        \
    OUT("if (PyAsn%s_ModInit(nModule) < 0) return NULL;\n", typeName); \
    INDENT(-1);                                                        \
    REDIR(OT_PY_IMPL_MOD_INCLUDES);                                    \
    OUT("#include \"%s_Py.h\"\n", typeName)

#endif