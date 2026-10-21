std::vector<std::string> letterCombinations(std::string_view digits) {
    std::vector<std::string> resultVector;
    std::unordered_map<char, std::vector<std::string>> digitMap = {{'2', {"a","b","c"}}, {'3', {"d","e","f"}}, {'4', {"g","h","i"}}, {'5', {"j","k","l"}}, {'6', {"m","n","o"}}, {'7', {"p","q","r","s"}}, {'8', {"t","u","v"}}, {'9', {"w","x","y","z"}}};
    for(const char c : digits){
        if(resultVector.empty()) resultVector.push_back("");
        std::vector<std::string> tempVector;
        tempVector.reserve(resultVector.size() * digitMap[c].size());
        for(const auto str : resultVector){
            for(const auto val : digitMap[c]){
                tempVector.push_back(str + val);
            }
        }
        resultVector = std::move(tempVector);
    }
    return resultVector;
}