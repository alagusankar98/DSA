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

### [3] Koko Eating Bananas

* **The Core Pattern:** Binary Search on Answers (Boundary Finding). Instead of searching a physical array, your search space is a range of *potential answers*: from `1` (minimum possible speed) to `max(piles)` (maximum useful speed). The validation function checks if a speed allows finishing within `h` hours. Because speed linearly correlates with time, the validation array maps out monotonically: `[False, False, True, True, True]`. You want the *first* `True`.
* **The "Gotcha":**
    * **Integer Ceiling Math:** Using `(pile / mid) + ((pile % mid == 0) ? 0 : 1)` relies on modulo and branching. Using `std::ceil((double)pile / mid)` requires expensive floating-point conversions. The FAANG systems standard for integer ceiling is **`(pile + mid - 1) / mid`**. This achieves a perfect ceiling using pure, branchless integer math.
    * **The `std::max_element` Trap:** `std::max()` only compares two values. For a vector, you must use `std::max_element(piles.begin(), piles.end())`. But crucially, this returns an *iterator*. You must dereference it `*std::max_element(...)` to get the value, which means you **must** ensure `!piles.empty()` first, otherwise you dereference `.end()` and trigger a Segfault (Undefined Behavior).
    * **Summation Overflow:** Target `h` is an `int`, but the *sum* of the hours taken at speed `1` on a massive array of piles will easily exceed 2.14 Billion. You must use `int64_t` or `uint64_t` for your running total of hours inside the validation function.
* **Time & Space Complexity:** $O(N \log M)$ Time (where $N$ is piles size, $M$ is max pile size) / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Why `return left;` works:** It feels much safer to track `minH = min(minH, mid)` inside the `hours <= h` block. But let's prove why `return left;` is perfectly safe:
        1. We only move `left = mid + 1` when the speed is **invalid** (`False`). Therefore, `left` can never settle on an invalid answer.
        2. We only move `right = mid - 1` when the speed is **valid** (`True`). We are aggressively squeezing the right boundary down to find a smaller valid answer.
        3. When the loop finally breaks (`left > right`), `right` has squeezed past the valid boundary and is resting on the last `False`. `left` has just pushed past the invalid boundary and is resting exactly on the very first `True`. 
        4. Thus, when the loop ends, `left` is mathematically guaranteed to be pointing at the minimum valid speed. Trusting `left` here is the ultimate mark of binary search mastery.

### [4] Find Minimum in Rotated Sorted Array

* **The Core Pattern:** Binary Search on Rotated Arrays. The goal is to find the "pivot" (the minimum element) by constantly comparing `nums[mid]` against `nums[right]` to determine which half is perfectly sorted and which half contains the anomaly.
* **Addressing The Core Doubts:**
    * **Direction & Duplicates:** Standard FAANG problems implicitly assume an *ascending* array that was rotated *clockwise*. (Anti-clockwise rotation by $K$ is mathematically identical to clockwise rotation by $N-K$, so the algorithm is the exact same). Standard versions also assume *unique* elements. If there are duplicates (e.g., `[3, 1, 3, 3]`), you can't tell which half is sorted, and the $O(\log N)$ guarantee collapses into $O(N)$.
    * **The "Sorted Segment" Rule (`nums[left] < nums[right]`):** Why is `left` automatically the minimum if this is true? Because a rotated array only has exactly *one* drop-off point. If `nums[left] < nums[right]`, it proves the drop-off point is *not* inside this segment. The segment is perfectly increasing, meaning the leftmost element is mathematically guaranteed to be the smallest.
    * **Asymmetric Bounds (`right = mid` vs `left = mid + 1`):** 
        * When `nums[mid] > nums[right]`, you *know* the pivot is to the right. But could `mid` itself be the minimum? **No**, because it is strictly greater than `nums[right]`. Since `mid` is impossible, you safely skip it: `left = mid + 1`.
        * When `nums[mid] < nums[right]`, the right half is sorted, so the pivot is to the left. But could `mid` itself be the minimum? **Yes!** If you just dropped into the sorted half, `mid` might be the start of it. Because `mid` is a valid candidate, you *cannot* discard it: `right = mid`.
        * *The Ghost Equality (`<` vs `<=`):* If you use `if (nums[mid] < nums[right])` and catch the rest with `else`, your `else` block technically catches `>=`. Why does this work without explicitly checking `>`? Because the `=` condition is mathematically impossible. Since all elements are unique, `nums[mid]` only equals `nums[right]` if `mid == right`. However, because integer division `mid = left + (right - left) / 2` always rounds down, and `left` is strictly less than `right` (enforced by the early return guard), `mid` will **never** equal `right`. Thus, the `else` block acts purely on `nums[mid] > nums[right]`.
        * *The Early Return Guard (`nums[left] <= nums[right]`):* If you use a `while (left <= right)` loop, checking if the current boundary is perfectly sorted *before* calculating `mid` is a massive optimization. If `nums[left] <= nums[right]`, you instantly `return nums[left];`. Not only does this save iterations by exiting the moment a sorted segment is found, but it completely nullifies the `left == right` infinite loop trap. When the pointers converge, `nums[left] <= nums[left]` is guaranteed to be true, safely returning the answer before `mid` is even calculated.
* **The "Gotcha":**
    * **The Empty Trap:** Failing to check `!nums.empty()` at the very beginning is lethal. If the array is empty, setting `right = nums.size() - 1` causes an unsigned integer underflow. `size_t` flips to `18,446,744,073,709,551,615`, instantly throwing you into an infinite loop or Segfault.
* **Time & Space Complexity:** $O(\log N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **The Convergence Rule:** Setting the loop to `while (left < right)` instead of `<=` is required here. Because we assign `right = mid`, using `<=` would cause an infinite loop when `left == right`. The search logically stops when the two pointers collapse onto the single minimum element.
