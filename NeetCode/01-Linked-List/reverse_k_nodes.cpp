struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
 };

ListNode* reverseKGroup(ListNode* head, int k) {
    ListNode dummyNode;
    dummyNode.next = head;
    ListNode* prevNodeToCurrentGroup = &dummyNode;
    while(true){
        // Nodes to track:
        // 1. Previous Node to current group
        // 2. Current group's head
        // 3. Last Node of current group (Reverse Head)
        // 4. Next Group's head
        ListNode* currentGroupHead = prevNodeToCurrentGroup->next;
        ListNode* lastNode = prevNodeToCurrentGroup;
        for(int i = 0; (i < k) && lastNode; i++){
            lastNode = lastNode->next;
        }

        if (!lastNode) break; // Not enough nodes to reverse

        // Links to update:
        // 1. Track next group's head and Sever last node of current group
        // 2. Attach previous node to current group with reversed head (after reversal)
        // 3. Attach current group head to next group head (after reversal)
        // 4. Update previous node to current group with current group head before end of loop
        ListNode* nextGroupHead = lastNode->next;
        lastNode->next = nullptr;

        ListNode* revHead = nullptr;
        ListNode* current = currentGroupHead;
        while(current){
            ListNode* next = current->next;
            current->next = revHead;
            revHead = current;
            current = next;
        }

        prevNodeToCurrentGroup->next = revHead;
        currentGroupHead->next = nextGroupHead;
        prevNodeToCurrentGroup = currentGroupHead;
    }

    return dummyNode.next;
}