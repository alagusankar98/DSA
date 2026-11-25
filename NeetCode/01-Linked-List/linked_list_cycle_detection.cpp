#include "linked_list_cycle_detection.hpp"

bool hasCycle(ListNode* head){
    if(!head) return false;

    ListNode* slow = head;
    ListNode* fast = head->next;

    while(fast && fast->next){
        if(fast == slow) return true;
        slow = slow->next;
        fast = fast->next->next;
    }
    return false;
}