/**
 * @file   tiledbpy_parse.cc
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
 * This file implements auxiliary functions used when parsing the parameters of
 * the various TileDB-Py functions.
 */

#include "tiledbpy_parse.h"




std::string tiledbpy_parse_errmsg = "";


int tiledbpy_parse_array_name(
    const char* Name,
    TileDB_ArraySchema& array_schema) {
  // Set array name
  size_t array_name_len = strlen(Name); 
  if(array_name_len > TILEDB_NAME_MAX_LEN) {
    tiledbpy_parse_errmsg = "Invalid array name length";
    return TILEDBPY_PARSE_ERR;
  }
  array_schema.array_name_ = (char*) malloc(array_name_len+1);
  strcpy(array_schema.array_name_, Name);

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_attributes(
    PyObject* Attributes,
    TileDB_ArraySchema& array_schema) {
  // Check if Attributes is a list
  if(Attributes != NULL) {
    array_schema.attribute_num_ = PyList_Size(Attributes);
    if(array_schema.attribute_num_ < 0) { // No a list
      tiledbpy_parse_errmsg = "'Attributes' must be a list of dictionaries";
      return TILEDBPY_PARSE_ERR;
    }
  }

  // --- If no attributes are given, use a single default argument 
  if(Attributes == NULL || array_schema.attribute_num_ == 0) {
    array_schema.attribute_num_ = 1;
    array_schema.attributes_ = (char**) malloc(sizeof(char*));
    size_t attribute_len = strlen(TILEDBPY_PARSE_DEFAULT_ATTRIBUTE);
    array_schema.attributes_[0] = (char*) malloc(attribute_len+1);
    strcpy(array_schema.attributes_[0], TILEDBPY_PARSE_DEFAULT_ATTRIBUTE);
    array_schema.cell_val_num_ = (int*) malloc(sizeof(int));
    array_schema.cell_val_num_[0] = 1;
    array_schema.compression_ = (int*) malloc(2*sizeof(int));
    for(int i=0; i<2; ++i)
      array_schema.compression_[i] = TILEDB_NO_COMPRESSION; 
    array_schema.types_ = (int*) malloc(2*sizeof(int));
    for(int i=0; i<2; ++i)
      array_schema.types_[i] = TILEDB_INT32;

    // Success
    return TILEDBPY_PARSE_OK;
  }

  // --- If one or more attributes are given, parse the attributes 

  // Initialization
  std::vector<std::string> attributes;
  attributes.resize(array_schema.attribute_num_);
  std::vector<int> types;
  types.resize(array_schema.attribute_num_+1);
  std::vector<int> cell_val_num;
  cell_val_num.resize(array_schema.attribute_num_);
  std::vector<int> compression;
  compression.resize(array_schema.attribute_num_+1);

  // Parse attributes
  PyObject *item;
  PyObject *key, *value, *pystring;
  std::string key_str, attribute;
  for (int i = 0; i < array_schema.attribute_num_; ++i) {
    // Get attribute dictionary
    item = PyList_GetItem(Attributes, i); 
    if (!PyDict_Check(item)) {
      tiledbpy_parse_errmsg = "'Attributes' must be a list of dictionaries";
      return TILEDBPY_PARSE_ERR;
    }

    // Error if the attribute is empty
    if(PyDict_Size(item) == 0) {
      tiledbpy_parse_errmsg = "Empty attribute provided";
      return TILEDBPY_PARSE_ERR;
    }

    // Initialization
    Py_ssize_t pos = 0;
    bool attribute_set = false;
    bool type_set = false;
    bool cell_val_num_set = false;
    bool compression_set = false;

    // Parse attribute dictionary
    while (PyDict_Next(item, &pos, &key, &value)) {  
      // Check if key is a string 
      if(!PyUnicode_Check(key)) {
        tiledbpy_parse_errmsg = 
            "The keys of an attribute dictionary must be strings";
        return TILEDBPY_PARSE_ERR;
      }

      // Get the key string
      pystring = PyUnicode_AsASCIIString(key);
      key_str = PyBytes_AsString(pystring);
      Py_DECREF(pystring);

      // Attribute name
      if(key_str == "Name") {
        // Check if it is a string
        if(!PyUnicode_Check(value)) {
          tiledbpy_parse_errmsg = "Attribute Name must be a string";
          return TILEDBPY_PARSE_ERR;
        }

        // Get attribute
        pystring = PyUnicode_AsASCIIString(value);
        attribute = PyBytes_AsString(pystring);
        Py_DECREF(pystring);

        // Check attribute length
        if(attribute.size() > TILEDB_NAME_MAX_LEN) {
          tiledbpy_parse_errmsg = "Invalid attribute name length";
          return TILEDBPY_PARSE_ERR;
        }

        // Set attribute
        attributes[i] = attribute; 
        attribute_set = true;
      } else if(key_str == "Type") {
        // Check if it is an integer
        if(!PyLong_Check(value)) {
          tiledbpy_parse_errmsg = "Attribute Type must be an integer";
          return TILEDBPY_PARSE_ERR;
        }
        // Set type
        types[i] = PyLong_AsLong(value);
        type_set = true;
      } else if(key_str == "ValNum") {
        // Check if it is an integer
        if(!PyLong_Check(value)) {
          tiledbpy_parse_errmsg = "Attribute ValNum must be an integer";
          return TILEDBPY_PARSE_ERR;
        }
        // Set cell val num
        cell_val_num[i] = PyLong_AsLong(value);
        cell_val_num_set = true;
      } else if(key_str == "Compression") {
        // Check if it is an integer
        if(!PyLong_Check(value)) {
          tiledbpy_parse_errmsg = "Attribute Compression must be an integer";
          return TILEDBPY_PARSE_ERR;
        }
        // Set compression
        compression[i] = PyLong_AsLong(value);
        compression_set = true;
      } else {
        tiledbpy_parse_errmsg = "Invalid attribute dictionary key";
        return TILEDBPY_PARSE_ERR;
      }  
    }

    // Error if attribute Name is not set
    if(!attribute_set) {
      tiledbpy_parse_errmsg = "Attribute Name missing";
      return TILEDBPY_PARSE_ERR;
    }

    // Default values for what is missing
    if(!type_set) 
      types[i] = TILEDB_INT32;
    if(!cell_val_num_set)
      cell_val_num[i] = 1;
    if(!compression_set)
      compression[i] = TILEDB_NO_COMPRESSION;
  }

  // Copy attributes
  array_schema.attributes_ = 
      (char**) malloc(array_schema.attribute_num_*sizeof(char*));
  for (int i = 0; i < array_schema.attribute_num_; ++i) {
    // Copy attribute
    size_t attribute_len = attributes[i].size();
    if(attributes[i] == "" || attribute_len > TILEDB_NAME_MAX_LEN) {
      tiledbpy_parse_errmsg = "Invalid attribute name length";
      return TILEDBPY_PARSE_ERR;
    }
    array_schema.attributes_[i] = (char*) malloc(attribute_len+1);
    strcpy(array_schema.attributes_[i], attributes[i].c_str());
  }

  // Copy types
  array_schema.types_ = 
      (int*) malloc((array_schema.attribute_num_+1)*sizeof(int));
  for(int i=0; i<array_schema.attribute_num_+1; ++i)
    array_schema.types_[i] = types[i];

  // Copy cell val num
  array_schema.cell_val_num_ = 
      (int*) malloc((array_schema.attribute_num_)*sizeof(int));
  for(int i=0; i<array_schema.attribute_num_; ++i) 
    array_schema.cell_val_num_[i] = cell_val_num[i];

  // Copy compression
  array_schema.compression_ = 
      (int*) malloc((array_schema.attribute_num_+1)*sizeof(int));
  for(int i=0; i<array_schema.attribute_num_+1; ++i)
    array_schema.compression_[i] = compression[i];

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_array_schema(
    PyObject* Attributes,
    int CellOrder,
    PyObject* Dimensions,
    const char* Name,
    long int TileCapacity,
    int TileOrder,
    int Type,
    TileDB_ArraySchema& array_schema) {
  // Initialization
  array_schema = {};

  // Parse array name
  if(tiledbpy_parse_array_name(Name, array_schema) != TILEDBPY_PARSE_OK) 
    return TILEDBPY_PARSE_ERR;

  // Parse tile capacity
  array_schema.capacity_ = (int64_t) TileCapacity;

  // Parse cell order
  if(tiledbpy_parse_cell_order(CellOrder, array_schema) != TILEDBPY_PARSE_OK) 
    return TILEDBPY_PARSE_ERR;

  // Parse type
  if(tiledbpy_parse_type(Type, array_schema) != TILEDBPY_PARSE_OK)
    return TILEDBPY_PARSE_ERR;

  // Parse tile order
  if(tiledbpy_parse_tile_order(TileOrder, array_schema) != TILEDBPY_PARSE_OK)
    return TILEDBPY_PARSE_ERR;

  // Parse attributes
  if(tiledbpy_parse_attributes(Attributes, array_schema) != TILEDBPY_PARSE_OK)
    return TILEDBPY_PARSE_ERR;

  // Parse dimensions
  if(tiledbpy_parse_dimensions(Dimensions, array_schema) != TILEDBPY_PARSE_OK)
    return TILEDBPY_PARSE_ERR;

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_cell_order(
    int CellOrder,
    TileDB_ArraySchema& array_schema) {
  // Parse cell order
  if(CellOrder == TILEDB_ROW_MAJOR || CellOrder == TILEDB_COL_MAJOR) {
    array_schema.cell_order_ = CellOrder;
  } else {
    tiledbpy_parse_errmsg = "Invalid cell order"; 
    return TILEDBPY_PARSE_ERR;
  }

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_dimensions(
    PyObject* Dimensions,
    TileDB_ArraySchema& array_schema) {
  // Check if Dimensions is a list
  int Dimensions_size = PyList_Size(Dimensions);
  if(Dimensions_size < 0) { // No a list
    tiledbpy_parse_errmsg = "'Dimensions' must be a list of dictionaries";
    return TILEDBPY_PARSE_ERR;
  }

  // Parse Dimensions list
  array_schema.dim_num_ = 0;
  int type = TILEDB_INT32;
  int compression = TILEDB_NO_COMPRESSION;
  std::vector<std::string> dimensions;
  std::vector<PyObject*> domain_py;
  std::vector<PyObject*> tile_extents_py;
  bool type_set = false;
  bool compression_set = false;
  PyObject *item;
  PyObject *key, *value, *pystring;
  std::string key_str, dimension;
  for (int i = 0; i < Dimensions_size; ++i) {
    // Get attribute dictionary
    item = PyList_GetItem(Dimensions, i); 
    if (!PyDict_Check(item)) {
      tiledbpy_parse_errmsg = "'Dimensions' must be a list of dictionaries";
      return TILEDBPY_PARSE_ERR;
    }

    // Error if the attribute is empty
    if(PyDict_Size(item) == 0) {
      tiledbpy_parse_errmsg = "Empty dimension provided";
      return TILEDBPY_PARSE_ERR;
    }

    // Initialization
    Py_ssize_t pos = 0;
    bool dimension_set = false;
    bool domain_set = false;
    bool tile_extent_set = false;
    bool is_type = false;
    bool is_name = false;

    // Parse attribute dictionary
    while (PyDict_Next(item, &pos, &key, &value)) {  
      // Check if key is a string 
      if(!PyUnicode_Check(key)) {
        tiledbpy_parse_errmsg = 
            "The keys of a dimension dictionary must be strings";
        return TILEDBPY_PARSE_ERR;
      }

      // Get the key string
      pystring = PyUnicode_AsASCIIString(key);
      key_str = PyBytes_AsString(pystring);
      Py_DECREF(pystring);
    
      // Get Type and Compression
      if(key_str == "Type" || key_str == "Compression") {
        // Error if Type, Compression are mixed with Name, Domain, TileExtent
        if(is_name) {
            tiledbpy_parse_errmsg = "Invalid Dimension dictionary";
            return TILEDBPY_PARSE_ERR;
        }

        // Indicate that this is dimension type dictionary
        is_type = true;

        // Get Type
        if(key_str == "Type") {
          // Error if it is set already
          if(key_str == "Type" && type_set) {
              tiledbpy_parse_errmsg = "Multiple dimension types given";
              return TILEDBPY_PARSE_ERR;
          }

          // Check if it is an integer
          if(!PyLong_Check(value)) {
            tiledbpy_parse_errmsg = "Dimension Type must be an integer";
            return TILEDBPY_PARSE_ERR;
          }

          // Set type
          type = PyLong_AsLong(value);
          type_set = true;
        } else if(key_str == "Compression") { // Get compression
          // Error if it is set already
          if(key_str == "Compression" && compression_set) {
              tiledbpy_parse_errmsg = "Multiple dimension compressions given";
              return TILEDBPY_PARSE_ERR;
          }

          // Check if it is an integer
          if(!PyLong_Check(value)) {
            tiledbpy_parse_errmsg = "Dimension Compression must be an integer";
            return TILEDBPY_PARSE_ERR;
          }

          // Set type
          compression = PyLong_AsLong(value);
          compression_set = true;
        }
      } else if(key_str == "Name"   ||          // Get Name, Domain, TileExtent
                key_str == "Domain" || 
                key_str == "TileExtent") {
        // Error if Type, Compression are mixed with Name, Domain, TileExtent
        if(is_type) {
            tiledbpy_parse_errmsg = "Invalid dimension dictionary";
            return TILEDBPY_PARSE_ERR;
        }

        // Indicate that this is dimension name dictionary
        is_name = true;

        if(key_str == "Name") {
          // Check if it is a string
          if(!PyUnicode_Check(value)) {
            tiledbpy_parse_errmsg = "Dimension Name must be a string";
            return TILEDBPY_PARSE_ERR;
          }

          // Get dimension
          pystring = PyUnicode_AsASCIIString(value);
          dimension = PyBytes_AsString(pystring);
          Py_DECREF(pystring);

          // Check dimension length
          if(dimension.size() > TILEDB_NAME_MAX_LEN) {
            tiledbpy_parse_errmsg = "Invalid dimension name length";
            return TILEDBPY_PARSE_ERR;
          }

          // Set dimension
          dimensions.push_back(dimension); 
          dimension_set = true;
        } else if(key_str == "Domain") {
          // Get domain
          domain_py.push_back(value);

          // Set domain
          domain_set = true;
        } else if(key_str == "TileExtent") {
          // Get tile extent
          tile_extents_py.push_back(value);

          // Set tile extent
          tile_extent_set = true;
        }
      } else {                                  // Error
        tiledbpy_parse_errmsg = "Invalid dimension dictionary key";
        return TILEDBPY_PARSE_ERR;
      }
    }

    // Error if one of Name, Domain, TileExtents is not set
    if(is_name) {
      if(!dimension_set) {
        tiledbpy_parse_errmsg = "Dimension Name missing";
        return TILEDBPY_PARSE_ERR;
      } else if(!domain_set) {
        tiledbpy_parse_errmsg = "Dimension Domain missing";
        return TILEDBPY_PARSE_ERR;
      } else if(!tile_extent_set) {
          tile_extents_py.push_back(NULL);
      }
    }

    // Increment dim num
    if(is_name)
      ++array_schema.dim_num_;
  }

  // Error if no dimensions given
  if(array_schema.dim_num_ == 0) {
    tiledbpy_parse_errmsg = "No Dimensions given";
    return TILEDBPY_PARSE_ERR;
  }

  // Copy type
  array_schema.types_[array_schema.attribute_num_] = type; 

  // Parse domain 
  void* domain = NULL;
  if(tiledbpy_parse_domain(domain_py, array_schema, domain) 
         != TILEDBPY_PARSE_OK) 
    return TILEDBPY_PARSE_ERR;

  // Copy domain
  array_schema.domain_ = domain;

  // Parse tile extents
  void* tile_extents = NULL;
  int has_tile_extents = 0;
  for(int i=0; i<array_schema.dim_num_; ++i) {
    if(tile_extents_py[i] != NULL) {
      has_tile_extents = 1;
      break;
    }
  }
  if(has_tile_extents &&
     tiledbpy_parse_tile_extents(tile_extents_py, array_schema, tile_extents)
         != TILEDBPY_PARSE_OK) 
    return TILEDBPY_PARSE_ERR;

  // Copy dimensions info
  array_schema.dimensions_ = 
      (char**) malloc(array_schema.dim_num_*sizeof(char*));
  for (int i = 0; i < array_schema.dim_num_; ++i) {
    // Copy dimension
    size_t dimension_len = dimensions[i].size();
    if(dimensions[i] == "" || dimension_len > TILEDB_NAME_MAX_LEN) {
      tiledbpy_parse_errmsg = "Invalid dimension name length";
      return TILEDBPY_PARSE_ERR;
    }
    array_schema.dimensions_[i] = (char*) malloc(dimension_len+1);
    strcpy(array_schema.dimensions_[i], dimensions[i].c_str());
  }
  array_schema.compression_[array_schema.attribute_num_] = compression; 
  array_schema.tile_extents_ = tile_extents;

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_domain(
    const std::vector<PyObject*>& domain_py,
    const TileDB_ArraySchema& array_schema,
    void*& domain) {
  // For easy reference
  int dim_num = array_schema.dim_num_;
  int attribute_num = array_schema.attribute_num_;
  int type = array_schema.types_[attribute_num];

  // Sanity check
  assert((int) domain_py.size() == dim_num);

  // Parse domain based on the dimensions type
  if(type == TILEDB_INT32) {
    // Get domain
    std::vector<int> domain_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check if domain item is a list of two values
      if(PyList_Size(domain_py[i]) != 2) {
        tiledbpy_parse_errmsg = "Domain must be a [low, high] list";
        return TILEDBPY_PARSE_ERR;
      }

      // Get domain endpoints
      for(int j=0; j<2; ++j) {
        // Check domain endpoint type
        PyObject* endpoint = PyList_GetItem(domain_py[i], j);
        if(!PyLong_Check(endpoint)) {
          tiledbpy_parse_errmsg = "Invalid domain type";
          return TILEDBPY_PARSE_ERR;
        }

        // Get domain endpoint
        domain_vec.push_back(PyLong_AsLong(endpoint));
      }
    }

    // Copy tile extents
    domain = malloc(2 * dim_num * sizeof(int));
    memcpy(domain, &domain_vec[0], 2 * dim_num * sizeof(int));
  } else if(type == TILEDB_INT64) {
    // Get domain
    std::vector<int64_t> domain_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check if domain item is a list of two values
      if(PyList_Size(domain_py[i]) != 2) {
        tiledbpy_parse_errmsg = "Domain must be a [low, high] list";
        return TILEDBPY_PARSE_ERR;
      }

      // Get domain endpoints
      for(int j=0; j<2; ++j) {
        // Check domain endpoint type
        PyObject* endpoint = PyList_GetItem(domain_py[i], j);
        if(!PyLong_Check(endpoint)) {
          tiledbpy_parse_errmsg = "Invalid domain type";
          return TILEDBPY_PARSE_ERR;
        }

        // Get domain endpoint
        domain_vec.push_back(PyLong_AsLong(endpoint));
      }
    }

    // Copy tile extents
    domain = malloc(2 * dim_num * sizeof(int64_t));
    memcpy(domain, &domain_vec[0], 2 * dim_num * sizeof(int64_t));
  } else if(type == TILEDB_FLOAT32) {
    // Get domain
    std::vector<float> domain_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check if domain item is a list of two values
      if(PyList_Size(domain_py[i]) != 2) {
        tiledbpy_parse_errmsg = "Domain must be a [low, high] list";
        return TILEDBPY_PARSE_ERR;
      }

      // Get domain endpoints
      for(int j=0; j<2; ++j) {
        // Check domain endpoint type
        PyObject* endpoint = PyList_GetItem(domain_py[i], j);
        if(!PyLong_Check(endpoint) && !PyFloat_Check(endpoint)) {
          tiledbpy_parse_errmsg = "Invalid domain type";
          return TILEDBPY_PARSE_ERR;
        }

        // Get domain endpoint
        domain_vec.push_back(PyFloat_AsDouble(endpoint));
      }
    }

    // Copy tile extents
    domain = malloc(2 * dim_num * sizeof(float));
    memcpy(domain, &domain_vec[0], 2 * dim_num * sizeof(float));
  } else if(type == TILEDB_FLOAT64) {
    // Get domain
    std::vector<double> domain_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check if domain item is a list of two values
      if(PyList_Size(domain_py[i]) != 2) {
        tiledbpy_parse_errmsg = "Domain must be a [low, high] list";
        return TILEDBPY_PARSE_ERR;
      }

      // Get domain endpoints
      for(int j=0; j<2; ++j) {
        // Check domain endpoint type
        PyObject* endpoint = PyList_GetItem(domain_py[i], j);
        if(!PyLong_Check(endpoint) && !PyFloat_Check(endpoint)) {
          tiledbpy_parse_errmsg = "Invalid domain type";
          return TILEDBPY_PARSE_ERR;
        }

        // Get domain endpoint
        domain_vec.push_back(PyFloat_AsDouble(endpoint));
      }
    }

    // Copy tile extents
    domain = malloc(2 * dim_num * sizeof(double));
    memcpy(domain, &domain_vec[0], 2 * dim_num * sizeof(double));
  }

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_tile_extents(
    const std::vector<PyObject*>& tile_extents_py,
    const TileDB_ArraySchema& array_schema,
    void*& tile_extents) {
  // For easy reference
  int dim_num = array_schema.dim_num_;
  int attribute_num = array_schema.attribute_num_;
  int type = array_schema.types_[attribute_num];

  // Sanity check
  assert((int) tile_extents_py.size() == dim_num);

  // Parse tile extents based on the dimensions type
  if(type == TILEDB_INT32) {
    // Get tile extents
    std::vector<int> tile_extents_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check type
      if(tile_extents_py[i] != NULL &&
         !PyLong_Check(tile_extents_py[i])) {
        tiledbpy_parse_errmsg = "Invalid tile extents type";
        return TILEDBPY_PARSE_ERR;
      }

      // Get tile extent
      if(tile_extents_py[i] != NULL) {
        tile_extents_vec.push_back(PyLong_AsLong(tile_extents_py[i]));
      } else {
        int* domain = (int*) array_schema.domain_;
        tile_extents_vec.push_back(domain[2*i+1] - domain[2*i] + 1);
      }
    }

    // Copy tile extents
    tile_extents = malloc(dim_num * sizeof(int));
    memcpy(tile_extents, &tile_extents_vec[0], dim_num * sizeof(int));
  } else if(type == TILEDB_INT64) {
    // Get tile extents
    std::vector<int64_t> tile_extents_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check type
      if(tile_extents_py[i] != NULL &&
         !PyLong_Check(tile_extents_py[i])) {
        tiledbpy_parse_errmsg = "Invalid tile extents type";
        return TILEDBPY_PARSE_ERR;
      }

      // Get tile extent
      if(tile_extents_py[i] != NULL) {
        tile_extents_vec.push_back(PyLong_AsLong(tile_extents_py[i]));
      } else {
        int64_t* domain = (int64_t*) array_schema.domain_;
        tile_extents_vec.push_back(domain[2*i+1] - domain[2*i] + 1);
      }
    }

    // Copy tile extents
    tile_extents = malloc(dim_num * sizeof(int64_t));
    memcpy(tile_extents, &tile_extents_vec[0], dim_num * sizeof(int64_t));
  } else if(type == TILEDB_FLOAT32) {
    // Get tile extents
    std::vector<float> tile_extents_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check type
      if(tile_extents_py[i] != NULL &&
         !PyLong_Check(tile_extents_py[i]) && 
         !PyFloat_Check(tile_extents_py[i])) {
        tiledbpy_parse_errmsg = "Invalid tile extents type";
        return TILEDBPY_PARSE_ERR;
      }

      // Get tile extent
      if(tile_extents_py[i] != NULL) {
        tile_extents_vec.push_back(PyFloat_AsDouble(tile_extents_py[i]));
      } else {
        float* domain = (float*) array_schema.domain_;
        tile_extents_vec.push_back(domain[2*i+1] - domain[2*i] + 1);
      }
    }

    // Copy tile extents
    tile_extents = malloc(dim_num * sizeof(float));
    memcpy(tile_extents, &tile_extents_vec[0], dim_num * sizeof(float));
  } else if(type == TILEDB_FLOAT64) {
    // Get tile extents
    std::vector<double> tile_extents_vec;
    for(int i=0; i<dim_num; ++i) {
      // Check type
      if(tile_extents_py[i] != NULL &&
         !PyLong_Check(tile_extents_py[i]) && 
         !PyFloat_Check(tile_extents_py[i])) {
        tiledbpy_parse_errmsg = "Invalid tile extents type";
        return TILEDBPY_PARSE_ERR;
      }

      // Get tile extent
      if(tile_extents_py[i] != NULL) {
        tile_extents_vec.push_back(PyFloat_AsDouble(tile_extents_py[i]));
      } else {
        double* domain = (double*) array_schema.domain_;
        tile_extents_vec.push_back(domain[2*i+1] - domain[2*i] + 1);
      }
    }

    // Copy tile extents
    tile_extents = malloc(dim_num * sizeof(double));
    memcpy(tile_extents, &tile_extents_vec[0], dim_num * sizeof(double));
  } else {
    tiledbpy_parse_errmsg = "Invalid dimension type";
    return TILEDBPY_PARSE_ERR;
  }

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_tile_order(
    int TileOrder,
    TileDB_ArraySchema& array_schema) {
  // Parse tile order
  if(TileOrder == TILEDB_ROW_MAJOR || TileOrder == TILEDB_COL_MAJOR) {
    array_schema.tile_order_ = TileOrder;
  } else {
    tiledbpy_parse_errmsg = "Invalid tile order"; 
    return TILEDBPY_PARSE_ERR;
  }

  // Success
  return TILEDBPY_PARSE_OK;
}

int tiledbpy_parse_type(
    int Type,
    TileDB_ArraySchema& array_schema) {
  // Parse type
  if(Type == TILEDB_DENSE) {
    array_schema.dense_ = 1;
  } else if(Type == TILEDB_SPARSE) {
    array_schema.dense_ = 0;
  } else {
    tiledbpy_parse_errmsg = "Invalid array type"; 
    return TILEDBPY_PARSE_ERR;
  }

  // Success
  return TILEDBPY_PARSE_OK;
}
