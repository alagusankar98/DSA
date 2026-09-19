std::string minWindow(std::string_view s, std::string_view t) {
    if(t.size() > s.size()) return "";

    std::array<int, 128> tCount{};
    std::array<int, 128> sCount{};

    for(size_t i = 0; i < t.size(); i++){
        int charIdx = static_cast<unsigned char>(t[i]);
        tCount[charIdx]++;
    }

    int tUniqueCharacters = 0;
    for(size_t i = 0; i < tCount.size(); i++){
        tUniqueCharacters += (tCount[i] != 0);
    }

    int windowMatchCharacters = 0;
    size_t minStart = 0;
    size_t minLength = std::numeric_limits<size_t>::max();

    for(size_t left = 0, right = 0; right < s.size(); right++){
        int rightCharIdx = static_cast<unsigned char>(s[right]);
        sCount[rightCharIdx]++;

        if(sCount[rightCharIdx] == tCount[rightCharIdx]) windowMatchCharacters++;

        if(windowMatchCharacters == tUniqueCharacters){
            // Keep right as anchor
            // Have to shrink left to see how much we can go before losing desired character
            while(left < right){
                int leftCharIdx = static_cast<unsigned char>(s[left]);
                if((tCount[leftCharIdx] > 0) && sCount[leftCharIdx] == tCount[leftCharIdx]) break; // Exit if matches are about to get spoiled
                sCount[leftCharIdx]--;
                left++;
            }

            // [left, right] holds a substring of t in s.
            // Check if [left, right] is less than minimum seen so far
            size_t currentMinLength = right - left + 1;
            if(currentMinLength < minLength){
                minStart = left;
                minLength = currentMinLength;
            }

            // Move left out of valid window for next iteration
            sCount[static_cast<unsigned char>(s[left])]--;
            windowMatchCharacters--;
            left++;
        }
    }

    return (minLength == std::numeric_limits<size_t>::max()) ? "" : std::string(s.substr(minStart, minLength));

}