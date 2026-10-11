long long calculateDistance(const std::vector<int>& point){
    return (point[0] * point[0] + point[1] * point[1]);
}
std::vector<std::vector<int>> kClosest(const std::vector<std::vector<int>>& points, int k) {
    std::vector<std::vector<int>> resultVector;
    resultVector.reserve(k);
    
    auto compare_coordinates = [this] (const std::vector<int>& point1, const std::vector<int>& point2) {
        // We need a min heap
        return calculateDistance(point1) < calculateDistance(point2);
    };
    
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, decltype(compare_coordinates)> maxHeap(compare_coordinates);

    // Push elements to min heap
    for(const auto& point : points){
        if(maxHeap.size() < k){
            maxHeap.push(point);
        } else if(calculateDistance(point) < calculateDistance(maxHeap.top())){
            maxHeap.pop();
            maxHeap.push(point);
        }
    }

    while(!maxHeap.empty()){
        resultVector.push_back(maxHeap.top()); maxHeap.pop();
    }

    return resultVector;
}