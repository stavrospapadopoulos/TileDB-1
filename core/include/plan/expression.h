/**
 * @file   expression.h
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
 * This file defines class Expression. 
 */

#ifndef __TILEDB_EXPRESSION_H__
#define __TILEDB_EXPRESSION_H__

#include <map>
#include <set>
#include <string>
#include <vector>




/* ********************************* */
/*             CONSTANTS             */
/* ********************************* */

/**@{*/
/** Return code. */
#define TILEDB_EXPR_OK          0
#define TILEDB_EXPR_ERR        -1
/**@}*/

/** Default error message. */
#define TILEDB_EXPR_ERRMSG std::string("[TileDB::Expression] Error: ")




/* ********************************* */
/*          GLOBAL VARIABLES         */
/* ********************************* */

extern std::string tiledb_expr_errmsg;

/** An expression node. */
typedef struct ExpressionNode {
  /**
   * The data stored in the expression node. If the type is a basic constant
   * type, then the data are simply the corresponding value. If the type
   * is TILEDB_OP_*, then, after the evaluation of the expression, 'data'
   * will hold the type followed by the value of the result of the expression
   * subtree rooted at this node. 
   */
  void* data_;
  /** The input (left, right) nodes (originating at the incoming edges). */
  ExpressionNode* in_[2];
  /** The output node (the node at the end of the outgoing edge). */
  ExpressionNode* out_;
  /** 
   * The type of node. It can be one of:
   * TILEDB_EPXR_NULL
   * TILEDB_EPXR_INT32
   * TILEDB_EPXR_INT64
   * TILEDB_EPXR_FLOAT32
   * TILEDB_EPXR_FLOAT64
   * TILEDB_EPXR_VAR
   * TILEDB_EPXR_OP_ADD
   * TILEDB_EPXR_OP_SUB
   * TILEDB_EPXR_OP_MUL
   * TILEDB_EPXR_OP_DIV
   * TILEDB_EPXR_OP_MOD
   */
  int type_;
} ExpressionNode;

/** Manages a TileDB expression object. */
class Expression {
 public:
  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */
 
  /** Constructor. */ 
  Expression();
 
  /** Destructor. */
  ~Expression();




  /* ********************************* */
  /*            ACCESSORS              */
  /* ********************************* */

  /** Returns all nodes in the expression as a set. */
  std::set<ExpressionNode*> gather_nodes() const;

  /**
   * Retrieves and returns the ids of the variables with the input names.
   *
   * @param var_names The names of the variables whose ids will be retrieved.
   * @param var_num The number of elements in 'var_names'.
   * @return The ids of the variables with the input names.
   */
  std::vector<int> get_var_ids(const char** var_names, int var_num) const;

  /** Retrieves and returns the names of all variables in the expression. */
  std::vector<std::string> get_var_names() const;

  /** Returns the terminal node of the expression. */
  ExpressionNode* terminal() const;

  /**
   * Return the type of the expression value.
   *
   * @param ret_type The returned type. It can be one of the following:
   *     - TILEDB_EXPR_NULL      (if expression null, or not evaluated yet)
   *     - TILEDB_EXPR_INT32
   *     - TILEDB_EXPR_INT64
   *     - TILEDB_EXPR_FLOAT32
   *     - TILEDB_EXPR_FLOAT64
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
  int type(int* ret_type) const;

  /**
   * Dumps the expression into a graph in GraphViz's dot format stored in a 
   * file.
   *
   * @param filename The path to the file where the graph will be stored.
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
  int todot(const char* filename) const;

  /**
   * Returns the value of an expression. If the epxression has not been 
   * evaluated, the function returns an error.
   *
   * @param ret_value The value of the expression to be returned. It is assumed
   *     that the caller has properly allocated memory for 'value', otherwise
   *     the function may segfault.
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
  int value(void* ret_value) const;

  /** Returns the variable ids map. */
  const std::map<ExpressionNode*, int>& var_ids() const;

  /** Returns the variable names map. */
  const std::map<ExpressionNode*, std::string>& var_names() const;
  
  /** Returns the variable nodes map. */
  const std::map<std::string, ExpressionNode*>& var_nodes() const;
  
  /** Returns the number of variable in the expression. */
  int var_num() const;

  /* ********************************* */
  /*             MUTATORS              */
  /* ********************************* */

  /**
   * Performs a binary operation on two expressions. If the input expressions
   * are a and b, and the object expression is c, the result expression is 
   * c = (a op b), where op is the input operator. Note that the result
   * expression is practically stored in the calling Expression object. 
   *
   * @param a The first expression.
   * @param b The second expression.
   * @param op The binary operator.  It can be one of the following:
   *     - TILEDB_EXPR_OP_ADD
   *     - TILEDB_EXPR_OP_SUB
   *     - TILEDB_EXPR_OP_MUL
   *     - TILEDB_EXPR_OP_DIV
   *     - TILEDB_EXPR_OP_MOD
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
  int binary_op(const Expression& a, const Expression& b, int op);

  /** 
   * Clears the expression tree and variables. 
   *
   * @note Do not clear an expression A that is connected (e.g., via a binary 
   *     operator) to another expression B before B is evaluated. This is
   *     because the expression nodes are not replicated; deleting an expression
   *     A translates to pruning the subtrees in all the expressions that A
   *     participates in. This may lead to segfaults.
   */
  void clear();

  /**
   * Evaluates an expression, assigning values to the involved variables.
   * If the expression cannot be evaluated, an error is returned.
   *
   * @param values The values assigned to the variables. Note that there
   *     must be a one-to-one correspondence between these values and
   *     the variable ids. To get the variable ids, call
   *     var_ids().
   * @param types The corresponding types of 'values'.
   * @param num The number of elements in 'values' and 'types'.
   * @return TILEDB_OK upon success, and TILEDB_ERR upon error.
   */
  int eval(
      const void** values,
      const int* types);

  /** 
   * Initializes an expression. It can be either with a constant, or a
   * variable. A constant creates a single node in the expression, which
   * servers as the terminal. A variable creates a variable node, connected
   * to an empty (un-evaluated) terminal node (with type TILEDB_EXPR_NULL). 
   *
   * @param type The type of the node to be created. It can be one of the
   *     following:
   *     - TILEDB_EXPR_INT32
   *     - TILEDB_EXPR_INT64
   *     - TILEDB_EXPR_FLOAT32
   *     - TILEDB_EXPR_FLOAT64
   *     - TILEDB_EXPR_VAR
   * @param data The data the expression is initialized with. If the 'type'
   *     argument is one of the basic data types, then the data is simply
   *     a constant, expected in the corresponding data type (e.g., if
   *     type=TILEDB_INT32 then a 32-bit integer is expected in 'data'). If
   *     type=TILEDB_VAR, then a variable name is expected as a string in
   *     'data'.
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   *
   * @note Never initialize two different expressions for the same variable.
   *    This can lead to problems if two different expressions including the 
   *    same variable are connected via some binary operator.
   */
  int init(int type, const void* data);

  /**
   * Evaluates an expression, purging (i.e., pruning) every expression tree that
   * is evaluated. If a subtree contains a variable for which no value has been
   * given as input, then this subtree will not be purged.
   *
   * @param names The name of the variables that will be assigned values.
   * @param values The corresponding assigned values to 'vars'.
   * @param types The corresponding types of 'values'.
   * @param num The number of elements in 'vars', 'values' and 'types'.
   * @return TILEDB_OK upon success, and TILEDB_ERR upon error.
   */
  int purge(
      const char** names,
      const void** values,
      const int* types,
      int num);




 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** The terminal node of the expression. */
  ExpressionNode* terminal_;
  /** Mnemonic: [var_name] -> node */
  std::map<std::string, ExpressionNode*> var_nodes_;
  /** Mnemonic: [node] -> var_name */
  std::map<ExpressionNode*, std::string> var_names_;
  /** Mnemonic: [node] -> var_id */
  std::map<ExpressionNode*, int> var_ids_;



  /* ********************************* */
  /*          PRIVATE METHODS          */
  /* ********************************* */

  /** Clears the contents of an expression node and deletes the node. */
  void delete_node(ExpressionNode* node) const;

  /**
   * Evaluates the input operator based on the input values and types
   * for the variables.
   *
   * @param node The operator node.
   * @param values The values of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @param types The types of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
   int eval_op(
      ExpressionNode* node, 
      const void** values, 
      const int* types) const;

  /**
   * Evaluates the input operator based on the input values and types
   * for the variables.
   *
   * @template T The type of the result stored in the operator.
   * @param node The operator node.
   * @param values The values of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @param types The types of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @return TILEDB_EXPR_OK upon success, and TILEDB_EXPR_ERR upon error.
   */
  template<class T>
  int eval_op(
      ExpressionNode* node, 
      const void** values, 
      const int* types) const;

  /**
   * Evaluates the input operator on the input values.
   *
   * @template T The type of the operands and retuned value.
   * @param op The operator.
   * @param a The first operand.
   * @param b The second operand.
   * @param c The result, namely (a op c).
   * @param print_error In case of error, print the error message in debug mode.
   */
  template<class T>
  int eval_op(int op, T a, T b, T& c, bool print_error=true) const;

  /**
   * This function is called when the input node has a single child which
   * is a variable, and the data and type of the variable must be stored  
   * in the input node, based on the input values and types.
   *
   * @param node The input node, whose child in a variable.
   * @param values The assigned values to all variables.
   * @param types The assigned types to all variables.
   * @return void
   */
  void eval_var(
      ExpressionNode* node, 
      const void** values, 
      const int* types) const;

  /**
   * Gets the value corresponding to the input node, coerced to the templated
   * type.
   *
   * @template T The type to which the returned value is coerced.
   * @param node The input expression node.
   * @param values The values of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @param types The types of the variables, with one-to-one correspondence
   *     ordered by variable id.
   * @param ret_value The returned node value.
   */
  template<class T>
  int get_value(
      ExpressionNode* node, 
      const void** var_values, 
      const int* var_types,
      T& ret_value) const;

  /**
   * Gets the value corresponding to the input node, coerced to the templated
   * type. This function assumes that the node is constant.
   *
   * @template T The type to which the returned value is coerced.
   * @param node The input expression node.
   * @param ret_value The returned node value.
   */
  template<class T>
  int get_value(ExpressionNode* node, T& ret_value) const;


  /** 
   * Returns true if the input type is an operator of the form 
   * TILEDB_EXPR_INT* or TILEDB_EXPR_FLOAT*.
   */
  bool is_constant(int type) const;

  /** 
   * Returns true if the input type is an operator of the form 
   * TILEDB_EXPR_OP_*.
   */
  bool is_operator(int type) const;

  /** Merges the input variables into the local variable bookkeeping. */
  void merge_vars(const std::map<ExpressionNode*, std::string>& var_names);

  /** 
   * Creates and returns a new expression node storing the input type and data. 
   */
  ExpressionNode* new_node(int type, const void* data) const;

  /**
   * Evaluates an expression, purging (i.e., pruning) every expression tree that
   * is evaluated. If a subtree contains a variable for which no value has been
   * given as input, then this subtree will not be purged.
   *
   * @param values The values assigned to the variables. Note that there
   *     must be a one-to-one correspondence between these values and
   *     the variable ids. To get the variable ids, call
   *     var_ids().
   * @param types The corresponding types of 'values'.
   * @return TILEDB_OK upon success, and TILEDB_ERR upon error.
   */
  int purge(
      const void** values,
      const int* types);

  /**
   * Purges an operator, replacing it with the resulting constant. Note that
   * the operator must have two constant children, otherwise the node is left
   * intact.
   *
   * @param values The values assigned to the variables. Note that there
   *     must be a one-to-one correspondence between these values and
   *     the variable ids. To get the variable ids, call
   *     var_ids().
   * @param types The corresponding types of 'values'.
   * @return void
   */
  void purge_op(ExpressionNode* node);
  
  /**
   * Purges a variable node, replacing it with the constant that the variable
   * is assigned to. If the variable is not assigned some value, the node
   * remains intact.
   *
   * @param var_values The values assigned to the variables. Note that there
   *     must be a one-to-one correspondence between these values and
   *     the variable ids. To get the variable ids, call
   *     var_ids().
   * @param var_types The corresponding types of 'var_values'.
   * @return void
   */
  void purge_var(
      ExpressionNode* node,
      const void** var_values,
      const int* var_types);

  /**
   * Purges the input operator assuming that both its children are constants. If
   * the node cannot be purged, then 
   *
   * @template T The type of the result stored in the operator.
   * @param node The operator node.
   * @return True if the operator has been purged and false otherwise.
   */
  template<class T>
  bool purge_op(ExpressionNode* node);


  /** 
   * Returns the type of the input expression node. If the input node 
   * corresponds to a variable, then the type is derived based on the
   * input 'types' and the stored variable id in the node.
   */
  int type(
      const ExpressionNode*,
      const int* types) const;
};

#endif
