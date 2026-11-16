std::pair<int, int> formPalindromeIndices(std::string_view s, int left, int right){
    const int n = static_cast<int>(s.size());
    while(left >= 0 && right < n && (s[left] == s[right])){
        left--;
        right++;
    }
    return {(left + 1), (right - 1)};
}
std::string longestPalindrome(std::string_view s) {
    if(s.empty()) return "";
    const int n = static_cast<int>(s.size());
    int start = 0;
    int end = 0;

    for(int i = 0; i < n; i++){
        if(auto [evenStart, evenEnd] = formPalindromeIndices(s, i, i+1); (evenEnd - evenStart) > (end - start)){
            start = evenStart;
            end = evenEnd;
        }
        if(auto [oddStart, oddEnd] = formPalindromeIndices(s, i, i); (oddEnd - oddStart) > (end - start)){
            start = oddStart;
            end = oddEnd;
        }
    }
    return std::string(s.data() + start, (end - start) + 1);
}