std::string minWindow(std::string_view s, std::string_view t) {
    if(t.size() > s.size()) return "";

    std::array<int, 128> diffCount{};
    std::array<int, 128> sCount{};

    for(const unsigned char c : t){
        diffCount[c]++;
    }

    int totalCharactersRequired = std::ssize(t);
    size_t minStart = 0;
    size_t minLength = std::numeric_limits<size_t>::max();

    for(size_t left = 0, right = 0; right < s.size(); right++){
        int rightCharIdx = static_cast<unsigned char>(s[right]);
        if(diffCount[rightCharIdx] > 0) totalCharactersRequired--;  // Decrement only for characters already in diffCount

        diffCount[rightCharIdx]--; // Ingest right character

        // Keep right as anchor
        // Have to shrink left to see how much we can go before losing desired character
        while(totalCharactersRequired == 0){
            // [left, right] holds a substring of t in s.
            // Check if [left, right] is less than minimum seen so far
            size_t currentMinLength = right - left + 1;
            if(currentMinLength < minLength){
                minStart = left;
                minLength = currentMinLength;
            }

            int leftCharIdx = static_cast<unsigned char>(s[left]);
            if(diffCount[leftCharIdx] == 0) totalCharactersRequired++; // I'm about to move left past a needed character
            diffCount[leftCharIdx]++;
            left++;
        }
    }

    return (minLength == std::numeric_limits<size_t>::max()) ? "" : std::string(s.substr(minStart, minLength));

}