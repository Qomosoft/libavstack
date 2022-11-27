//给你一个链表的头节点 head 和一个整数 val ，请你删除链表中所有满足 Node.val == val 的节点，并返回 新的头节点 。
//
//
//
// 示例 1：
//
//
//输入：head = [1,2,6,3,4,5,6], val = 6
//输出：[1,2,3,4,5]
//
//
// 示例 2：
//
//
//输入：head = [], val = 1
//输出：[]
//
//
// 示例 3：
//
//
//输入：head = [7,7,7,7], val = 7
//输出：[]
//
//
//
//
// 提示：
//
//
// 列表中的节点数目在范围 [0, 10⁴] 内
// 1 <= Node.val <= 50
// 0 <= val <= 50
//
//
// Related Topics 递归 链表 👍 1093 👎 0

#include <iostream>
#include <initializer_list>
#include <iterator>
#include <algorithm>
#include <string>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class List {
public:
    List(const initializer_list<int> &elements) : header_(nullptr) {
        ListNode *cur = nullptr;

        for (const auto &i : elements) {
            if (header_ == nullptr) {
                header_ = new ListNode(i);
                cur = header_;
                continue;
            }

            cur->next = new ListNode(i);
            cur = cur->next;
        }
    }

    virtual ~List() {
        ListNode *cur = header_->next;
        while (cur) {
            ListNode *next = cur->next;
            delete cur;
            cur = next;
        }
        delete header_;
        header_ = nullptr;
    }

    ListNode *GetHeader() { return header_; }

    friend ostream &operator<<(ostream &out, const List &l);

private:
    ListNode *header_;
};

ostream &operator<<(ostream &out, const List &l) {
    if (!l.header_) {
        out << "[]";
        return out;
    }

    cout << '[';
    ListNode *cur = l.header_;
    string comma = "";
    while (cur) {
        out << comma << cur->val;
        cur = cur->next;
        comma = ",";
    }
    out << ']';

    return out;
}

//leetcode submit region begin(Prohibit modification and deletion)
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
    ListNode* removeElements(ListNode* head, int val) {
        ListNode* dummyHead = new ListNode(0, head);
        ListNode* cur = dummyHead;
        while (cur->next) {
            if (cur->next->val == val) {
                ListNode *next = cur->next;
                cur->next = cur->next->next;
                delete exchange(next, nullptr);
            } else {
                cur = cur->next;
            }
        }
        return dummyHead->next;
    }
};

//leetcode submit region end(Prohibit modification and deletion)

int main(int argc, const char *argv[])
{
    List list = {1, 2, 3, 4, 5, 6, 7};
    Solution solution;
    solution.removeElements(list.GetHeader(), 4);
    cout << list << endl;
    return 0;
}
