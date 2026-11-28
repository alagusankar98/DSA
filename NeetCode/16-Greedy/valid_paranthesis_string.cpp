bool checkValidString(std::string_view s) {
    // [low, high] to keep track of range of possible choices
    // '(' increments both
    // ')' decrements both
    // '*' means I have three choices: decrement, same or increment [lo-1, hi + 1] captures it

    int low = 0;
    int high = 0;

    for(const char c : s) {
        if(c == '(') {
            low++;
            high++;
        } else if(c == ')') {
            low--;
            high--;
        } else {
            // '*'
            low--;
            high++;
        }

        low = std::max(low, 0); // Trim [-1, 0, 1] to [0, 1] since '*' does not have enough open braces to consume
        if(high < 0) return false;
    }
    return (low == 0);
}