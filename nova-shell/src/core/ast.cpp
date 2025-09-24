#include "core/ast.h"

// Implement accept()
void ExecNode::accept(Visitor& v) { v.visit(*this); }

// ========== AST Wrapper Methods ==========
ExecNode* AST::add_exec_node(std::string cmd, std::vector<std::string> args) {
  auto node = std::make_unique<ExecNode>();
  node->command = std::move(cmd);
  node->args = std::move(args);
  return static_cast<ExecNode*>(append_node(std::move(node)));
}

void AST::traverse(Visitor& v) {
  BaseNode* current = head.get();
  while (current) {
    current->accept(v);
    current = current->next.get();
  }
}

bool AST::delete_node(BaseNode* target) {
  if (!head) return false;

  if (head.get() == target) { // deleting head
    head = std::move(head->next);
    return true;
  }

  BaseNode* current = head.get();
  while (current->next) {
    if (current->next.get() == target) {
      current->next = std::move(current->next->next);
      return true;
    }
    current = current->next.get();
  }
  return false;
}

BaseNode* AST::append_node(std::unique_ptr<BaseNode> new_node) {
  if (!head) {
    head = std::move(new_node);
    return head.get();
  }
  BaseNode* current = head.get();
  while (current->next) current = current->next.get();
  current->next = std::move(new_node);
  return current->next.get();
}
