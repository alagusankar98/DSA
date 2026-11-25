bool canFinish(int numCourses, const std::vector<std::vector<int>>& prerequisites) {
    if(prerequisites.empty()) return true; // Early return for no edges

    std::vector<int> preRequisiteCount(numCourses, 0);
    std::vector<int> edges(prerequisites.size(), 0);
    std::vector<int> offset(numCourses + 1, 0);

    for(const auto& prerequisite : prerequisites){
        const int destination_node = prerequisite[0];
        const int source_node = prerequisite[1];
        assert((destination_node >= 0) && (source_node >= 0) && (destination_node < numCourses) && (source_node < numCourses));
        preRequisiteCount[destination_node]++; // Counts edges coming into node
        offset[source_node + 1]++; // Counts edges going out of node (store in next index)
    }

    std::vector<int> bfsVector;
    bfsVector.reserve(numCourses);

    for(int i = 0; i < numCourses; i++){
        // Push courses with no pre-requisite into queue (Readily available courses)
        if(preRequisiteCount[i] == 0) bfsVector.push_back(i);
        offset[i + 1] += offset[i];
    }

    std::vector<int> cursor(offset.begin(), offset.end() - 1);

    // Fill in edges based on offset from (destination, source) pair
    for(const auto& prerequisite : prerequisites){
        const int destination_node = prerequisite[0];
        const int source_node = prerequisite[1];
        edges[cursor[source_node]] = destination_node;
        cursor[source_node]++;
    }

    int read = 0;
    while(read < static_cast<int>(bfsVector.size())) { // If bfsVector size is 0, every course has a pre-req, so a cycle exists
        // Decrement counter for all courses that are waiting on current available course
        const int startIdx = offset[bfsVector[read]];
        const int endIdx = offset[bfsVector[read] + 1];
        read++;

        for(int i = startIdx; i < endIdx; i++){
            // Decrease count for each dependency of done course
            const int dependentCourseIdx = edges[i];
            if(--preRequisiteCount[dependentCourseIdx] == 0) bfsVector.push_back(dependentCourseIdx);
        }
    }

    return (numCourses == read);
}