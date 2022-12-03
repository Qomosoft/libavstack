//设计链表的实现。您可以选择使用单链表或双链表。单链表中的节点应该具有两个属性：val 和 next。val 是当前节点的值，next 是指向下一个节点的指针
///引用。如果要使用双向链表，则还需要一个属性 prev 以指示链表中的上一个节点。假设链表中的所有节点都是 0-index 的。
//
// 在链表类中实现这些功能：
//
//
// get(index)：获取链表中第 index 个节点的值。如果索引无效，则返回-1。
// addAtHead(val)：在链表的第一个元素之前添加一个值为 val 的节点。插入后，新节点将成为链表的第一个节点。
// addAtTail(val)：将值为 val 的节点追加到链表的最后一个元素。
// addAtIndex(index,val)：在链表中的第 index 个节点之前添加值为 val 的节点。如果 index 等于链表的长度，则该节点将附加
//到链表的末尾。如果 index 大于链表长度，则不会插入节点。如果index小于0，则在头部插入节点。
// deleteAtIndex(index)：如果索引 index 有效，则删除链表中的第 index 个节点。
//
//
//
//
// 示例：
//
//
//MyLinkedList linkedList = new MyLinkedList();
//linkedList.addAtHead(1);
//linkedList.addAtTail(3);
//linkedList.addAtIndex(1,2);   //链表变为1-> 2-> 3
//linkedList.get(1);            //返回2
//linkedList.deleteAtIndex(1);  //现在链表是1-> 3
//linkedList.get(1);            //返回3
//
//
//
//
// 提示：
//
//
// 0 <= index, val <= 1000
// 请不要使用内置的 LinkedList 库。
// get, addAtHead, addAtTail, addAtIndex 和 deleteAtIndex 的操作次数不超过 2000。
//
//
// Related Topics 设计 链表 👍 694 👎 0

#include <iostream>
#include <memory>
#include <algorithm>
#include <string>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(nullptr) {}
};


class MyLinkedList {
public:
    MyLinkedList() : size_(0) {
        head_ = new ListNode();
    }

    MyLinkedList(const initializer_list<int> &elements) {
        head_ = new ListNode();
        ListNode *cur = head_;
        for (auto &element : elements) {
            cur->next = new ListNode(element);
            cur = cur->next;
            size_++;
        }
    }

    virtual ~MyLinkedList() {
        ListNode *cur = head_;
        while (cur) {
            ListNode *next = cur->next;
            delete cur;
            cur = next;
        }
        head_ = nullptr;
        size_ = 0;
    }

    int get(int index) {
        if (index < 0 || index >= size_) {
            return -1;
        }
        ListNode *cur = head_;
        for (int i = 0; i <= index; i++) {
            cur = cur->next;
        }
        return cur->val;
    }

    void addAtHead(int val) {
        addAtIndex(0, val);
    }

    void addAtTail(int val) {
        addAtIndex(size_, val);
    }

    void addAtIndex(int index, int val) {
        if (index > size_) {
            return;
        }
        index = max(0, index);
        size_++;
        ListNode *pre = head_;
        for (int i = 0; i < index; i++) {
            pre = pre->next;
        }
        ListNode *cur = new ListNode(val);
        cur->next = pre->next;
        pre->next = cur;
    }

    void deleteAtIndex(int index) {
        if (index < 0 || index >= size_) {
            return;
        }
        size_--;
        ListNode *pre = head_;
        for (int i = 0; i < index; i++) {
            pre = pre->next;
        }
        ListNode *cur = pre->next;
        pre->next = pre->next->next;
        delete cur;
    }
private:
    int size_;
    ListNode *head_;

public:
    friend ostream &operator<<(ostream &out, const MyLinkedList &list);
};

ostream &operator<<(ostream &out, const MyLinkedList &list) {
    out << '[';
    string comma = "";
    ListNode *cur = list.head_->next;
    while (cur) {
        out << comma << cur->val;
        comma = ",";
        cur = cur->next;
    }
    out << ']';
    return out;
}

/**
 * Your MyLinkedList object will be instantiated and called as such:
 * MyLinkedList* obj = new MyLinkedList();
 * int param_1 = obj->get(index);
 * obj->addAtHead(val);
 * obj->addAtTail(val);
 * obj->addAtIndex(index,val);
 * obj->deleteAtIndex(index);
 */

int main(int argc, const char *argv[])
{
//    MyLinkedList list = {1, 2, 3, 4, 5, 6, 7};
    MyLinkedList list;
    list.addAtHead(2);
    cout << list << endl;
    list.addAtHead(1);
    cout << list << endl;
    list.addAtTail(3);
    cout << list << endl;
    list.addAtIndex(3, 4);
    cout << list << endl;
    return 0;
}