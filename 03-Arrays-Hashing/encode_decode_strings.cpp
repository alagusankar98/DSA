std::string encode(const std::vector<std::string>& strs) {
    std::string encodedString;
    size_t encodedStringLength = 0;
    for(const auto& str : strs){
        encodedStringLength += 20 + 1 + str.size(); // 20 for string length and one for '#'
    }
    encodedString.reserve(encodedStringLength);

    for(const auto& str : strs){
        encodedString.append(std::to_string(str.size()));
        encodedString.append("#");
        encodedString.append(str);
    }
    return encodedString;
}

std::vector<std::string> decode(std::string_view s) {
    std::vector<std::string> resultVector;
    const char* ptr = s.data();
    const char* end = ptr + s.size();
    while(ptr < end){
        size_t subStringLength = 0;
        auto [nonNumberPos, _] = std::from_chars(ptr, end, subStringLength); // subStringLength populated with '91#' to 91
        ptr = nonNumberPos + 1; // To skip '#'
        resultVector.emplace_back(ptr, subStringLength); // Start pointer and length for 'emplace' to construct string from
        ptr += subStringLength;
    }
    return resultVector;
}