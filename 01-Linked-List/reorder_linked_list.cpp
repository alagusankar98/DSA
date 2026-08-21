#include "list_node.hpp"

void reorderList(ListNode* head) {
    if(!head ||!head->next) return;

    ListNode* middleNode = head;
    ListNode* fast = head->next;

    while(fast && fast->next){
        middleNode = middleNode->next;
        fast = fast->next->next;
    }

    // First half of list is now >= Second Half because of changed initialization
    // 'middleNode' points to exactly floor(n/2)th node
    ListNode* secondHead = middleNode->next;
    middleNode->next = nullptr;

    // Reverse from middle node
    ListNode* revHead = nullptr;
    while(secondHead){
        auto next = secondHead->next;
        secondHead->next = revHead;
        revHead = secondHead;
        secondHead = next;
    }

    // Interweave head and revHead (head has equal or one element greater)
    ListNode dummyNode;
    ListNode* newList = &dummyNode;
    while(revHead){
        ListNode* firstNext = head->next;
        ListNode* secondNext = revHead->next;

        head->next = revHead;
        revHead->next = firstNext;

        head = firstNext;
        revHead = secondNext;
    }
    newList->next = head;
    head = dummyNode.next;
}