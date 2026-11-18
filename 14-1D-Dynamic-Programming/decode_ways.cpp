int numDecodings(std::string_view s) {
    const int n = static_cast<int>(s.size());
    int next1 = 1;
    int next2 = 0;
    for(int i = n-1; i >= 0; i--){
        int count1 = 0, count2 = 0;
        if(s[i] != '0'){
            count1 = next1;
            if((i + 1) < n && (s[i] == '1' || (s[i] == '2' && s[i + 1] <= '6') ) ){
                count2 = next2;
            }
        }
        int temp = count1 + count2;
        next2 = next1;
        next1 = temp;
    }
    return next1;
}