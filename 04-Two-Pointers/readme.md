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

### [4] Container With Most Water

* **The Core Pattern:** Opposite Ends Two-Pointer. Start pointers at the edges (`left = 0`, `right = size - 1`). Calculate the current area: `min(height[left], height[right]) * (right - left)`. To maximize the area, always move the pointer that points to the shorter line inward, because keeping the shorter line strictly limits the maximum possible height for any future (and narrower) container.
* **The "Gotcha":**
    * **Pointer Skipping Optimization:** You can heavily optimize the algorithm by skipping subsequent lines that are shorter than or equal to the line you just moved away from. However, this must be implemented carefully using inner `while` loops.
    * **The `<=` Operator for Skipping:** When fast-forwarding, the inner condition *must* use `<=` (e.g., `while (left < right && height[left] <= heightLeft) left++;`). If you only use `<` (or try to look ahead with `height[left+1]`), you will get trapped in an infinite loop if the next line is exactly the *same* height as the current one. The pointer won't move past the duplicate, and the loop will spin forever.
    * **Why `<=` works over `<`:** If the next line is the same height, the container's width has decreased but the bottleneck height hasn't improved. Thus, the resulting area is mathematically guaranteed to be smaller. Therefore, lines of *equal* height must also be skipped to find a taller line.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Initial Intuition vs Optimization:** Found the standard $O(N)$ solution quickly on the first go by just moving the smaller pointer by 1. The challenge was trying to push it further by explicitly zooming past shorter lines.
    * **Boundary and Loop Traps:** Stumbled through several infinite loops and failed conditions (like `height[right-1] < heightRight`) during the optimization attempt. 
    * **The "Why":** The realization was that fast-forwarding requires actively consuming the elements (using the current pointer `left` or `right` and incrementing/decrementing it *inside* the condition check or loop body), and that identical heights offer zero benefit when width is decreasing, making the `<=` operator strictly necessary for the skip logic.

### [5] Trapping Rain Water

* **The Core Pattern:** Opposite Ends Two-Pointer with Tracking Maximums. Maintain `left` and `right` pointers, along with `leftMax` and `rightMax` variables to track the highest boundary seen from both sides. The key realization is that the water trapped above any column is determined strictly by `min(leftMax, rightMax) - height[i]`. By always moving the pointer corresponding to the smaller overall boundary (`height[left] < height[right]`), you guarantee that the *other* side's maximum is at least as high, meaning your local maximum is the true bottleneck.
* **The "Gotcha":**
    * **The Single Anchor Trap:** Attempting to use a single anchor index and a moving index looking for a larger boundary fails because it expects a larger right boundary that may never exist (e.g., if the array simply slopes downwards).
    * **Branchless Optimization:** You can completely eliminate nested `if/else` checks when updating the water counter. Instead of explicitly checking if the current height is less than the max, simply update the max *first*: `leftMax = max(leftMax, height[left])`. Then immediately add `leftMax - height[left]` to your total. If the height was a new maximum, this automatically evaluates to `0` (leaving the counter unchanged), avoiding the need for an `else` statement entirely!
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Initial Intuition:** Mind went numb initially. Tried to force the "Container With Most Water" single anchor logic, but quickly realized it breaks if the moving index never finds a taller anchor to "close" the trap. 
    * **The "Ah-Ha" Moment:** Processed the problem during a commute and realized the absolute necessity of a *second* anchor (tracking the max from both sides). The bottleneck logic clicked: if `height[left] < height[right]`, then `left` is the strictly limiting factor regardless of what heights exist in the middle, so we can safely calculate trapped water on the `left` and move it inward.
    * **Code Elegance:** The final logic optimization provided by a code review was brilliant. Calculating `max` first and letting `max - current` evaluate to `0` cleanly removes all inner conditional branching, making the code incredibly sleek.