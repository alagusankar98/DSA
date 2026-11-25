bool checkInclusion(std::string_view s1, std::string_view s2) {
    if(s1.size() > s2.size()) return false;
    std::array<int, 26> diffCount{};
    for(size_t i = 0; i < s1.size(); i++){
        diffCount[static_cast<unsigned char>(s1[i]) - 'a']++;
        diffCount[static_cast<unsigned char>(s2[i]) - 'a']--;
    }

    uint32_t matchBits = 0;
    for(size_t i = 0; i < 26; i++){
        if(diffCount[i] == 0){
            matchBits |= (1 << i);
        }
    }

    size_t left = 0;
    
    for(size_t right = s1.size(); right < s2.size(); right++, left++){
        
        // Check if all 26 characters match
        if(matchBits == 0x03FFFFFF) return true;

        int leftCharIndex = static_cast<unsigned char>(s2[left]) - 'a';
        int rightCharIndex = static_cast<unsigned char>(s2[right]) - 'a';
        if(leftCharIndex == rightCharIndex) continue;

        // Let go of left character
        diffCount[leftCharIndex]++;
        diffCount[rightCharIndex]--;

        if(diffCount[leftCharIndex] == 0){
            matchBits |= (1 << leftCharIndex);
        } else {
            matchBits &= (~(1 << leftCharIndex));
        }

        if(diffCount[rightCharIndex] == 0){
            matchBits |= (1 << rightCharIndex);
        } else {
            matchBits &= (~(1 << rightCharIndex));
        }
    }
    return (matchBits == 0x03FFFFFF);
}