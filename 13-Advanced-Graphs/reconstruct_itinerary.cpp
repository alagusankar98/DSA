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

        const std::string& getNode(int i) const {
            return indexToNodeVector_[i];
        }

        int size() const {
            return size_;
        }

};
class Solution {
public:
    void dfs(const std::vector<int>& offset, const std::vector<int>& edges, std::vector<int>& cursor, int idx, std::vector<int>& resultVector){

        // Visit all connected nodes of idx
        while(cursor[idx] < offset[idx + 1]){
            int destinationIdx = edges[cursor[idx]++];
            if(cursor[destinationIdx] < offset[destinationIdx + 1]) dfs(offset, edges, cursor, destinationIdx, resultVector);
            resultVector.push_back(destinationIdx);
        }
    }

    void sortEdges(const StringNode& stringNodes, const std::vector<int>& offset, std::vector<int>& edges, const int n){
        for(int i = 0; i < n; i++){
            // Sort each node's edges based on their string values
            std::sort(edges.begin() + offset[i], edges.begin() + offset[i + 1], 
            [&](const int a, const int b){
                return stringNodes.getNode(a) < stringNodes.getNode(b);
            });
        }
    }

    std::vector<std::string> findItinerary(const std::vector<std::vector<std::string>>& tickets) {
        StringNode stringNodes(tickets.size() + 1);
        std::vector<std::pair<int, int>> edgePairIdxVector;
        edgePairIdxVector.reserve(tickets.size());

        static constexpr std::string_view ROOT_NODE = "JFK";

        stringNodes.add_or_get_node(std::string(ROOT_NODE)); // Reserve index '0' for root node 'JFK'

        for(const auto& ticket : tickets){
            int sourceIdx = stringNodes.add_or_get_node(ticket[0]);
            int destinationIdx = stringNodes.add_or_get_node(ticket[1]);
            edgePairIdxVector.emplace_back(sourceIdx, destinationIdx);
        }

        const int n = stringNodes.size();

        std::vector<int> offset(n + 1, 0);
        for(const auto& [sourceIdx, _] : edgePairIdxVector){
            offset[sourceIdx]++;
        }

        for(int i = 0; i < n; i++){
            offset[i + 1] += offset[i];
        }

        std::vector<int> edges(tickets.size());
        for(const auto& [sourceIdx, destinationIdx] : edgePairIdxVector){
            edges[--offset[sourceIdx]] = destinationIdx;
        }

        std::vector<int> cursor(offset.begin(), offset.end() - 1);

        sortEdges(stringNodes, offset, edges, n);

        std::vector<int> resultVector;
        resultVector.reserve(tickets.size() + 1);

        dfs(offset, edges, cursor, 0, resultVector);
        resultVector.push_back(0);
        const int numberOfNodes = static_cast<int>(resultVector.size());

        std::vector<std::string> resultStringVector;
        resultStringVector.reserve(tickets.size() + 1);
        for(int i = (numberOfNodes - 1); i >= 0; i--){
            resultStringVector.push_back(stringNodes.getNode(resultVector[i]));
        }
        return resultStringVector;
    }
};