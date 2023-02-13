#pragma once

#include <iostream>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

void PrintBinaryTree(const std::string &prefix, const TreeNode *node, bool is_left) {
    if(!node) return;
    std::cout << prefix;
    std::cout << (is_left ? "|--" : "L--" );

    // print the value of the node
    std::cout << node->val << std::endl;

    // enter the next tree level - left and right branch
    PrintBinaryTree(prefix + (is_left ? "|   " : "    "), node->left, true);
    PrintBinaryTree(prefix + (is_left ? "|   " : "    "), node->right, false);
}

void PrintBinaryTree(const TreeNode *node) {
    PrintBinaryTree("", node, false);
}

