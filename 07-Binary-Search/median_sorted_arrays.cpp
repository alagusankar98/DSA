double findMedianSortedArrays(const std::vector<int>& nums1, const std::vector<int>& nums2) {
    if(nums1.size() > nums2.size()) return findMedianSortedArrays(nums2, nums1);
    const int n1 = static_cast<int>(nums1.size());
    const int n2 = static_cast<int>(nums2.size());
    const int totalHalf = (n1 + n2 + 1) / 2;
    int leftA = 0;
    int rightA = n1;
    
    while(leftA <= rightA){
        int sectionA = leftA + (rightA - leftA) / 2;
        int sectionB = totalHalf - sectionA;
        
        int maxLeftSectionA = (sectionA > 0) ? nums1[sectionA - 1] : std::numeric_limits<int>::min();
        int maxLeftSectionB = (sectionB > 0) ? nums2[sectionB - 1] : std::numeric_limits<int>::min();
        int minRightSectionA = (sectionA < n1) ? nums1[sectionA] : std::numeric_limits<int>::max();
        int minRightSectionB = (sectionB < n2) ? nums2[sectionB] : std::numeric_limits<int>::max();

        if(maxLeftSectionA <= minRightSectionB && maxLeftSectionB <= minRightSectionA){
            // Both left halves combined are now lesser than their right halves
            if(((n1 + n2) % 2) != 0){
                // Odd length
                return std::max(maxLeftSectionA, maxLeftSectionB);
            } else {
                // Even length
                return static_cast<double>(std::max(maxLeftSectionA, maxLeftSectionB) + std::min(minRightSectionA, minRightSectionB)) / 2;
            }
        } else if (maxLeftSectionA > minRightSectionB){
            rightA = sectionA - 1;
        } else {
            leftA = sectionA + 1;
        }
    }
    return 0;
}