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

*(Problems and your insights will be added here as you solve them)*
