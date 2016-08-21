/**
 * @file   tiledbpy_parse.h
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
 * This file declares auxiliary functions used when parsing the parameters of
 * the various TileDB-Py functions.
 */

#ifndef __TILEDBPY_PARSE_H__
#define __TILEDBPY_PARSE_H__

#include <Python.h>
#include <string>
#include <vector>
#include "c_api.h"

/* Error codes. */
#define TILEDBPY_PARSE_OK      0
#define TILEDBPY_PARSE_ERR    -1

/* Default attribute name. */
#define TILEDBPY_PARSE_DEFAULT_ATTRIBUTE "v"

/* Global error message variable. */
extern std::string tiledbpy_parse_errmsg;

/* Parse the array name. */
int tiledbpy_parse_array_name(
    const char* Name,
    TileDB_ArraySchema& array_schema);

/* Parse the array attributes. */
int tiledbpy_parse_attributes(
    PyObject* Attributes,
    TileDB_ArraySchema& array_schema);

/* Parses the TileDB array schema arguments and checks for errors. */
int tiledbpy_parse_array_schema(  
    PyObject* Attributes,
    int CellOrder,
    PyObject* Dimensions,
    const char* Name,
    long int TileCapacity,
    int TileOrder,
    int Type,
    TileDB_ArraySchema& array_schema);

/* Parse the cell order. */
int tiledbpy_parse_cell_order(
    int CellOrder,
    TileDB_ArraySchema& array_schema);

/* Parse the dimensions. */
int tiledbpy_parse_dimensions(
    PyObject* Dimensions,
    TileDB_ArraySchema& array_schema);

/* Parse domain. */
int tiledbpy_parse_domain(
    const std::vector<PyObject*>& domain_py,
    const TileDB_ArraySchema& array_schema,
    void*& domain);

/* Parse arguments for evaluating an expression. */
int tiledbpy_parse_expression_eval(
    PyObject* Variables,
    char** names,
    void** values,
    int* types,
    int var_num);

/* Parse tile extents. */
int tiledbpy_parse_tile_extents(
    const std::vector<PyObject*>& tile_extents_py,
    const TileDB_ArraySchema& array_schema,
    void*& tile_extents);

/* Parse the tile order. */
int tiledbpy_parse_tile_order(
    int TileOrder,
    TileDB_ArraySchema& array_schema);

/* Parse the type. */
int tiledbpy_parse_type(
    int Type,
    TileDB_ArraySchema& array_schema);

#endif
