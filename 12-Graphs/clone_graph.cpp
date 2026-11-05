class Solution {
public:
    Node* add_or_get_node(Node* node, std::unordered_map<Node*, Node*>& seen){
        auto [it, isNew] = seen.try_emplace(node, nullptr); 
        if(isNew){
            // If node does not exist, create a new one and add
            it->second = new Node(node->val);
        }
        return it->second;
    }

    Node* cloneGraph(Node* node) {
        if(!node) return nullptr;
        std::vector<Node*> nodeVector;
        std::unordered_map<Node*, Node*> seen;
        nodeVector.push_back(node);
        std::size_t read = 0;
        
        while(read < nodeVector.size()){
            Node* origNode = nodeVector[read];
            read++;
            Node* copyNode = add_or_get_node(origNode, seen);
            copyNode->neighbors.reserve(origNode->neighbors.size());

            for(auto& neighborNode : origNode->neighbors){
                // Process neighbor node next in queue if it is not processed already
                if(!seen.contains(neighborNode)){
                    nodeVector.push_back(neighborNode);
                }
                Node* neighborCopyNode = add_or_get_node(neighborNode, seen);
                copyNode->neighbors.push_back(neighborCopyNode);
            }
        }
        return seen.at(node);
    }
};