#include <vector>

int findDuplicate(std::vector<int>& nums){
    if(nums.empty()) return -1;

    int slow = nums[0];
    int fast = nums[0];

    while(true){
        slow = nums[slow];
        fast = nums[nums[fast]];
        if(slow == fast) break;
    }

    slow = nums[0];
    while(slow != fast){
        slow = nums[slow];
        fast = nums[fast];
    }
    return fast;
}