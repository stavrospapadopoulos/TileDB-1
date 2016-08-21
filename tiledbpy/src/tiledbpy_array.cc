/**
 * @file   tiledbpy_array.cc
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
 * This file implements tiledbpy.Array.
 */

#include <iostream>
#include "c_api.h"
#include "tiledbpy_array.h"



/* Internals of an Array object. */
typedef struct Array {
  PyObject_HEAD
  // The array name
  char name[TILEDB_NAME_MAX_LEN]; 
} Array;

static void Array_dealloc(Array* self) {
/* TODO
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
*/

std::cout << "Deleting Array\n";

  // Free object
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * Array_new(
    PyTypeObject *type, 
    PyObject *args, 
    PyObject *kwds) {
  // Create a new Array object 
  Array *self = (Array *)type->tp_alloc(type, 0);

std::cout << "Creating new Array\n";

  // Initializations
  strcpy(self->name, "");

  // Success
  return (PyObject *)self;
}

static int Array_init(Array *self, PyObject *args, PyObject *kwds) {
/* TODO
  PyObject *first=NULL, *last=NULL, *tmp;

  static char *kwlist[] = {"first", "last", "number", NULL};

  if (! PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
                                    &first, &last,
                                    &self->number))
  return -1;

    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }

    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }

*/

std::cout << "Initializing new Array\n";

  // Success
  return 0;
}



/* The Array type definition. */
PyTypeObject ArrayType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  /* tp_name */
  "tiledbpy.Array", 
  /* tp_basicsize */
  sizeof(Array), 
  /* tp_itemsize */
  0,                               
  /* tp_dealloc */
  (destructor)Array_dealloc,
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
  0, 
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
  "A TileDB array", 
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
  (initproc)Array_init,      
  /* tp_alloc */
  0,
  /* tp_new */
  Array_new, 
};

