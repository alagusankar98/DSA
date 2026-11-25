#include "list_node.hpp"

ListNode* removeNthFromEnd(ListNode* head, int n) {
    if(!head) return head;
    ListNode dummyNode;
    dummyNode.next = head;
    ListNode* current = head;

    // Points to (n + 1)th node from start
    for(int i = 0; i < n; ++i){
        if(current){
            current = current->next;
        } else {
            // Bruh, length of linked list < n
            return head;
        }
    }

    ListNode* prevNode = &dummyNode;
    // current goes (l-n-1) steps further to reach nullptr
    // first node becomes node previous to be deleted.
    while(current){
        prevNode = prevNode->next;
        current = current->next;
    }

    ListNode* nodeToDelete = prevNode->next;

    prevNode->next = nodeToDelete->next;
    delete nodeToDelete;

    return dummyNode.next;
}