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
* **Setting a Bit:** `n = n | (1 << i)`
* **Clearing a Bit:** `n = n & ~(1 << i)`
* **Toggling a Bit:** `n = n ^ (1 << i)`

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