std::string encode(const std::vector<std::string>& strs) {
    std::string encodedString;
    for(const auto str : strs){
        encodedString += std::to_string(str.size()) + "#" + str;
    }
    std::cout << encodedString;
    return encodedString;
}

std::vector<std::string> decode(std::string s) {
    std::vector<std::string> resultVector;
    for(size_t i = 0; i < s.size(); ){
        size_t sizeEndPos = s.find("#", i);
        auto sizeStr = s.substr(i, sizeEndPos - i);
        i = sizeEndPos + 1;
        size_t subStringLength = static_cast<size_t>(std::stoull(sizeStr));
        std::string subString = s.substr(i, subStringLength);
        resultVector.push_back(subString);
        i += subStringLength;
    }
    return resultVector;
}