int longestCommonSubsequence(std::string_view text1, std::string_view text2) {
    if(text2.size() > text1.size()) return longestCommonSubsequence(text2, text1); // To keep n2 smaller resulting in lesser space per row
    const int n1 = static_cast<int>(text1.size());
    const int n2 = static_cast<int>(text2.size());
    std::vector<int> current(n2 + 1 , 0);
    std::vector<int> next(current);
    
    for(int i = n1 - 1; i >= 0; i--){
        for(int j = n2 - 1; j >= 0; j--){
            if(text1[i] != text2[j]) {
                current[j] = std::max(current[j + 1], next[j]);
            } else {
                current[j] = 1 + next[j + 1];
            }
        }
        std::swap(next, current);
    }
    return next[0];
}