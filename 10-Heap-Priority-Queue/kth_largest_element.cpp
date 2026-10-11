class KthLargest {
private:
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
    size_t k_;
public:
    KthLargest(int k, vector<int>& nums) {
        k_ = k;
        for(const int num : nums){
            add(num);
        }
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