bool isMatchHelper(std::string_view s, std::string_view pattern, int i, int j, const int n1, const int n2) {
    if(j == n2) return (i == n1);

    if((j + 1) < n2 && pattern[j + 1] == '*') {
        // Consume 0 or more characters of pattern[j] in s
        // while((i < n1) && (pattern[j] == '.' || s[i] == pattern[j])) i++;

        // Either string s ran out or I have a mismatch.
        // Mismatch is fine in case of * since it can consume 0 characters as well
        // Either way next call should be to (j + 2)
        return isMatchHelper(s, pattern, i, j + 2, n1, n2) || (i < n1) && (s[i] == pattern[j] || pattern[j] == '.') && isMatchHelper(s, pattern, i + 1, j, n1, n2);
    } else if(i < n1) {
        if(s[i] == pattern[j] || pattern[j] == '.') return isMatchHelper(s, pattern, i + 1, j + 1, n1, n2);
    }

    return false;
}
bool isMatch(std::string_view s, std::string_view p) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(p.size());

    return isMatchHelper(s, p, 0, 0, n1, n2);
}