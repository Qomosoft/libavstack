//给你一个含重复值的二叉搜索树（BST）的根节点 root ，找出并返回 BST 中的所有 众数（即，出现频率最高的元素）。
//
// 如果树中有不止一个众数，可以按 任意顺序 返回。
//
// 假定 BST 满足如下定义：
//
//
// 结点左子树中所含节点的值 小于等于 当前节点的值
// 结点右子树中所含节点的值 大于等于 当前节点的值
// 左子树和右子树都是二叉搜索树
//
//
//
//
// 示例 1：
//
//
//输入：root = [1,null,2,2]
//输出：[2]
//
//
// 示例 2：
//
//
//输入：root = [0]
//输出：[0]
//
//
//
//
// 提示：
//
//
// 树中节点的数目在范围 [1, 10⁴] 内
// -10⁵ <= Node.val <= 10⁵
//
//
//
//
// 进阶：你可以不使用额外的空间吗？（假设由递归产生的隐式调用栈的开销不被计算在内）
//
// Related Topics 树 深度优先搜索 二叉搜索树 二叉树 👍 687 👎 0

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
    vector<int> findMode(TreeNode* root) {
        dfs(root);
        for (auto& item : count_) {
            if (max_count_ == item.second) {
                result_.push_back(item.first);
            }
        }

        return result_;
    }

    void dfs(TreeNode* root) {
        if (!root) return;
        count_[root->val]++;
        max_count_ = std::max(max_count_, count_[root->val]);
        dfs(root->left);
        dfs(root->right);
    }

private:
    vector<int> result_;
    int max_count_ = 0;
    unordered_map<int, int> count_;
};
