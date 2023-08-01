//给定两个整数数组 preorder 和 inorder ，其中 preorder 是二叉树的先序遍历， inorder 是同一棵树的中序遍历，请构造二叉树并
//返回其根节点。
//
//
//
// 示例 1:
//
//
//输入: preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]
//输出: [3,9,20,null,null,15,7]
//
//
// 示例 2:
//
//
//输入: preorder = [-1], inorder = [-1]
//输出: [-1]
//
//
//
//
// 提示:
//
//
// 1 <= preorder.length <= 3000
// inorder.length == preorder.length
// -3000 <= preorder[i], inorder[i] <= 3000
// preorder 和 inorder 均 无重复 元素
// inorder 均出现在 preorder
// preorder 保证 为二叉树的前序遍历序列
// inorder 保证 为二叉树的中序遍历序列
//
//
// Related Topics 树 数组 哈希表 分治 二叉树 👍 2021 👎 0

#include "binary_tree_def.h"
#include <vector>

using namespace std;

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        if (preorder.empty()) return nullptr;

        int root_value = preorder.front();
        auto* root = new TreeNode(root_value);
        if (preorder.size() == 1) return root;

        int root_index;
        for (root_index = 0; root_index < inorder.size(); ++root_index) {
            if (inorder[root_index] == root_value) break;
        }

        vector<int> left_inorder(inorder.begin(), inorder.begin() + root_index);
        vector<int> right_inorder(inorder.begin() + root_index + 1, inorder.end());
        vector<int> left_preorder(preorder.begin() + 1, preorder.begin() + left_inorder.size() + 1);
        vector<int> right_preorder(preorder.begin() + 1 + left_inorder.size(), preorder.end());

        root->left = buildTree(left_preorder, left_inorder);
        root->right = buildTree(right_preorder, right_inorder);

        return root;
    }
};

int main(int argc, const char* argv[]) {
    vector<int> preorder = {3,9,20,15,7}, inorder = {9,3,15,20,7};
    Solution solution;
    auto* root = solution.buildTree(preorder, inorder);
    return 0;
}
