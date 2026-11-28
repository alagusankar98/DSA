std::string add(std::string_view num1, std::string_view num2) {
    const int n1 = static_cast<int>(num1.size());
    const int n2 = static_cast<int>(num2.size());
    // if(n1 == 0) return num2;
    // if(n2 == 0) return num1;
    int i = n1 - 1;
    int j = n2 - 1;
    uint8_t carry = 0;
    std::string resultString;
    resultString.reserve(((n1 > n2) ? n1 : n2) + 1);

    while(i >=0 || j >= 0 || carry){
        int val1 = (i >= 0) ? (num1[i] - '0') : 0;
        int val2 = (j >= 0) ? (num2[j] - '0') : 0;
        int sum =  val1 + val2 + carry;
        resultString.push_back((sum % 10) + '0');
        carry = sum / 10;
        i--;
        j--;
    }

    std::reverse(resultString.begin(), resultString.end());

    return resultString;
}

std::string multiplyDigit(std::string_view num1, uint8_t digit) {
    if(digit == 0 || num1.empty()) return "";
    const int n1 = static_cast<int>(num1.size());
    int i = n1 - 1;
    uint8_t carry = 0;
    std::string resultString;
    resultString.reserve(n1 + 1);

    while(i >=0 || carry){
        int val1 = (i >= 0) ? (num1[i] - '0') : 0;
        int sum = val1 * digit + carry;
        resultString.push_back((sum % 10) + '0');
        carry = sum / 10;
        i--;
    }

    std::reverse(resultString.begin(), resultString.end());

    return resultString;
}

std::string multiply(std::string_view num1, std::string_view num2) {
    const int n1 = static_cast<int>(num1.size());
    const int n2 = static_cast<int>(num2.size());
    if(n1 < n2) return multiply(num2, num1);

    std::string resultString;
    for(const char c : num2) {
        std::string productString = multiplyDigit(num1, c - '0');
        resultString.push_back('0');
        resultString = add(resultString, productString);
    }

    return resultString;
}