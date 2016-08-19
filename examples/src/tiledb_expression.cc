/**
 * @file   tiledb_expression.cc
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
 * It explores the TileDB expression API. Specifically, it constructs and
 * evaluates simple expression a * 5 + b  = 20 setting a=3, b=5.1.
 * It also purges this expression to 15 + b, by setting a=3.
 */

#include <cstring>
#include "c_api.h"

/* Create expression expr = a * 5 + b */
void create_expression(TileDB_Expression*& expr) {
  // Variable 'a'
  TileDB_Expression* tmp_expr1;
  const char a[] = "a";
  tiledb_expression_init(&tmp_expr1, TILEDB_EXPR_VAR, a);

  // Constant 5
  TileDB_Expression* tmp_expr2;
  int c = 5;
  tiledb_expression_init(&tmp_expr2, TILEDB_EXPR_INT32, &c);

  // Binary operation a * 5
  TileDB_Expression* tmp_expr3;
  tiledb_expression_binary_op(
      tmp_expr1, 
      tmp_expr2, 
      &tmp_expr3, 
      TILEDB_EXPR_OP_MUL); 

  // Variable 'b'
  TileDB_Expression* tmp_expr4;
  const char b[] = "b";
  tiledb_expression_init(&tmp_expr4, TILEDB_EXPR_VAR, b);

  // Binary operation (a * 5) + 'b' --> final expression
  tiledb_expression_binary_op(tmp_expr3, tmp_expr4, &expr, TILEDB_EXPR_OP_ADD);

  // Clean up
  free(tmp_expr1);
  free(tmp_expr2);
  free(tmp_expr3);
  free(tmp_expr4);

  // NOTE: We do NOT invoke tiledb_expression_clear on any tmp_expr*.
  // We will invoke it once for expr, and the memory of tmp_expr*
  // will be deallocated there. 
}

/* Print info about the expression on the screen */
void print_info(TileDB_Expression* expr) {
  // Get number of variables and variable names
  int var_num;
  tiledb_expression_var_num(expr, &var_num);
  char** var_names = new char*[var_num];
  for(int i=0; i<var_num; ++i)
    var_names[i] = new char[TILEDB_NAME_MAX_LEN];
  tiledb_expression_var_names(expr, var_names, &var_num);

  // Print info
  printf("Number of variables: %d\n", var_num);
  printf("Variables: \n");
  for(int i=0; i<var_num; ++i) 
    printf("\t%s\n", var_names[i]);

  // Clean up
  for(int i=0; i<var_num; ++i)
    delete [] var_names[i];
  delete [] var_names;
}

/* Evaluates the expression for an int 'a' and a double 'b'. */
void evaluate(
    TileDB_Expression* expr, 
    int a,
    double b) {
  // Get number of variables and variable names
  int var_num;
  tiledb_expression_var_num(expr, &var_num);
  char** var_names = new char*[var_num];
  for(int i=0; i<var_num; ++i)
    var_names[i] = new char[TILEDB_NAME_MAX_LEN];
  tiledb_expression_var_names(expr, var_names, &var_num);

  // Get variable ids - Important!
  int var_ids[2];
  tiledb_expression_var_ids(expr, (const char**) var_names, var_num, var_ids); 

  // Initialization
  int a_type= TILEDB_EXPR_INT32;
  int b_type = TILEDB_EXPR_FLOAT64;
  void* values[2];
  int types[2];

  // Allocate properly the variable values
  for(int i=0; i<var_num; ++i) {
    if(!strcmp(var_names[i], "a")) {
      types[var_ids[i]] = a_type;
      values[var_ids[i]] = malloc(sizeof(int));
      memcpy(values[var_ids[i]], &a, sizeof(int));
    }
    if(!strcmp(var_names[i], "b")) {
      types[var_ids[i]] = b_type;
      values[var_ids[i]] = malloc(sizeof(double));
      memcpy(values[var_ids[i]], &b, sizeof(double));
    }
  }

  // Evaluate 
  tiledb_expression_eval(expr, (const void**) values, types);

  // Get type of the result
  int res_type = TILEDB_EXPR_NULL;
  tiledb_expression_type(expr, &res_type);

  // Get type and print info
  printf("\nAssigning a=%d (int32), b=%lf (float64):\n", a, b);
  printf("\tResult type: ");
  if(res_type == TILEDB_EXPR_INT32) {
    printf("int32\n");
    int res;
    tiledb_expression_value(expr, &res);
    printf("\tResult value: %d\n", res);
  } else if(res_type == TILEDB_EXPR_INT64) {
    printf("int64\n");
    int64_t res;
    tiledb_expression_value(expr, &res);
    printf("\tResult value: %lld\n", res);
  } else if(res_type == TILEDB_EXPR_FLOAT32) {
    printf("float32\n");
    float res;
    tiledb_expression_value(expr, &res);
    printf("\tResult value: %f\n", res);
  } else if(res_type == TILEDB_EXPR_FLOAT64) {
    printf("float64\n");
    double res;
    tiledb_expression_value(expr, &res);
    printf("\tResult value: %lf\n", res);
  }
  
  // Clean up
  for(int i=0; i<var_num; ++i)
    delete [] var_names[i];
  delete [] var_names;
}

int main() {
  // Create the expression
  TileDB_Expression* expr;
  create_expression(expr); 

  // Export expression to a dot file
  tiledb_expression_todot(expr, "expr.dot");

  // Print info about the expression on the screen
  printf("\n--- Expression a * 5 + b ---\n\n");
  print_info(expr);

  // Evaluate the expression and print some info
  evaluate(expr, 3, 5.1);

  // Purge expression setting a=3
  const char* names[1];
  names[0] = "a";
  int* values[1];
  values[0] = new int;
  *values[0] = 3;
  int types[1];
  types[0] = TILEDB_EXPR_INT32;
  tiledb_expression_purge(expr, names, (const void**) values, types, 1);

  // Export purged expression to a dot file
  tiledb_expression_todot(expr, "expr_purged.dot");

  // Print info about the purged expression on the screen
  printf("\n--- Expression 15 + b ---\n\n");
  print_info(expr);
  
  // Evaluate the purged expression (the value for a will be ignored)
  evaluate(expr, 3, 5.1);

  // Clean up
  // NOTE: tiledb_expression_clear on expr is necessary!
  tiledb_expression_clear(expr);
  free(expr);

  return 0;
}
