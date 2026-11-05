/*
// Definition for a Node.
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node() {
        val = 0;
        neighbors = vector<Node*>();
    }
    Node(int _val) {
        val = _val;
        neighbors = vector<Node*>();
    }
    Node(int _val, vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
};
*/

class Solution {
private:
    std::unordered_map<Node*, Node*> originalToCopyNodeMap_;
public:
    Node* add_or_get_node(Node* node){
        if(auto it = originalToCopyNodeMap_.find(node); it != originalToCopyNodeMap_.end()){
            return it->second;
        }

        // If node does not exist, create a new one and add
        Node* newNode = new Node(node->val);
        originalToCopyNodeMap_[node] = newNode;
        return newNode;
    }

    Node* cloneGraph(Node* node) {
        if(!node) return nullptr;
        std::queue<Node*> nodeQueue;
        std::unordered_set<Node*> seen;

        Node* root = add_or_get_node(node);
        nodeQueue.push(node);
        seen.insert(node);
        
        while(!nodeQueue.empty()){
            Node* origNode = nodeQueue.front(); nodeQueue.pop();
            Node* copyNode = add_or_get_node(origNode);
            seen.insert(origNode);
            copyNode->neighbors.reserve(origNode->neighbors.size());

            for(auto& neighborNode : origNode->neighbors){
                // Process neighbor node next in queue if it is not processed already
                if(!seen.contains(neighborNode)){
                    nodeQueue.push(neighborNode);
                    seen.insert(neighborNode);
                }
                Node* neighborCopyNode = add_or_get_node(neighborNode);
                copyNode->neighbors.push_back(neighborCopyNode);
            }
        }
        return root;
    }
};