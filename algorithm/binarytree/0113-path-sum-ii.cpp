//给你二叉树的根节点 root 和一个整数目标和 targetSum ，找出所有 从根节点到叶子节点 路径总和等于给定目标和的路径。
//
// 叶子节点 是指没有子节点的节点。
//
//
//
//
//
//
//
// 示例 1：
//
//
//输入：root = [5,4,8,11,null,13,4,7,2,null,null,5,1], targetSum = 22
//输出：[[5,4,11,2],[5,8,4,5]]
//
//
// 示例 2：
//
//
//输入：root = [1,2,3], targetSum = 5
//输出：[]
//
//
// 示例 3：
//
//
//输入：root = [1,2], targetSum = 0
//输出：[]
//
//
//
//
// 提示：
//
//
// 树中节点总数在范围 [0, 5000] 内
// -1000 <= Node.val <= 1000
// -1000 <= targetSum <= 1000
//
//
// Related Topics 树 深度优先搜索 回溯 二叉树 👍 1004 👎 0

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
    vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
        if (!root) return result;
        path.push_back(root->val);
        dfs(root, targetSum - root->val);
        return result;
    }

private:
    vector<vector<int>> result;
    vector<int> path;

    void dfs(TreeNode* root, int sum) {
        if (!root->left && !root->right && sum == 0) {
            result.push_back(path);
            return;
        }

        if (!root->left && !root->right) return;

        if (root->left) {
            path.push_back(root->left->val);
            sum -= root->left->val;
            dfs(root->left, sum);
            sum += root->left->val;
            path.pop_back();
        }

        if (root->right) {
            path.push_back(root->right->val);
            sum -= root->right->val;
            dfs(root->right, sum);
            sum += root->right->val;
            path.pop_back();
        }
    }
};