bool isMatch(std::string_view s, std::string_view p) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(p.size());

    std::vector<uint8_t> current(n2 + 1, 0);
    current[n2] = 1;
    std::vector<uint8_t> next(current);

    for(int i = n1; i >= 0; i--) {
        current[n2] = (i == n1);
        for(int j = n2 - 1; j >= 0; j--) {
            const bool firstMatch = ((i < n1) && ((s[i] == p[j]) || (p[j] == '.')));
            if((j + 1) < n2 && p[j + 1] == '*') {
                // Consume 0 or more characters of pattern[j] in s

                // Check both skip branch and take branch (consume one character and let next recursion consume next character)
                // Skip branch : (i, j + 2) to skip pattern and i to let next recursion match ith character  (0 matches, skip '.*' or 'a*')
                // Take branch : Verify if character matches before consuming it (i + 1, j) indicates one character consumed and retaining pattern for next recursion
                current[j] = current[j + 2] || (firstMatch && next[j]);
            } else {
                current[j] = (firstMatch && next[j + 1]);
            }
        }
        std::swap(current, next);
    }
    return next[0];
}