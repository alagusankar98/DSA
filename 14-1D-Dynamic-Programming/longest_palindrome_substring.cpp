std::string formPalindrome(std::string_view s, int left, int right){
    const int n = static_cast<int>(s.size());
    while(left >= 0 && right < n && (s[left] == s[right])){
        left--;
        right++;
    }
    return std::string(s.data() + left + 1, ((right - 1) - (left + 1) + 1));
}
std::string longestPalindrome(std::string_view s) {
    const int n = static_cast<int>(s.size());
    std::string resultString;

    for(int i = 0; i < n; i++){
        auto oddPalindromStr = formPalindrome(s, i, i); // Odd length palindrome
        auto evenPalindromStr = formPalindrome(s, i, i+1); // Odd length palindrome
        if(oddPalindromStr.size() > resultString.size()){
            resultString = std::move(oddPalindromStr);
        }
        if(evenPalindromStr.size() > resultString.size()){
            resultString = std::move(evenPalindromStr);
        }
    }
    return resultString;
}