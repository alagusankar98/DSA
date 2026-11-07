bool validTree(int n, const std::vector<std::vector<int>>& edges) {
    std::size_t maxEdges = (static_cast<std::size_t>(n) * (n - 1)) / 2;
    if(edges.size() > maxEdges) return false;

    std::vector<int> offset(n + 1, 0);
    for(const auto& edge : edges){
        assert(edge[0] >= 0 && edge[0] < n && edge[1] >= 0 && edge[1] < n);
        offset[edge[0]]++;
        offset[edge[1]]++;
    }

    for(size_t i = 0; i < n; i++){
        offset[i + 1] += offset[i];
    }

    std::vector<int> edgeVector(2 * edges.size(), 0);
    for(const auto& edge : edges){
        edgeVector[--offset[edge[0]]] = edge[1];
        edgeVector[--offset[edge[1]]] = edge[0];
    }

    std::vector<std::pair<int,int>> bfsVector;
    bfsVector.reserve(n);
    std::unordered_set<int> seen;
    
    bfsVector.emplace_back(0, -1);
    seen.insert(0);
    int read = 0;
    while(read < static_cast<int>(bfsVector.size())){
        const auto [sourceIdx, pushIdx] = bfsVector[read];
        read++;

        for(int i = offset[sourceIdx]; i < offset[sourceIdx + 1]; i++){
            const int connectedNodeIdx = edgeVector[i];
            if(seen.contains(connectedNodeIdx)){
                // We know sourceIdx->connectedIdx connection exists
                // Check if connectedIdx is the same node that pushed sourceIdx here
                if(connectedNodeIdx != pushIdx) return false;
            } else {
                bfsVector.emplace_back(connectedNodeIdx, sourceIdx);
                seen.insert(connectedNodeIdx);
            }
        }
    }
    return (read == n);
}