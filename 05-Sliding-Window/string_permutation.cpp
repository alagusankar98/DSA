bool checkZeros(const std::array<int, 26>& arrayVal){
    for(const int val : arrayVal){
        if(val != 0) return false;
    }
    return true;;
}
bool checkInclusion(std::string_view s1, std::string_view s2) {
    if(s1.size() > s2.size()) return false;
    std::array<int, 26> seen{};
    for(size_t i = 0; i < s1.size(); i++){
        seen[static_cast<unsigned char>(s1[i]) - 'a']++;
        seen[static_cast<unsigned char>(s2[i]) - 'a']--;
    }

    int left = 0;
    int right = std::ssize(s1);
    const int n = std::ssize(s2);
    while(right < n){
        if(checkZeros(seen)) return true;
        seen[static_cast<unsigned char>(s2[left]) - 'a']++;
        seen[static_cast<unsigned char>(s2[right]) - 'a']--;
        left++;
        right++;
    }
    return checkZeros(seen);
}