int characterReplacement(std::string_view s, int k) {
    int left = 0;
    int right = left;
    int n = std::ssize(s);
    int longestSequence = 0;
    int currentWindowPopularCount = 0;
    std::array<int, 26> seen{};
    while(right < n){
        unsigned char currentChar = static_cast<unsigned char>(s[right]);
        seen[currentChar - 'A']++;

        currentWindowPopularCount = std::max(currentWindowPopularCount, seen[currentChar - 'A']);
        if((right - left + 1) - currentWindowPopularCount > k){
            // For invalid window, Move left anchor forward by 1 to make the window valid again
            // Loop does not execute for popular character since currentWindowPopularCount will also increase
            seen[s[left] - 'A']--;
            left++;
        }

        longestSequence = std::max(longestSequence, (right - left + 1));
        right++;
    }
    return longestSequence;
}