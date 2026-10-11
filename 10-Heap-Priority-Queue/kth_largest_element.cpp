class KthLargest {
private:
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
    size_t k_;
public:
    KthLargest(int k, const std::vector<int>& nums) : minHeap_(nums.begin(), nums.end()), k_(k){
        while(minHeap_.size() > k) minHeap_.pop();
    }
    
    int add(int val) {
        if(minHeap_.size() < k_){
            // Heap isn't full yet, populate values
            minHeap_.push(val);
        } else if(val > minHeap_.top()){
            // Heap is full, but new value is greater than current
            // poorest in heap
            minHeap_.pop();
            minHeap_.push(val);
        }
        return minHeap_.top();
    }
};