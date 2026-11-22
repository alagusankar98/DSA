bool wordBreak(std::string_view s, const std::vector<std::string>& wordDict) {
    const int n = static_cast<int>(s.size());
    std::unordered_set<std::string_view> wordSet;
    wordSet.reserve(wordDict.size());
    size_t maxLen = 0;
    for(const auto& word : wordDict) {
        wordSet.insert(word);
        maxLen = std::max(maxLen, word.size());
    }
    std::vector<uint8_t> dp(n + 1, 0);
    dp[n] = 1;
    
    for(int i = n - 1; i >= 0; i--) {
        if(!dp[i + 1]) continue; // If index 'i+1' is not splittable, No point in trying to match words with end index 'i'
        for(int wordLength = std::min(i + 1, static_cast<int>(maxLen)); wordLength > 0; wordLength--) {
            // Trying to match word with end index 'i'
            if(!dp[i + 1 - wordLength] && wordSet.contains(s.substr(i + 1 - wordLength, wordLength))) {
                // "!dp[i + 1 - wordLength]" check to skip substring comparison if a previous run has already set it
                dp[i + 1 - wordLength] = 1;
            }
        }
    }
    return dp[0];
}