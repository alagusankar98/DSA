# Bit Manipulation - Topic Guide

## 1. Core Fundamentals

### Bitwise Operators (C++)
* **AND (`&`)**: `1` if both bits are `1`, else `0`.
* **OR (`|`)**: `1` if at least one bit is `1`, else `0`.
* **XOR (`^`)**: `1` if bits are different, `0` if they are the same.
* **NOT (`~`)**: Inverts all bits (`0` becomes `1`, `1` becomes `0`).
* **Left Shift (`<<`)**: Shifts bits to the left, filling with `0`. Equivalent to multiplying by 2.
* **Right Shift (`>>`)**: Shifts bits to the right. Equivalent to dividing by 2.

### Key XOR Properties
* **Identity:** `x ^ 0 = x`
* **Self-Inverse:** `x ^ x = 0`
* **Commutativity & Associativity:** Order does not matter. `a ^ b ^ c ^ a = (a ^ a) ^ (b ^ c) = 0 ^ b ^ c = b ^ c`

---

## 2. General Summary / Quick Reference

When tackling Bit Manipulation problems, keep these core patterns in mind:

* **Cumulative XOR:** Used to find missing or singular elements by canceling out pairs.
* **Bit Masking:** Isolating specific bits. To check if the $i$-th bit is set, use `(n & (1 << i)) != 0`.
* **Setting a Bit:** `n = n | (1 << i)`.
* **Clearing a Bit:** `n = n & ~(1 << i)`.
* **Toggling a Bit:** `n = n ^ (1 << i)`.
* **Brian Kernighan's Algorithm (`n & (n - 1)`):** Instantly deletes the rightmost `1` bit. This is the ultimate "skip the zeros" trick for counting bits efficiently.
* **Bitwise Addition (Half-Adder):** To add numbers without the `+` operator, use `a ^ b` to find the base sum and `(a & b) << 1` to generate the carry. Repeat until the carry is 0.
* **DP + Bit Shifting:** When iterating through a range of numbers to find bit properties, right-shifting a number (`i >> 1`) maps it to a smaller, previously calculated subproblem.
* **Bit-by-Bit Construction:** To reverse or build a binary number, extract the Least Significant Bit (LSB) with `n & 1`, left-shift your result buffer, and append the bit using bitwise OR (`|`).
* **Safe Shifting (UB/Overflow):** Right-shifting negative numbers fills with `1`s (sign extension), which causes infinite loops. Always cast to an `unsigned int` before calculating carries to avoid undefined behavior.

---

## 3. Problem Strategies & Patterns

### [1] Single Number

* **The Core Pattern:** Cumulative XOR.
* **The "Gotcha":** Understanding the initial state. You must initialize your tracking variable to `0`. If you initialize it to anything else (or initialize it to the first element but accidentally include the first element in the loop again), the math collapses.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:** Visualizing why an initial value of `0` works was the hardest part. The breakthrough is combining two core properties: 
    1. `x ^ x = 0` (Duplicates cancel out completely).
    2. `x ^ 0 = x` (XORing any number with `0` leaves the number unchanged).
    Because XOR is commutative, the array's order does not matter. All the duplicate pairs annihilate each other into `0`. The running total eventually becomes `0 ^ single_number`, which perfectly isolates and returns the `single_number`.

### [2] Number of 1 Bits (Hamming Weight)

* **The Core Pattern:** Right Shift & Masking (`n & 1`), or Brian Kernighan's Algorithm (`n & (n - 1)`).
* **The "Gotcha":**
    * **Complexity Clarification:** A loop shifting bit by bit runs for the total number of bits. The number of bits representing a value $N$ is roughly $\log_2(N)$. Therefore, the complexity is $O(\log N)$ relative to the number's value. However, since integers have a fixed width (e.g., 32 bits), we generally classify this as $O(1)$ time.
    * **The STL Shortcut:** In modern C++ (C++20 and later), you can use `std::popcount(n)` from the `<bit>` header to count set bits in hardware-optimized $O(1)$ time. 
* **Time & Space Complexity:** O(1) Time / O(1) Space.
* **The Struggle & Insights:**
    * **Branchless Programming:** Using `count += (n & 1)` instead of an `if (n & 1)` check is a highly optimal pattern. It avoids branch prediction penalties on the CPU.
    * **The "Skip Zeros" Magic (Brian Kernighan's Algorithm):** To iterate *only* for the number of `1`s and skip all `0`s, use the operation `n = n & (n - 1)`. 
        * **Why it works:** Subtracting `1` from a number flips all the bits starting from the rightmost `1` down to the end. 
        * **Example:** Let `n = 12` (Binary `1100`).
        * `n - 1 = 11` (Binary `1011`).
        * Notice how the rightmost `1` (and everything after it) flipped. 
        * `n & (n - 1)` becomes `1100 & 1011 = 1000`. 
        * You just deleted the rightmost `1` in a single operation! Loop this `while (n > 0)`, and the loop runs exactly as many times as there are `1`s.


### [3] Counting Bits

* **The Core Pattern:** Dynamic Programming + Bitwise Shift. The optimal recurrence relation is `dp[i] = dp[i >> 1] + (i & 1)`.
* **The "Gotcha":**
    * **Vector Initialization:** While `reserve()` combined with `push_back()` is decent, constructing the vector with its exact required size up front (`std::vector<int> res(n + 1);`) and assigning via index (`res[i] = ...`) is the most optimal approach. It eliminates the capacity-check overhead of `push_back()`.
    * **Inclusive Bounds:** Missing that the target `n` is inclusive. The array must be sized to `n + 1`, and the iteration condition must be `i <= n`. 
* **Time & Space Complexity:** O(N) Time / O(N) Space (for the output array).
* **The Struggle & Insights:**
    * **The Brute Force Trap:** The immediate instinct was to wrap the $O(1)$ Brian Kernighan's algorithm in a `for` loop. While perfectly functional, doing that for $N$ numbers results in a slightly slower $O(N \log N)$ overall time.
    * **The DP Breakthrough:** Realizing that binary patterns build on top of previously computed smaller numbers. Right-shifting a number by 1 (`i >> 1`) lops off the final bit. Because the loop iterates upward from 0, the answer for that right-shifted prefix has *already been calculated*.
    * **The Visualization:** Take `10` (Binary `1010`). If you split by right-shifting it, you get `5` (Binary `101`) and the last bit ( `10 & 1 = 0`). We already know how many `1`s are in `5`. We just take that cached answer and add whatever the last bit of `10` happens to be (using `10 & 1`). Thus, the entire problem is reduced to a single $O(1)$ lookup and addition per number.

### [4] Reverse Bits

* **The Core Pattern:** Bit-by-Bit Construction. Extract the Least Significant Bit (LSB) from `n`, append it to a `result` variable, and shift both variables in opposite directions.
* **The "Gotcha":**
    * **Shift Order:** Shifting `result` *after* appending the bit will result in one extra, accidental left-shift at the very end of the loop, giving a completely wrong answer. You must left-shift `result` *before* inserting the new bit.
    * **OR vs Addition:** Using `result | bit` and `result + bit` are functionally identical here because the target slot in `result` is guaranteed to be `0` after a left-shift. However, bitwise OR (`|`) is the standard idiom and clearly communicates intent.
    * **The "Optimization" Trap:** Short-circuiting the loop when `n == 0` and shifting the remainder (`res << (32 - count)`) is logically very clever! However, in C++, a fixed `for (int i = 0; i < 32; ++i)` loop is generally faster. A fixed loop allows the compiler to completely "unroll" the instructions. Adding a dynamic `while (n > 0)` introduces branch-prediction overhead and requires extra CPU cycles to track the `count` variable. 
* **Time & Space Complexity:** O(1) Time / O(1) Space.
* **The Struggle & Insights:** 
    * Getting stuck on the sequence of operations. Realized that left-shifting the new number *before* appending the bit is the correct approach, followed by right-shifting `n` to discard the processed bit.
    * Experimented with loop optimizations. Figured out how to mathematically terminate early by tracking the number of processed bits and manually padding the remaining zeros at the end with a bulk left-shift of `32 - count`.


### [5] Missing Number

* **The Core Pattern:** Cumulative XOR (Index vs Value) OR Mathematical Summation (Gauss's Formula).
* **The "Gotcha":**
    * **Out-of-Bounds Segfaults:** The array contains $n$ numbers but has a size of $n$ (missing one element). If you try to iterate with `i <= n` to cover the full mathematical range and simultaneously access `nums[i]`, you will hit an out-of-bounds segfault on the very last iteration. You must loop strictly with `i < n` and incorporate the final $n$ either by XORing `i + 1` inside the loop or initializing your tracking variable to $n$.
    * **Math Overflow:** The alternative solution is using Gauss's formula: $\frac{n(n+1)}{2}$ minus the sum of the array. While mathematically elegant and also $O(N)$ time, it is highly susceptible to Integer Overflow if $n$ is exceptionally large. The XOR approach is completely immune to overflow, making it the safer, production-grade choice.
    * **Modern C++ `std::ssize()`:** Relying on `nums.size()` returns an unsigned `size_t`, which often triggers compiler warnings when compared against signed integers like `int i`. Using `std::ssize(nums)` (C++20) returns a signed integer, cleanly bypassing the need for `static_cast<int>(nums.size())`.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **The XOR Pairing:** Realized that XORing every index and every value together will cause all present numbers to cancel out with their corresponding indices (since $x \oplus x = 0$), leaving only the missing number behind.
    * **Loop Design:** Debated between using `i + 1` within the loop versus altering the termination condition, ultimately realizing the latter breaks array boundaries. 
    * **Multiple Solutions:** Discovered the mathematical summation approach as a viable alternative to bit manipulation, highlighting the importance of recognizing different paradigms for the same problem.

### [6] Reverse Integer

* **The Core Pattern:** Digit Extraction (`% 10`) and Rebuilding (`res = res * 10 + digit`), combined with proactive Overflow Bounding.
* **The "Gotcha":**
    * **The `INT32_MIN` Trap:** Attempting to convert negative numbers to positive using `abs(x)` or `x * -1` will cause undefined behavior/overflow if `x` is `INT32_MIN`. The maximum positive value of a 32-bit signed int is `2147483647`, while the minimum is `-2147483648`.
    * **Modulo Sign Retention:** In C++, the `%` operator natively retains the sign of the dividend (e.g., `-123 % 10 = -3`). You do not need to temporarily convert negative numbers to positive!
    * **Loop Condition:** When natively handling both positive and negative numbers, the loop condition must be `while (x != 0)`, not `while (x > 0)`.
* **Time & Space Complexity:** O(log(x)) Time (where base is 10, meaning the number of digits) / O(1) Space.
* **The Struggle & Insights:**
    * **Operator Swap:** Accidentally swapped `%` (for extracting the last digit) and `/` (for discarding the last digit). A quick reminder: modulo gets the remainder, division chops the number down.
    * **Redundant Logic:** Started by storing the negative state and converting to positive, which overcomplicated the loop. Simplifying it to just handle the negative digits natively made the code much cleaner.
    * **Optimization:** Precomputed the overflow bound (`INT32_MAX / 10`) into a local variable before the loop. While highly optimized, production-grade modern C++ compilers handle this automatically, pulling it into a `constexpr` is still an excellent habit for writing self-documenting code.


### [7] Sum of Two Integers

* **The Core Pattern:** Half-Adder Logic via Bit Manipulation. 
    * Base Addition (without carry) = `a ^ b`
    * Carry Generator = `(a & b) << 1`
* **The "Gotcha":**
    * **Negative Number Infinite Loops:** Attempting a bit-by-bit extraction (`num >> 1`) inside a `while (num != 0)` loop will infinite-loop on negative numbers because arithmetic right-shifts fill with `1`s to preserve the sign. You must strictly bound it to a 32-iteration `for` loop if doing it manually.
    * **C++ Undefined Behavior:** In C++, left-shifting a negative number is technically undefined behavior (UB). To safely calculate the carry, you must cast the bitwise AND to an unsigned integer before shifting: `unsigned int carry = (unsigned int)(a & b) << 1;`.
* **Time & Space Complexity:** $O(1)$ Time / $O(1)$ Space. (Runs in a maximum of 32 iterations).
* **The Struggle & Insights:**
    * **The Full-Adder Trap:** Spent a long time trying to build a literal hardware full-adder in software using truth tables. It was overly complex and failed on negative numbers due to sign-extension edge cases.
    * **The Base-10 Epiphany:** The optimal approach calculates the "base sum" and the "carries" separately, then adds them together until there are no carries left. This is best visualized in base-10:
        * `999 + 999`
        * Base sum (ignoring carries): `9+9=8`, `9+9=8`, `9+9=8` $\rightarrow 888$
        * Carries: `9+9` generates a `1`, shifted left $\rightarrow 1110$
        * Next pass: `888 + 1110`. 
        * Base sum: `8+0=8`, `8+1=9`, `8+1=9`, `0+1=1` $\rightarrow 1998$.
        * Carry: $0$. 
        * Since the carry is $0$, the loop terminates. The exact same cascading logic applies to base-2 with `^` and `&`.