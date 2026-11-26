bool isMatchHelper(std::string_view s, std::string_view pattern, int i, int j, const int n1, const int n2) {
    if(j == n2) return (i == n1);
    // Check if we have an exact character match or if regex pattern is a wild card
    bool firstMatch = (i < n1) && ((s[i] == pattern[j]) || (pattern[j] == '.'));

    if((j + 1) < n2 && pattern[j + 1] == '*') {
        // Consume 0 or more characters of pattern[j] in s

        // Either string s ran out or I have a mismatch.
        // Mismatch is fine in case of * since it can consume 0 characters as well
        // Check both skip branch and take branch (consume one character and let next recursion consume next character)
        // Skip branch : (i, j + 2) to skip pattern and i to let next recursion match ith character  (0 matches, skip '.*' or 'a*')
        // Take branch : Verify if character matches before consuming it (i + 1, j) indicates one character consumed and retaining pattern for next recursion
        return isMatchHelper(s, pattern, i, j + 2, n1, n2) || firstMatch && isMatchHelper(s, pattern, i + 1, j, n1, n2);
    }

    return firstMatch && isMatchHelper(s, pattern, i + 1, j + 1, n1, n2);;
}
bool isMatch(std::string_view s, std::string_view p) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(p.size());

    return isMatchHelper(s, p, 0, 0, n1, n2);
}