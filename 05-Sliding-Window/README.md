# Sliding Window - Topic Guide

## 1. Core Fundamentals

Initializing `left = 0` and `right = 0` and expanding with a `while (right < n)` loop is the universal skeleton for almost every Sliding Window problem. 
While standard Two Pointers start at opposite ends and converge, a Sliding Window starts both pointers at `0` and moves them in the same direction, expanding and contracting like a caterpillar.

By standardizing on this `right < n` expansion loop, you mathematically guarantee $O(N)$ time complexity because both `left` and `right` only ever move forward. Each element is processed exactly twice at most (once when `right` enters, once when `left` exits).

---

## 2. General Summary / Quick Reference

When tackling Sliding Window problems, keep these core patterns in mind:

### Variation 1: The Dynamic Window (Constraint-Based)
The window size grows and shrinks dynamically based on a specific condition.
* **The Expansion:** Always increment `right` unconditionally at the end of the loop to ingest new data.
* **The Contraction:** Use an inner `while` loop (or an `if` statement) to advance `left` whenever your window violates the problem's constraints (e.g., "sum is too large," "found a duplicate character," or "found a cheaper buy price").

```cpp
int left = 0, right = 0;
while (right < n) {
    // 1. Add nums[right] to current window state

    // 2. While window is INVALID, remove nums[left] and left++
    while (/* window violates constraint */) {
        // remove nums[left] from state
        left++;
    }

    // 3. Update max/min answer using valid window
    // 4. right++;
}
```

### Variation 2: The Fixed Window (Size-Based)
The window size is explicitly exactly `k`.
* **The Expansion:** Advance `right` to ingest data exactly like the dynamic window.
* **The Slide:** Once your window hits size `k` (when `right - left + 1 == k`), you evaluate your answer. Then, on every subsequent iteration, you must move both `left` and `right` simultaneously to maintain that exact size.

```cpp
int left = 0, right = 0;
while (right < n) {
    // 1. Add nums[right] to state

    if (right - left + 1 == k) {
        // 2. Update answer
        // 3. Remove nums[left] from state
        // 4. left++;
    }
    // 5. right++;
}
```

---

## 3. Problem Strategies & Patterns

### [1] Best Time to Buy and Sell Stock

* **The Core Pattern:** Dynamic Sliding Window. The `left` edge represents your buy day, and the `right` edge represents your sell day. Expand `right` every iteration to check new prices.
* **The "Gotcha":**
    * **The Slide Condition:** If you encounter a price at `right` that is *lower* than your price at `left`, your current window is obsolete. You would never buy at the higher `left` price when you could buy at the new, lower `right` price. Collapse the window by moving `left` directly to `right` (`left = right`), instantly starting a new potential holding period.
    * **Implicit vs Explicit Window:** You can solve this by explicitly managing `left` and `right` indices, or implicitly by just tracking a `minLeft` variable and comparing it to the current price. Using `left` and `right` directly maps to the universal sliding window pattern.
    * **Variable Initialization:** When tracking global minimums/maximums, initialize them correctly. For example, a tracking minimum (`minLeft`) should be initialized to `std::numeric_limits<int>::max()`, NOT `INT_MIN`. (Using `<limits>` is also preferred over C-style macros).
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Bridging Two Pointers to Window:** Initially struggled to see how this differed from Two Pointers. Realized that instead of converging from opposite sides, both start at `0` and move right. `right` scouts ahead, and `left` snaps forward whenever a better starting point is found.
    * **Simulating the Pointer Mechanics:** Hand-traced the logic to prove it works. E.g., for `[7, 1, 5]`, `right` hits `1`, it's lower than `left` (7), so `left` jumps to the index of `1`. Then `right` moves to `5`, finding a valid profit. The caterpillar movement clicked.

### [2] Longest Substring Without Repeating Characters

* **The Core Pattern:** Dynamic Sliding Window + Index Cache. Expand the `right` pointer to read characters. Use a cache to store the last seen index of each character. If a duplicate is found, instantly snap the `left` pointer to the index right after the duplicate.
* **The "Gotcha":**
    * **The Rewind Violation:** Never move the `right` pointer backward. Doing so destroys the $O(N)$ time complexity and corrupts the window state. The sliding window strictly expands `right` forward and shrinks `left` forward.
    * **Hardware Optimization (`std::array` vs `std::unordered_map`):** Since the ASCII space is exactly 256 characters, using a `std::unordered_map` is a massive anti-pattern. It triggers heap allocations and cache misses. A flat `std::array<int, 256>` initialized to `0` takes exactly 1KB, fits perfectly in the L1 CPU cache, and provides pure $O(1)$ pointer-arithmetic lookups.
    * **The "Ghost Character" Trap (`std::max`):** When jumping `left` forward, you must use `left = std::max(left, cache[currentChar])`. Because the global cache remembers the *entire* string's history, looking up a character you saw previously might return an index that is *behind* your current `left` pointer. Without `std::max`, your `left` pointer would jump backward, re-introducing old duplicates into your active window (e.g., the string "abba").
    * **The `+1` Initialization Trick:** Instead of filling the array with `-1` and doing `cache[char] + 1`, you can zero-initialize the array (`{0}`) and store `right + 1`. This allows a seamless check: `if (cache[char] != 0)` and direct assignment `left = std::max(left, cache[char])`.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space (Strictly bounded 256-element array).
* **The Struggle & Insights:**
    * **Map Confusion:** Initially struggled with how to track elements and defaulted to `unordered_map`, not realizing the hardware-level implications for small, fixed domains.
    * **Sliding Window Mechanics:** First instinct was to reset `right` backward upon finding a duplicate. The breakthrough was understanding that the window only needs to *shrink* from the `left` by jumping it past the old duplicate, leaving `right` exactly where it is to continue exploring.
