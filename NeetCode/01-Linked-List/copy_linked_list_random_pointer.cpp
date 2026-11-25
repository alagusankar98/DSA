#include <unordered_map>

class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = nullptr;
        random = nullptr;
    }
};

Node* copyRandomList(Node* head) {
    Node dummyNode(-1);
    Node* newList = &dummyNode;
    std::unordered_map<Node*, Node*> randomMap;
    
    Node* current = head;
    while(current){
        Node* newNode = new Node(current->val);
        randomMap[current] = newNode;
        newList->next = newNode;
        newList = newList->next;
        current = current->next;
    }

    current = head;
    while(current){
        Node* copyNode = randomMap[current];
        if(current->random){
            copyNode->random = randomMap[current->random];
        }
        current = current->next;
    }

    return dummyNode.next;
}