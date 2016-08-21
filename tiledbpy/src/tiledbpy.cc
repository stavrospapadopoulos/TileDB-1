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
 * This file implements the tiledbpy module, the Python API for TileDB.
 */

#include <iostream>
#include "tiledbpy.h"
#include "tiledbpy_array.h"
#include "tiledbpy_build.h"
#include "tiledbpy_expression.h"
#include "tiledbpy_indvariable.h"
#include "tiledbpy_parse.h"



/* TileDB context */
TileDB_CTX* tiledb_ctx = NULL;


/* ****************************** */
/*             MACROS             */
/* ****************************** */

#define ERRMSG(x) std::string(x) + "\n --> " + std::string(tiledb_errmsg)

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

/* The TileDB Python exception. */
PyObject* tiledbpy_error = NULL;




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
    std::string errmsg = ERRMSG("Failed to retrieve array schema");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
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
    std::string errmsg = ERRMSG("Failed to free array schema");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    Py_DECREF(ArraySchema);
    return NULL;
  }

  // Success
  return ArraySchema;
}

/* Clears a TileDB object. */
static PyObject* tiledbpy_clear(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Dir;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.clear)' "
          "to see the usage of 'clear'",
         &Dir)) 
    return NULL;

  // Clear directory
  if(tiledb_clear(tiledb_ctx, Dir) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to clear TileDB object");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
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
    std::string errmsg = ERRMSG("Failed to create array");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Free array schema
  if(tiledb_array_free_schema(&array_schema) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to free array schema");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
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
    std::string errmsg = ERRMSG("Failed to create group");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
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
    std::string errmsg = ERRMSG("Failed to create workspace");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Success - Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/* Deletes a TileDB object. */
static PyObject* tiledbpy_delete(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Dir;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.delete)' "
          "to see the usage of 'delete'",
         &Dir)) 
    return NULL;

  // Delete directory
  if(tiledb_delete(tiledb_ctx, Dir) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to delete TileDB object");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
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
    std::string errmsg = ERRMSG("Cannot finalize TileDB context");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Set context to NULL
  tiledb_ctx = NULL;

  // Return nothing
  Py_INCREF(Py_None);
  return Py_None;
}

/*
 * Returns the list of the TileDB objects in a directory along with their types.
 */
static PyObject* tiledbpy_ls(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* Dir;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "s|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.ls)' to see the usage of 'ls'",
         &Dir)) 
    return NULL;

  // Count the number of TileDB objects 
  int dir_num;
  if(tiledb_ls_c(tiledb_ctx, Dir, &dir_num) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to list the TileDB objects");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Retrieve the TileDB objects
  PyObject* LSList = NULL;
  if(dir_num > 0) {
    char** dirs = new char*[dir_num];
    int* dir_types = new int[dir_num];
    for(int i=0; i<dir_num; ++i)
      dirs[i] = new char[TILEDB_NAME_MAX_LEN]; 
    if(tiledb_ls(tiledb_ctx, Dir, dirs, dir_types, &dir_num) != TILEDB_OK) {
      std::string errmsg = ERRMSG("Failed to list the TileDB objects");
      PyErr_SetString(tiledbpy_error, errmsg.c_str());
      return NULL;
    }

    // Build the TileDB object list 
    tiledbpy_build_ls((const char**) dirs, dir_types, dir_num, LSList);

    // Clean up
    for(int i=0; i<dir_num; ++i)
      delete [] dirs[i];
    delete [] dirs;
    delete [] dir_types;
  }


  // Return the list if not NULL, otherwise empty list
  if(LSList != NULL) 
    return LSList;
  else 
    return PyList_New(0);
}

/* Returns the list of the TileDB objects workspaces. */
static PyObject* tiledbpy_ls_workspaces(
    PyObject* self, 
    PyObject* args) {
  // Count the number of TileDB workspaces 
  int workspace_num;
  if(tiledb_ls_workspaces_c(tiledb_ctx, &workspace_num) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to list the TileDB workspaces");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Retrieve the TileDB workspaces
  PyObject* LSList = NULL;
  if(workspace_num > 0) {
    char** workspaces = new char*[workspace_num];
    for(int i=0; i<workspace_num; ++i)
      workspaces[i] = new char[TILEDB_NAME_MAX_LEN]; 
    if(tiledb_ls_workspaces(tiledb_ctx, workspaces, &workspace_num) != 
       TILEDB_OK) {
      std::string errmsg = ERRMSG("Failed to list the TileDB workspaces");
      PyErr_SetString(tiledbpy_error, errmsg.c_str());
      return NULL;
    }

    // Build the TileDB workspace list 
    tiledbpy_build_ls_workspaces(
        (const char**) workspaces, 
        workspace_num, 
        LSList);

    // Clean up
    for(int i=0; i<workspace_num; ++i)
      delete [] workspaces[i];
    delete [] workspaces;
  }


  // Return the list if not NULL, otherwise empty list
  if(LSList != NULL) 
    return LSList;
  else 
    return PyList_New(0);
}

/* Moves a TileDB object. */
static PyObject* tiledbpy_move(
    PyObject* self, 
    PyObject* args) {
  // Function arguments
  const char* OldDir;
  const char* NewDir;

  // Parse function arguments
  if(!PyArg_ParseTuple(
         args,
         "ss|;"
          "Invalid function arguments. "
          "Type 'help(tiledbpy.move)' "
          "to see the usage of 'move'",
         &OldDir, 
         &NewDir)) 
    return NULL;

  // Delete directory
  if(tiledb_move(tiledb_ctx, OldDir, NewDir) != TILEDB_OK) {
    std::string errmsg = ERRMSG("Failed to move TileDB object");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Success - Return nothing
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
  { "clear", tiledbpy_clear, METH_VARARGS, TILEDBPY_DOC_CLEAR },
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
  { "delete", tiledbpy_delete, METH_VARARGS, TILEDBPY_DOC_DELETE },
  { "finalize", tiledbpy_finalize, METH_NOARGS, TILEDBPY_DOC_FINALIZE },
  { "ls", tiledbpy_ls, METH_VARARGS, TILEDBPY_DOC_LS },
  { "ls_workspaces", 
     tiledbpy_ls_workspaces, 
     METH_NOARGS, 
     TILEDBPY_DOC_LS_WORKSPACES },
  { "move", tiledbpy_move, METH_VARARGS, TILEDBPY_DOC_MOVE },
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
    std::string errmsg = ERRMSG("Cannot finalize TileDB context");
    PyErr_SetString(tiledbpy_error, errmsg.c_str());
    return NULL;
  }

  // Create IndVariable type
  if (PyType_Ready(&IndVariableType) < 0)
    return NULL;

  // Create Expression type
  if (PyType_Ready(&ExpressionType) < 0)
    return NULL;

  // Create Array type
  if (PyType_Ready(&ArrayType) < 0)
    return NULL;

  // Create the module
  PyObject* m = PyModule_Create(&tiledbpy_module); 
  if(m == NULL)
    return NULL;

  // Add IndVariable type to module
  Py_INCREF(&IndVariableType);
  PyModule_AddObject(m, "IndVariable", (PyObject *)&IndVariableType); 

  // Add Expression type to module
  Py_INCREF(&IndVariableType);
  PyModule_AddObject(m, "Expression", (PyObject *)&ExpressionType); 

  // Add Array type to module
  Py_INCREF(&ArrayType);
  PyModule_AddObject(m, "Array", (PyObject *)&ArrayType); 

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
  PyModule_AddIntConstant(m, "TILEDBPY_WORKSPACE", TILEDB_WORKSPACE);
  PyModule_AddIntConstant(m, "TILEDBPY_GROUP", TILEDB_GROUP);
  PyModule_AddIntConstant(m, "TILEDBPY_ARRAY", TILEDB_ARRAY);
  PyModule_AddIntConstant(m, "TILEDBPY_METADATA", TILEDB_METADATA);

  // Return the module
  return m;
}

#undef TILEDBPY_EXPORT
#ifdef __cplusplus
}
#endif
