int formPalindromeLength(std::string_view s, int left, int right) {
    const int n = static_cast<int>(s.size());
    while(left >= 0 && right < n && s[left] == s[right]){
        left--;
        right++;
    }
    return ((right - 1) - (left + 1) + 1); // (last valid right index - last valid left index), +1 to include the base character
}
int countSubstrings(std::string_view s) {
    const int n = static_cast<int>(s.size());

    int count = 0;
    for(int i = 0; i < n; i++){
        int oddLength = formPalindromeLength(s, i, i);
        int evenLength = formPalindromeLength(s, i, i+1);
        
        count += ((oddLength + evenLength + 1) / 2); // Condensed form of (((oddLength + 1) / 2) + (evenLength / 2));
    }
    return count;
}