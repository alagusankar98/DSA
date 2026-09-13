struct ArrayHasher {
    std::size_t operator()(const std::array<uint16_t, 26>& valueArray) const{
        std::size_t hashValue = 17;
        for(const auto val : valueArray){
            hashValue *= 31 + val;
        }
        return hashValue;
    }
};
std::vector<std::vector<std::string>> groupAnagrams(const std::vector<std::string>& strs) {
    std::unordered_map<std::array<uint16_t, 26>, std::vector<std::string>, ArrayHasher> anagramMap;
    anagramMap.reserve(strs.size());
    for(const auto& str : strs){
        std::array<uint16_t, 26> charCount = {0};
        for(size_t i = 0; i < str.size(); i++){
            charCount[str[i] - 'a']++;
        }

        anagramMap[charCount].push_back(str);
    }

    std::vector<std::vector<std::string>> resultVector;
    resultVector.reserve(anagramMap.size());
    for(const auto& [_, val]: anagramMap){
        resultVector.push_back(val);
    }
    return resultVector;
}