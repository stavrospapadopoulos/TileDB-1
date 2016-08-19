/**
 * @file   expression.cc
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
 * This file implements the Expression class.
 */

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stack>
#include <typeinfo>
#include <unistd.h>
#include "constants.h"
#include "expression.h"
#include "utils.h"

/* ****************************** */
/*             MACROS             */
/* ****************************** */

#ifdef VERBOSE
#  define PRINT_ERROR(x) std::cerr << TILEDB_EXPR_ERRMSG << x << ".\n" 
#else
#  define PRINT_ERROR(x) do { } while(0) 
#endif




/* ****************************** */
/*        GLOBAL VARIABLES        */
/* ****************************** */

std::string tiledb_expr_errmsg = "";




/* ****************************** */
/*   CONSTRUCTORS & DESTRUCTORS   */
/* ****************************** */

Expression::Expression() {
  // Initializations
  terminal_ = NULL; 
}

Expression::~Expression() {
}




/* ****************************** */
/*           ACCESSORS            */
/* ****************************** */

std::set<ExpressionNode*> Expression::gather_nodes() const {
  // Nodes to be returned
  std::set<ExpressionNode*> nodes;

  // We run a postorder traversal using a stack
  std::stack<ExpressionNode*> st;
  ExpressionNode* root = terminal_;

  // Trivial case
  if(root == NULL)
   return nodes;
 
  // Postorder traversal
  do {
    // Move to the leftmost node
    while(root) {
      if(root->in_[1])
        st.push(root->in_[1]);
      st.push(root);

      // Set root as root's left child
      root = root->in_[0];
    }

    // Pop an item and set as root
    root = st.top();
    st.pop();

    // Process right child of root first
    if(!st.empty() && st.top() == root->in_[1]) {
      st.pop();
      st.push(root);
      root = root->in_[1];
    } else {
      // Process root
      nodes.insert(root);
      root = NULL; 
    }
  } while(!st.empty());

  // Return
  return nodes;
}

std::vector<int> Expression::get_var_ids(
    const char** var_names,
    int var_num) const {
  // Get variable ids
  std::vector<int> ids;
  std::map<std::string, ExpressionNode*>::const_iterator it;
  std::map<std::string, ExpressionNode*>::const_iterator it_end = 
      var_nodes_.end();
  
  for(int i=0; i<var_num; ++i) { 
    it = var_nodes_.find(var_names[i]);
    if(it != it_end)
      ids.push_back(var_ids_.find(it->second)->second);
    else 
      ids.push_back(-1);
  }

  // Return variable ids
  return ids;
}

std::vector<std::string> Expression::get_var_names() const {
  // Get variable names
  std::vector<std::string> names;
  std::map<std::string, ExpressionNode*>::const_iterator it = 
      var_nodes_.begin();
  std::map<std::string, ExpressionNode*>::const_iterator it_end = 
      var_nodes_.end();
  
  for(; it != it_end; ++it) 
    names.push_back(it->first);
 
  // Return variable names
  return names;
}

const std::map<ExpressionNode*, int>& Expression::var_ids() const {
  return var_ids_;
}


ExpressionNode* Expression::terminal() const {
  return terminal_;
}

int Expression::type(int* ret_type) const {
  // Copy type
  if(terminal_ == NULL) 
    *ret_type = TILEDB_EXPR_NULL;
  else 
    *ret_type = terminal_->type_;
  
  // Success
  return TILEDB_EXPR_OK;
}

int Expression::todot(const char* filename) const {
  // Gather nodes
  std::set<ExpressionNode*> nodes = gather_nodes();
  std::set<ExpressionNode*>::iterator node_it = nodes.begin();
  std::set<ExpressionNode*>::iterator node_it_end = nodes.end();

  // Create a map from nodes to ids
  std::map<ExpressionNode*, int> node_ids;
  for(int node_id=0; node_it != node_it_end; ++node_it, ++node_id) 
    node_ids[*node_it] = node_id;

  // Initialize string stream
  std::stringstream ss;

  // Write header
  ss << "digraph TileDB_Expression {\n";

  // Write node info to string stream 
  for(node_it=nodes.begin(); node_it != node_it_end; ++node_it) {
    // For easy reference
    ExpressionNode* node = *node_it;

    // Write node
    if(node->type_ == TILEDB_EXPR_NULL) {
      ss << "n" << node_ids[node] << "[label=\"NULL\"]\n"; 
    } else if(node->type_ == TILEDB_EXPR_VAR) {
      std::string var_name = var_names_.find(node)->second;
      ss << "n" << node_ids[node] 
         << "[label=\"" << var_name << "\"]\n";
    } else if(is_operator(node->type_)){
      switch(node->type_) {
        case TILEDB_EXPR_OP_ADD:
          ss << "n" << node_ids[node] << "[label=\"+\"]\n";
          break;
        case TILEDB_EXPR_OP_SUB:
          ss << "n" << node_ids[node] << "[label=\"-\"]\n";
          break;
        case TILEDB_EXPR_OP_MUL:
          ss << "n" << node_ids[node] << "[label=\"*\"]\n";
          break;
        case TILEDB_EXPR_OP_DIV:
          ss << "n" << node_ids[node] << "[label=\"/\"]\n";
          break;
        case TILEDB_EXPR_OP_MOD:
          ss << "n" << node_ids[node] << "[label=\"%\"]\n";
          break;
        default:
          assert(0);
      }
    } else if(is_constant(node->type_)) {
      switch(node->type_) {
        case TILEDB_EXPR_INT32:
          ss << "n" << node_ids[node] 
             << "[label=\"" << *((int*)node->data_) << "\"]\n";
          break;
        case TILEDB_EXPR_INT64:
          ss << "n" << node_ids[node] 
             << "[label=\"" << *((int64_t*)node->data_) << "\"]\n";
          break;
        case TILEDB_EXPR_FLOAT32:
          ss << "n" << node_ids[node] 
             << "[label=\"" << *((float*)node->data_) << "\"]\n";
          break;
        case TILEDB_EXPR_FLOAT64:
          ss << "n" << node_ids[node] 
             << "[label=\"" << *((double*)node->data_) << "\"]\n";
          break;
        default:
          assert(0);
      }
    } else {
      assert(0); // The code should never reach here
    }
    
    // Write edges (only the outgoing is sufficient)
    if(node->out_ != NULL) 
      ss << "n" << node_ids[node] << "->" 
         << "n" << node_ids[node->out_] << "\n";
  } 

  // Footer
  ss << "}";

  // Open dot file
  std::string real_filename = real_dir(filename);
  int fd = open(real_filename.c_str(), O_WRONLY | O_CREAT | O_SYNC, S_IRWXU);
  if(fd == -1) {
    std::string errmsg = 
        std::string("Failed to create dot file; ") + strerror(errno);
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg; 
    return TILEDB_EXPR_ERR;
  }

  // Write to dot file
  ssize_t ss_len = ss.str().size();
  ssize_t bytes_written = write(fd, ss.str().c_str(), ss_len);
  if(bytes_written != ss_len) {
    std::string errmsg = 
        std::string("Failed to write to dot file; ") + strerror(errno);
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg; 
    return TILEDB_EXPR_ERR;
  } 

  // Sync dot file
  if(fsync(fd)) {
    std::string errmsg = 
        std::string("Failed to sync dot file; ") + strerror(errno);
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg; 
    return TILEDB_EXPR_ERR;
  }

  // Close dot file
  if(close(fd)) {
   std::string errmsg = 
       std::string("Failed to close dot file; ") + strerror(errno);
   PRINT_ERROR(errmsg);
   tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg; 
   return TILEDB_EXPR_ERR;
  }

  // Success
  return TILEDB_EXPR_OK;
}

int Expression::value(void* ret_value) const {
  // Sanity checks
  if(terminal_ == NULL) {
    std::string errmsg = 
        "Cannot get expression value; Expression is null";
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
    return TILEDB_EXPR_ERR;
  } else if(terminal_->type_ == TILEDB_EXPR_NULL) {
    std::string errmsg = 
        "Cannot get expression value; Expression not evaluated";
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
    return TILEDB_EXPR_ERR;
  } else if(terminal_->type_ == TILEDB_EXPR_VAR) {
    assert(0); // The terminal should never be a variable
  }

  // Copy value
  if(terminal_->type_ == TILEDB_EXPR_INT32)
    memcpy(ret_value, terminal_->data_, sizeof(int));
  else if(terminal_->type_ == TILEDB_EXPR_INT64)
    memcpy(ret_value, terminal_->data_, sizeof(int64_t));
  else if(terminal_->type_ == TILEDB_EXPR_FLOAT32)
    memcpy(ret_value, terminal_->data_, sizeof(float));
  else if(terminal_->type_ == TILEDB_EXPR_FLOAT64)
    memcpy(ret_value, terminal_->data_, sizeof(double));
  else
    assert(0); // Code should never reach here
  
  // Success
  return TILEDB_EXPR_OK;
}

const std::map<ExpressionNode*, std::string>& Expression::var_names() const {
  return var_names_;
}

const std::map<std::string, ExpressionNode*>& Expression::var_nodes() const {
  return var_nodes_;
}

int Expression::var_num() const {
  return var_nodes_.size();
}




/* ****************************** */
/*            MUTATORS            */
/* ****************************** */

int Expression::binary_op(
    const Expression& a,
    const Expression& b,
    int op) {
  // Sanity check on operator
  if(op != TILEDB_EXPR_OP_ADD &&
     op != TILEDB_EXPR_OP_SUB &&
     op != TILEDB_EXPR_OP_MUL &&
     op != TILEDB_EXPR_OP_DIV &&
     op != TILEDB_EXPR_OP_MOD) {
    std::string errmsg = "Cannot perform binary operation; Invalid operator";
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
  }

  // Create new operator node
  ExpressionNode* op_node = new_node(op, NULL);

  // Find proper left and right child nodes
  ExpressionNode *left, *right;
  ExpressionNode* tmp_left = a.terminal();
  ExpressionNode* tmp_right = b.terminal();
  if(tmp_left->type_ == TILEDB_EXPR_NULL) {
    // Remove the terminal node of expression a 
    left = tmp_left->in_[0];
    delete_node(tmp_left);
  } else {
    left = tmp_left;
  }
  if(tmp_right->type_ == TILEDB_EXPR_NULL) {
    // Remove the terminal node of expression b
    right = tmp_right->in_[0];
    delete_node(tmp_right);
  } else {
    right = tmp_right;
  }

  // Connect the terminal nodes of the input expressions
  op_node->in_[0] = left;
  op_node->in_[1] = right;
  left->out_ = op_node;
  right->out_ = op_node;

  // Create new terminal node and connect the operator node to it
  terminal_ = new_node(TILEDB_EXPR_NULL, NULL); 
  op_node->out_ = terminal_;
  terminal_->in_[0] = op_node;

  // Merge the map of variables of the two expressions
  var_nodes_ = a.var_nodes();
  var_names_ = a.var_names();
  var_ids_ = a.var_ids();
  merge_vars(b.var_names());

  // Success
  return TILEDB_EXPR_OK;
}
  
void Expression::clear() {
  // Gather nodes to delete
  std::set<ExpressionNode*> nodes = gather_nodes();

  // Delete nodes
  std::set<ExpressionNode*>::iterator it = nodes.begin();
  std::set<ExpressionNode*>::iterator it_end = nodes.end();
  for(; it != it_end; ++it) 
    delete_node(*it);

  var_nodes_.clear();
  var_names_.clear(); 
  var_ids_.clear();
  terminal_ = NULL;
}

int Expression::eval(
    const void** values,
    const int* types) {
  // Sanity checks
  if(terminal_ == NULL) {
    std::string errmsg = "Cannot evaluate expression; Terminal node is null";
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
    return TILEDB_EXPR_ERR;
  }

  // Trivial case #1 - Terminal is constant, so do nothing
  if(is_constant(terminal_->type_))
    return TILEDB_EXPR_OK;

  // Trivial case #2 - Terminal is the output of a variable
  // Copy the variable value
  assert(terminal_->in_[0] != NULL);
  if(terminal_->in_[0]->type_ == TILEDB_EXPR_VAR) {
    eval_var(terminal_, values, types);
    return TILEDB_EXPR_OK;
  }

  // General case from this point on - The terminal is connected to a binary
  // operator

  // Nullify the type of the terminal node to indicate that the expression
  // has not been evaluated yet
  terminal_->type_ = TILEDB_EXPR_NULL;

  // We run a postorder traversal using a stack, and starting at the first 
  // operator
  std::stack<ExpressionNode*> st;
  ExpressionNode* root = terminal_->in_[0];
  assert(is_operator(root->type_));

  // Postorder traversal
  do {
    // Move to the leftmost node
    while(root) {
      if(root->in_[1])
        st.push(root->in_[1]);
      st.push(root);

      // Set root as root's left child
      root = root->in_[0];
    }

    // Pop an item and set as root
    root = st.top();
    st.pop();

    // Process right child of root first
    if(!st.empty() && st.top() == root->in_[1]) {
      st.pop();
      st.push(root);
      root = root->in_[1];
    } else {
      // Process root
      if(is_operator(root->type_) && 
         eval_op(root, values, types) != TILEDB_EXPR_OK) 
        return TILEDB_EXPR_ERR;
      root = NULL; 
    }
  } while(!st.empty());

  // Get final value into the terminal node
  ExpressionNode* op = terminal_->in_[0];
  assert(is_operator(op->type_));
  int op_type = *((int*)op->data_);
  assert(is_constant(op_type));
  void* op_value = (char*)op->data_ + sizeof(int);
  terminal_->type_ = op_type;
  memcpy(terminal_->data_, op_value, sizeof(double)); // Max size

  // Success 
  return TILEDB_EXPR_OK;
}

int Expression::init(int type, const void* data) {
  // Sanity checks on type
  if(type != TILEDB_EXPR_INT32 &&
     type != TILEDB_EXPR_INT64 && 
     type != TILEDB_EXPR_FLOAT32 && 
     type != TILEDB_EXPR_FLOAT64 &&
     type != TILEDB_EXPR_VAR) {
    std::string errmsg = "Cannot initialize expression; Invalid type";
    PRINT_ERROR(errmsg);
    tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
    return TILEDB_EXPR_ERR;
  } 

  // Clear the expression first
  clear();

  // If variable which has not been seen again, store its name
  if(type == TILEDB_EXPR_VAR) {
    // Create variable node
    ExpressionNode* var_node = new_node(type, NULL);

    // Update variable bookkeeping
    std::map<std::string, ExpressionNode*>::iterator it;
    const char* name = (const char*) data;
    it = var_nodes_.find(name);
    if(it == var_nodes_.end())  {
      var_nodes_[name] = var_node;
      var_names_[var_node] = name;
      var_ids_[var_node] = 0;
    }

    // Create terminal node
    int null_type = TILEDB_EXPR_NULL;
    terminal_ = new_node(null_type, NULL); 

    // Connect variable node with terminal node
    terminal_->in_[0] = var_node;
    var_node->out_ = terminal_;
  } else { // Not a variable; this is a basic type
    terminal_= new_node(type, data);
  }

  // Success
  return TILEDB_EXPR_OK;
}



int Expression::purge(
    const char** names,
    const void** values,
    const int* types,
    int num) {
  // Initialization
  int var_num = this->var_num();
  int* var_types = new int[var_num];
  const void** var_values = new const void*[var_num];
  for(int i=0; i<var_num; ++i) {
    var_types[i] = TILEDB_EXPR_NULL;
    var_values[i] = NULL;
  }
  std::map<std::string, ExpressionNode*>::iterator it;
  std::map<std::string, ExpressionNode*>::iterator it_end = var_nodes_.end();

  // Get variable types and values
  for(int i=0; i<num; ++i) {
    it = var_nodes_.find(names[i]);
    if(it == it_end) {
      std::string errmsg = "Cannot purge expression; Invalid variable name";
      PRINT_ERROR(errmsg);
      tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
      return TILEDB_EXPR_ERR;
    } else {
      var_types[var_ids_.find(it->second)->second] = types[i]; 
      var_values[var_ids_.find(it->second)->second] = values[i]; 
    }
  }

  // Purge
  int rc = purge(var_values, var_types);
  
  // Update variable ids
  if(rc == TILEDB_EXPR_OK) {
    std::map<ExpressionNode*, int> new_var_ids;
    std::map<ExpressionNode*, int>::iterator it = var_ids_.begin();
    std::map<ExpressionNode*, int>::iterator it_end = var_ids_.end();
    for(int var_id=0; it != it_end; ++it, ++var_id) 
      new_var_ids[it->first] = var_id;
    var_ids_ = new_var_ids;
  }

  // Clean up
  delete [] var_types;
  delete [] var_values;

  // Return 
  return rc;
}


/* ****************************** */
/*        PRIVATE METHODS         */
/* ****************************** */

void Expression::delete_node(ExpressionNode* node) const {
  free(node->data_);
  free(node);
}

inline
int Expression::eval_op(
    ExpressionNode* node,
    const void** values,
    const int* types) const {
  // Sanity check
  assert(is_operator(node->type_));

  // For easy reference
  ExpressionNode* left = node->in_[0];
  ExpressionNode* right = node->in_[1];

  // Get types
  int left_type = type(left, types);
  int right_type = type(right, types);
  int type = (left_type > right_type) ? left_type : right_type; 

  // Evaluate the value based on the type
  int rc;
  switch(type) {
    case TILEDB_EXPR_INT32:
      rc = eval_op<int>(node, values, types); 
      break;
    case TILEDB_EXPR_INT64:
      rc = eval_op<int64_t>(node, values, types); 
      break;
    case TILEDB_EXPR_FLOAT32:
      rc = eval_op<float>(node, values, types); 
      break;
    case TILEDB_EXPR_FLOAT64:
      rc = eval_op<double>(node, values, types); 
      break;
    default:
      assert(0); // Code should never reach here 
  }

  // Set node type
  *((int*)node->data_) = type;

  // Return
  return rc;
}

template<class T>
inline
int Expression::eval_op(
    ExpressionNode* node,
    const void** values,
    const int* types) const {
  // Get values converted in the coerced type
  T left_value, right_value, op_value;
  if(get_value<T>(node->in_[0], values, types, left_value) != TILEDB_EXPR_OK)
    return TILEDB_EXPR_ERR;
  if(get_value<T>(node->in_[1], values, types, right_value) != TILEDB_EXPR_OK)
    return TILEDB_EXPR_ERR;

  // Evaluate the operator on the coerced values
  if(eval_op(node->type_, left_value, right_value, op_value) != TILEDB_EXPR_OK)
    return TILEDB_EXPR_ERR;

  // Copy the operator value to the node
  void*  node_value = (char*) node->data_ + sizeof(int);
  memcpy(node_value, &op_value, sizeof(T));
 
  // Success
  return TILEDB_EXPR_OK;
}

template<class T>
inline
int Expression::eval_op(int op, T a, T b, T& c, bool print_error) const {
  std::string errmsg;

  // Evaluate the operator
  switch(op) {
    case TILEDB_EXPR_OP_ADD:
      c = a+b;
      break;
    case TILEDB_EXPR_OP_SUB:
      c = a-b;
      break;
    case TILEDB_EXPR_OP_MUL:
      c = a*b;
      break;
    case TILEDB_EXPR_OP_DIV:
      c = a/b;
      break;
    case TILEDB_EXPR_OP_MOD:
      if(print_error) {
        errmsg = "Cannot evaluate mod operator; Invalid operand type";
        PRINT_ERROR(errmsg);
        tiledb_expr_errmsg = TILEDB_EXPR_ERRMSG + errmsg;
      }
      return TILEDB_EXPR_ERR;
      break;
    default:
      assert(0); // The code should never reach here 
  }

  // Success
  return TILEDB_EXPR_OK;
}

template<>
inline
int Expression::eval_op<int>(
    int op, 
    int a, 
    int b, 
    int& c, 
    bool print_error) const {
  // Evaluate the operator
  switch(op) {
    case TILEDB_EXPR_OP_ADD:
      c = a+b;
      break;
    case TILEDB_EXPR_OP_SUB:
      c = a-b;
      break;
    case TILEDB_EXPR_OP_MUL:
      c = a*b;
      break;
    case TILEDB_EXPR_OP_DIV:
      c = a/b;
      break;
    case TILEDB_EXPR_OP_MOD:
      c = a % b;
      break;
    default:
      assert(0); // The code should never reach here 
  }

  // Success
  return TILEDB_EXPR_OK;
}

template<>
inline
int Expression::eval_op<int64_t>(
    int op, 
    int64_t a, 
    int64_t b, 
    int64_t& c,
    bool print_error) const {
  // Evaluate the operator
  switch(op) {
    case TILEDB_EXPR_OP_ADD:
      c = a+b;
      break;
    case TILEDB_EXPR_OP_SUB:
      c = a-b;
      break;
    case TILEDB_EXPR_OP_MUL:
      c = a*b;
      break;
    case TILEDB_EXPR_OP_DIV:
      c = a/b;
      break;
    case TILEDB_EXPR_OP_MOD:
      c = a % b;
      break;
    default:
      assert(0); // The code should never reach here 
  }

  // Success
  return TILEDB_EXPR_OK;
}

void Expression::eval_var(
    ExpressionNode* node,
    const void** var_values,
    const int* var_types) const {
  // Sanity checks
  assert(node);
  assert(node->in_[0]);
  assert(!node->in_[1]);
  assert(node->in_[0]->type_ == TILEDB_EXPR_VAR);

  // For easy reference
  ExpressionNode* var_node = node->in_[0];

  // Check if the variable value is given
  int var_id = (var_ids_.find(var_node))->second; 
  if(var_types[var_id] == TILEDB_EXPR_NULL)
    return;

  // Assign type and value
  node->type_ = var_types[var_id];
  if(node->type_ == TILEDB_EXPR_INT32) 
    memcpy(node->data_, var_values[var_id], sizeof(int));
  else if(node->type_ == TILEDB_EXPR_INT64) 
    memcpy(node->data_, var_values[var_id], sizeof(int64_t));
  else if(node->type_ == TILEDB_EXPR_FLOAT32) 
    memcpy(node->data_, var_values[var_id], sizeof(float));
  else if(node->type_ == TILEDB_EXPR_FLOAT64) 
    memcpy(node->data_, var_values[var_id], sizeof(double));
  else
    assert(0); // The code should never reach here
}
   
template<class T>
inline
int Expression::get_value(
    ExpressionNode* node,
    const void** var_values,
    const int* var_types,
    T& ret_value) const {
  if(node->type_ == TILEDB_EXPR_INT32) { 
    int value;
    memcpy(&value, node->data_, sizeof(int));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_INT64) {
    int64_t value;
    memcpy(&value, node->data_, sizeof(int64_t));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_FLOAT32) {
    float value;
    memcpy(&value, node->data_, sizeof(float));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_FLOAT64) {
    double value;
    memcpy(&value, node->data_, sizeof(double));
    ret_value = value;
  } else if(is_operator(node->type_)) {
    int op_type;
    memcpy(&op_type, node->data_, sizeof(int));
    void* op_value = (char*) node->data_ + sizeof(int);
    if(op_type == TILEDB_EXPR_INT32) {
      int value;
      memcpy(&value, op_value, sizeof(int));
      ret_value = value;
    } else if(op_type == TILEDB_EXPR_INT64) {
      int64_t value;
      memcpy(&value, op_value, sizeof(int64_t));
      ret_value = value;
    } else if(op_type == TILEDB_EXPR_FLOAT32) {
      float value;
      memcpy(&value, op_value, sizeof(float));
      ret_value = value;
    } else if(op_type == TILEDB_EXPR_FLOAT64) {
      double value;
      memcpy(&value, op_value, sizeof(double));
      ret_value = value;
    }
  } else if(node->type_ == TILEDB_EXPR_VAR) {
    int var_id = (var_ids_.find(node))->second;
    if(var_types[var_id] == TILEDB_EXPR_INT32) {
      int var_value;
      memcpy(&var_value, var_values[var_id], sizeof(int));
      ret_value = var_value;
    } else if(var_types[var_id] == TILEDB_EXPR_INT64) {
      int64_t var_value;
      memcpy(&var_value, var_values[var_id], sizeof(int64_t));
      ret_value = var_value;
    } else if(var_types[var_id] == TILEDB_EXPR_FLOAT32) {
      float var_value;
      memcpy(&var_value, var_values[var_id], sizeof(float));
      ret_value = var_value;
    } else if(var_types[var_id] == TILEDB_EXPR_FLOAT64) {
      double var_value;
      memcpy(&var_value, var_values[var_id], sizeof(double));
      ret_value = var_value;
    }
  } else {
    assert(0); // The code should not reach here
  }

  // Success
  return TILEDB_EXPR_OK;
}

template<class T>
inline
int Expression::get_value(ExpressionNode* node, T& ret_value) const {
  // Sanity check
  assert(is_constant(node->type_));

  if(node->type_ == TILEDB_EXPR_INT32) { 
    int value;
    memcpy(&value, node->data_, sizeof(int));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_INT64) {
    int64_t value;
    memcpy(&value, node->data_, sizeof(int64_t));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_FLOAT32) {
    float value;
    memcpy(&value, node->data_, sizeof(float));
    ret_value = value;
  } else if(node->type_ == TILEDB_EXPR_FLOAT64) {
    double value;
    memcpy(&value, node->data_, sizeof(double));
    ret_value = value;
  } else {
    assert(0); // The code should not reach here
  }

  // Success
  return TILEDB_EXPR_OK;
}

inline
bool Expression::is_constant(int type) const {
  return type == TILEDB_EXPR_INT32 ||
         type == TILEDB_EXPR_INT64 ||
         type == TILEDB_EXPR_FLOAT32 ||
         type == TILEDB_EXPR_FLOAT64;
}

inline
bool Expression::is_operator(int type) const {
  return type == TILEDB_EXPR_OP_ADD ||
         type == TILEDB_EXPR_OP_SUB ||
         type == TILEDB_EXPR_OP_MUL ||
         type == TILEDB_EXPR_OP_DIV ||
         type == TILEDB_EXPR_OP_MOD;
}

void Expression::merge_vars(
    const std::map<ExpressionNode*, std::string>& var_names) {
  std::map<ExpressionNode*, std::string>::const_iterator it = 
      var_names.begin();
  std::map<ExpressionNode*, std::string>::const_iterator it_end = 
      var_names.end();
  int var_num;

  for(; it != it_end; ++it) {
    var_num = (int) var_names_.size();
    if(var_nodes_.find(it->second) == var_nodes_.end()) { 
      var_nodes_[it->second] = it->first;
      var_names_[it->first] = it->second;
      var_ids_[it->first] = var_num;
    }
  }
}

ExpressionNode* Expression::new_node(int type, const void* data) const {
  // Create a new node
  ExpressionNode* node = (ExpressionNode*)malloc(sizeof(ExpressionNode));

  // Populate the node
  node->in_[0] = NULL;
  node->in_[1] = NULL;
  node->out_ = NULL;
  node->type_ = type;
  if(type == TILEDB_EXPR_INT32) {
    node->data_ = malloc(sizeof(int));
    memcpy(node->data_, data, sizeof(int));
  } else if(type == TILEDB_EXPR_INT64) {
    node->data_ = malloc(sizeof(int64_t));
    memcpy(node->data_, data, sizeof(int64_t));
  } else if(type == TILEDB_EXPR_FLOAT32) {
    node->data_ = malloc(sizeof(float));
    memcpy(node->data_, data, sizeof(float));
  } else if(type == TILEDB_EXPR_FLOAT64) {
    node->data_ = malloc(sizeof(double));
    memcpy(node->data_, data, sizeof(double));
  } else if(type == TILEDB_EXPR_VAR) {
    node->data_ = malloc(sizeof(double));  // Assign the maximum space
    // No need to initialize any data
  } else if(is_operator(type)) {
    node->data_ = malloc(sizeof(int) + sizeof(double));
    // No initialization of data needed
  } else if(type == TILEDB_EXPR_NULL) {
    // This is the terminal node before evaluation.
    // We assign the maximum possible bytes since we do not know the type.
    node->data_ = malloc(sizeof(double));
  } else { 
    assert(0);  // Code should never reach here
  }

  // Return the new node
  return node;
}

int Expression::purge(
    const void** values,
    const int* types) {
  // If the expression is null or a constant, do nothing
  if(terminal_ == NULL || 
     (terminal_->in_[0] == NULL && is_constant(terminal_->type_)))
    return TILEDB_EXPR_OK;

  // Terminal is the output of a variable.
  // If variable given, then copy the variable value and replace the node
  // with a constant node
  assert(terminal_->in_[0] != NULL);
  if(terminal_->in_[0]->type_ == TILEDB_EXPR_VAR) {
    eval_var(terminal_, values, types);
    delete_node(terminal_->in_[0]);
    terminal_->in_[0] = NULL;
    return TILEDB_EXPR_OK;
  }

  // General case from this point on - The terminal is connected to a binary
  // operator

  // Nullify the type of the terminal node to indicate that the expression
  // has not been evaluated yet
  terminal_->type_ = TILEDB_EXPR_NULL;

  // We run a postorder traversal using a stack
  std::stack<ExpressionNode*> st;
  ExpressionNode* root = terminal_->in_[0];
  assert(is_operator(root->type_));

  // Postorder traversal
  do {
    // Move to the leftmost node
    while(root) {
      if(root->in_[1])
        st.push(root->in_[1]);
      st.push(root);

      // Set root as root's left child
      root = root->in_[0];
    }

    // Pop an item and set as root
    root = st.top();
    st.pop();

    // Process right child of root first
    if(!st.empty() && st.top() == root->in_[1]) {
      st.pop();
      st.push(root);
      root = root->in_[1];
    } else {
      // Process root
      if(root->type_ == TILEDB_EXPR_VAR)
        purge_var(root, values, types);
      else if(is_operator(root->type_)) 
        purge_op(root);
      root = NULL; 
    }
  } while(!st.empty());

  // Get final value into the terminal node
  if(is_constant(terminal_->in_[0]->type_)) {
    ExpressionNode* op = terminal_->in_[0];
    assert(is_operator(op->type_));
    int op_type = *((int*)op->data_);
    assert(is_constant(op_type));
    void* op_value = (char*)op->data_ + sizeof(int);
    terminal_->type_ = op_type;
    memcpy(terminal_->data_, op_value, sizeof(double)); // Max size
    delete_node(op);
    terminal_->in_[0] = NULL;
  }

  // Success 
  return TILEDB_EXPR_OK;
}

void Expression::purge_op(ExpressionNode* node) {
  // For easy reference
  ExpressionNode* left = node->in_[0];
  ExpressionNode* right = node->in_[1];
  int left_type = left->type_;
  int right_type = right->type_;

  // Both children must be constants
  if(!is_constant(left_type) || !is_constant(right_type)) 
    return;

  // Get result type
  int type = (left_type > right_type) ? left_type : right_type; 

  // Evaluate the value based on the type
  bool purged;
  switch(type) {
    case TILEDB_EXPR_INT32:
      purged = purge_op<int>(node); 
      break;
    case TILEDB_EXPR_INT64:
      purged = purge_op<int64_t>(node); 
      break;
    case TILEDB_EXPR_FLOAT32:
      purged = purge_op<float>(node); 
      break;
    case TILEDB_EXPR_FLOAT64:
      purged = purge_op<double>(node); 
      break;
    default:
      assert(0); // Code should never reach here 
  }

  // If the node has been purged, make it constant and delete its children
  if(purged) {
    node->type_ = type; 
    delete_node(left);
    delete_node(right);
    node->in_[0] = NULL;
    node->in_[1] = NULL;
  }
}

template<class T>
inline
bool Expression::purge_op(ExpressionNode* node) {
  // Get values converted in the coerced type
  T left_value, right_value, op_value;
  get_value<T>(node->in_[0], left_value);
  get_value<T>(node->in_[1], right_value);

  // Evaluate the operator on the coerced values
  if(eval_op(
         node->type_, 
         left_value, 
         right_value, 
         op_value,
         false) == TILEDB_EXPR_OK) {
    // Copy the operator value to the node
    memcpy(node->data_, &op_value, sizeof(T));
    return true;
  } 

  // Not purged
  return false;
}

void Expression::purge_var(
    ExpressionNode* node,
    const void** var_values,
    const int* var_types) {
  // Sanoty check
  assert(node->type_ == TILEDB_EXPR_VAR);

  // If the variable has not been proivided, return
  int var_id = var_ids_[node]; 
  if(var_types[var_id] == TILEDB_EXPR_NULL)
    return;

  // Assign type and value
  node->type_ = var_types[var_id];
  if(node->type_ == TILEDB_EXPR_INT32) 
    memcpy(node->data_, var_values[var_id], sizeof(int));
  else if(node->type_ == TILEDB_EXPR_INT64) 
    memcpy(node->data_, var_values[var_id], sizeof(int64_t));
  else if(node->type_ == TILEDB_EXPR_FLOAT32) 
    memcpy(node->data_, var_values[var_id], sizeof(float));
  else if(node->type_ == TILEDB_EXPR_FLOAT64) 
    memcpy(node->data_, var_values[var_id], sizeof(double));
  else
    assert(0); // The code should never reach here

  // Update variable bookkeeping
  std::map<ExpressionNode*, std::string>::iterator it = var_names_.find(node);
  std::map<ExpressionNode*, std::string>::iterator it_end = var_names_.end();
  if(it != it_end) {
    var_ids_.erase(node);
    var_nodes_.erase(it->second);
    var_names_.erase(node); 
  }
}

int Expression::type(
    const ExpressionNode* node,
    const int* types) const {
  if(node->type_ == TILEDB_EXPR_VAR) // Variable 
    return types[(var_ids_.find((ExpressionNode*) node))->second];
  else if(is_operator(node->type_)) // Operator
    return *((int*)node->data_);
  else                               // Constant
    return node->type_;
}
