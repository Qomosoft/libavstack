//给定两个整数数组 inorder 和 postorder ，其中 inorder 是二叉树的中序遍历， postorder 是同一棵树的后序遍历，请你构造并
//返回这颗 二叉树 。
//
//
//
// 示例 1:
//
//
//输入：inorder = [9,3,15,20,7], postorder = [9,15,7,20,3]
//输出：[3,9,20,null,null,15,7]
//
//
// 示例 2:
//
//
//输入：inorder = [-1], postorder = [-1]
//输出：[-1]
//
//
//
//
// 提示:
//
//
// 1 <= inorder.length <= 3000
// postorder.length == inorder.length
// -3000 <= inorder[i], postorder[i] <= 3000
// inorder 和 postorder 都由 不同 的值组成
// postorder 中每一个值都在 inorder 中
// inorder 保证是树的中序遍历
// postorder 保证是树的后序遍历
//
//
// Related Topics 树 数组 哈希表 分治 二叉树 👍 1057 👎 0

#include "binary_tree_def.h"
#include <vector>
#include <unordered_map>

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
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        if (postorder.empty()) return nullptr;

        int root_value = postorder.back();
        auto* root = new TreeNode(root_value);
        if (postorder.size() == 1) return root;

        int root_index;
        for (root_index = 0; root_index < inorder.size(); ++root_index) {
            if (inorder[root_index] == root_value) break;
        }

        vector<int> left_inorder(inorder.begin(), inorder.begin() + root_index);
        vector<int> right_inorder(inorder.begin() + root_index + 1, inorder.end());

        postorder.resize(postorder.size() - 1);
        vector<int> left_postorder(postorder.begin(), postorder.begin() + left_inorder.size());
        vector<int> right_postorder(postorder.begin() + left_inorder.size(), postorder.end());

        root->left = buildTree(left_inorder, left_postorder);
        root->right = buildTree(right_inorder, right_postorder);

        return root;
    }
};

class SolutionI {
public:
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        if (inorder.empty() || postorder.empty()) return nullptr;
        return dfs(inorder, 0, inorder.size(), postorder, 0, postorder.size());
    }

    TreeNode* dfs(vector<int>& inorder, int inorder_begin, int inorder_end, vector<int>& postorder, int postorder_begin, int postorder_end) {
        if (postorder_begin == postorder_end) return nullptr;

        int root_value = postorder[postorder_end - 1];
        auto* root = new TreeNode(root_value);
        if (postorder_end - postorder_begin == 1) return root;

        int root_index;
        for (root_index = 0; root_index < inorder_end; ++root_index) {
            if (inorder[root_index] == root_value) break;
        }

        int left_inorder_begin = inorder_begin, left_inorder_end = root_index;
        int right_inorder_begin = root_index + 1, right_inorder_end = inorder_end;

        int left_postorder_begin = postorder_begin, left_postorder_end = postorder_begin + (root_index - inorder_begin);
        int right_postorder_begin = postorder_begin + (root_index - inorder_begin), right_postorder_end = postorder_end - 1;

        root->left = dfs(inorder, left_inorder_begin, left_inorder_end, postorder, left_postorder_begin, left_postorder_end);
        root->right = dfs(inorder, right_inorder_begin, right_inorder_end, postorder, right_postorder_begin, right_postorder_end);

        return root;
    }
};