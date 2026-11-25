void paranthesisHelper(std::vector<std::string>& resultVector, std::string& tempString, int n, int open, int close){
    if(open == close && tempString.size() == 2 * n) {
        resultVector.push_back(tempString);
        return;
    }

    if(open < n){
        tempString.push_back('(');
        paranthesisHelper(resultVector, tempString, n, open + 1, close);
        tempString.pop_back();
    }
    if(close < open){
        tempString.push_back(')');
        paranthesisHelper(resultVector, tempString, n, open, close + 1);
        tempString.pop_back();
    }
}
std::vector<std::string> generateParenthesis(int n) {
    std::vector<std::string> resultVector;
    std::string tempString;
    paranthesisHelper(resultVector, tempString, n, 0, 0);
    return resultVector;

}