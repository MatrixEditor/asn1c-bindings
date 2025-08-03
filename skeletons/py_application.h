#ifndef _PyApplication_H_
#define _PyApplication_H_

#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef struct {
    PyObject* str__write;
    PyObject* str__getvalue;

    PyObject* PyBytesIO_Type;
    PyObject* PyBitArray_Type;
} PyCompatTable_t;

extern PyCompatTable_t* PyCompatTable;

int PyCompat_Init(void);
void PyCompat_Clear(void);

#define PyCompat_DEF_STRUCT(name)        \
    typedef struct _##name##_Py {        \
        PyObject_HEAD name##_t ob_value; \
        int s_valid;                     \
    } Py##name##Object;

#define PyCompat_DEF_TYPE(name) extern PyTypeObject Py##name##_Type;

#endif