int lengthOfLongestSubstring(std::string_view s) {
    int maxUniqueCount = 0;
    std::array<int, 256> currentCharMap{0};
    int left = 0;
    int right = left;
    int n = std::ssize(s);

    while(right < n){
        unsigned char currentChar = static_cast<unsigned char>(s[right]);
        if(currentCharMap[currentChar] != 0){
            // Already seen
            left = std::max(left, currentCharMap[currentChar]); // max used to move anchor to latest duplicate instead of old duplicate
        }
        currentCharMap[currentChar] = right + 1; // Store the next character index to jump to in case of duplicates
        maxUniqueCount = std::max(maxUniqueCount, (right - left + 1));
        right++;
    }
    return maxUniqueCount;
}