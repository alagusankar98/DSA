#include <cassert>
class KthLargest {
private:
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap_;
    int k_;
public:
    KthLargest(int k, vector<int>& nums) {
        for(const int num : nums){
            minHeap_.push(num);
        }
        k_ = k;
    }
    
    int add(int val) {
        minHeap_.push(val);
        while(minHeap_.size() > k_) minHeap_.pop();
        return minHeap_.top();
    }
};