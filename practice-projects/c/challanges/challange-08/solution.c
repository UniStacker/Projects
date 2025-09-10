#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node {
  int value;
  Node* left;
  Node* right;
};

// ===== Function decs =====
Node* create_node(int value);
void _grow_tree(Node* parent, Node* neighborLeft, Node* neighborRight, size_t curr_depth, size_t max_depth);
Node* create_tree(int root_value, size_t depth);
void _print_depth(Node* nodes[], size_t curr_depth, size_t max_depth);
void print_tree(Node* tree, size_t depth);
void _free_tree(Node* tree, Node** visited, int* count);
void free_tree(Node* tree, size_t depth);


int main(int argc, char* argv[]) {
  int root_value = 5;
  size_t depth = 4;

  // Read command-line input (if any)
  if (argc == 3) {
    sscanf(argv[1], "%d", &root_value);
    sscanf(argv[2], "%ld", &depth);
  }

  Node* tree = create_tree(root_value, depth);
  print_tree(tree, depth);

  free_tree(tree, depth);
  return 0;
}

// ===== Function defs =====
Node* create_node(int value) {
  Node* node = (Node*)malloc(sizeof(Node));
  if (!node) {
    perror("Couldn't allocate memory for a node.\n");
    exit(EXIT_FAILURE);
  }

  node->value = value;
  node->left = NULL;
  node->right = NULL;

  return node;
}

// Creating tge tree like structure
void _grow_tree(Node* parent, Node* neighborLeft, Node* neighborRight, size_t curr_depth, size_t max_depth) {
  if (curr_depth == max_depth) return; 

  if (neighborLeft) {
    if (neighborLeft->right) parent->left = neighborLeft->right;
    else parent->left = create_node(parent->value + neighborLeft->value);
  }
  else if (!parent->left) {
    parent->left = create_node(parent->value + curr_depth * 2 - 1);
  }

  if (neighborRight) {
    if (neighborRight->left) parent->right = neighborRight->left;
    else parent->right = create_node(parent->value + neighborRight->value);
  }
  else if (!parent->right) {
    parent->right = create_node(parent->value + curr_depth * 2);
  }

  _grow_tree(parent->left, NULL, parent->right, curr_depth+1, max_depth);
  _grow_tree(parent->right, parent->left, NULL, curr_depth+1, max_depth);
}

Node* create_tree(int root_value, size_t depth) {
  Node* root = create_node(root_value);
  _grow_tree(root, NULL, NULL, 1, depth);
  
  return root;
}

// Printing gave me more trouble than creating the damn graph-like tree
void _print_depth(Node* nodes[], size_t curr_depth, size_t max_depth) {
  if (curr_depth == max_depth+1) return;
  Node* nextNodes[curr_depth + 1];
  int nextNodeIdx = 0;
  
  for (int i=0; nodes[i] != NULL; i++) {
    // Print node
    int indent = (i == 0) ? max_depth - curr_depth++ : 2;
    while (indent--) printf("\t");
    printf("|%d|", nodes[i]->value);

    // Add it's children to the nextNodes array
    if (!(nextNodeIdx-1 >= 0 && nextNodes[nextNodeIdx-1] == nodes[i]->left))
      nextNodes[nextNodeIdx++] = nodes[i]->left;
    nextNodes[nextNodeIdx++] = nodes[i]->right;
  }
  printf("\n");

  // Suffixing a terminator at the end to know where the array ends
  nextNodes[nextNodeIdx] = NULL;

  // Printing the next depth
  _print_depth(nextNodes, curr_depth, max_depth);
}

void print_tree(Node* tree, size_t depth) {
  Node* nodes[] = {tree, NULL};
  _print_depth(nodes, 1, depth);
}


// Freeing was harder than I thought
void _free_tree(Node* tree, Node** visited, int* count) {
  if (tree == NULL) return;

  // Skip if visited
  for (int i = 0; i < *count; i++)
    if (visited[i] == tree) return;

  // Append node to visited
  visited[(*count)++] = tree;

  _free_tree(tree->left, visited, count);
  _free_tree(tree->right, visited, count);

  free(tree);
}

void free_tree(Node* tree, size_t depth) {
  int count = 0;
  int length = (1 << depth) - 1; // 2^depth - 1 (Maximum number of nodes for a binary tree)
  Node** visited = (Node**)malloc(sizeof(Node*[length]));
  if (!visited) {
    perror("Couldn't allocate memory for visited array.\n");
    exit(EXIT_FAILURE);
  }

  _free_tree(tree, visited, &count);
  free(visited);
}
