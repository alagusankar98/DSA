int getSum(int a, int b)
{
    while (b != 0){
        int carry = (a & b); // Carry without sum
        a = a ^ b;           // Bitwise sum without carry
        b = carry << 1;      // Prepare carry for addition in next iteration
    }
    return a;
}