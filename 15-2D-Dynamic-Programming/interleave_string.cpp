bool isInterleave(std::string_view s1, std::string_view s2, std::string_view s3) {
    const int n1 = static_cast<int>(s1.size());
    const int n2 = static_cast<int>(s2.size());
    if(n2 > n1) return isInterleave(s2, s1, s3);
    const int n3 = static_cast<int>(s3.size());

    if((n1 + n2) != n3) return false;

    std::vector<uint8_t> current(n2 + 1, 0);
    std::vector<uint8_t> next(current);

    for(int i = n1; i >= 0; i--) {
        for(int j = n2; j >= 0; j--) {
            bool resultFlag = false;
            if(i < n1 && s3[i + j] == s1[i]) {
                // Still have characters left in s1
                resultFlag = next[j];
            }
            if(j < n2 && s3[i + j] == s2[j]) {
                // Still have characters left in s2
                resultFlag = resultFlag || current[j + 1];
            }
            current[j] = resultFlag;
            if(i == n1 && j == n2) current[j] = 1;
        }
        std::swap(current, next);
    }
    return next[0];
}