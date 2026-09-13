#include <unordered_set>

bool hasDuplicate(const std::vector<int>& nums) {
    std::unordered_set<int> seenNumbers;
    seenNumbers.reserve(nums.size()); // To reduce rehashing
    for(const int num : nums){
        if (auto [~, inserted] = seenNumbers.insert(num); !inserted) return true;
    }
    return false;
}