bool canFinish(int numCourses, const std::vector<std::vector<int>>& prerequisites) {
    if(prerequisites.empty()) return true; // Early return for no edges

    std::vector<std::vector<int>> courseGraph(numCourses);
    std::vector<int> preRequisiteCount(numCourses, 0);

    for(const auto& prerequisite : prerequisites){
        const int destination_node = prerequisite[0];
        const int source_node = prerequisite[1];
        assert((destination_node < numCourses) && (source_node < numCourses));
        courseGraph[source_node].push_back(destination_node);
        preRequisiteCount[destination_node]++;
    }

    std::vector<std::size_t> bfsVector;
    bfsVector.reserve(numCourses);

    for(std::size_t i = 0; i < preRequisiteCount.size(); i++){
        // Push courses with no pre-requisite into queue (Readily available courses)
        if(preRequisiteCount[i] == 0) bfsVector.push_back(i);
    }

    std::size_t read = 0;
    while(read < bfsVector.size()) { // If bfsVector size is 0, every course has a pre-req, so a cycle exists
        // Decrement counter for all courses that are waiting on current available course
        auto& dependentCourses = courseGraph[bfsVector[read]];
        read++;

        for(const int dependentCourseIdx : dependentCourses){
            // Decrease count for each dependency of done course
            preRequisiteCount[dependentCourseIdx]--;
            if(preRequisiteCount[dependentCourseIdx] == 0) bfsVector.push_back(dependentCourseIdx);
        }
    }

    return (numCourses == read);
}