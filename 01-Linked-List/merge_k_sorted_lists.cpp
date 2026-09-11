#include <vector>
using namespace std;
struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
 };
 
ListNode* sortTwoLists(ListNode* list1, ListNode* list2){
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
    newList->next = list1 ? list1 : list2; // Append the non-empty list

    return dummyNode.next;
}
ListNode* mergeKLists(vector<ListNode*>& lists) {
    if(lists.empty()) return nullptr;
    int n = static_cast<int>(lists.size());
    while(n > 1){
        int inner_sort_size = n / 2;
        for(int i = 0; i < inner_sort_size; i++){
            lists[i] = sortTwoLists(lists[i], lists[n - i - 1]);
        }
        n = (n + 1)/2;
    }
    return lists[0];
}