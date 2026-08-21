#include "test_utils.hpp"

// Creates a Linked List from Vector. REMEMBER TO FREE LIST CREATED FROM HERE (ONCE DONE)
ListNode* createListNode(const std::vector<int>& values){
    ListNode dummyNode;
    ListNode* newList = &dummyNode;
    for(const auto val : values){
        newList->next = new ListNode(val);
        newList = newList->next;
    }
    return dummyNode.next;
}

// Converts a Linked List to Vector
std::vector<int> toVector(ListNode* head){
    std::vector<int> result;
    ListNode* current = head;
    while(current){
        result.push_back(current->val);
        current = current->next;
    }
    return result;
}

// Free heap used by Linked list
void freeList(ListNode* head){
    ListNode* current = head;
    while(current){
        ListNode* nodeToDelete = current;
        current = current->next;
        delete nodeToDelete;
    }
}