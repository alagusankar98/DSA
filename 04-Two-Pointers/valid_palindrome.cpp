bool isPalindrome(string s) {
    size_t left = 0;
    size_t right = s.size();
    while(left < right){
        if(!std::isalnum(s[left])){
            left++;
            continue;
        }
        if(!std::isalnum(s[right])){
            right--;
            continue;
        }
        if(std::toupper(s[left]) != std::toupper(s[right])) return false;
        left++;
        right--;
    }
    return true;
}