bool isValid(std::string_view s) {
    std::stack<char, std::vector<char>> bracketStack;
    std::unordered_map<char, char> bracketMap = {{')', '('}, {'}', '{'}, {']', '['}};
    for(const char c : s){
        if(auto it = bracketMap.find(c); (!bracketStack.empty()) && (it != bracketMap.end())){
            if(bracketStack.top() != it->second) return false;
            bracketStack.pop();
        } else {
            bracketStack.push(c);
        }
    }
    return bracketStack.empty();
}