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

### [5] Search in Rotated Sorted Array

* **The Core Pattern:** Segment Identification. You cannot run standard binary search directly on a rotated array. However, every time you calculate `mid`, **at least one half of the array is guaranteed to be perfectly sorted**.
    1. Identify the sorted half (e.g., if `nums[left] <= nums[mid]`, the left half is sorted).
    2. Ask: "Is the target mathematically inside this sorted boundary?" (e.g., `target >= nums[left] && target < nums[mid]`).
    3. If yes, it is guaranteed to be in that half. Discard the unsorted half.
    4. If no, it *must* be in the unsorted half. Discard the sorted half.
* **The "Gotcha":**
    * **The Nested Binary Search Trap:** A common beginner instinct is to identify the sorted half and immediately launch a *new, separate* standard binary search function on it. This causes messy code duplication. Simply adjusting your `left` and `right` pointers lets the main loop naturally converge without any helper functions.
    * **The 2-Element Infinite Loop:** If your first check is `if (nums[mid] == target) return mid;` and it fails, you **must** strictly exclude `mid` from your next boundaries (`left = mid + 1` or `right = mid - 1`). If you mistakenly do `right = mid`, the moment your search space drops to 2 elements (`left - right == 1`), `mid` will repeatedly equal `left`, it won't equal the target, `right` will become `mid`, and the pointers will never change. Infinite loop.
* **Time & Space Complexity:** $O(\log N)$ Time / $O(1)$ Space.
* **The Struggle & Insights:**
    * **Blind vs Informed Search:** Learned a profound binary search rule: You cannot make any mathematical assumptions about an unsorted array, but you *can* use a sorted half to completely rule out possibilities. Finding the sorted half first is the key that unlocks the problem.

### [6] Time Based Key-Value Store

* **The Core Pattern:** Data Design + Binary Search for "Floor". The data structure is an `unordered_map<string, vector<pair<int, string>>>`. Because the problem guarantees that timestamps are always added in strictly increasing order, every `vector` is naturally sorted just by calling `.emplace_back()`. This allows you to run $O(\log N)$ binary searches on the values.
* **The Mathematical Partition (`left` vs `right`):** 
    * When searching for a timestamp that isn't exactly in the array, you want the largest timestamp that is `< target`.
    * A common instinct is to create a variable like `closestValidIndex` and record `mid` every time you move right (`left = mid + 1`). This is safe, but fundamentally unnecessary.
    * In a standard `while(left <= right)` loop, `left` constantly hunts for invalid (too large) numbers by moving right, and `right` constantly hunts for valid (too small) numbers by moving left.
    * The exact moment the loop breaks (`left > right`), the pointers cross paths and perfectly partition the array. `left` will mathematically *always* land on the first element `> target`. `right` will mathematically *always* land on the last element `< target`. Thus, you can just return `array[right].second`. The algorithm's constraints force `right` into the exact position you want.
* **The "Gotcha":**
    * **Early Exit Optimizations:** You can completely bypass the $O(\log N)$ binary search with $O(1)$ bounds checks. If `timestamp < array.front().first`, it's before any records existed (`return ""`). If `timestamp >= array.back().first`, it's after the latest record (`return array.back().second`). While `right` would naturally point to `.back()` anyway, the early exit saves ALU cycles.
    * **Naming Conventions:** Avoid complex names like `personMoodMap_`. In a systems environment, simple and generic names like `data_` or `store_` are preferred for scalable data structures.
* **Time & Space Complexity:** `set()` is $O(1)$ Time. `get()` is $O(\log N)$ Time. Space is $O(N)$.
* **The Struggle & Insights:**
    * **Pointer Confidence:** Initially struggled to figure out how to catch the "closest" value if an exact match wasn't found. Doing the micro-execution trace by hand proved that the `right` pointer flawlessly tracks the floor boundary. Trusting the math of the crossover state is a massive milestone in binary search mastery.

### [7] Median of Two Sorted Arrays

> **Read this first (the one-paragraph summary):** You are *not* binary searching for a number. You are binary searching for a **cut**. Draw a vertical line through *both* arrays so that everything to the left of both lines forms the exact *first half* of the merged array. Because both arrays are already sorted, you never build the merged array — you just hunt for the correct cut position, and the median falls out of the **four elements physically touching the two cut lines**. This is the hardest problem in the Neetcode 150; it breaks almost everyone on the first pass. If it feels murky, that is normal.

* **The Core Pattern:** Binary Search on the **Partition** (not on a value). Run the search on the *smaller* array only, and let the larger array's cut be mathematically forced by it.

#### The Mental Model (build this before touching code)
Imagine both arrays merged and sorted into one line, then a wall dropped down the exact middle:
* **The Left Bag** = everything left of the wall. **The Right Bag** = everything right of it.
* The median lives *at the wall*. For it to be the true median, two things must hold:
    1. **Size:** the Left Bag holds exactly half the elements (or one extra when the total is odd).
    2. **Order:** *everything* in the Left Bag ≤ *everything* in the Right Bag.
* **The Left Bag is built from `Left A` + `Left B`.** That is the only combination that matters. If the total is odd, the single biggest number in that bag *is* the median — you never look at the Right side at all.
* **The Right side is only a tripwire.** Sorting already guarantees `Left A ≤ Right A` and `Left B ≤ Right B` inside each array. The one thing we *don't* get for free is how `Left A` compares to `Right B` (and vice-versa). So the Right side exists purely as an alarm system to prove the Left Bag is valid.

#### The Cut is a Wall, Not an Element (the #1 source of bugs)
`sectionA` is **the count of elements Array A contributes to the Left Bag**, not an index into A.
* If `sectionA == 2`, then A donates `nums1[0]` and `nums1[1]` to the Left Bag.
* Therefore the boundary elements are:
    * **Left max of A** = `nums1[sectionA - 1]` (the last element *before* the wall)
    * **Right min of A** = `nums1[sectionA]` (the first element *after* the wall)
* The two arrays are a **seesaw**: the Left Bag's total size is fixed, so once A's contribution is chosen, B's is forced: `sectionB = totalHalf - sectionA`.

#### The `+1` in `totalHalf = (n1 + n2 + 1) / 2`
This single offset lets one formula handle both parities via C++ truncating division:
* **Even total** (e.g. 8): `(8 + 1) / 2 = 4`. The `+1` is eaten by truncation → both sides get exactly half.
* **Odd total** (e.g. 9): `(9 + 1) / 2 = 5`. The Left Bag is forced to take the **extra** element.
* **The payoff:** when odd, the median is simply `max(leftMax)` — no branching on which side the middle landed. Without the `+1`, the extra element lands on the Right side and you'd need separate pointer math for odd vs even.

#### The Four Boundaries + Virtual Infinity
The wall can legally fall *outside* an array (A contributes nothing, or everything). Reading `nums1[sectionA - 1]` when `sectionA == 0` is `nums1[-1]` → segfault; reading `nums1[sectionA]` when `sectionA == n1` reads past the end → segfault. The fix is to treat a missing left side as `-∞` and a missing right side as `+∞`, which *always* pass the ≤ comparisons harmlessly:
```cpp
int maxLeftA  = (sectionA > 0)  ? nums1[sectionA - 1] : std::numeric_limits<int>::min();
int maxLeftB  = (sectionB > 0)  ? nums2[sectionB - 1] : std::numeric_limits<int>::min();
int minRightA = (sectionA < n1) ? nums1[sectionA]     : std::numeric_limits<int>::max();
int minRightB = (sectionB < n2) ? nums2[sectionB]     : std::numeric_limits<int>::max();
```

#### The Tripwire + Pointer Updates
Only **one** cross-boundary rule needs checking (the in-array order is free from sorting):
```cpp
if (maxLeftA <= minRightB && maxLeftB <= minRightA) {
    // PERFECT CUT. Left Bag is valid → compute median and return.
    if ((n1 + n2) % 2 != 0)
        return std::max(maxLeftA, maxLeftB);                                    // odd: biggest in Left Bag
    return (std::max(maxLeftA, maxLeftB) + std::min(minRightA, minRightB)) / 2.0; // even: average across the wall
} else if (maxLeftA > minRightB) {
    // A donated a number too big for the Left Bag → shrink A's contribution.
    rightA = sectionA - 1;
} else {
    // B spilled a big number into the Left Bag → A must contribute more.
    leftA = sectionA + 1;
}
```
Why the median needs *only* these four values: every other element in the Left Bag sits *behind* its champion (`maxLeftA` or `maxLeftB`) and every element in the Right Bag sits *ahead* of its champion. The number at the dead center of the universe can only be one of the four elements kissing the wall.

* **The "Gotcha":**
    * **Search bounds are `[0, n1]`, NOT `[0, n1 - 1]`.** The cut is a *count*, and A can legally contribute **all** `n1` of its elements to the Left Bag. Initialize `int rightA = n1;` (not `n1 - 1`). Capping at `n1 - 1` blocks the "A gives everything" scenario, which forces `sectionB` to demand more elements than B physically has, and the math collapses. *(This was the final one-line bug — everything else was correct.)*
    * **The Cut ≠ Element index.** Left boundary is `section - 1`, right boundary is `section`. Assuming the left half "includes `sectionA` itself" (using `nums[sectionA]` for the left max and `nums[sectionA + 1]` for the right min) misaligns the entire partition and leaves a gap.
    * **The Copy-Paste Index Bug:** When writing the four boundaries, it is fatally easy to write `minRightSectionB = nums1[...]` instead of `nums2[...]`. If B is the larger array, that reads out of bounds instantly. Every A-boundary indexes `nums1`; every B-boundary indexes `nums2`.
    * **Always search the smaller array:** Start with `if (nums1.size() > nums2.size()) return findMedianSortedArrays(nums2, nums1);`. This guarantees `sectionB = totalHalf - sectionA` never goes negative and pins the complexity to `O(log(min(m, n)))`.
    * **The `2.0` (not `2`) Trap:** In the even case, divide by `2.0` (and `static_cast<double>` the sum first) or integer division silently truncates your median.
    * **Defensive `> 0` / `< n` over `== 0` / `== size`:** Using range guards instead of strict equality means that even if a pointer ever drifted out of range from an upstream bug, the ternary gracefully falls back to the infinity sentinel instead of slipping past an `==` check into invalid memory.
* **Time & Space Complexity:** $O(\log(\min(m, n)))$ Time / $O(1)$ Space. (The recursion swap is a single tail call, not real recursion depth.)
* **The Brute-Force Detour (and why it's a failure here):** The instinct is to half-merge: walk both arrays up to `(m + n) / 2`, always advancing the pointer at the smaller value. The trap is trying to *reverse-engineer* the median from the pointer positions after the loop — the pointers point at the *future* candidates, not the values you just consumed, so `idx - 1` needs a messy web of "which array supplied the last element" checks (worse when one array is exhausted). **The fix is to decouple data from pointers:** carry two explicit value variables, `prevVal` and `currVal`, shifting `currVal → prevVal` and reassigning `currVal` each step. Then odd → `currVal`, even → `(prevVal + currVal) / 2.0`, and you never look at the pointers. But this is $O(m + n)$ — the problem *mandates* $O(\log(m + n))$, so brute force is an automatic interview failure. Worth coding once to feel the pointer-vs-value distinction, then discard.
* **The Struggle & Insights:**
    * **Three days on the wall.** Dragged this one across three sessions. The murk was never the code — it was refusing to accept that the four boundary elements are *sufficient*. The unlock: internalizing that a sorted array means the interior of each bag is provably irrelevant; only the elements touching the wall can ever be the center.
    * **"Why bother with the Right side?"** The reframe that finally landed: the Right side is not part of the answer, it is a *measuring stick*. Bad cut example — `A: [1, 100 | 105, 106]`, `B: [2, 3 | 4, 5]` → Left Bag `[1, 100, 2, 3]`. The `100` (champion of Left A) dwarfs the `4` (min of Right B), so the alarm fires: `100` must be thrown right, `4` pulled left. That single comparison is the entire correction signal.
    * **The seesaw clicked the pointer logic:** because `sectionB` is forced by `sectionA`, moving *only* A's cut left or right automatically rebalances B in the opposite direction. `maxLeftA > minRightB` means "A is too heavy" → `rightA = sectionA - 1` → next iteration picks a smaller `sectionA`, which pulls more from B to compensate.
    * **Systems takeaway:** the `+1` offset and the `±∞` sentinels are both branch-elimination tricks — they collapse "odd vs even" and "in-bounds vs cliff" into unified arithmetic instead of nested `if/else`, exactly the kind of ALU-level tidiness this whole track keeps drilling.
