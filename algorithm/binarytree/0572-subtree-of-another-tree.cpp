//
//
// 给你两棵二叉树 root 和 subRoot 。检验 root 中是否包含和 subRoot 具有相同结构和节点值的子树。如果存在，返回 true ；否则
//，返回 false 。
//
//
//
// 二叉树 tree 的一棵子树包括 tree 的某个节点和这个节点的所有后代节点。tree 也可以看做它自身的一棵子树。
//
//
//
// 示例 1：
//
//
//输入：root = [3,4,5,1,2], subRoot = [4,1,2]
//输出：true
//
//
// 示例 2：
//
//
//输入：root = [3,4,5,1,2,null,null,null,null,0], subRoot = [4,1,2]
//输出：false
//
//
//
//
// 提示：
//
//
// root 树上的节点数量范围是 [1, 2000]
// subRoot 树上的节点数量范围是 [1, 1000]
// -10⁴ <= root.val <= 10⁴
// -10⁴ <= subRoot.val <= 10⁴
//
//
// Related Topics 树 深度优先搜索 二叉树 字符串匹配 哈希函数 👍 888 👎 0

#include "binary_tree_def.h"

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
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        if (root == nullptr && subRoot == nullptr) return true;
        else if (root != nullptr && subRoot == nullptr) return false;
        else if (root == nullptr && subRoot != nullptr) return false;
        else if (check(root, subRoot)) return true;
        else return isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
    }

    bool check(TreeNode *p, TreeNode *q) {
        if (p == nullptr && q == nullptr) return true;
        else if (p != nullptr && q == nullptr) return false;
        else if (p == nullptr && q != nullptr) return false;
        else return p->val == q->val && check(p->left, q->left) && check(p->right, q->right);
    }
};