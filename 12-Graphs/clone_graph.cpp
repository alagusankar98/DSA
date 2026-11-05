class Solution {
private:
    std::pair<Node*,bool> add_or_get_node(Node* node, std::unordered_map<Node*, Node*>& seen){
        auto [it, isNew] = seen.try_emplace(node, nullptr); 
        if(isNew){
            // If new node, create a copy node and change reference
            it->second = new Node(node->val);
        }
        return {it->second, isNew};
    }
public:

    Node* cloneGraph(Node* node) {
        if(!node) return nullptr;
        std::vector<Node*> nodeVector;
        std::unordered_map<Node*, Node*> seen;
        std::size_t read = 0;

        auto [root, _] = add_or_get_node(node, seen);
        nodeVector.push_back(node);
        
        while(read < nodeVector.size()){
            Node* origNode = nodeVector[read];
            read++;
            auto [copyNode, _] = add_or_get_node(origNode, seen);
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