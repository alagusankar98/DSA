bool isPalindrome(std::string_view s) {
    int left = 0;
    int right = std::ssize(s) - 1;
    while(left < right){
        while((left < right) && !std::isalnum(static_cast<unsigned char>(s[left]))){
            left++;
        }
        while((left < right) && !std::isalnum(static_cast<unsigned char>(s[right]))){
            right--;
        }
        if (left >= right) break;
        if(std::toupper(static_cast<unsigned char>(s[left])) != std::toupper(static_cast<unsigned char>(s[right]))) return false;
        left++;
        right--;
    }
    return true;
}