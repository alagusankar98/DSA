int numDistinct(std::string_view s, std::string_view t) {
    const int n1 = static_cast<int>(s.size());
    const int n2 = static_cast<int>(t.size());

    if(n2 > n1) return 0;

    std::vector<unsigned int> current(n2 + 1, 0);
    current[n2] = 1;

    for(int i = n1 - 1; i >= 0; i--) {
        for(int j = 0; j < n2; j++) {
            if(s[i] == t[j]) current[j] += current[j + 1];
        }
    }

    return static_cast<int>(current[0]);
}