int16_t parseInteger(std::string_view str){
    int16_t result = 0;

    auto _ = std::from_chars(str.data(), str.data() + str.size(), result);

    return result;
}
int evalRPN(const vector<string>& tokens) {
    std::pair<int, int> data{};
    
    // Get data first
    if(!tokens.empty()) data.first = parseInteger(tokens[0]);

    for(size_t i = 1; i < tokens.size(); i++){
        if(tokens[i] == "+"){
            data.first += data.second;
        } else if (tokens[i] == "*") {
            data.first *= data.second;
        } else if (tokens[i] == "/") {
            data.first /= data.second;
        } else if (tokens[i] == "-") {
            data.first -= data.second;
        } else {
            data.second = parseInteger(tokens[i]);
        }
    }
    return data.first;
}