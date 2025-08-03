#ifndef ASN1_COMPILED_PY_OUTPUT_H
#define ASN1_COMPILED_PY_OUTPUT_H

#include "asn1c_out.h"

#define PY_GEN_BEGIN_FUNC() \
    OUT("{\n");             \
    INDENT(+1)

#define PY_GEN_END_FUNC() \
    INDENT(-1);           \
    OUT("}\n")

#define PY_GEN_ASNTYPE_INIT_INLINE(name)                                 \
    OUT("static inline void Py%s_Init(Py%sObject *pObj)\n", name, name); \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_INIT(name) \
    OUT("void Py%s_Init(Py%sObject *pObj);\n", name, name)

#define PY_GEN_ASNTYPE_FROMPY_INLINE(name)                                 \
    OUT("static inline int Py%s_FromPython(PyObject *pObj, %s_t *pDst)\n", \
        name, name);                                                       \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_FROMPY(name) \
    OUT("int Py%s_FromPython(PyObject *pObj, %s_t *pDst);\n", name, name)

#define PY_GEN_ASNTYPE_TOPY_INLINE(name)                                    \
    OUT("static inline PyObject *Py%s_ToPython(%s_t *pSrc)\n", name, name); \
    PY_GEN_BEGIN_FUNC()

#define PY_GEN_ASNTYPE_TOPY(name) \
    OUT("PyObject *Py%s_ToPython(%s_t *pSrc);\n", name, name)


#endif