# Two Pointers - Topic Guide

## 1. Core Fundamentals

### Pointer Types & Indexing
* **Unsigned vs. Signed:** Standard library `.size()` returns an unsigned `size_t`. If a pointer/index decrements below `0`, it underflows to a massive positive number, causing out-of-bounds segfaults. Always prefer signed integers (`int left = 0, right = std::ssize(nums) - 1;`) for bidirectional two-pointer logic.
* **`std::string_view`:** A lightweight, non-owning view of a string (just a pointer and a size). It should almost always be passed **by value** (`std::string_view s`). Passing it as `const std::string_view` or `const std::string_view&` is redundant and can actually degrade performance via unnecessary indirection.

### Character Evaluation (`<cctype>`)
* **The `unsigned char` Requirement:** Functions like `std::isalnum`, `std::tolower`, and `std::toupper` technically take an `int` argument. Passing a raw `char` can lead to Undefined Behavior (UB) if the character is negative (like some extended ASCII characters). Always cast: `std::isalnum(static_cast<unsigned char>(c))`.

---

## 2. General Summary / Quick Reference

When tackling Two Pointers problems, keep these core patterns in mind:

* **Opposite Ends (Left/Right):** Start at `0` and `size - 1`. Used for palindromes, reversing arrays, or finding sums in sorted arrays. 
    * *Golden Rule:* Always use a `while (left < right)` outer loop.
* **Same Direction (Fast/Slow):** Both start at `0`. Used for cycle detection, removing duplicates, or finding the middle of a list.
* **Inner Loop Boundaries:** If you use inner `while` loops to skip elements, **you must repeat the bounds check** (`left < right`) inside the inner loop condition to prevent the pointers from crossing over each other.

---

## 3. Problem Strategies & Patterns

### [1] Valid Palindrome

* **The Core Pattern:** Opposite Ends Two-Pointer. Shrink from both sides simultaneously, skipping non-alphanumeric characters, and comparing the resulting valid characters case-insensitively.
* **The "Gotcha":**
    * **`size_t` Underflow:** Initializing `right = s.size()` instead of `s.size() - 1` is an instant out-of-bounds error. Using `size_t` instead of `int` makes it impossible to safely check `right >= 0` without risking catastrophic underflow. Use `std::ssize(s) - 1`.
    * **Undefined Behavior in `<cctype>`:** Forgetting to cast characters to `unsigned char` before passing them to `std::tolower` or `std::isalnum` is technically UB in C++. 
    * **Inner Loop State Traps:** Using a local cached variable (like `leftChar = s[left]`) inside an inner `while` loop condition causes an infinite loop because `leftChar` never updates as `left` increments. Always evaluate the live array access: `while (left < right && !std::isalnum(static_cast<unsigned char>(s[left])))`.
    * **Post-Skip Boundary Check:** After the inner loops finish skipping spaces/symbols, you must verify `if (left >= right) break;` *before* doing the character comparison, otherwise you might compare crossed pointers.
    * **Branch Predictor Penalties:** Using an `if (!isalnum) { left++; continue; }` forces the CPU to constantly guess the branch outcome and jump back to the top of the outer loop. Tightly packed inner `while` loops are much faster for the hardware.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * Transitioning to C++20 `std::string_view` simplifies passing the string, but remembering it doesn't need `const` takes some unlearning. 
    * Refined the mechanics of skipping elements. Found the optimal setup: an outer `while (left < right)`, followed by two inner `while` loops for skipping invalid chars (with their own `left < right` bounds checks), followed by a lower-case comparison.