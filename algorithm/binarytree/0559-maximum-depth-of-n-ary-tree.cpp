//给定一个 N 叉树，找到其最大深度。
//
// 最大深度是指从根节点到最远叶子节点的最长路径上的节点总数。
//
// N 叉树输入按层序遍历序列化表示，每组子节点由空值分隔（请参见示例）。
//
//
//
// 示例 1：
//
//
//
//
//输入：root = [1,null,3,2,4,null,5,6]
//输出：3
//
//
// 示例 2：
//
//
//
//
//输入：root = [1,null,2,3,4,5,null,null,6,7,null,8,null,9,10,null,null,11,null,12,
//null,13,null,null,14]
//输出：5
//
//
//
//
// 提示：
//
//
// 树的深度不会超过 1000 。
// 树的节点数目位于 [0, 10⁴] 之间。
//
//
// Related Topics 树 深度优先搜索 广度优先搜索 👍 334 👎 0


/*
// Definition for a Node.
class Node {
public:
    int val;
    vector<Node*> children;

    Node() {}

    Node(int _val) {
        val = _val;
    }

    Node(int _val, vector<Node*> _children) {
        val = _val;
        children = _children;
    }
};
*/

#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

class Node {
public:
    int val;
    vector<Node*> children;

    Node() {}
    Node(int val) : val(val) {}
    Node(int val, vector<Node*> children) : val(val), children(children) {}
};

class Solution {
public:
    int maxDepth(Node* root) {
        if (!root) return 0;
        int ans = 0;
        for (auto node : root->children) {
            ans = std::max(ans, maxDepth(node));
        }

        return ans + 1;
    }
};

class SolutionI {
public:
    int maxDepth(Node* root) {
        int depth = 0;
        if (!root) return depth;
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            int size = q.size();
            ++depth;
            for (int i = 0; i < size; ++i) {
                Node *node = q.front();
                q.pop();
                for (int j = 0; j < node->children.size(); ++j) {
                    if (node->children[j]) q.push(node->children[j]);
                }
            }
        }

        return depth;
    }
};
