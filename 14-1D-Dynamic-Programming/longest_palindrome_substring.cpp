int formPalindromeLength(std::string_view s, int left, int right){
    const int n = static_cast<int>(s.size());
    while(left >= 0 && right < n && (s[left] == s[right])){
        left--;
        right++;
    }
    return ((right - 1) - (left + 1) + 1);
}
std::string longestPalindrome(std::string_view s) {
    const int n = static_cast<int>(s.size());
    int start = 0;
    int length = 0;

    for(int i = 0; i < n; i++){
        int maxLength = std::max(formPalindromeLength(s, i, i), formPalindromeLength(s, i, i+1));
        if(maxLength > length){
            start = i - ((maxLength - 1) / 2);
            length = maxLength;
        }
    }
    return std::string(s.data() + start, length);
}