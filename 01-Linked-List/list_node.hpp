#ifndef LIST_NODE_HPP
#define LIST_NODE_HPP

class ListNode{
    public:
        int val;
        ListNode* next;
        ListNode() : val(0), next(nullptr) {}
        ListNode(int x) : val(x), next(nullptr) {}
        ListNode(int x, ListNode* next) : val(x), next(next) {}
};

#endif