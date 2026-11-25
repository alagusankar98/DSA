int lastStoneWeight(const std::vector<int>& stones) {
    std::priority_queue<int, std::vector<int>, std::less<int>> maxHeap(stones.begin(), stones.end());

    while(maxHeap.size() > 1){
        int first = maxHeap.top(); maxHeap.pop();
        int second = maxHeap.top(); maxHeap.pop();

        if(first != second) maxHeap.push(first - second);
    }

    return maxHeap.empty() ? 0 : maxHeap.top();
}