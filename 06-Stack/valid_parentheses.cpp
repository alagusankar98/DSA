bool isValid(std::string_view s) {
    if(s.size() % 2 != 0) return false;
        std::vector<char> bracketStack;
        bracketStack.reserve(s.size() / 2);
        for(const char c : s){
            switch(c) {
                case '(' : bracketStack.push_back(')'); break;
                case '[' : bracketStack.push_back(']'); break;
                case '{' : bracketStack.push_back('}'); break;
                case ')':
                case '}':
                case ']':
                    if(bracketStack.empty() || bracketStack.back() != c) return false;
                    bracketStack.pop_back();
                    break;
                default:
                    return false;
            }
        }
        return bracketStack.empty();
}