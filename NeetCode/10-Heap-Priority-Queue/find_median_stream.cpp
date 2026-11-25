class MedianFinder {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
    std::priority_queue<int> maxHeap_;
public:
    MedianFinder() = default;
    
    void addNum(int num) {
        if(minHeap_.empty()) {
            minHeap_.push(num);
            return;
        }

        if(num > minHeap_.top()){
            minHeap_.push(num);
        } else {
            maxHeap_.push(num);
        }
        
        if(maxHeap_.size() > minHeap_.size()){
            // Max Heap became large
            // Max heap size should always be <= min heap size
            minHeap_.push(maxHeap_.top());
            maxHeap_.pop();
        } else if (minHeap_.size() > (maxHeap_.size() + 1)){
            // Min heap became large
            maxHeap_.push(minHeap_.top());
            minHeap_.pop();
        }
    }
    
    double findMedian() const {
        assert(!minHeap_.empty());
        if(minHeap_.size() == maxHeap_.size()){
            return (static_cast<double>(minHeap_.top()) + maxHeap_.top()) / 2;
        } else {
            return minHeap_.top();
        }
    }
};