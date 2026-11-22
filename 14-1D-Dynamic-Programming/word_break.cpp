bool wordBreakHelper(std::string_view s, const std::vector<std::string>& wordDict, size_t i) {
    if(i == s.size()) return true;

    for(const auto& word : wordDict) {
        // Compare each word against current position 'i'
        if(size_t wordLength = word.size(); (i + wordLength) <= s.size() && s.substr(i, wordLength) == word) {
            if(wordBreakHelper(s, wordDict, (i + wordLength))) return true;
        }
    }

    return false;
}
bool wordBreak(std::string_view s, const std::vector<std::string>& wordDict) {
    return wordBreakHelper(s, wordDict, 0);
}