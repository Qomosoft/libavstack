//给你一个链表，删除链表的倒数第 n 个结点，并且返回链表的头结点。
//
//
//
// 示例 1：
//
//
//输入：head = [1,2,3,4,5], n = 2
//输出：[1,2,3,5]
//
//
// 示例 2：
//
//
//输入：head = [1], n = 1
//输出：[]
//
//
// 示例 3：
//
//
//输入：head = [1,2], n = 1
//输出：[1]
//
//
//
//
// 提示：
//
//
// 链表中结点的数目为 sz
// 1 <= sz <= 30
// 0 <= Node.val <= 100
// 1 <= n <= sz
//
//
//
//
// 进阶：你能尝试使用一趟扫描实现吗？
//
// Related Topics 链表 双指针 👍 2335 👎 0

#include <iostream>
#include <string>
#include <memory>
#include <initializer_list>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
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
    // recursive
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if (!head) return nullptr;
        head->next = removeNthFromEnd(head->next, n);
        if (cur_++ == n) return head->next;
        return head;
    }

    ListNode* removeNthFromEndI(ListNode* head, int n) {
        unique_ptr<ListNode> dummy_head = make_unique<ListNode>(0, head);
        ListNode* fast = head;
        ListNode* slow = dummy_head.get();
        for (int i = 0; i < n; ++i) {
            fast = fast->next;
        }
        while (fast) {
            fast = fast->next;
            slow = slow->next;
        }
        ListNode *node = slow->next;
        slow->next = slow->next->next;
        delete node;

        return dummy_head->next;
    }

private:
    int cur_ = 0;
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

int main(int argc, const char *argv[])
{
    ListNode *dummy_head = GenerateList({1, 2, 3, 4, 5, 6});
    cout << dummy_head->next << endl;
    Solution solution;
//    cout << solution.removeNthFromEnd(dummy_head->next, 2);
    cout << solution.removeNthFromEndI(dummy_head->next, 2);
    return 0;
}
