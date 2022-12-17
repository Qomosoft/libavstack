//给定一个链表的头节点 head ，返回链表开始入环的第一个节点。 如果链表无环，则返回 null。
//
// 如果链表中有某个节点，可以通过连续跟踪 next 指针再次到达，则链表中存在环。 为了表示给定链表中的环，评测系统内部使用整数 pos 来表示链表尾连接到
//链表中的位置（索引从 0 开始）。如果 pos 是 -1，则在该链表中没有环。注意：pos 不作为参数进行传递，仅仅是为了标识链表的实际情况。
//
// 不允许修改 链表。
//
//
//
//
//
//
// 示例 1：
//
//
//
//
//输入：head = [3,2,0,-4], pos = 1
//输出：返回索引为 1 的链表节点
//解释：链表中有一个环，其尾部连接到第二个节点。
//
//
// 示例 2：
//
//
//
//
//输入：head = [1,2], pos = 0
//输出：返回索引为 0 的链表节点
//解释：链表中有一个环，其尾部连接到第一个节点。
//
//
// 示例 3：
//
//
//
//
//输入：head = [1], pos = -1
//输出：返回 null
//解释：链表中没有环。
//
//
//
//
// 提示：
//
//
// 链表中节点的数目范围在范围 [0, 10⁴] 内
// -10⁵ <= Node.val <= 10⁵
// pos 的值为 -1 或者链表中的一个有效索引
//
//
//
//
// 进阶：你是否可以使用 O(1) 空间解决此题？
//
// Related Topics 哈希表 链表 双指针 👍 1881 👎 0

#include <iostream>
#include <initializer_list>
#include <string>
#include <unordered_set>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

//leetcode submit region begin(Prohibit modification and deletion)
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        unordered_set<ListNode*> visited;
        ListNode *cur = head;

        while (cur) {
            if (visited.count(cur)) {
                return cur;
            }
            visited.insert(cur);
            cur = cur->next;
        }

        return nullptr;
    }

    ListNode *detectCycleI(ListNode *head) {
        ListNode *slow = head, *fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) {
                ListNode *cur = head;
                while (cur != slow) {
                    cur = cur->next;
                    slow = slow->next;
                }

                return cur;
            }
        }

        return nullptr;
    }
};
//leetcode submit region end(Prohibit modification and deletion)

ListNode *GenerateCycleList(const initializer_list<int> &elements) {
    ListNode *dummy_head = new ListNode();
    ListNode *cur = dummy_head;
    for (auto &element : elements) {
        cur->next = new ListNode(element);
        cur = cur->next;
    }

    cur->next = dummy_head->next->next;

    return dummy_head;
}

ostream &operator<<(ostream &out, ListNode *head) {
    out << '[';
    ListNode *cur = head;
    string comma = "";

    while (cur) {
        out << comma << cur->val;
        comma = ",";
        cur = cur->next;
    }
    out << ']';

    return out;
}


int main(int argc, const char* argv[])
{
    ListNode *dummy_head = GenerateCycleList({1, 2, 3, 4, 5, 6, 7});
//    cout << dummy_head->next;
    Solution solution;
//    ListNode *ans = solution.detectCycle(dummy_head->next);
    ListNode *ans = solution.detectCycleI(dummy_head->next);
    cout << ans->val << endl;
    return 0;
}
