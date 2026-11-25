#include "list_node.hpp"

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode dummyNode;
    ListNode* newList = &dummyNode;
    int carry = 0;

    while(carry || l1 || l2){
        int val1 = l1 ? l1->val : 0;
        int val2 = l2 ? l2->val : 0;

        int sum = val1 + val2 + carry;
        newList->next = new ListNode(sum % 10);
        carry = sum / 10;
        newList = newList->next;

        l1 = l1 ? l1->next : l1;
        l2 = l2 ? l2->next : l2;
    }
    return dummyNode.next;
}