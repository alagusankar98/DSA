std::vector<std::string> letterCombinations(std::string_view digits) {
    std::vector<std::string> resultVector;
    static constexpr std::array<std::string_view, 8> digitMap = {"abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
for(const char c : digits){
    if(resultVector.empty()) resultVector.push_back("");
    std::vector<std::string> tempVector;
    const std::string_view digitValue = digitMap[c - '0' - 2];
    tempVector.reserve(resultVector.size() * digitValue.size());
    for(const auto& str : resultVector){
        for(const auto val : digitValue){
            tempVector.emplace_back(str).push_back(val);
        }
    }
        resultVector = std::move(tempVector);
    }
    return resultVector;
}

// Recursive version

// void letterGenerator(std::string_view s, const std::array<std::string_view, 8>& digitMap, std::vector<std::string>& resultVector, std::string& tempString, size_t i){
//     if(i >= s.size()){
//         resultVector.push_back(tempString);
//         return; // Used all in input string
//     }

//     int charIdx = s[i] - '0' - 2;
//     const std::string_view digitValues = digitMap[charIdx];
//     for(size_t j = 0; j < digitValues.size(); j++){
//         tempString.push_back(digitValues[j]);
//         letterGenerator(s, digitMap, resultVector, tempString, i + 1);
//         tempString.pop_back();
//     }
// }
// std::vector<std::string> letterCombinations(std::string_view digits) {
//     std::vector<std::string> resultVector;
//     static constexpr std::array<std::string_view, 8> digitMap = {"abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
//     std::string tempString;
//     tempString.reserve(digits.size());
//     if(!digits.empty())letterGenerator(digits, digitMap, resultVector, tempString, 0);
//     return resultVector;
// }