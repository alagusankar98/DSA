std::string multiply(std::string_view num1, std::string_view num2) {
    const int n1 = static_cast<int>(num1.size());
    const int n2 = static_cast<int>(num2.size());
    if(num1 == "0" || num2 == "0") return "0";
    if(n1 < n2) return multiply(num2, num1);

    std::vector<int> resultVector(n1 + n2, 0);

    for(int j = n2 - 1; j >= 0; j--) {
        for(int i = n1 - 1; i >= 0; i--) {
            int product = (num1[i] - '0') * (num2[j] - '0');
            resultVector[i + j + 1] += (product % 10);
            resultVector[i + j] += (product / 10);
        }
    }
    const int n = n1 + n2;

    std::string resultString;
    resultString.reserve(n);
    int carry = 0;
    int firstNonZeroIdx = n - 1;
    for(int i = n - 1; i >= 0; i--) {
        int sum = carry + resultVector[i];
        // if(sum == 0 && carry == 0) break;
        // resultString.push_back((sum % 10) + '0');
        resultVector[i] = (sum % 10);
        if(resultVector[i]) firstNonZeroIdx = i;
        carry = sum / 10;
    }

    for(int i = firstNonZeroIdx; i < n; i++) {
        resultString.push_back(resultVector[i] + '0');
    }

    return resultString;
}