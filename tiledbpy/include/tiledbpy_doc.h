/**
 * @file   tiledbpy_doc.h
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
 * This file contains the documentation of the TileDB-Py functions.
 */

#ifndef __TILEDBPY_DOC_H__
#define __TILEDBPY_DOC_H__


#define TILEDBPY_DOC_ARRAY_SCHEMA                                              \
    "Retrieves the schema of a TileDB array.\n"                                \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   array_name (string): \n"                                               \
    "      The path of the array whose schema will be retrieved.\n"            \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "   A dictionary with the following key-value pairs:\n"                    \
    "\n"                                                                       \
    "   - Name (string):\n"                                                    \
    "     The array name.\n"                                                   \
    "\n"                                                                       \
    "   - CellOrder (int):\n"                                                  \
    "     The cell order. It is either TILEDBPY_ROW_MAJOR or\n"                \
    "     TILEDBPY_COL_MAJOR.\n"                                               \
    "\n"                                                                       \
    "   - Attributes (list of dict):\n"                                        \
    "     This is a list of dictionares, where each dictionary entry\n"        \
    "     corresponds to an attribute with the following key-value pairs:\n"   \
    "\n"                                                                       \
    "     * Name (string):\n"                                                  \
    "       The attribute name.\n"                                             \
    "\n"                                                                       \
    "     * Type (int):\n"                                                     \
    "       The attribute type. It is one of TILEDBPY_INT32,\n"                \
    "       TILEDBPY_INT64, TILEDBPY_FLOAT32, TILEDBPY_FLOAT64,\n"             \
    "       TILEDBPY_CHAR.\n"                                                  \
    "\n"                                                                       \
    "     * ValNum (int):\n"                                                   \
    "       The number of attribute values in a single cell. Special\n"        \
    "       value TILEDBPY_VAR_NUM indicates a variable number of\n"           \
    "       attribute values.\n"                                               \
    "\n"                                                                       \
    "     * Compression (int):\n"                                              \
    "       The compression type for this attribute. It is either\n"           \
    "       TILEDBPY_GZIP or TILEDBPY_NO_COMPRESSION.\n"                       \
    "\n"                                                                       \
    "   - Dimensions (list of dict):\n"                                        \
    "     This is a list of dictionares, where each dictionary entry\n"        \
    "     corresponds to either a single dimension or info about all\n"        \
    "     dimensions collectively.\n"                                          \
    "\n"                                                                       \
    "     There is at least one dictionary corresponding to a dimension.\n"    \
    "     Each such dictionary consists of the following key-value pairs:\n"   \
    "\n"                                                                       \
    "     * Name (string):\n"                                                  \
    "       The dimension name.\n"                                             \
    "\n"                                                                       \
    "     * Domain ([low (numeric), high (numeric)]):\n"                       \
    "       The dimension domain, expressed as a list of two values, the\n"    \
    "       low and high endpoints of the domain. These values are\n"          \
    "       numerics whose type complies with the type of dimensions\n"        \
    "       (see below).\n"                                                    \
    "\n"                                                                       \
    "     * TileExtent (numeric):\n"                                           \
    "       The tile extent across this dimension. The tile extent is a\n"     \
    "       numeric whose type complies with the type of dimensions (see\n"    \
    "       below).\n"                                                         \
    "\n"                                                                       \
    "     There is an additional optional dictionary entry, which contains\n"  \
    "     info about all dimensions collectively. This consists of the\n"      \
    "     following string keywords:\n"                                        \
    "\n"                                                                       \
    "     * Type (int):\n"                                                     \
    "       The type of all dimensions. It is one of TILEDBPY_INT32,\n"        \
    "       TILEDBPY_INT64, TILEDBPY_FLOAT32, or TILEDBPY_FLOAT64.\n"          \
    "\n"                                                                       \
    "     * Compression (int):\n"                                              \
    "       The compression type of the dimension values (coordinates).\n"     \
    "       It is either TILEDBPY_GZIP or TILEDB_NO_COMPRESSION.\n"            \
    "\n"                                                                       \
    "   - TileCapacity (long):\n"                                              \
    "     The tile capacity.\n"                                                \
    "\n"                                                                       \
    "   - TileOrder (int):\n"                                                  \
    "     The tile order. is either TILEDBPY_ROW_MAJOR or\n"                   \
    "     TILEDBPY_COL_MAJOR.\n"                                               \
    "\n"                                                                       \
    "   - Type (int):\n"                                                       \
    "     The array type. It is either TILEDBPY_DENSE or TILEDBPY_SPARSE.\n"   \
    "\n"                                                                       \
    "Example:\n"                                                               \
    "   An example array schema returned by the function looks like this:\n"   \
    "\n"                                                                       \
    "   {'Attributes': "                                                       \
         "[{'Compression': 0, 'Name': 'v', 'Type': 0, 'ValNum': 1}],\n"        \
    "    'CellOrder': 0,\n"                                                    \
    "    'Dimensions': [{'Domain': [0, 9], 'Name': 'rows', 'TileExtent': 5},\n"\
    "    {'Domain': [0, 19], 'Name': 'cols', 'TileExtent': 10},\n"             \
    "    {'Compression': 0, 'Type': 0}],\n"                                    \
    "    'Name': "                                                             \
            "'home/spapadop/TileDB/my_workspace/A',\n"                          \
    "    'TileCapacity': 10000,\n"                                             \
    "    'TileOrder': 0,\n"                                                    \
    "    'Type': 1}\n"                                                         \
    "\n"                                                                       \
    "   Note that the constants (e.g., in Type, Compression, etc.) should\n"   \
    "   be checked with the TILEDBPY_* values. For instance, to check if\n"    \
    "   the array is dense in a variable 'schema' that stores some array\n"    \
    "   schema, do the following:\n"                                           \
    "\n"                                                                       \
    "   schema['Type'] == tiledbpy.TILEDBPY_DENSE\n"                           \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      If the array schema cannot be retrieved for the input array.\n"     \
    "   TypeError:\n"                                                          \
    "      If the input argument does not respect the expected type."          \


#define TILEDBPY_DOC_CREATE_ARRAY                                              \
    "Creates a new TileDB array.\n"                                            \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   Name (string, madatory):\n"                                            \
    "      The array name. It is a directory, whose parent must be a\n"        \
    "      TileDB workspace, or group.\n"                                      \
    "\n"                                                                       \
    "   CellOrder (int, optional):\n"                                          \
    "      The cell order. It can be either TILEDBPY_ROW_MAJOR or\n"           \
    "      TILEDBPY_COL_MAJOR. If omitted, the default TILEDBPY_ROW_MAJOR\n"   \
    "      is used.\n"                                                         \
    "\n"                                                                       \
    "   Attributes (list of dict, optional):\n"                                \
    "      This is a list of dictionares, where each dictionary entry\n"       \
    "      corresponds to an attribute with the following string keywords:\n"  \
    "\n"                                                                       \
    "      - Name (string, mandatory):\n"                                      \
    "        The attribute name.\n"                                            \
    "\n"                                                                       \
    "      - Type (int, optional):\n"                                          \
    "        The attribute type. It can be one of TILEDBPY_INT32,\n"           \
    "        TILEDBPY_INT64, TILEDBPY_FLOAT32, TILEDBPY_FLOAT64,\n"            \
    "        TILEDBPY_CHAR.\n"                                                 \
    "\n"                                                                       \
    "      - ValNum (int, optional):\n"                                        \
    "        The number of attribute values in a single cell. Special\n"       \
    "        value TILEDBPY_VAR_NUM indicates a variable number of\n"          \
    "        attribute values. If omitted, the default value 1 is used.\n"     \
    "\n"                                                                       \
    "      - Compression (int, optional):\n"                                   \
    "        The compression type for this attribute. It can be either\n"      \
    "        TILEDBPY_GZIP or TILEDBPY_NO_COMPRESSION. If omitted, the\n"      \
    "        default value TILEDBPY_NO_COMPRESSION is used.\n"                 \
    "\n"                                                                       \
    "      If the Attributes list is empty, then a single attribute\n"         \
    "      is created, with default values Name='v', Type=TILEDBPY_INT32,\n"   \
    "      ValNum=1, Compression=TILEDBPY_NO_COMPRESSION.\n"                   \
    "\n"                                                                       \
    "   Dimensions (list of dict, mandatory):\n"                               \
    "      This is a list of dictionares, where each dictionary entry\n"       \
    "      corresponds to either a single dimension or info about all\n"       \
    "      dimensions collectively.\n"                                         \
    "\n"                                                                       \
    "      There must be at least one dictionary corresponding to a\n"         \
    "      dimension. Each such dictionary consists of the following\n"        \
    "      string keywords:\n"                                                 \
    "\n"                                                                       \
    "      - Name (string, mandatory):\n"                                      \
    "        The dimension name.\n"                                            \
    "\n"                                                                       \
    "      - Domain ([low (numeric), high (numeric)], mandatory):\n"           \
    "        The dimension domain, expressed as a list of two values, the\n"   \
    "        low and high endpoints of the domain. These values are\n"         \
    "        numerics whose type must comply with the type of dimensions\n"    \
    "        (see below).\n"                                                   \
    "\n"                                                                       \
    "      - TileExtent (numeric, optional):\n"                                \
    "        The tile extent across this dimension. Note that, for sparse\n"   \
    "        arrays, the tile extent merely shapes the cell order. The\n"      \
    "        tile extent is a numeric whose type must comply with the\n"       \
    "        type of dimensions (see below). If there are more than one\n"     \
    "        dimensions and at least one tile extent has been specified\n"     \
    "        for some dimension, the tile extents that are not set take\n"     \
    "        as default values the range of the corresponding dimension\n"     \
    "        domain (i.e., each tile covers the entire dimension domain.\n"    \
    "\n"                                                                       \
    "      There can be an additional optional dictionary entry, which\n"      \
    "      contains info about all dimensions collectively. This consists\n"   \
    "      of the following string keywords:\n"                                \
    "\n"                                                                       \
    "      - Type (int, optional):\n"                                          \
    "        The type of all dimensions. It can be one of TILEDBPY_INT32,\n"   \
    "        TILEDBPY_INT64, TILEDBPY_FLOAT32, or TILEDBPY_FLOAT64.\n"         \
    "\n"                                                                       \
    "      - Compression (int, optional):\n"                                   \
    "        The compression type of the dimension values (coordinates).\n"    \
    "        It can be either TILEDBPY_GZIP or TILEDB_NO_COMPRESSION.\n"       \
    "\n"                                                                       \
    "   TileCapacity (long, optional):\n"                                      \
    "      The tile capacity. If omitted, the default TileDB capacity\n"       \
    "      is used.\n"                                                         \
    "\n"                                                                       \
    "   TileOrder (int, optional):\n"                                          \
    "      The tile order (applicable only to dense arrays). It can be\n"      \
    "      either TILEDBPY_ROW_MAJOR or TILEDBPY_COL_MAJOR. If omitted,\n"     \
    "      the default TILEDBPY_ROW_MAJOR is used.\n"                          \
    "\n"                                                                       \
    "   Type (int, optional):\n"                                               \
    "      The array type. It can be either TILEDBPY_DENSE or\n"               \
    "      TILEDBPY_SPARSE. If omitted, the default type TILEDBPY_DENSE\n"     \
    "      is used.\n"                                                         \
    "\n"                                                                       \
    "Examples:\n"                                                              \
    "   - create_array(\n"                                                     \
    "        Name='A',\n"                                                      \
    "        Dimensions=[\n"                                                   \
    "           { 'Name':'rows', 'Domain':[0,9],  'TileExtent':5 },\n"         \
    "           { 'Name':'cols', 'Domain':[0,19], 'TileExtent':10 }])\n"       \
    "\n"                                                                       \
    "     It creates an array with name 'A'. The array has two dimensions,\n"  \
    "     with names 'rows' and 'cols', domains [0,9] and [0,19] and tile\n"   \
    "     extents 5 and 10, respectively. The dimensions are of the default\n" \
    "     type TILEDBPY_INT32 and have default compression type\n"             \
    "     TILEDBPY_NO_COMPRESSION.\n"                                          \
    "\n"                                                                       \
    "     Since no attributes are given, a default attribute is created with\n"\
    "     name 'v', type TILEDBPY_INT32, ValNum 1, and compression type\n"     \
    "     TILEDBPY_NO_COMPRESSION.\n"                                          \
    "\n"                                                                       \
    "     The rest of the array schema items are set to their default\n"       \
    "     values, namely, the array type is TILEDBPY_DENSE, the TileOrder\n"   \
    "     and CellOrder are TILEDBPY_ROW_MAJOR, and TileCapacity is set to\n"  \
    "     the default TileDB tile capacity.\n"                                 \
    "\n"                                                                       \
    "   - create_array(\n"                                                     \
    "        Name='~/B',\n"                                                    \
    "        Attributes=[\n"                                                   \
    "           { 'Name':'a1', 'Compression':TILEDBPY_GZIP },\n"               \
    "           { 'Name':'a2', 'Type':TILEDBPY_CHAR, "                         \
                              "'VAL_NUM':TILEDBPY_VAR_NUM } ],\n"              \
    "        Dimensions=[\n"                                                   \
    "           { 'Name':'d1', 'Domain':[1,10000] },\n"                        \
    "           { 'Name':'d2', 'Domain':[1,20000] },\n"                        \
    "           { 'Type': TILEDBPY_INT64, 'Compression'=TILEDBPY_GZIP } ],\n"  \
    "        Type=TILEDBPY_SPARSE,\n"                                          \
    "        CellOrder=TILEDBPY_COL_MAJOR,\n"                                  \
    "        TileCapacity=10)\n"                                               \
    "\n"                                                                       \
    "     It creates an array with two attributes. The first attribute has\n"  \
    "     name 'a1', and compression type TILEDBPY_GZIP. It is of default\n"   \
    "     type TILEDBPY_INT32 and has default ValNum 1. The second attribute\n"\
    "     has name 'a2', type TILEDBPY_CHAR and ValNum TILEDBPY_VAR_NUM.\n"    \
    "     This essentially corresponds to a string type, or an arbitrary\n"    \
    "     object serialized into a sequence of bytes. The attribute has\n"     \
    "     compression type TILEDBPY_NO_COMPRESION.\n"                          \
    "\n"                                                                       \
    "     The array has two dimensions 'd1', 'd2' with domains [1,10000] and\n"\
    "     [1,20000], respectively. No tile extents are specified. The\n"       \
    "     dimension values (coordinates) are of type TILEDBPY_INT64, and\n"    \
    "     have compression type TILEDBPY_GZIP.\n"                              \
    "\n"                                                                       \
    "     The array is of type TILEDBPY_SPARSE, and has CellOrder\n"           \
    "     TILEDBPY_COL_MAJOR and TileCapacity 10. TileOrder is ignored since\n"\
    "     no tile extents are specified.\n"                                    \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "   PyNone\n"                                                              \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      If the input array schema is invalid or if the TileDB array\n"      \
    "      cannot be created.\n"                                               \
    "   TypeError:\n"                                                          \
    "      If the input arguments do not respect the expected types."          \


#define TILEDBPY_DOC_CREATE_GROUP                                              \
    "Creates a new TileDB group.\n"                                            \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   group (string): \n"                                                    \
    "      The directory of the group to be created in the file system.\n"     \
    "      This should be a directory whose parent is a TileDB workspace or\n" \
    "      another TileDB group. This directory should not already exist.\n"   \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "    PyNone\n"                                                             \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      If the TileDB group cannot be created.\n"                           \
    "   TypeError:\n"                                                          \
    "      If the input argument does not respect the expected type."         



#define TILEDBPY_DOC_CREATE_WORKSPACE                                          \
    "Creates a new TileDB workspace.\n"                                        \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   workspace (string): \n"                                                \
    "      The directory of the workspace to be created in the file\n"         \
    "      system. This directory should not be inside another TileDB\n"       \
    "      workspace, group, array or metadata directory. Moreover,the \n"     \
    "      directory should not already exist.\n"                              \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "    PyNone\n"                                                             \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      If the TileDB workspace cannot be created.\n"                       \
    "   TypeError:\n"                                                          \
    "      If the input argument does not respect the expected type."         


#define TILEDBPY_DOC_FINALIZE                                                  \
    "Finalizes the TileDB-Py module.\n"                                        \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   void\n"                                                                \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "   PyNone\n"                                                              \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      If the TileDB context cannot be finalized."                        

#define TILEDBPY_DOC_VERSION                                                   \
    "Returns the version of the TileDB library used by the TileDB-Py module.\n"\
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   void\n"                                                                \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "   PyNone"

#endif
