int reverse(int x) {
    int negative_threshold = INT32_MIN/10;
    int positive_threshold = INT32_MAX/10;
    int reverseNum = 0;

    while(x != 0){
        int digit = (x % 10);

        if(reverseNum < negative_threshold || (reverseNum == negative_threshold && (digit < -8))){
            return 0;
        }

        if(reverseNum > positive_threshold || (reverseNum == positive_threshold && (digit > 7))){
            return 0;
        }
        
        reverseNum = (reverseNum * 10) + digit;
        x = x / 10;
    }
    return reverseNum;
}