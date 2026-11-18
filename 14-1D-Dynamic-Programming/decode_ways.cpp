int numDecodings(std::string_view s) {
    const int n = static_cast<int>(s.size());
    int next1 = 1, next2 = 0;
    for(int i = n - 1; i >= 0; i--){
        int current = 0;
        if(s[i] != '0') {
            current += next1;
            if((i + 1 < n) && (s[i] == '1' || (s[i] == '2' && s[i+1] <= '6'))){
                current += next2;
            }
        }
        next2 = next1;
        next1 = current;
        if(!next1 && !next2) return 0;
    }
    return next1;
}