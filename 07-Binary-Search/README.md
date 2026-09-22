# Binary Search - Topic Guide

## 1. Core Fundamentals

Binary Search is the ultimate $O(\log N)$ optimization pattern. It strictly requires a **sorted** search space (or a space that exhibits a monotonic boolean property, like "False, False, True, True").

### The Standard Template
The safest and most universal Binary Search skeleton prevents infinite loops and integer overflows:
```cpp
int left = 0;
int right = static_cast<int>(nums.size()) - 1;

while (left <= right) {
    // PREVENTS INTEGER OVERFLOW! Do NOT use (left + right) / 2
    int mid = left + (right - left) / 2;
    
    if (nums[mid] == target) {
        return mid; // Found it
    } else if (nums[mid] < target) {
        left = mid + 1; // Discard left half
    } else {
        right = mid - 1; // Discard right half
    }
}
return -1; // Not found
```

### The C++ STL Mastery (`<algorithm>`)
As a Systems Engineer, you must know that C++ provides natively optimized Binary Search functions. If a problem is a trivial binary search, you can often use these. If the problem asks you to *implement* it, knowing these exist proves your STL mastery.
* **`std::binary_search(begin, end, val)`:** Returns a `bool`. Is it there or not?
* **`std::lower_bound(begin, end, val)`:** Returns an iterator to the **first** element that is $\ge$ `val`. (Crucial for finding the start of duplicates).
* **`std::upper_bound(begin, end, val)`:** Returns an iterator to the **first** element that is strictly $>$ `val`.

---

## 2. General Summary / Quick Reference

When tackling Binary Search problems, keep these core variations in mind:

### Variation 1: The Exact Match
The standard template. Search a sorted array for a specific target. `left <= right` with `mid +/- 1` bounds update.

### Variation 2: Finding Boundaries (First / Last Occurrence)
Instead of returning immediately when `nums[mid] == target`, you record the index and *keep searching* in the left half (to find the first occurrence) or the right half (to find the last occurrence).

### Variation 3: Search Space on Answers (Meta Binary Search)
The hardest and most common FAANG variation (e.g., Koko Eating Bananas). You aren't searching an array of numbers; you are searching a *range of possible answers* (like "1 to 1000 bananas per hour"). You guess a speed (`mid`), run a verification function, and binary search your guess up or down.

---

## 3. Problem Strategies & Patterns

### [1] Binary Search

* **The Core Pattern:** The Exact Match Template. Establish `left` and `right` boundaries. Calculate `mid`. If `nums[mid]` is too large, discard the right half. If it's too small, discard the left half.
* **The "Gotcha":**
    * **The `<=` Condition:** Using `while (left < right)` is a fatal logic error for finding exact matches. If the target is sitting exactly at the boundary where `left` and `right` converge (or if the array only has 1 element), the loop will break before evaluating it. You *must* use `<=` to evaluate the final remaining index.
    * **The Integer Overflow Trap:** Using `mid = (left + right) / 2` is mathematically correct but fundamentally broken in systems engineering. If `left` and `right` are both extremely large indices (e.g., both $> 1$ Billion), their sum exceeds the 32-bit signed integer limit (`2.14 Billion`), overflowing into a negative number and instantly segfaulting your array lookup. Always use `left + (right - left) / 2`.
    * **Directional Awareness:** Do not blindly memorize `right = mid - 1`. That only works for an ascending array. If the array is strictly descending, a number *exceeding* the target means the target is further right, requiring `left = mid + 1`. Always anchor your logic to the sort direction.
* **Time & Space Complexity:** $O(\log N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Visualization:** Taking the time to visualize the search space halving was critical. It makes the bounds logic (`mid - 1` vs `mid + 1`) intuitive rather than just memorized syntax.
    * **Systems-Level Math:** Learned the integer overflow trick exactly as documented in the core fundamentals. It is one of the most commonly tested "hidden" traps in FAANG interviews.

### [2] Search a 2D Matrix

* **The Core Pattern:** Binary Search Expansion. You can approach this in two ways: 
    1. **Two-Pass:** Run a binary search on the first column to locate the exact row (`matrix[mid][0] <= target && target <= matrix[mid][cols-1]`). Once the row is found, run a standard binary search on that specific row.
    2. **Simulated 1D:** Treat the total elements as `M * N`. Run a standard binary search from `0` to `total - 1`, converting `mid` back into 2D coordinates on the fly.
* **The "Gotcha":**
    * **The Empty Trap:** Checking `!matrix.empty()` isn't enough. You must also check `!matrix[0].empty()` before reading `matrix[mid][0]`.
    * **The 1D Coordinate Math:** `row = mid / cols` and `col = mid % cols`. Why doesn't the total number of `rows` matter in this calculation? Because `cols` represents the physical width of one chunk. Dividing by width tells you how many full chunks (rows) you've bypassed. Modulo tells you your exact offset within the current chunk.
    * **Multiplication Overflow:** If using the 1D approach, `int total = rows * cols` can instantly overflow if the matrix is large. You must cast it: `int64_t total = static_cast<int64_t>(rows) * cols;`.
    * **The Hardware Reality (ALU Costs):** The Simulated 1D approach looks cleaner on paper, but the **Two-Pass approach is actually faster**. Why? The 1D approach forces the CPU to execute integer division (`/`) and modulo (`%`) on *every single iteration* of the loop. Division is incredibly expensive for the ALU compared to the simple pointer math/comparisons used in the Two-Pass approach.
    * **The "Contiguous" Trap:** You cannot use raw pointer arithmetic to jump between rows in a `std::vector<std::vector<int>>`. Unlike a flat C-style array `int[5][5]`, a vector of vectors is an array of pointers to scattered heap allocations.
* **Time & Space Complexity:** $O(\log(M \times N))$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Logic Fumbles:** Initially flipped the `>=` and `<=` logic when searching for the row bounding box.
    * **Mathematical Visualizations:** Spent time mapping out `mid / cols` by hand to finally trust the math. Realizing that the number of rows only dictates the *maximum* possible `mid`, but has zero impact on extracting coordinates from a given `mid`, was a massive lightbulb moment.
