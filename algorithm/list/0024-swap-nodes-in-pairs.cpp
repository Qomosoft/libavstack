//给你一个链表，两两交换其中相邻的节点，并返回交换后链表的头节点。你必须在不修改节点内部的值的情况下完成本题（即，只能进行节点交换）。
//
//
//
// 示例 1：
//
//
//输入：head = [1,2,3,4]
//输出：[2,1,4,3]
//
//
// 示例 2：
//
//
//输入：head = []
//输出：[]
//
//
// 示例 3：
//
//
//输入：head = [1]
//输出：[1]
//
//
//
//
// 提示：
//
//
// 链表中节点的数目在范围 [0, 100] 内
// 0 <= Node.val <= 100
//
//
// Related Topics 递归 链表 👍 1646 👎 0

#include <iostream>
#include <string>
#include <initializer_list>
#include <memory>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

ListNode *GenerateList(const initializer_list<int> &elements) {
    ListNode *dummy_head = new ListNode();
    ListNode *cur = dummy_head;
    for (auto &element : elements) {
        cur->next = new ListNode(element);
        cur = cur->next;
    }

    return dummy_head;
}

ostream &operator <<(ostream &out, ListNode *head) {
    out << '[';
    ListNode *cur = head;
    string comma = "";

    while (cur) {
        out << comma << cur->val;
        cur = cur->next;
        comma = ",";
    }
    out << ']';

    return out;
}

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        unique_ptr<ListNode> dummyHead = make_unique<ListNode>();
        dummyHead->next = head;
        ListNode* cur = dummyHead.get();
        while (cur->next != nullptr && cur->next->next != nullptr) {
            ListNode* node1 = cur->next;
            ListNode* node2 = cur->next->next;
            cur->next = node2;
            node1->next = node2->next;
            node2->next = node1;
            cur = node1;
        }
        return dummyHead->next;
    }
};

int main(int argc, const char *argv[])
{
    ListNode *dummy_head = GenerateList({1, 2, 3, 4, 5, 6});
    Solution solution;
    cout << dummy_head->next << endl;
    cout << solution.swapPairs(dummy_head->next);

    return 0;
}