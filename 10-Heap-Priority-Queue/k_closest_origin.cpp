long long calculateDistance(const std::vector<int>& point){
    return (point[0] * point[0] + point[1] * point[1]);
}
std::vector<std::vector<int>> kClosest(const std::vector<std::vector<int>>& points, int k) {
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(k);
    
    auto compare_coordinates = [this] (const std::vector<int>& point1, const std::vector<int>& point2) {
        // We need a min heap
        return calculateDistance(point1) > calculateDistance(point2);
    };
    
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, decltype(compare_coordinates)> minHeap(points.begin(), points.end(), compare_coordinates);

    for(int i = 0; i < k; i++){
        assert(!minHeap.empty() && "Not enough k-elements in heap");
        resultVector.push_back(minHeap.top()); minHeap.pop();
    }

    return resultVector;
}