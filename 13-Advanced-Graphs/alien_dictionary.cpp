class Solution {
public:
    string alienOrder(vector<string>& words) {
        std::array<std::uint8_t, 26> nodes{};
        int n = 0;
        std::array<int, 26> inDegree{};
        std::array<int, 27> offset{};
        std::vector<std::pair<int, int>> edgePairs;
        edgePairs.reserve(words.size() - 1);
        for(size_t i = 0; i < words.size(); i++){
            for(size_t j = 0; j < words[i].size(); j++){
                if(!nodes[words[i][j] - 'a']){
                    n++;
                    nodes[words[i][j] - 'a'] = 1;
                }
            }

            if(i < 1) continue; // Start after having one pair

            size_t minSize = std::min(words[i - 1].size(), words[i].size());
            // Stop at first non-equal index
            size_t j = 0;
            while(j < minSize && ((words[i][j] == words[i - 1][j]))) j++;

            if(j == minSize) {
                // Loop exited with all letters as same till equal indices
                if(words[i - 1].size() > words[i].size()) return ""; // Wrong order ["abc", "ab"] case
                continue; // Else no edge, move on to next pair
            }

            // Use edge
            inDegree[words[i][j] - 'a']++;
            offset[words[i-1][j] - 'a']++;
            edgePairs.emplace_back(words[i-1][j] - 'a', words[i][j] - 'a');
        }

        std::string resultString;
        resultString.reserve(n);

        for(int i = 0; i < 26; i++){
            offset[i+1] += offset[i];
            if(nodes[i] && inDegree[i] == 0) resultString.push_back(i + 'a');
        }

        std::vector<int> edges(edgePairs.size(), 0);
        for(const auto& [sourceNode, destinationNode] : edgePairs){
            edges[--offset[sourceNode]] = destinationNode;
        }

        int read = 0;
        while(read < static_cast<int>(resultString.size())){
            auto currentNode = resultString[read] - 'a';
            read++;

            for(int i = offset[currentNode]; i < offset[currentNode + 1]; i++){
                // Access all neighbors of current node
                int neighborNode = edges[i];
                if(--inDegree[neighborNode] == 0) resultString.push_back(neighborNode + 'a');
            }
        }

        if (n != read) return "";

        return resultString;
    }
};