#include <unordered_set>

bool hasDuplicate( std::vector<int>& nums) {

    std::sort(nums.begin(), nums.end(), std::less<int>());

    return std::adjacent_find(nums.begin(), nums.end()) != nums.end();
}