bool wordBreak(std::string_view s, const std::vector<std::string>& wordDict) {
    const int n = static_cast<int>(s.size());
    std::vector<uint8_t> dp(n + 1, 0);
    dp[n] = 1;
    
    for(int i = n - 1; i >= 0; i--) {
        if(!dp[i + 1]) continue; // If index 'i+1' is not splittable, No point in trying to match words with end index 'i'
        for(const auto& word : wordDict) {
            // Trying to match word with end index 'i'
            if(int wordLength = static_cast<int>(word.size()); (i + 1) >= wordLength && !dp[i + 1 - wordLength] && s.substr(i + 1 - wordLength, wordLength) == word) {
                // "!dp[i + 1 - wordLength]" check to override substring comparison if a previous run has already set it
                dp[i + 1 - wordLength] = 1;
            }
        }
    }
    return dp[0];
}