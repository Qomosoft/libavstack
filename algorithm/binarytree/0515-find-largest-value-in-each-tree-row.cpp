//给定一棵二叉树的根节点 root ，请找出该二叉树中每一层的最大值。
//
//
//
// 示例1：
//
//
//
//
//输入: root = [1,3,2,5,3,null,9]
//输出: [1,3,9]
//
//
// 示例2：
//
//
//输入: root = [1,2,3]
//输出: [1,3]
//
//
//
//
// 提示：
//
//
// 二叉树的节点个数的范围是 [0,10⁴]
//
// -2³¹ <= Node.val <= 2³¹ - 1
//
//
//
//
// Related Topics 树 深度优先搜索 广度优先搜索 二叉树 👍 304 👎 0

#include "binary_tree_def.h"
#include <vector>
#include <queue>
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
    vector<int> largestValues(TreeNode* root) {
        vector<int> result;
        if (!root) return result;

        queue<TreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            int size = q.size();
            int val = INT_MIN;
            for (int i = 0; i < size; ++i) {
                TreeNode *node = q.front();
                q.pop();
                val = std::max(val, node->val);
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }
            result.push_back(val);
        }

        return result;
    }
};