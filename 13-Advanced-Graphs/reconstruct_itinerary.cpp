class StringNode {
    private:
        std::unordered_map<std::string, int> nodeToIndexMap_;
        std::vector<std::string> indexToNodeVector_;
        int size_;
    public:
        StringNode(std::size_t n){
            nodeToIndexMap_.reserve(n + 1);
            indexToNodeVector_.reserve(n + 1);
            size_ = 0;
        }

        int add_or_get_node(const std::string& node){
            auto [it, isNew] = nodeToIndexMap_.try_emplace(node, size_);
            if(isNew){
                size_++;
                indexToNodeVector_.push_back(node);
            }
            return it->second;
        }

        std::string getNode(int i) const {
            return indexToNodeVector_[i];
        }

        int size() const {
            return size_;
        }


};
class Solution {
public:
    void dfs(const StringNode& stringNodes, const std::vector<int>& offset, std::vector<int>& edges, std::vector<int>& outDegree, int idx, std::vector<std::string>& resultVector){

        // Visit all connected nodes of idx
        for(int i = offset[idx]; i < offset[idx + 1]; i++){
            if(edges[i] != -1){
                int destinationIdx = edges[i];
                edges[i] = -1;
                outDegree[idx]--;
                if(outDegree[destinationIdx] != 0) dfs(stringNodes, offset, edges, outDegree, destinationIdx, resultVector);
                resultVector.push_back(stringNodes.getNode(destinationIdx));
            }
        }
    }

    void sortEdges(const StringNode& stringNodes, const std::vector<int>& offset, std::vector<int>& edges){
        for(int i = 0; i < offset.size() - 1; i++){
            // Sort each node's edges
            
            std::vector<std::pair<std::string, int>> nodeIdxPairVector;
            nodeIdxPairVector.reserve(offset[i + 1] - offset[i]);
            for(int j = offset[i]; j < offset[i + 1]; j++){
                nodeIdxPairVector.emplace_back(stringNodes.getNode(edges[j]), edges[j]);
            }

            std::sort(nodeIdxPairVector.begin(), nodeIdxPairVector.end());
            std::size_t idx = 0;

            for(int j = offset[i]; j < offset[i + 1]; j++){
                auto [_, newIdx] = nodeIdxPairVector[idx++];
                edges[j] = newIdx;
            }
        }
    }

    std::vector<std::string> findItinerary(const std::vector<std::vector<std::string>>& tickets) {
        StringNode stringNodes(tickets.size() + 1);
        std::vector<std::pair<int, int>> edgePairIdxVector;
        edgePairIdxVector.reserve(tickets.size());

        static constexpr std::string ROOT_NODE = "JFK";

        stringNodes.add_or_get_node(ROOT_NODE); // Reserve index '0' for root node 'JFK'

        for(const auto& ticket : tickets){
            int sourceIdx = stringNodes.add_or_get_node(ticket[0]);
            int destinationIdx = stringNodes.add_or_get_node(ticket[1]);
            edgePairIdxVector.emplace_back(sourceIdx, destinationIdx);
        }

        const int n = stringNodes.size();

        std::vector<int> offset(n + 1, 0);
        for(const auto [sourceIdx, _] : edgePairIdxVector){
            offset[sourceIdx]++;
        }

        std::vector<int> outDegree(offset.begin(), offset.end() - 1);

        for(int i = 0; i < n; i++){
            offset[i + 1] += offset[i];
        }

        std::vector<int> edges(tickets.size());
        for(const auto [sourceIdx, destinationIdx] : edgePairIdxVector){
            edges[--offset[sourceIdx]] = destinationIdx;
        }

        sortEdges(stringNodes, offset, edges);

        std::vector<std::string> resultVector;
        resultVector.reserve(tickets.size() + 1);

        dfs(stringNodes, offset, edges, outDegree, 0, resultVector);
        resultVector.push_back(ROOT_NODE);

        return std::vector(resultVector.rbegin(), resultVector.rend());
    }
};