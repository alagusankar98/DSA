
bool isInterleaveHelper(std::string_view s1, std::string_view s2, std::string_view s3, int i, int j) {
    if(i == s1.size() && j == s2.size()) return true;

    bool resultFlag = false;

    if(i < s1.size() && s3[i + j] == s1[i]) {
        // Still have characters left in s1
        resultFlag = isInterleaveHelper(s1, s2, s3, i + 1, j);
    }

    if(j < s2.size() && s3[i + j] == s2[j]) {
        // Still have characters left in s2
        resultFlag = resultFlag || isInterleaveHelper(s1, s2, s3, i, j + 1);
    }

    return resultFlag;
}
bool isInterleave(std::string_view s1, std::string_view s2, std::string_view s3) {
    const int n1 = static_cast<int>(s1.size());
    const int n2 = static_cast<int>(s2.size());
    const int n3 = static_cast<int>(s3.size());

    if((n1 + n2) != n3) return false;

    return isInterleaveHelper(s1, s2, s3, 0, 0);
}