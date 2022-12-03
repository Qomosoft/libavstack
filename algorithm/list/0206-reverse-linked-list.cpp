//给你单链表的头节点 head ，请你反转链表，并返回反转后的链表。
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
//输入：head = [1,2,3,4,5]
//输出：[5,4,3,2,1]
//
//
// 示例 2：
//
//
//输入：head = [1,2]
//输出：[2,1]
//
//
// 示例 3：
//
//
//输入：head = []
//输出：[]
//
//
//
//
// 提示：
//
//
// 链表中节点的数目范围是 [0, 5000]
// -5000 <= Node.val <= 5000
//
//
//
//
// 进阶：链表可以选用迭代或递归方式完成反转。你能否用两种方法解决这道题？
//
// Related Topics 递归 链表 👍 2870 👎 0

#include <iostream>
#include <string>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(nullptr) {}
};

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
    ListNode* reverseList(ListNode* head) {
        ListNode *prev = nullptr, *cur = head;
        while (cur) {
            ListNode *next = cur->next;
            cur->next = prev;
            prev = cur;
            cur = next;
        }

        return prev;
    }

    friend ostream &operator<<(ostream& out, ListNode *head);
};

ostream &operator<<(ostream &out, ListNode *head) {
    out << '[';
    string comma = "";
    ListNode *cur = head;
    while (cur) {
        out << comma << cur->val;
        comma = ",";
        cur = cur->next;
    }
    out << ']';

    return out;
}

ListNode *GenerateList(const initializer_list<int> &elements) {
    ListNode *head = nullptr, *cur = nullptr;
    for (auto &element : elements) {
        if (head == nullptr) {
            head = new ListNode(element);
            cur = head;
        } else {
            cur->next = new ListNode(element);
            cur = cur->next;
        }
    }

    return head;
}

int main(int argc, const char *argv[])
{
    ListNode *head = GenerateList({1, 2, 3, 4, 5, 6, 7});
    Solution solution;
    cout << head << endl;
    cout << solution.reverseList(head) << endl;
    return 0;
}
