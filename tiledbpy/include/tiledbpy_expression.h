/**
 * @file   tiledbpy_expression.h
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
 * Header file of tiledbpy.Expression.
 */

#ifndef __TILEDBPY_EXPRESSION_H__
#define __TILEDBPY_EXPRESSION_H__

#include <Python.h>

#define TILEDBPY_EXPR_DOC                                                      \
    "A TileDB-Py expression. \n"     \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "(for the __new__ method)\n"                                               \
    "\n"                                                                       \
    "The method takes either no argument (empty expression), or one of the\n"  \
    "following:\n"                                                             \
    "   - An independent variable (IndVariable object)\n"                      \
    "   - An integer (long)\n"                                                 \
    "   - A floating point number (double)\n"                                  \
    "\n"                                                                       \
    "Binary operations\n"                                                      \
    "-----------------\n"                                                      \
    "\n"                                                                       \
    "The other operand must be an independent variable, long, double or\n"     \
    "TileDB-Py expression. The result is always a TileDB-Py expression. The \n"\
    "following operations are currently supported:\n"                          \
    "  - __add__,     + : addition\n"                                          \
    "  - __sub__,     - : subtraction\n"                                       \
    "  - __mul__,     * : multiplication\n"                                    \
    "  - __truediv__, / : true division\n"                                     \
    "  - __mod__,     % : modulo\n"                                            \
    "\n"                                                                       \
    "Exceptions\n"                                                             \
    "----------\n"                                                             \
    "\n"                                                                       \
    "tiledbpy.error:\n"                                                        \
    "   In case something goes wrong in the TileDB library.\n"                 \
    "TypeError:\n"                                                             \
    "   If the input arguments do not respect the expected types."         


#define TILEDBPY_EXPR_DOC_EVAL                                                 \
    "Evaluates the expression for the input value assignments to variables. \n"\
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "  variable assignements  (dict, madatory):\n"                             \
    "      A dictionary with pairs (IndVariable: value), where each pair\n"    \
    "      corresponds to a value being assigned to an expression variable.\n" \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "    The result of the evaluated expression.\n"                            \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      In case something goes wrong in the TileDB library.\n"              \
    "   TypeError:\n"                                                          \
    "      If the input argument does not respect the expected type."         


#define TILEDBPY_EXPR_DOC_TODOT                                                \
    "Exports the expression in GraphViz's dot format into the input file. \n"  \
    "\n"                                                                       \
    "Args:\n"                                                                  \
    "   filename (string, madatory):\n"                                        \
    "      The file into which the expression is exported.\n"                  \
    "\n"                                                                       \
    "Returns:\n"                                                               \
    "    PyNone\n"                                                             \
    "\n"                                                                       \
    "Raises:\n"                                                                \
    "   tiledbpy.error:\n"                                                     \
    "      In case something goes wrong in the TileDB library.\n"              \
    "   TypeError:\n"                                                          \
    "      If the input argument does not respect the expected type."         



/* Internals of an Expression object. */
typedef struct Expression {
  PyObject_HEAD
  // The TileDB expression
  TileDB_Expression* expr; 
} Expression;



#ifdef __cplusplus
extern "C" {
#endif

#if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#  define TILEDBPY_EXPORT __attribute__((visibility("default")))
#else
#  define TILEDBPY_EXPORT
#endif




/* The Expression type declaration. */
extern PyTypeObject ExpressionType;

}

#endif
