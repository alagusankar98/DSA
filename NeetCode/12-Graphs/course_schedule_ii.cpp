std::vector<int> findOrder(int numCourses, const std::vector<std::vector<int>>& prerequisites) {
    std::vector<int> inDegree(numCourses, 0);
    std::vector<int> offset(numCourses + 1, 0);

    for(const auto& prerequisite : prerequisites){
        const int sourceNode = prerequisite[1];
        const int destinationNode = prerequisite[0];
        assert(sourceNode >= 0 && sourceNode < numCourses && destinationNode >= 0 && destinationNode < numCourses);
        inDegree[destinationNode]++;
        offset[sourceNode]++;
    }

    std::vector<int> bfsVector;
    bfsVector.reserve(numCourses);
    for(int i = 0; i < numCourses; i++){
        if(inDegree[i] == 0) bfsVector.push_back(i);
        offset[i + 1] += offset[i];
    }

    std::vector<int> edges(prerequisites.size(), 0);

    for(const auto& prerequisite : prerequisites){
        const int sourceNode = prerequisite[1];
        const int destinationNode = prerequisite[0];
        edges[--offset[sourceNode]] = destinationNode;
    }

    int read = 0;
    while(read < static_cast<int>(bfsVector.size())){
        const int sourceIdx = bfsVector[read];
        read++;

        for(int i = offset[sourceIdx]; i < offset[sourceIdx + 1]; i++){
            const int dependentIdx = edges[i];
            if(--inDegree[dependentIdx] == 0) bfsVector.push_back(dependentIdx);
        }
    }

    if (read == numCourses) return bfsVector;
    
    return {};
}