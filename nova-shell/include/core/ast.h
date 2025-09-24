#include <iostream>
#include <memory>
#include <string>
#include <pair>
#include "utils/types.h"


struct Visitor;

// ========== Base Node ==========
struct BaseNode {
  std::unique_ptr<BaseNode> next;
  virtual ~BaseNode() = default;
  virtual void accept(Visitor& v) = 0;
};

// ========== Node Types ==========
struct ExecNode : BaseNode {
  std::string command;
  vec_str args;

  std::unique_ptr<BaseNode> pipe;
  omap_str redirects;

  void accept(Visitor& v) override;
};


// ========== Visitor ==========
struct Visitor {
  virtual void visit(ExecNode& node) = 0;
};

// ========== AST Wrapper ==========
class AST {
  std::unique_ptr<BaseNode> head;
  size_t length { 0 };

public:
  AST() = default;
  ~AST() = default;

  // ========== Node adding methods ==========
  ExecNode* add_exec_node(std::string cmd, std::vector<std::string> args);

  // Traverse with a visitor
  void traverse(Visitor& v);

  // Delete node (by pointer)
  bool delete_node(BaseNode* target);

  // Helper to append a node at the end
  BaseNode* append_node(std::unique_ptr<BaseNode> new_node);
};
