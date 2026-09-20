int parseInteger(std::string_view str){
    int result = 0;

    auto _ = std::from_chars(str.data(), str.data() + str.size(), result);

    return result;
}
int evalRPN(const vector<string>& tokens) {
    std::vector<int> numStack;

    for(size_t i = 0; i < tokens.size(); i++){
        if(tokens[i] == "+"){
            int second = numStack.back();
            numStack.pop_back();
            int first = numStack.back();
            numStack.pop_back();
            numStack.push_back(first + second);
        } else if (tokens[i] == "*") {
            int second = numStack.back();
            numStack.pop_back();
            int first = numStack.back();
            numStack.pop_back();
            numStack.push_back(first * second);
        } else if (tokens[i] == "/") {
            int second = numStack.back();
            numStack.pop_back();
            int first = numStack.back();
            numStack.pop_back();
            numStack.push_back(first / second);
        } else if (tokens[i] == "-") {
            int second = numStack.back();
            numStack.pop_back();
            int first = numStack.back();
            numStack.pop_back();
            numStack.push_back(first - second);
        } else {
            
            numStack.push_back(parseInteger(tokens[i]));
        }
    }
    return numStack.back();
}