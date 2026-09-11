struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
 };

ListNode* reverseBetween(ListNode* head, int left, int right) {
        ListNode dummyNode;
        dummyNode.next = head;
        ListNode* previousNodeToGroup = &dummyNode;

        // Traverse till (left - 1)
        for(int i = 0; i < (left - 1); i++){
            previousNodeToGroup = previousNodeToGroup->next;
        }
        ListNode* currentGroupHead = previousNodeToGroup->next;

        // Traverse till right
        ListNode* lastNode = currentGroupHead;
        for(int i = 0; i < (right - left); i++){
            lastNode = lastNode->next;
        }

        // Track the next node and sever link
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

        // Update links
        previousNodeToGroup->next = revHead;
        currentGroupHead->next = nextGroupHead;

        return dummyNode.next;
    }