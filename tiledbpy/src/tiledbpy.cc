/**
 * @file   tiledbpy.cc
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
 * This file implements TileDB-Py, the Python API for TileDB.
 */

#include <Python.h>
#include <string>
#include "c_api.h"
#include "tiledbpy_doc.h"
#include "tiledbpy_parse.h"
#include "tiledbpy_build.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#  define TILEDBPY_EXPORT __attribute__((visibility("default")))
#else
#  define TILEDBPY_EXPORT
#endif


/* ****************************** */
/*             GLOBAL             */
/* ****************************** */

/* TileDB context */
TileDB_CTX* tiledb_ctx = NULL;

/* The TileDB Python exception. */
static PyObject* tiledbpy_error;




/* ****************************** */
/*       MODULE FUNCTIONS         */
/* ****************************** */

/* Retrieves the schema of a TileDB array. */
static PyObject* tiledbpy_array_schema(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Array;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.array_schema)' "
          "to see the usage of 'array_schema'",
         &Array)) 
    return NULL;

  // Load array schema
  TileDB_ArraySchema array_schema;
  if(tiledb_array_load_schema(tiledb_ctx, Array, &array_schema) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to retrieve array schema");
    // TODO: set string more properly
    return NULL;
  }

  // Build the array schema tuple
  PyObject* ArraySchema;
  if(tiledbpy_build_array_schema(array_schema, ArraySchema) != 
     TILEDBPY_BUILD_OK) {
    PyErr_SetString(tiledbpy_error, tiledbpy_build_errmsg.c_str());
    return NULL;
  }

  // Free schema
  if(tiledb_array_free_schema(&array_schema) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to free array schema");
    Py_DECREF(ArraySchema);
    // TODO: proper error message
    return NULL;
  }

  // Success
  return ArraySchema;
}

/* Creates a TileDB array. */
static PyObject* tiledbpy_create_array(
    PyObject* self, 
    PyObject* args,
    PyObject* keywds) { 
  // Permissible keywords
  static const char* kwlist[] = {
    "Name",
    "Dimensions",
    "Attributes",
    "CellOrder",
    "TileCapacity",
    "TileOrder",
    "Type",
    NULL
  }; 

  // Function arguments to be retrieved
  PyObject* Attributes = NULL;
  int CellOrder = TILEDB_ROW_MAJOR;
  PyObject* Dimensions = NULL;
  const char* Name = NULL;
  long int TileCapacity = 0;
  int TileOrder = TILEDB_ROW_MAJOR;
  int Type = TILEDB_DENSE;
 
  // Parse function arguments
  if(!PyArg_ParseTupleAndKeywords(
         args,
         keywds,
         "sO|Oilii;"
          "Invalid TileDB array schema. "
          "Type 'help(tiledbpy.create_array)' "
          "to see the usage of 'create_array'",
         (char**) kwlist,
         &Name,
         &Dimensions,
         &Attributes,
         &CellOrder,
         &TileCapacity,
         &TileOrder,
         &Type)) 
    return NULL;

  // Parse array schema
  TileDB_ArraySchema array_schema;
  if(tiledbpy_parse_array_schema(
          Attributes,
          CellOrder,
          Dimensions,
          Name,
          TileCapacity,
          TileOrder,
          Type,
          array_schema) != TILEDBPY_PARSE_OK) {
    // Raise exception in case of error
    PyErr_SetString(tiledbpy_error, tiledbpy_parse_errmsg.c_str());
    return NULL;
  }

  // Create array
  if(tiledb_array_create(tiledb_ctx, &array_schema) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to create array");
    // TODO: set string more properly
    return NULL;
  }

  // Free array schema
  if(tiledb_array_free_schema(&array_schema) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to free array schema");
    // TODO: proper error message
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Creates a TileDB group. */
static PyObject* tiledbpy_create_group(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Group;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.create_group)' "
          "to see the usage of 'create_group'",
         &Group)) 
    return NULL;

  // Create workspace
  if(tiledb_group_create(tiledb_ctx, Group) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to create group");
    // TODO: set string more properly
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Creates a TileDB workspace. */
static PyObject* tiledbpy_create_workspace(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Workspace;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.create_workspace)' "
          "to see the usage of 'create_workspace'",
         &Workspace)) 
    return NULL;

  // Create workspace
  if(tiledb_workspace_create(tiledb_ctx, Workspace) != TILEDB_OK) {
    PyErr_SetString(tiledbpy_error, "Failed to create workspace");
    // TODO: set string more properly
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Performs necessary finalizations for the module. */
static PyObject* tiledbpy_finalize(PyObject* self, PyObject* args) {
  /* Finalize context. */
  if(tiledb_ctx != NULL &&
     tiledb_ctx_finalize(tiledb_ctx) != TILEDB_OK) {
    // Raise exception if finalization fails.
    PyErr_SetString(tiledbpy_error, "Cannot finalize TileDB context");
    // TODO
    return NULL;
  }

  // Set context to NULL
  tiledb_ctx = NULL;

  // Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Returns the TileDB module version. */
static PyObject* tiledbpy_version(PyObject* self, PyObject* args) {
  return Py_BuildValue("s", TILEDB_VERSION);
}




/* ****************************** */
/*        MODULE CREATION         */
/* ****************************** */

/* Module method table. */
static struct PyMethodDef tiledbpy_methods[] = {
  { "array_schema", 
    (PyCFunction) tiledbpy_array_schema, 
    METH_VARARGS,
    TILEDBPY_DOC_ARRAY_SCHEMA },
  { "create_array", 
    (PyCFunction) tiledbpy_create_array, 
    METH_VARARGS|METH_KEYWORDS,
    TILEDBPY_DOC_CREATE_ARRAY }, 
  { "create_group", 
    (PyCFunction) tiledbpy_create_group, 
    METH_VARARGS,
    TILEDBPY_DOC_CREATE_GROUP },
  { "create_workspace", 
    (PyCFunction) tiledbpy_create_workspace, 
    METH_VARARGS,
    TILEDBPY_DOC_CREATE_WORKSPACE },
  { "finalize", tiledbpy_finalize, METH_NOARGS, TILEDBPY_DOC_FINALIZE },
  { "version", tiledbpy_version, METH_NOARGS, TILEDBPY_DOC_VERSION },
  { NULL, NULL, 0, NULL }
};

/* Module definition structure. */
static struct PyModuleDef tiledbpy_module {
  PyModuleDef_HEAD_INIT,
  "tiledbpy",
  "The TileDB Python module. "
  "For extensive documentation, please visit www.tiledb.org.",
  -1,
  tiledbpy_methods 
}; 

/* Module initialization. */
TILEDBPY_EXPORT
PyObject* PyInit_tiledbpy() {
  // Initialize the TileDB context
  if(tiledb_ctx == NULL &&
     tiledb_ctx_init(&tiledb_ctx, NULL) != TILEDB_OK) {
    // Raise exception if initialization fails.
    PyErr_SetString(tiledbpy_error, "Cannot initialize TileDB context");
    return NULL;
  }

  // Create the module
  PyObject* m = PyModule_Create(&tiledbpy_module); 
  if(m == NULL)
    return NULL;

  // Initialize module exception
  tiledbpy_error = PyErr_NewException("tiledbpy.error", NULL, NULL);
  Py_INCREF(tiledbpy_error);
  PyModule_AddObject(m, "error", tiledbpy_error);
 
  // Initialize constants
  PyModule_AddIntConstant(m, "TILEDBPY_INT32", TILEDB_INT32);
  PyModule_AddIntConstant(m, "TILEDBPY_INT64", TILEDB_INT64);
  PyModule_AddIntConstant(m, "TILEDBPY_FLOAT32", TILEDB_FLOAT32);
  PyModule_AddIntConstant(m, "TILEDBPY_FLOAT64", TILEDB_FLOAT64);
  PyModule_AddIntConstant(m, "TILEDBPY_CHAR", TILEDB_CHAR);
  PyModule_AddIntConstant(m, "TILEDBPY_VAR_NUM", TILEDB_VAR_NUM);
  PyModule_AddIntConstant(m, "TILEDBPY_GZIP", TILEDB_GZIP);
  PyModule_AddIntConstant(m, "TILEDBPY_NO_COMPRESSION", TILEDB_NO_COMPRESSION);
  PyModule_AddIntConstant(m, "TILEDBPY_SPARSE", TILEDB_SPARSE);
  PyModule_AddIntConstant(m, "TILEDBPY_DENSE", TILEDB_DENSE);
  PyModule_AddIntConstant(m, "TILEDBPY_COL_MAJOR", TILEDB_COL_MAJOR);
  PyModule_AddIntConstant(m, "TILEDBPY_ROW_MAJOR", TILEDB_ROW_MAJOR);

  // Return the module
  return m;
}

#undef TILEDBPY_EXPORT
#ifdef __cplusplus
}
#endif
