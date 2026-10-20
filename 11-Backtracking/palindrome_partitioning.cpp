bool isPalindrome(std::string_view s){
    int left = 0;
    int right = std::ssize(s) - 1;

    while(left < right){
        if(s[left] != s[right]) return false;
        left++;
        right--;
    }

    return true;
}
void partitionHelper(std::string_view s, std::vector<std::vector<std::string>>& resultVector, std::vector<std::string>& tempVector, size_t startIdx){
    if(startIdx >= s.size()){
        resultVector.push_back(tempVector);
        return;
    }
    
    for(size_t i = startIdx; i < s.size(); i++){
        std::string tempString(s.substr(startIdx, i - startIdx + 1));
        if(isPalindrome(tempString)) {
            tempVector.push_back(tempString);
            partitionHelper(s, resultVector, tempVector, i + 1);
            tempVector.pop_back();
        }
    }
}

std::vector<std::vector<std::string>> partition(std::string_view s) {
    std::vector<std::vector<std::string>> resultVector;
    std::string tempString;
    std::vector<std::string> tempVector;
    partitionHelper(s, resultVector, tempVector, 0);
    return resultVector;
}