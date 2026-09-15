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

### [2] Two Sum II - Input Array Is Sorted

* **The Core Pattern:** Opposite Ends Two-Pointer. Calculate `sum = nums[left] + nums[right]`. If the sum is too large, decrement `right`. If it's too small, increment `left`. 
* **The "Gotcha":**
    * **1-Indexed Return Constraints:** Always read the problem constraints carefully. Returning exactly what you are asked for (e.g., `return {left + 1, right + 1};`) prevents failing simple test cases.
    * **The Subtraction Trap:** Avoid calculating the difference (e.g., `int diff = (nums[left] + nums[right]) - target`). While mathematically sound, it introduces an unnecessary subtraction instruction for the ALU and opens the door for integer overflow/underflow if the values are extreme. Calculate `sum` and compare it directly to `target`.
* **Time & Space Complexity:** O(N) Time / O(1) Space.
* **The Struggle & Insights:**
    * **Visualizing the Proof:** Struggled to trust that moving the pointers wouldn't accidentally skip the target pair. The logic holds because the array is sorted. If `sum > target`, the current `right` value is too large even when paired with the *smallest* available value (`left`). It will inherently be too large for any other remaining value, meaning `right` can be permanently discarded. The same logic applies inversely for `left` when `sum < target`.
    * **ALU Optimization:** Iterated on the condition checks, moving from a multi-step difference calculation to a clean, highly optimized direct comparison.

### [3] 3Sum

* **The Core Pattern:** Sort Array + Anchor and Two-Pointer. Sort the array first. Iterate through the array with an `anchor` element. For each `anchor`, run a standard Two Sum II (Opposite Ends) on the remaining elements to find a sum of `0` with the anchor.
* **The "Gotcha":**
    * **Duplicate Skipping (Outer Anchor):** If the current `anchor` is the same as the previous `anchor`, you must `continue` to avoid generating duplicate triplets.
    * **Duplicate Skipping (Inner Pointers):** When a valid triplet is found (`sum == 0`), you must advance the `left` pointer and decrement the `right` pointer, then advance `left` while skipping over any duplicates to avoid identical triplets. Doing this *only* when `sum == 0` is critical; if you skip duplicates normally for every step, you might skip a valid pair where two identical numbers are required (e.g., `[-2, 1, 1]` requires both `-1`s).
    * **Early Exit for Positive Anchor:** Because the array is sorted, if the `anchor` element becomes strictly greater than `0`, it is mathematically impossible for the sum of three numbers to ever reach `0` (since all subsequent elements will also be positive). Break the outer loop immediately to save CPU cycles.
* **Time & Space Complexity:** $O(N^2)$ Time / $O(1)$ or $O(N)$ Space (depending on the sorting algorithm implementation).
* **The Struggle & Insights:**
    * **The "Ah-Ha" Moment:** Initially stared at the problem without realizing the optimal solution. The realization was that by fixing one number (the anchor), the problem perfectly reduces down to the standard Two Sum problem, which is easily solved with two pointers *if* the array is sorted.
    * **Understanding Inner Skips:** Struggled with why we only skip inner duplicates when `sum == 0`. Realized that skipping them generically outside of this condition could break valid solutions where duplicates are required to sum to the target, whereas skipping *after* a successful find prevents identical triplets.