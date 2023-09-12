//给你一个二叉搜索树的根节点 root ，返回 树中任意两不同节点值之间的最小差值 。
//
// 差值是一个正数，其数值等于两值之差的绝对值。
//
//
//
// 示例 1：
//
//
//输入：root = [4,2,6,1,3]
//输出：1
//
//
// 示例 2：
//
//
//输入：root = [1,0,48,null,null,12,49]
//输出：1
//
//
//
//
// 提示：
//
//
// 树中节点的数目范围是 [2, 10⁴]
// 0 <= Node.val <= 10⁵
//
//
//
//
// 注意：本题与 783 https://leetcode-cn.com/problems/minimum-distance-between-bst-
//nodes/ 相同
//
// Related Topics 树 深度优先搜索 广度优先搜索 二叉搜索树 二叉树 👍 501 👎 0

#include "binary_tree_def.h"

#include <algorithm>
#include <vector>
#include <climits>

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
    void dfs(TreeNode* root) {
        if (!root) return;
        dfs(root->left);
        bst_values_.push_back(root->val);
        dfs(root->right);
    }

    int getMinimumDifference(TreeNode* root) {
        dfs(root);
        if (bst_values_.size() < 2) return 0;
        int ans = INT_MAX;
        for (int i = 1; i < bst_values_.size(); ++i) {
            ans = std::min(ans, std::abs(bst_values_[i] - bst_values_[i -1]));
        }

        return ans;
    }

private:
    vector<int> bst_values_;
};

class SolutionI {
public:
    void dfs(TreeNode* root) {
        if (!root) return;
        dfs(root->left);
        if (pre) ans_ = std::min(ans_, root->val - pre->val));
        pre = root;
        dfs(root->right);
    }
    int getMinimumDifference(TreeNode* root) {
        dfs(root);
        return ans_;
    }

private:
    int ans_ = INT_MAX;
    TreeNode* pre = nullptr;
};
