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
