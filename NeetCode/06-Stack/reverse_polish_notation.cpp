int evalRPN(const vector<string>& tokens) {
    if(tokens.empty()) return 0;
    std::vector<int> numStack;

    // Each operand reduce two numbers to 1
    // Final result has to be 1 number
    // For 'n' numbers, there has to be 'n-1' operands
    // Therefore, total number of elements in RPN expression is '2*n - 1'
    // Out of '2*n - 1', only 'n' is stored in stack
    // So 'N = (2 * n) - 1' => 'n = (N + 1) / 2' for stack reserve
    numStack.reserve((tokens.size() + 1 )/ 2);

    for(const auto& str : tokens){
        if(str.size() == 1 && !std::isdigit(str[0])){
            // This is an operator

            // Check if stack has atleast two elements
            assert((numStack.size() > 1) && "Stack either empty or has only one operand");
            int right = numStack.back(); numStack.pop_back();
            int left = numStack.back(); numStack.pop_back();
            switch(str[0]){ // Switch works only on char (convertible to int), not on string
            case '+': 
                numStack.push_back(left + right); break;
            case '-':
                numStack.push_back(left - right); break;
            case '*':
                numStack.push_back(left * right); break;
            case '/':
                numStack.push_back(left / right); break;
            default:
                assert(false && "Invalid operand");
                break;
            }
        } else {
            // This is an operand (number)

            int result = 0;

            auto first = str.data();
            const char* last = str.data() + str.size();

            // Decay "+5" to "5" before invoking from_chars
            if((first != last) && (*first == '+')){
                first++;
            }

            auto [_, ec] = std::from_chars(first, last, result);

            assert(ec == std::errc() && "Error during number parsing");

            numStack.push_back(result);
        }
        
    }
    assert(numStack.size() == 1 && "Decayed final stack does not result in single number");
    return numStack.back();
}