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
 * This file implements the Python API for TileDB.
 */

#include <Python.h>
#include "c_api.h"

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
/*             MODULE             */
/* ****************************** */

/* Performs necessary initializations for the module. */
static PyObject* tiledbpy_init(PyObject* self, PyObject* args) {
  // Initialize the TileDB context
  TileDB_CTX* tiledb_ctx;
  if(tiledb_ctx_init(&tiledb_ctx, NULL) != TILEDB_OK) {
    // Raise exception if initialization fails.
    PyErr_SetString(tiledbpy_error, "Cannot initialize TileDB context");
    return NULL;
  }

  // Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Performs necessary finalizations for the module. */
static PyObject* tiledbpy_finalize(PyObject* self, PyObject* args) {
  /* Finalize context. */
  if(tiledb_ctx_finalize(tiledb_ctx) != TILEDB_OK) {
    // Raise exception if finalization fails.
    PyErr_SetString(tiledbpy_error, "Cannot finalize TileDB context");
    return NULL;
  }

  // Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Returns the TileDB module version. */
static PyObject* tiledbpy_version(PyObject* self, PyObject* args) {
  return Py_BuildValue("s", TILEDB_VERSION);
}

/* Module method table. */
static struct PyMethodDef tiledbpy_methods[] = {
  { "finalize", tiledbpy_finalize, METH_NOARGS, 
    "Finalizes the TileDB module." },
  { "init", tiledbpy_init, METH_NOARGS,
    "Initializes the TileDB module." },
  { "version", tiledbpy_version, METH_NOARGS,
    "Returns the version of the TileDB library used by this module." },
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

  // Create the module
  PyObject* m = PyModule_Create(&tiledbpy_module); 
  if(m == NULL)
    return NULL;

  // Initialize module exception
  tiledbpy_error = PyErr_NewException("tiledbpy.error", NULL, NULL);
  Py_INCREF(tiledbpy_error);
  PyModule_AddObject(m, "error", tiledbpy_error);

  // Return the module
  return m;
}

#undef TILEDBPY_EXPORT
#ifdef __cplusplus
}
#endif
