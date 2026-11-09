// std::vector<std::string> findItinerary(std::vector<std::vector<std::string>>& tickets) {
//     std::unordered_map<std::string, std::vector<std::string>> adjacencyList;
//     for(const auto& ticket : tickets){
//         adjacencyList[ticket[0]].push_back(ticket[1]);
//     }

//     // Sort slices
//     for(auto& [_, destinationNodes] : adjacencyList){
//         std::sort(destinationNodes.rbegin(), destinationNodes.rend());
//     }

//     std::vector<std::string> resultVector;
//     resultVector.reserve(tickets.size() + 1);
    
//     std::stack<std::string> dfsStack;
//     dfsStack.push("JFK");

//     while(!dfsStack.empty()){
//         auto& sourceNode = dfsStack.top();
//         auto& destinationNodes = adjacencyList[sourceNode];

//         if(destinationNodes.empty()){
//             resultVector.push_back(std::move(sourceNode));
//             dfsStack.pop();
//         } else {
//             dfsStack.push(std::move(destinationNodes.back()));
//             destinationNodes.pop_back();
//         }
//     }

//     std::reverse(resultVector.begin(), resultVector.end());

//     return resultVector;
// }

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

    void sortEdges(const std::vector<int>& offset, std::vector<int>& edges, const int n){
        for(int i = 0; i < n; i++){
            // Sort each node's edges based on their slice values (slice values themselves are ordered in increasing order when compared to their string counterparts)
            std::sort(edges.begin() + offset[i], edges.begin() + offset[i + 1]);
        }
    }

    int getIdx(const std::vector<std::string_view>& nodeNames, std::string_view str, const int n){
        
        int left = 0;
        int right = n - 1;

        while(left <= right){
            int mid = left + (right - left) / 2;
            int res = nodeNames[mid].compare(str);
            if(res == 0){
                return mid;
            } else if(res > 0){
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        return -1; // Not found
    }

    std::vector<std::string> findItinerary(const std::vector<std::vector<std::string>>& tickets) {
        std::vector<std::pair<int, int>> edgePairIdxVector;
        edgePairIdxVector.reserve(tickets.size());

        static constexpr std::string_view ROOT_NODE = "JFK";

        std::vector<std::string_view> nodeNames;
        nodeNames.reserve(tickets.size() * 2);
        for(const auto& ticket : tickets){
            for(const auto& node : ticket){
                nodeNames.push_back(node);
            }
        }

        std::sort(nodeNames.begin(), nodeNames.end());
        nodeNames.erase(std::unique(nodeNames.begin(), nodeNames.end()), nodeNames.end());
        const int n = static_cast<int>(nodeNames.size());

        for(const auto& ticket : tickets){
            int sourceIdx = getIdx(nodeNames, ticket[0], n);
            int destinationIdx = getIdx(nodeNames, ticket[1], n);
            edgePairIdxVector.emplace_back(sourceIdx, destinationIdx);
        }

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

        sortEdges(offset, edges, n);

        std::vector<int> resultVector;
        resultVector.reserve(tickets.size() + 1);

        int rootIdx = getIdx(nodeNames, ROOT_NODE, n);
        dfs(offset, edges, cursor, rootIdx, resultVector);
        resultVector.push_back(rootIdx);
        const int numberOfNodes = static_cast<int>(resultVector.size());

        std::vector<std::string> resultStringVector;
        resultStringVector.reserve(tickets.size() + 1);
        for(int i = (numberOfNodes - 1); i >= 0; i--){
            resultStringVector.emplace_back(nodeNames[resultVector[i]]);
        }
        return resultStringVector;
    }
};