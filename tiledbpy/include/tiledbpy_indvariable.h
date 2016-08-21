/**
 * @file   tiledbpy_indvariable.h
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
 * Header file of tiledbpy.IndVariable.
 */

#ifndef __TILEDBPY_INDVARIABLE_H__
#define __TILEDBPY_INDVARIVALE_H__

#include <Python.h>



#define TILEDBPY_INDVAR_DOC                                                    \
    "An independent variable, used typically in TileDB-Py expressions. \n"     \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "(for the __new__ method)\n"                                               \
    "\n"                                                                       \
    "An independent variable name (string, mandatory)\n"                       \
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



/* The IndVariable object definition. */
typedef struct IndVariable {
  PyObject_HEAD
  // The variable name
  char* name; 
} IndVariable;


#ifdef __cplusplus
extern "C" {
#endif

#if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#  define TILEDBPY_EXPORT __attribute__((visibility("default")))
#else
#  define TILEDBPY_EXPORT
#endif


/* The IndVariable type declaration. */
extern PyTypeObject IndVariableType;

}

#endif
