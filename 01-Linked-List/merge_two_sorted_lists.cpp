#include "merge_two_sorted_lists.hpp"

ListNode* mergeTwoLists(ListNode* list1, ListNode* list2){
    ListNode dummyNode;
    ListNode* newList = &dummyNode;

    while(list1 && list2){
        if(list1->val < list2->val){
            newList->next = list1;
            list1 = list1->next;
        } else {
            newList->next = list2;
            list2 = list2->next;
        }
        newList = newList->next;
    }
    newList->next = list1 ? list1 : list2;
    return dummyNode.next;
}