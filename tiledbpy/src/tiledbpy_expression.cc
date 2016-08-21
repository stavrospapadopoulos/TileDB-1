/**
 * @file   tiledbpy_expression.cc
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
 * This file implements tiledbpy.Expression.
 */

#include <cassert>
#include <iostream>
#include "c_api.h"
#include "tiledbpy.h"
#include "tiledbpy_indvariable.h"
#include "tiledbpy_expression.h"
#include "tiledbpy_parse.h"




/* ****************************** */
/*             MACROS             */
/* ****************************** */

#define ERRMSG(x) std::string(x) + "\n --> " + std::string(tiledb_errmsg)




/* Performs l op r */
static PyObject* Expression_binary_op(PyObject* l, PyObject *r, int op) {
  // Initialization
  TileDB_Expression* expr_l = NULL;
  TileDB_Expression* expr_r = NULL;
 
  // Get left and right expressions
  if(PyObject_TypeCheck(l, &ExpressionType)) {          // - Left is Expression
    // Create right expression
    if(PyLong_Check(r)) {                                // Constant long
      int type = TILEDB_EXPR_INT64;
      int64_t value = PyLong_AsLong(r);
      if(tiledb_expression_init(&expr_r, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with Expression failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyFloat_Check(r)) {                        // Constant double
      int type = TILEDB_EXPR_FLOAT64;
      double value = PyFloat_AsDouble(r);
      if(tiledb_expression_init(&expr_r, type, &value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with Expression failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyObject_TypeCheck(r, &IndVariableType)) { // IndVariable
      int type = TILEDB_EXPR_VAR;
      void* value = ((IndVariable*)r)->name;
      if(tiledb_expression_init(&expr_r, type, value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with Expression failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else if(PyObject_TypeCheck(r, &ExpressionType)) {  // Expression
      expr_r = ((Expression*)r)->expr;
    } else {                                             // Error
      std::string errmsg = "Binary operation with Expression failed";
      PyErr_SetString(PyExc_TypeError, errmsg.c_str());
      return NULL;
    }

    // Create left expression
    expr_l = ((Expression*)l)->expr;
  } else if(PyObject_TypeCheck(r, &ExpressionType)) { // - Right is Expression
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
    } else if(PyObject_TypeCheck(l, &IndVariableType)) { // IndVariable
      int type = TILEDB_EXPR_VAR;
      void* value = ((IndVariable*)l)->name;
      if(tiledb_expression_init(&expr_l, type, value) != TILEDB_OK) {
        std::string errmsg = ERRMSG("Binary operation with Expression failed");
        PyErr_SetString(tiledbpy_error, errmsg.c_str());
        return NULL;
      }
    } else {                                             // Error
      std::string errmsg = "Binary operation with Expression failed";
      PyErr_SetString(PyExc_TypeError, errmsg.c_str());
      return NULL;
    }

    // Create right expression
    expr_r = ((Expression*)r)->expr;
  } 

  // Perform binary operation
  TileDB_Expression* expr;
  if(tiledb_expression_binary_op(expr_l, expr_r, &expr, op) 
     != TILEDB_OK) {
    std::string errmsg = ERRMSG("Binary operation with Expression failed");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Create a new Expression object 
  PyObject* empty_list = Py_BuildValue("()");
  PyObject* res = PyObject_CallObject((PyObject*) &ExpressionType, empty_list);
  ((Expression*)res)->expr = expr;

  return res;
}

static PyObject* Expression_add(PyObject* l, PyObject *r) {
  return Expression_binary_op(l, r, TILEDB_EXPR_OP_ADD);
}

static PyObject* Expression_sub(PyObject* l, PyObject *r) {
  return Expression_binary_op(l, r, TILEDB_EXPR_OP_SUB);
}

static PyObject* Expression_mul(PyObject* l, PyObject *r) {
  return Expression_binary_op(l, r, TILEDB_EXPR_OP_MUL);
}

static PyObject* Expression_div(PyObject* l, PyObject *r) {
  return Expression_binary_op(l, r, TILEDB_EXPR_OP_DIV);
}

static PyObject* Expression_mod(PyObject* l, PyObject *r) {
  return Expression_binary_op(l, r, TILEDB_EXPR_OP_MOD);
}

static PyNumberMethods Expression_as_number = {
  /* nb_add */
  (binaryfunc)Expression_add,
  /* nb_subtract */
  (binaryfunc)Expression_sub,
  /* nb_multiply */
  (binaryfunc)Expression_mul,
  /* nb_remainder */
  (binaryfunc)Expression_mod,
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
  (binaryfunc)Expression_div,
  /* nb_inplace_floor_divide */
  0,
  /* nb_inplace_true_divide */
  0,
  /* nb_index */
  0
};


static void Expression_dealloc(Expression* self) {
  // Clean up
  tiledb_expression_clear(self->expr);

  // Free object
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Expression_new(
    PyTypeObject *type, 
    PyObject *args, 
    PyObject *kwds) {
  // Create a new Expression object 
  Expression *self = (Expression *)type->tp_alloc(type, 0);

  // Initializations
  self->expr = NULL;

  // Success
  return (PyObject *)self;
}

static int Expression_init(Expression *self, PyObject *args) {
  // Initializations
  self->expr = NULL;

  // Get number of arguments
  int argc = PyTuple_Size(args);

  // Sanity checks
  if(argc > 1) {
    std::string errmsg = 
        "Failed to initialize Expression object; Invalid arguments";
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return -1;
  }

  // No arguments - return
  if(argc == 0) 
    return 0;
   
  // --- A single argument from here and onwards ---
 
  // Initialize expression based on the type
  PyObject* arg = PyTuple_GetItem(args, 0);
  int rc;
  if(PyLong_Check(arg)) {                                // Constant long
    int type = TILEDB_EXPR_INT64;
    int64_t value = PyLong_AsLong(arg);
    rc = tiledb_expression_init(&(self->expr), type, &value);
  } else if(PyFloat_Check(arg)) {                        // Constant double
    int type = TILEDB_EXPR_FLOAT64;
    double value = PyFloat_AsDouble(arg);
    rc = tiledb_expression_init(&(self->expr), type, &value);
  } else if(PyObject_TypeCheck(arg, &IndVariableType)) { // IndVariable
    int type = TILEDB_EXPR_VAR;
    void* value = ((IndVariable*)arg)->name;
    rc = tiledb_expression_init(&(self->expr), type, value);
  } else {
    std::string errmsg = 
        "Failed to initialize Expression object; Invalid arguments";
    PyErr_SetString(PyExc_TypeError, errmsg.c_str());
    return -1;
  }

  // Error
  if(rc != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to initialize Expression object");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return -1;
  } 

  // Success
  return 0;
}

static PyObject* Expression_eval(Expression* self, PyObject* args) {
  // Get dictionary
  if(PyTuple_Size(args) != 1) {
    std::string errmsg = 
        "Failed to evaluate Expression object; Invalid arguments";
    PyErr_SetString(PyExc_TypeError, errmsg.c_str());
    return NULL;
  }
  PyObject* dict = PyTuple_GetItem(args, 0);
  if(!PyDict_Check(dict)) {
    std::string errmsg = 
        "Failed to evaluate Expression object; Invalid arguments";
    PyErr_SetString(PyExc_TypeError, errmsg.c_str());
    return NULL;
  } 

  // Get number of dictionary entries
  int var_num = PyDict_Size(dict);

  // The expression arguments
  void** sorted_values = NULL;
  int* sorted_types = NULL;

  // If variables provided
  if(var_num != 0) {
    // Initializations
    char** names = new char*[var_num];
    void** values = new void*[var_num];
    int* types = new int[var_num];
    int* ids = new int[var_num];
    int rc_p, rc_i = TILEDB_OK;

    // Get names, values and types
    rc_p = tiledbpy_parse_expression_eval(dict, names, values, types, var_num); 

    // Get variable ids
    if(rc_p == TILEDBPY_PARSE_OK)
      rc_i = tiledb_expression_var_ids(
                 self->expr, 
                 (const char**) names, 
                 var_num, 
                 ids); 
     
    // Get sorted values and types
    if(rc_p == TILEDBPY_PARSE_OK && rc_i == TILEDB_OK) {
      sorted_values = new void*[var_num];
      sorted_types = new int[var_num];
      for(int i=0; i<var_num; ++i) {
        sorted_values[ids[i]] = values[i];
        sorted_types[ids[i]] = types[i];
      }
    }

    // Clean up
    delete [] ids;
    for(int i=0; i<var_num; ++i)
      delete [] names[i];
    delete [] names;
    delete [] values;
    delete [] types;
    
    // Handle errors
    if(rc_p != TILEDBPY_PARSE_OK) {
      PyErr_SetString(PyExc_TypeError, tiledbpy_parse_errmsg.c_str());
      return NULL;
    }
    if(rc_i != TILEDB_OK) {
      std::string errmsg = ERRMSG("Failed to evaluate Expression object");
      PyErr_SetString(tiledbpy_error, errmsg.c_str());
      return NULL;
    }
  }

  // Evaluate expression
  int rc_e = tiledb_expression_eval(
                 self->expr, 
                 (const void**) sorted_values, 
                 sorted_types);

  // Get result type
  int res_type;
  int rc_t = TILEDB_OK;
  if(rc_e == TILEDB_OK)
    rc_t = tiledb_expression_type(self->expr, &res_type);

  // Get result value
  PyObject* res; 
  int rc_v = TILEDB_OK;
  if(rc_e == TILEDB_OK && rc_t == TILEDB_OK) {
    if(res_type == TILEDB_EXPR_INT32 || res_type == TILEDB_EXPR_INT64) { 
      int res_value;
      rc_v = tiledb_expression_value(self->expr, &res_value);
      res = PyLong_FromLong(res_value);
    } else if(res_type == TILEDB_EXPR_FLOAT32 || 
              res_type == TILEDB_EXPR_FLOAT64) { 
      double res_value;
      rc_v = tiledb_expression_value(self->expr, &res_value);
      res = PyLong_FromDouble(res_value);
    } else {
      assert(0); // The code should never reach here
    }
  } 

  // Clean up
  if(sorted_values != NULL) {
    for(int i=0; i<var_num; ++i) 
      free(sorted_values[i]);
    delete [] sorted_values;
    delete [] sorted_types;
  }


 // Handle errors
  if(rc_e != TILEDB_OK || rc_t != TILEDB_OK || rc_v != TILEDB_OK) { 
    std::string errmsg = ERRMSG("Failed to evaluate Expression object");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Return result
  return res;
}

static PyObject* Expression_todot(Expression* self, PyObject* args) {
  // Parse arguments
  const char* Filename = NULL;
  if (!PyArg_ParseTuple(
       args, 
       "s; Failed to export Expression object to dot format; Invalid arguments",
       &Filename)) 
    return NULL;

  if(tiledb_expression_todot(self->expr, Filename) != TILEDB_OK) {
    std::string errmsg = 
        ERRMSG("Failed to export Expression object to dot format");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef ExpressionType_methods[] = {
  { "eval", 
    (PyCFunction)Expression_eval, 
    METH_VARARGS, 
    TILEDBPY_EXPR_DOC_EVAL },
  { "todot", 
    (PyCFunction)Expression_todot, 
    METH_VARARGS, 
    TILEDBPY_EXPR_DOC_TODOT },
  {NULL} /* Sentinel */
};

/* The Expression type definition. */
PyTypeObject ExpressionType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  /* tp_name */
  "tiledbpy.Expression", 
  /* tp_basicsize */
  sizeof(Expression), 
  /* tp_itemsize */
  0,                               
  /* tp_dealloc */
  (destructor)Expression_dealloc,
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
  &Expression_as_number, 
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
  TILEDBPY_EXPR_DOC, 
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
  ExpressionType_methods,
  /* tp_members */
  0,
  /* tp_getset */
  0, 
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
  (initproc)Expression_init, 
  /* tp_alloc */
  0,
  /* tp_new */
  Expression_new, 
};

