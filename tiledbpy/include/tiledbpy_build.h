/**
 * @file   tiledbpy_build.h
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
 * This file declares the functions used for building Python objects for 
 * TileDB-Py. 
 */

#ifndef __TILEDBPY_BUILD_H__
#define __TILEDBPY_BUILD_H__

#include<string>
#include "c_api.h"

/* Error codes. */
#define TILEDBPY_BUILD_OK      0
#define TILEDBPY_BUILD_ERR    -1

/* Global error message variable. */
extern std::string tiledbpy_build_errmsg;

/* Builds a Python tuple from the TileDB array schema. */
int tiledbpy_build_array_schema(
    const TileDB_ArraySchema& array_schema,
    PyObject*& ArraySchema);

/* 
 * Builds a Python tuple for the TileDB array attributes from the 
 * TileDB array schema. 
 */
void tiledbpy_build_attributes(
    const TileDB_ArraySchema& array_schema,
    PyObject*& Attributes);

/* 
 * Builds a Python tuple for the TileDB array dimensions from the 
 * TileDB array schema. 
 */
void tiledbpy_build_dimensions(
    const TileDB_ArraySchema& array_schema,
    PyObject*& Dimensions);

/**
 * Builds a Python list with tuples of the form (TileDB object, type),
 * which resulted from the invocation of tiledb_ls.
 */
void tiledbpy_build_ls(
    const char** dirs,
    const int* dir_types,
    int dir_num,
    PyObject*& LSList);

/**
 * Builds a Python list with the TileDB workspace strings returned 
 * from the invocation of tiledb_ls_workspaces.
 */
void tiledbpy_build_ls_workspaces(
    const char** workspaces,
    int workspace_num,
    PyObject*& LSList);


#endif
