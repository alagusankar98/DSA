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

### [3] Longest Repeating Character Replacement

* **The Core Pattern:** Dynamic Window shifting to Fixed Window via Invariants. The master formula to check if a window is valid is: `[Length of Window] - [Count of Most Frequent Character] <= k`. In physical terms: `Total Letters - Popular Letters = Garbage Letters`. If `Garbage <= k`, the window is valid.
* **The "Gotcha":**
    * **The `if` vs `while` Optimization:** Because `right` increments by exactly `1` every iteration, the window size grows by exactly `1`. If the previous window was valid, the new window can violate the `k` constraint by *at most* `1`. Therefore, an inner `while` loop is entirely unnecessary. A simple `if` statement to increment `left` once perfectly maintains the bounds and removes a layer of CPU branch prediction overhead.
    * **"Shift, Don't Shrink":** Because we only care about finding the *maximum* length, we don't actually need to shrink the window when it becomes invalid. When the `if` condition triggers, both `right` (at loop end) and `left` increment. The invalid window physically shifts to the right, maintaining its peak size, waiting to ingest a character that makes it valid again.
    * **The "Historical Max" (Stale Data):** When `left` increments and kicks a character out of the window, you decrement its count in your array, but you *do not* rescan the array to decrease `maxFrequency`. Why? Because `maxFrequency` acts as a high-water mark. Since we only want to beat our current maximum window size, we mathematically cannot do that until we find a *new* character whose frequency exceeds the historical peak. Leaving the data "stale" keeps the inner loop strictly $O(1)$.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space (26-element array).
* **The Struggle & Insights:**
    * **Abstract Math vs Physical Frame:** Initially got lost trying to anchor the target character strictly to `s[left]`. The breakthrough was dropping the abstract math and visualizing the window as a physical box: count the most popular letter, subtract it from the total, and check if the remaining "odd ones out" can be covered by the `k` magic wands.
    * **Discovering the Invariant:** Discovered independently that because `right` increments by 1, the garbage limit can only fail by 1. This logically eliminates the need for a `while` loop. Recognizing these mathematical invariants is the exact differentiator between a standard brute-force slider and a senior-level optimized shifting window.

### [4] Permutation in String

* **The Core Pattern:** Fixed-Size Sliding Window. A permutation of `s1` inside `s2` must be the exact length of `s1`. The window initializes to the length of `s1`, and then physically slides across `s2` by incrementing both `left` and `right` simultaneously. Use a single delta array (`diffCount`) to track character frequencies: `(+)` means `s1` needs more, `(-)` means `s2` has an excess, and `0` means perfect equilibrium. 
* **The "Gotcha":**
    * **Delta Arithmetic Inversion:** If your `diffCount` array is built by adding `s1` and subtracting `s2` characters initially, you must mirror that logic correctly when the window slides. The character leaving at `left` reduces `s2`'s footprint, meaning you must ADD to the delta (`diffCount[left]++`). The character entering at `right` increases `s2`'s footprint, meaning you must SUBTRACT (`diffCount[right]--`). Flipping these operators will completely destroy the state map.
    * **The Bitmask Optimization (O(1) Matches):** Instead of looping through the 26-element array every slide to check for all `0`s, use a `uint32_t matchBits` variable. Think of it as a panel of 26 lightbulbs. When a letter's delta hits `0`, turn its bit ON (`matchBits |= (1 << index)`). If it deviates from `0`, turn it OFF (`matchBits &= ~(1 << index)`). A valid permutation is found the instant all 26 bits are ON, represented by the hex mask `0x03FFFFFF`.
    * **The Same-Character Bypass:** A massive, free hardware optimization. If the character being ejected at `left` is the exact same character being ingested at `right` (e.g., sliding past an 'a' to ingest an 'a'), the mathematical state of the window has not changed at all. Doing a simple `if (leftChar == rightChar) continue;` completely bypasses two memory writes and multiple bitwise operations, keeping the CPU blazing fast.
* **Time & Space Complexity:** $O(N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Understanding Fixed vs Dynamic:** Struggled initially because I was trying to apply dynamic window stretching to a problem that requires a perfectly rigid frame.
    * **Tracking the Invariant:** Failed a cheeky optimization attempt by inverting the math operators. Learned that in systems engineering, if you define a memory state (like "s2 subtracts from the pool"), you must rigorously trace and enforce that invariant through every single mutation.
    * **Bare-Metal Mastery:** Managed to take a conceptually heavy array-checking algorithm and successfully distill it into raw CPU register bit manipulation (`0x03FFFFFF`). The result is impenetrable, branchless (where possible), and highly idiomatic C++.

### [5] Minimum Window Substring

* **The Core Pattern:** Dynamic Sliding Window with Target Ledger. Use a single `std::array<int, 128>` as a live ledger. Map the target string's required frequencies (positive). Expand `right` to ingest characters (decrement the array). When the required characters reach 0, freeze `right` and shrink `left` to squeeze out garbage characters (increment the array) until a required character's count goes above 0, breaking the valid state.
* **The "Gotcha":**
    * **The Single-Array State Machine:** Don't use two arrays (one for targets, one for the current window). It requires synchronizing state and complex break conditions. A single delta array seamlessly handles garbage: if a character isn't needed, its count goes negative when ingested and back to 0 when ejected, never interfering with the `> 0` check used for required characters.
    * **Tracking Required vs Unique:** Don't track unique character matches. Track the total *number* of required characters (`int required = t.size()`). When you ingest a character whose count in the array is `> 0`, you decrement `required`. When `required == 0`, the window is valid. When you eject a character whose count hits `0` (meaning it's about to become positive again), you increment `required`, breaking the window.
    * **Sentinel Initialization:** Initialize `minLength` to `std::numeric_limits<size_t>::max()`. This acts as an untouchable sentinel. If it hasn't changed at the end of the function, safely return `""` instead of risking a string copy or out-of-bounds error.
    * **The "Late Eval" Micro-Optimization:** Inside the `while(required == 0)` shrink loop, DO NOT evaluate the `currentMinLength` and update `minLength` on every single iteration. For a fixed `right` anchor, the window size strictly decreases as `left` advances. Therefore, the absolute tightest window is mathematically guaranteed to be the exact state right before the loop breaks. Place the size calculation *inside* the `if (count == 0)` block that breaks the window. This bypasses thousands of redundant branch evaluations when skipping massive blocks of garbage characters.
* **Time & Space Complexity:** $O(|S| + |T|)$ Time / $O(1)$ Space (Strictly bounded 128-element array taking 512 bytes).
* **The Struggle & Insights:**
    * **String Pollution:** Initially struggled with how to define "valid" when the window contains garbage characters. The breakthrough was understanding that tracking `required` completely insulates the logic from pollution.
    * **Pointer Invalidation:** Ran into segfaults and inverted minimum checks early on. Learned to rigorously trace the array boundaries and use sentinel values correctly.
    * **The Micro-Optimization:** Proudly discovered the "Late Eval" optimization independently. Realized that computing sizes while skipping garbage is a waste of ALU cycles, moving the check exactly to the breaking point.
