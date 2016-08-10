/**
 * @file   tiledbpy_build.cc
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
 * This file implements functions that build Python objects for TileDB-Py.
 */

#include <Python.h>
#include "tiledbpy_build.h"




std::string tiledbpy_build_errmsg = "";



int tiledbpy_build_array_schema(
    const TileDB_ArraySchema& array_schema,
    PyObject*& ArraySchema) {
  // Create the array schema dictionary
  ArraySchema = PyDict_New(); 

  // Set array name
  PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("Name"),
      PyUnicode_FromString(array_schema.array_name_));

  // Set attributes
  PyObject* Attributes;
  tiledbpy_build_attributes(array_schema, Attributes);
  PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("Attributes"),
      Attributes);

  // Set dimensions
  PyObject* Dimensions;
  tiledbpy_build_dimensions(array_schema, Dimensions);
  PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("Dimensions"),
      Dimensions);

  // Set cell order
  PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("CellOrder"),
      PyLong_FromLong(array_schema.cell_order_));

   // Set tile order
   PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("TileOrder"),
      PyLong_FromLong(array_schema.tile_order_));

    // Set tile capacity
    PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("TileCapacity"),
      PyLong_FromLong(array_schema.capacity_));

    // Set type
    PyDict_SetItem(
      ArraySchema, 
      PyUnicode_FromString("Type"),
      PyLong_FromLong(
        (array_schema.dense_) ? TILEDB_DENSE : TILEDB_SPARSE));

  // Success
  return TILEDBPY_BUILD_OK;
}


void tiledbpy_build_attributes(
    const TileDB_ArraySchema& array_schema,
    PyObject*& Attributes) {
  // Create attributes
  Attributes = PyList_New(array_schema.attribute_num_);

  // Set attributes
  for(int i=0; i<array_schema.attribute_num_; ++i) {
    // New attribute
    PyObject* Attribute = PyDict_New(); 
   
    // Set attribute name 
    PyDict_SetItem(
        Attribute, 
        PyUnicode_FromString("Name"),
        PyUnicode_FromString(array_schema.attributes_[i]));

    // Set attribute type
    PyDict_SetItem(
      Attribute, 
      PyUnicode_FromString("Type"),
      PyLong_FromLong(array_schema.types_[i]));

    // Set attribute cell val num
    PyDict_SetItem(
      Attribute, 
      PyUnicode_FromString("ValNum"),
      PyLong_FromLong(array_schema.cell_val_num_[i]));

    // Set compression
    PyDict_SetItem(
      Attribute, 
      PyUnicode_FromString("Compression"),
      PyLong_FromLong(array_schema.compression_[i]));

    // Insert attribute in list 
    PyList_SetItem(Attributes, i, Attribute);
  }
}

void tiledbpy_build_dimensions(
    const TileDB_ArraySchema& array_schema,
    PyObject*& Dimensions) {
  // For easy reference
  int attribute_num = array_schema.attribute_num_;
  int dim_num = array_schema.dim_num_;

  // Create attributes
  Dimensions = PyList_New(dim_num+1);

  for(int i=0; i<dim_num; ++i) {
    // New attribute
    PyObject* Dimension = PyDict_New(); 
   
    // Set dimension name 
    PyDict_SetItem(
        Dimension, 
        PyUnicode_FromString("Name"),
        PyUnicode_FromString(array_schema.dimensions_[i]));

    // Set domain and tile extents based on the dimensions type
    if(array_schema.types_[attribute_num] == TILEDB_INT32) {
      // Set domain
      int* domain = (int*) array_schema.domain_;
      PyObject* Domain = PyList_New(2);
      PyList_SetItem(Domain, 0, PyLong_FromLong(domain[2*i]));
      PyList_SetItem(Domain, 1, PyLong_FromLong(domain[2*i+1]));
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("Domain"),
          Domain);

      // Set tile extent
      int* tile_extents = (int*) array_schema.tile_extents_;
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("TileExtent"),
          PyLong_FromLong(tile_extents[i]));
    } else if(array_schema.types_[attribute_num] == TILEDB_INT64) {
      // Set domain
      int64_t* domain = (int64_t*) array_schema.domain_;
      PyObject* Domain = PyList_New(2);
      PyList_SetItem(Domain, 0, PyLong_FromLong(domain[2*i]));
      PyList_SetItem(Domain, 1, PyLong_FromLong(domain[2*i+1]));
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("Domain"),
          Domain);

      // Set tile extent
      int64_t* tile_extents = (int64_t*) array_schema.tile_extents_;
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("TileExtent"),
          PyLong_FromLong(tile_extents[i]));
    } else if(array_schema.types_[attribute_num] == TILEDB_FLOAT32) {
      // Set domain
      float* domain = (float*) array_schema.domain_;
      PyObject* Domain = PyList_New(2);
      PyList_SetItem(Domain, 0, PyFloat_FromDouble(domain[2*i]));
      PyList_SetItem(Domain, 1, PyFloat_FromDouble(domain[2*i+1]));
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("Domain"),
          Domain);

      // Set tile extent
      float* tile_extents = (float*) array_schema.tile_extents_;
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("TileExtent"),
          PyFloat_FromDouble(tile_extents[i]));
    } else if(array_schema.types_[attribute_num] == TILEDB_FLOAT64) {
      // Set domain
      double* domain = (double*) array_schema.domain_;
      PyObject* Domain = PyList_New(2);
      PyList_SetItem(Domain, 0, PyFloat_FromDouble(domain[2*i]));
      PyList_SetItem(Domain, 1, PyFloat_FromDouble(domain[2*i+1]));
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("Domain"),
          Domain);

      // Set tile extent
      double* tile_extents = (double*) array_schema.tile_extents_;
      PyDict_SetItem(
          Dimension, 
          PyUnicode_FromString("TileExtent"),
          PyFloat_FromDouble(tile_extents[i]));
    }

    // Insert dimension in list 
    PyList_SetItem(Dimensions, i, Dimension);
  }

  // Set dimension info (Type and Compression)
  PyObject* DimensionInfo = PyDict_New();
  PyDict_SetItem(
      DimensionInfo, 
      PyUnicode_FromString("Type"),
      PyLong_FromLong(array_schema.types_[attribute_num]));
  PyDict_SetItem(
      DimensionInfo, 
      PyUnicode_FromString("Compression"),
      PyLong_FromLong(array_schema.compression_[attribute_num]));
  PyList_SetItem(Dimensions, dim_num, DimensionInfo);
}
