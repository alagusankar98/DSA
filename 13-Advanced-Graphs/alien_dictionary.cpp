class Solution {
public:
    string alienOrder(vector<string>& words) {
        std::array<std::uint8_t, 26> nodes{};
        int n = 0;
        for(size_t i = 0; i < words.size(); i++){
            for(size_t j = 0; j < words[i].size(); j++){
                if(!nodes[words[i][j] - 'a']){
                    n++;
                    nodes[words[i][j] - 'a'] = 1;
                }
            }
        }

        int edgeCount = 0;
        std::vector<int> inDegree(26, 0);
        std::vector<int> offset(26 + 1, 0);
        for(int i = 1; i < words.size(); i++){
            size_t minSize = std::min(words[i - 1].size(), words[i].size());
            int prevEdgeCount = edgeCount;
            for(size_t j = 0; j < minSize; j++){
                if(words[i][j] == words[i - 1][j]) continue;
                inDegree[words[i][j] - 'a']++;
                offset[words[i-1][j] - 'a']++;
                edgeCount++;
                break;
            }
            if(prevEdgeCount == edgeCount){
                if(words[i - 1].size() > words[i].size()) return "";
            }
        }

        std::string resultString;

        for(int i = 0; i < 26; i++){
            offset[i+1] += offset[i];
            if(nodes[i] && inDegree[i] == 0) resultString.push_back(i + 'a');
        }

        std::vector<int> edges(edgeCount, 0);
        for(int i = 1; i < words.size(); i++){
            size_t minSize = std::min(words[i - 1].size(), words[i].size());
            for(size_t j = 0; j < minSize; j++){
                if(words[i][j] == words[i - 1][j]) continue;
                edges[--offset[words[i-1][j] - 'a']] = words[i][j] - 'a';
                break;
            }
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

        return (n == read) ? resultString : "";
    }
};