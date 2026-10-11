class KthLargest {
private:
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
    int k_;
public:
    KthLargest(int k, vector<int>& nums) {
        k_ = k;
        for(const int num : nums){
            add(num);
        }
    }
    
    int add(int val) {
        minHeap_.push(val);
        if(minHeap_.size() > k_) minHeap_.pop();
        return minHeap_.top();
    }
};