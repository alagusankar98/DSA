std::string multiply(std::string_view num1, std::string_view num2) {
    if(num1 == "0" || num2 =="0") return "0";
    const int n1 = static_cast<int>(num1.size());
    const int n2 = static_cast<int>(num2.size());
    const int n = n1 + n2;
    std::vector<int> interSumVector(n, 0);

    for(int i = n1 - 1; i >= 0; i--) {
        for(int j = n2 - 1; j >= 0; j--) {
            int product = (num1[i] - '0') * (num2[j] - '0');
            interSumVector[i + j + 1] += (product % 10);
            interSumVector[i + j] += (product / 10);
        }
    }

    int firstNonZeroIdx = n;
    int carry = 0;
    for(int i = n - 1; i >= 0; i--) {
        int sum = interSumVector[i] + carry;
        carry = sum / 10;
        interSumVector[i] = sum % 10;
        if(interSumVector[i]) firstNonZeroIdx = i;
    }

    std::string resultString;
    resultString.reserve(n);

    for(int i = firstNonZeroIdx; i < n; i++) {
        resultString.push_back(static_cast<char>(interSumVector[i] + '0'));
    }

    return resultString;
}