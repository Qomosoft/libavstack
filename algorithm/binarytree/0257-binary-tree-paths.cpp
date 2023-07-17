//给你一个二叉树的根节点 root ，按 任意顺序 ，返回所有从根节点到叶子节点的路径。
//
// 叶子节点 是指没有子节点的节点。
//
// 示例 1：
//
//
//输入：root = [1,2,3,null,5]
//输出：["1->2->5","1->3"]
//
//
// 示例 2：
//
//
//输入：root = [1]
//输出：["1"]
//
//
//
//
// 提示：
//
//
// 树中节点的数目在范围 [1, 100] 内
// -100 <= Node.val <= 100
//
//
// Related Topics 树 深度优先搜索 字符串 回溯 二叉树 👍 974 👎 0

#include "binary_tree_def.h"
#include <string>
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
    vector<string> binaryTreePaths(TreeNode* root) {
        vector<string> result;
        vector<int> path;
        if (!root) return result;
        traversal(root, path, result);
        return result;
    }

    void traversal(TreeNode* cur, vector<int>& path, vector<string>& result) {
        path.push_back(cur->val);
        //叶子节点
        if (cur->left == nullptr && cur->right == nullptr) {
            string cur_path;
            for (int i = 0; i < path.size() - 1; ++i) {
                cur_path += to_string(path[i]);
                cur_path += "->";
            }
            cur_path += to_string(path.back());
            result.push_back(cur_path);
            return;
        }

        if (cur->left) { // left
            traversal(cur->left, path, result);
            path.pop_back(); // backtrack
        }

        if (cur->right) { // right
            traversal(cur->right, path, result);
            path.pop_back(); // backtrack
        }
    }
};

class SolutionI {
public:
    vector<string> binaryTreePaths(TreeNode* root) {
        vector<string> result;
        string path;
        if (!root) return result;
        traversal(root, path, result);

        return result;
    }

    void traversal(TreeNode* cur, string path, vector<string>& result) {
        path += to_string(cur->val);
        if (cur->left == nullptr && cur->right == nullptr) {
            result.push_back(path);
            return;
        }

        if (cur->left) traversal(cur->left, path + "->", result);
        if (cur->right) traversal(cur->right, path + "->", result);
    }
};