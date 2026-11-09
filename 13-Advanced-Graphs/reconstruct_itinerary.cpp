class StringNode {
    private:
        std::unordered_map<std::string, int> nodeToIndexMap_;
        std::vector<std::string> indexToNodeVector_;
        int size_;
    public:
        StringNode(std::size_t n){
            nodeToIndexMap_.reserve(n + 1);
            indexToNodeVector_
            size_ = 0;
        }

        int add_or_get_node(const& std::string node){
            auto [it, isNew] = nodeToIndexMap_.try_emplace(node, size_);
            if(isNew) size_++;
            return it->second;
        }

        int size() const {
            return size_;
        }


};
class Solution {
public:
    std::vector<std::string> findItinerary(const std::vector<std::vector<std::string>>& tickets) {
        StringNode stringNodes(tickets.size() + 1);
        std::vector<std::pair<int, int>> edgePairIdxVector;
        edgePairIdxVector.reserve(tickets.size());

        stringNodes.add_or_get_node("JFK"); // Reserve index '0' for root node 'JFK'

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

        for(int i = 0; i < n; i++){
            offset[i + 1] += offset[i];
        }

        std::vector<int> edges(tickets.size());
        for(const auto [sourceIdx, destinationIdx] : edgePairIdxVector){
            edges[--offset[sourceIdx]] = destinationIdx;
        }


        std::vector<std::string> resultVector;
        resultVector.reserve(tickets.size() + 1);

        return resultVector;
    }
};