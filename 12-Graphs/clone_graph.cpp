class Solution {
private:
    std::pair<Node*,bool> add_or_get_node(Node* node, std::array<Node*, 101>& seen){
        assert(node->val <= 100);
        Node*& slot = seen[node->val];
        bool isNew = !slot;
        if(isNew){
            // If new node, create a copy node and change reference
            slot = new Node(node->val);
        }
        return {slot, isNew};
    }
public:

    Node* cloneGraph(Node* node) {
        if(!node) return nullptr;
        std::vector<Node*> nodeVector;
        std::array<Node*, 101> seen{};
        std::size_t read = 0;

        auto [root, _] = add_or_get_node(node, seen);
        nodeVector.reserve(100);
        nodeVector.push_back(node);
        
        while(read < nodeVector.size()){
            Node* origNode = nodeVector[read];
            read++;
            Node* copyNode = seen[origNode->val];
            copyNode->neighbors.reserve(origNode->neighbors.size());

            for(Node* neighborNode : origNode->neighbors){
                // Process neighbor node next in queue if it is not discovered already
                auto [neighborCopyNode, isNew] = add_or_get_node(neighborNode, seen);
                if(isNew){
                    nodeVector.push_back(neighborNode);
                }
                copyNode->neighbors.push_back(neighborCopyNode);
            }
        }
        return root;
    }
};