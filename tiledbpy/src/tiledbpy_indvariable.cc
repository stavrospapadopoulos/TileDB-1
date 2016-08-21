/**
 * @file   tiledbpy_indvariable.cc
 *
 * @section LICENSE
 *
 * The MIT License
 * 
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * @section DESCRIPTION
 *
 * This file implements tiledbpy.IndVariable.
 */

#include <iostream>
#include "c_api.h"
#include "tiledbpy.h"
#include "tiledbpy_expression.h"
#include "tiledbpy_indvariable.h"




/* ****************************** */
/*             MACROS             */
/* ****************************** */

#define ERRMSG(x) std::string(x) + "\n --> " + std::string(tiledb_errmsg)



/* Performs l op r */
static PyObject* IndVariable_binary_op(PyObject* l, PyObject *r, int op) {
  // Initialization
  TileDB_Expression* expr_l = NULL;
  TileDB_Expression* expr_r = NULL;
 
  // Get left and right expressions
  if(PyObject_TypeCheck(l, &IndVariableType)) {        // - Left is IndVariable
    // Create right expression
    if(PyLong_Check(r)) {                                // Constant long
      int type = TILEDB_EXPR_INT64;
      int64_t value = PyLong_AsLong(r);
      if(tiledb_expression_init(&expr_r, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyFloat_Check(r)) {                        // Constant double
      int type = TILEDB_EXPR_FLOAT64;
      double value = PyFloat_AsDouble(r);
      if(tiledb_expression_init(&expr_r, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyObject_TypeCheck(r, &IndVariableType)) { // IndVariable
      int type = TILEDB_EXPR_VAR;
      void* value = ((IndVariable*)r)->name;
      if(tiledb_expression_init(&expr_r, type, value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyObject_TypeCheck(r, &ExpressionType)) {  // Expression
      expr_r = ((Expression*)r)->expr;
    } else {                                             // Error
      std::string errmsg = "Binary operation with IndVariable failed";
      PyErr_SetString(PyExc_TypeError, errmsg.c_str());
      return NULL;
    }

    // Create left expression
    int type = TILEDB_EXPR_VAR;
    void* value = ((IndVariable*)l)->name;
    if(tiledb_expression_init(&expr_l, type, value) != TILEDB_OK) {
      if(expr_r != NULL && !PyObject_TypeCheck(r, &ExpressionType))
        tiledb_expression_clear(expr_r);
      std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
      PyErr_SetString(tiledbpy_error, errmsg.c_str());
      return NULL;
    }
  } else if(PyObject_TypeCheck(r, &IndVariableType)) { // - Right is IndVariable
    if(PyLong_Check(l)) {                                // Constant long
      int type = TILEDB_EXPR_INT64;
      int64_t value = PyLong_AsLong(l);
      if(tiledb_expression_init(&expr_l, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyFloat_Check(l)) {                        // Constant double
      int type = TILEDB_EXPR_FLOAT64;
      double value = PyFloat_AsDouble(l);
      if(tiledb_expression_init(&expr_l, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else {                                             // Error
      std::string errmsg = "Binary operation with IndVariable failed";
      PyErr_SetString(PyExc_TypeError, errmsg.c_str());
      return NULL;
    }

    // Create right expression
    int type = TILEDB_EXPR_VAR;
    void* value = ((IndVariable*)r)->name;
    if(tiledb_expression_init(&expr_r, type, value) != TILEDB_OK) {
      if(expr_l != NULL)
        tiledb_expression_clear(expr_l);
      std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
      PyErr_SetString(tiledbpy_error, errmsg.c_str());
      return NULL;
    }
  } 

  // Perform binary operation
  TileDB_Expression* expr;
  if(tiledb_expression_binary_op(expr_l, expr_r, &expr, op) 
     != TILEDB_OK) {
    std::string errmsg = ERRMSG("Binary operation with IndVariable failed");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Create a new Expression object 
  PyObject* empty_list = Py_BuildValue("()");
  PyObject* res = PyObject_CallObject((PyObject*) &ExpressionType, empty_list);
  ((Expression*)res)->expr = expr;

  return res;
}

static PyObject* IndVariable_add(PyObject* l, PyObject *r) {
  return IndVariable_binary_op(l, r, TILEDB_EXPR_OP_ADD);
}

static PyObject* IndVariable_sub(PyObject* l, PyObject *r) {
  return IndVariable_binary_op(l, r, TILEDB_EXPR_OP_SUB);
}

static PyObject* IndVariable_mul(PyObject* l, PyObject *r) {
  return IndVariable_binary_op(l, r, TILEDB_EXPR_OP_MUL);
}

static PyObject* IndVariable_div(PyObject* l, PyObject *r) {
  return IndVariable_binary_op(l, r, TILEDB_EXPR_OP_DIV);
}

static PyObject* IndVariable_mod(PyObject* l, PyObject *r) {
  return IndVariable_binary_op(l, r, TILEDB_EXPR_OP_MOD);
}

/* Numeric methods for IndVariable. */
static PyNumberMethods IndVariable_as_number = {
  /* nb_add */
  (binaryfunc)IndVariable_add,
  /* nb_subtract */
  (binaryfunc)IndVariable_sub,
  /* nb_multiply */
  (binaryfunc)IndVariable_mul,
  /* nb_remainder */
  (binaryfunc)IndVariable_mod,
  /* nb_divmod */
  0,
  /* nb_power */
  0,
  /* nb_negative */
  0,
  /* nb_positive */
  0,
  /* nb_absolute */
  0,
  /* nb_bool */
  0,
  /* nb_invert */
  0,
  /* nb_lshift */
  0,
  /* nb_rshift */
  0,
  /* nb_and */
  0,
  /* nb_xor */
  0,
  /* nb_or */
  0,
  /* nb_int */
  0,
  /* nb_reserved */
  0,
  /* nb_float */
  0,
  /* nb_inplace_add */
  0,
  /* nb_inplace_subtract */
  0,
  /* nb_inplace_multiply */
  0,
  /* nb_inplace_remainder */
  0,
  /* nb_inplace_power */
  0,
  /* nb_inplace_lshift */
  0,
  /* nb_inplace_rshift */
  0,
  /* nb_inplace_and */
  0,
  /* nb_inplace_xor */
  0,
  /* nb_inplace_or */
  0,
  /* nb_floor_divide */
  0,
  /* nb_true_divide */
  (binaryfunc)IndVariable_div,
  /* nb_inplace_floor_divide */
  0,
  /* nb_inplace_true_divide */
  0,
  /* nb_index */
  0
};

/* Called when an IndVariable object is deleted. */
static void IndVariable_dealloc(IndVariable* self) {
  // Free name
  if(self->name != NULL)
    free(self->name);

  // Free IndVariable object
  Py_TYPE(self)->tp_free((PyObject*)self);
}

/* Called when an IndVariable object is created. */
static PyObject * IndVariable_new(
    PyTypeObject *type, 
    PyObject *args, 
    PyObject *kwds) {
  // Create a new IndVariable object 
  IndVariable *self = (IndVariable *)type->tp_alloc(type, 0);

  // Initializations
  self->name = NULL;

  // Success
  return (PyObject *)self;
}

static int IndVariable_init(IndVariable *self, PyObject *args, PyObject *kwds) {
  // Keyword list
  static const char *kwlist[] = {"name", NULL};

  // Parse arguments
  const char* Name = NULL;
  if (!PyArg_ParseTupleAndKeywords(
       args, 
       kwds, 
       "s; Failed to initialize IndVariable object; Invalid arguments", 
       (char**) kwlist, 
       &Name)) 
    return -1;

  // Sanity check on name length
  size_t name_len = strlen(Name);
  if(name_len > TILEDB_NAME_MAX_LEN) {
    PyErr_SetString(
        PyExc_TypeError, 
        "Failed to initialize IndVariable object; "
        "Invalid variable name length");
    return -1;
  }

  // Copy arguments to members
  if(self->name == NULL) {
    self->name = (char*) malloc(name_len);
  } else if(strlen(self->name) < name_len) {
    free(self->name);
    self->name = (char*) malloc(name_len);
  }
  strcpy(self->name, Name);

  // Success
  return 0;
}

/* Getter for name. */
static PyObject* IndVariable_getname(IndVariable* self, void* closure) {
  if(self->name != NULL)
    return Py_BuildValue("s", self->name);
  else
    return Py_BuildValue("s", "");
}

/* Setter for name */
static int IndVariable_setname(
    IndVariable* self, 
    PyObject* value, 
    void* closure) {
  // Check type
  if(!PyUnicode_Check(value)) {
    PyErr_SetString(
        PyExc_TypeError, 
        "Failed to set variable name; Invalid type");
    return -1;
  }

  // Get argument and check length
  PyObject* pystring = PyUnicode_AsASCIIString(value);
  const char* Name = PyBytes_AsString(pystring);
  size_t name_len = strlen(Name);
  if(name_len > TILEDB_NAME_MAX_LEN) {
    PyErr_SetString(
        PyExc_TypeError, 
        "Failed to initialize IndVariable object; "
        "Invalid variable name length");
    return -1;
  }

  // Copy argument to member 
  if(self->name == NULL) {
    self->name = (char*) malloc(name_len);
  } else if(strlen(self->name) < name_len) {
    free(self->name);
    self->name = (char*) malloc(name_len);
  }
  strcpy(self->name, Name);

  // Success
  return 0; 
}

/* Definition of getter and setters. */
static PyGetSetDef IndVariable_getset[] = {
  {(char*) "name", 
   (getter)IndVariable_getname, 
   (setter)IndVariable_setname, 
   (char*) "Independent variable name", 
   NULL},
  {NULL}
};

/* The IndVariable type definition. */
PyTypeObject IndVariableType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  /* tp_name */
  "tiledbpy.IndVariable", 
  /* tp_basicsize */
  sizeof(IndVariable), 
  /* tp_itemsize */
  0,                               
  /* tp_dealloc */
  (destructor)IndVariable_dealloc,
  /* tp_print */
  0,
  /* tp_getattr */
  0, 
  /* tp_setattr */
  0, 
  /* tp_reserved */
  0, 
  /* tp_repr */
  0, 
  /* tp_as_number */
  &IndVariable_as_number, 
  /* tp_as_sequence */
  0,                         
  /* tp_as_mapping */
  0, 
  /* tp_hash  */
  0,  
  /* tp_call */
  0, 
  /* tp_str */
  0, 
  /* tp_getattro */
  0,                
  /* tp_setattro */
  0, 
  /* tp_as_buffer */
  0, 
  /* tp_flags */
  Py_TPFLAGS_DEFAULT,        
  /* tp_doc */
  TILEDBPY_INDVAR_DOC,
  /* tp_traverse */
  0,                         
  /* tp_clear */
  0, 
  /* tp_richcompare */
  0, 
  /* tp_weaklistoffset */
  0,                         
  /* tp_iter */
  0, 
  /* tp_iternext */
  0, 
  /* tp_methods */
  0,
  /* tp_members */
  0,
  /* tp_getset */
  IndVariable_getset, 
  /* tp_base */
  0, 
  /* tp_dict */
  0, 
  /* tp_descr_get */
  0, 
  /* tp_descr_set */
  0, 
  /* tp_dictoffset */
  0, 
  /* tp_init */
  (initproc)IndVariable_init, 
  /* tp_alloc */
  0,
  /* tp_new */
  IndVariable_new, 
};

