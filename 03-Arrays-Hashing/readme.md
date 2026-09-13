# Arrays & Hashing - Topic Guide

## 1. Core Fundamentals

### Hash Tables in C++ (`std::unordered_set` / `std::unordered_map`)
* **Under the Hood:** Standard C++ implements these using an array of buckets, where each bucket is a linked list (Separate Chaining) to handle collisions. 
* **Performance:** Average case $O(1)$ for lookups/insertions. Worst case $O(N)$ if many collisions occur.
* **The Cache Problem:** Because it relies on linked lists, memory is allocated randomly on the heap. This destroys data locality, causing frequent CPU cache misses and defeating hardware prefetchers. 
* **Rehashing:** When the number of elements exceeds the `bucket_count * load_factor` (usually 1.0), the container allocates a larger bucket array and re-hashes every single element. Always use `.reserve(n)` if you know the maximum size beforehand to avoid this massive overhead.

### Standard Algorithms (`<algorithm>`)
* **`std::sort`:** Runs in $O(N \log N)$ time. By default, it sorts in ascending order using `operator<`.
* **`std::adjacent_find`:** Runs in $O(N)$ time. Returns an iterator to the first element that is equal to the element immediately following it.

---

## 2. General Summary / Quick Reference

When tackling Arrays & Hashing problems, keep these patterns in mind:
* **The Space-Time Tradeoff:** Hash maps/sets give you $O(1)$ lookups at the cost of $O(N)$ extra space. Sorting gives you $O(1)$ space (if mutable) but costs $O(N \log N)$ time.
* **STL is Your Friend:** Prefer standard algorithms like `std::adjacent_find` over manual loops when possible. It makes code self-documenting.

---

## 3. Problem Strategies & Patterns

### [1] Contains Duplicate

* **The Core Pattern:** 
    1. **Time-Optimized:** Use `std::unordered_set` to track seen elements.
    2. **Space-Optimized (Mutable):** `std::sort` followed by `std::adjacent_find`.
* **The "Gotcha":**
    * **Missing `.reserve()`:** Failing to reserve space in an `unordered_set` causes dynamic resizing and rehashing, which tanks performance on massive inputs.
    * **Functor Instantiation:** When passing a custom comparator to STL algorithms, you must pass an *instance* of the type. `std::less<int>` is a type; `std::less<int>()` creates the callable object. (Note: `std::sort` defaults to `std::less` anyway, so it can be omitted entirely).
    * **Const Correctness & Copies:** If a function signature enforces a `const vector<int>&`, sorting requires making an internal copy, which costs $O(N)$ space. At that point, you might as well use a Hash Set to get $O(N)$ time instead of $O(N \log N)$ time.
    * **Single-Pass Insertion (The `insert` Trick):** Calling `.contains()` followed by `.insert()` forces the container to hash the value and traverse the bucket *twice*. Instead, rely on `.insert()` directly. It returns a `std::pair<iterator, bool>`, where the boolean is `false` if the element already existed.
        * *Idiomatic Pattern:* `if (!seen.insert(num).second) return true;`
* **Time & Space Complexity:** 
    * Hash Set: $O(N)$ Time / $O(N)$ Space.
    * Sort + Find: $O(N \log N)$ Time / $O(1)$ Space (if sorting in-place).
* **The Struggle & Insights:**
    * **CPU Cache Penalties:** Realized that `unordered_set` is heavily cache-unfriendly due to its node-based (linked-list) backend. 
    * **Pass-by-Value vs. Internal Copy:** Taking a parameter by value (`vector<int> nums`) in the function signature is functionally different from taking a `const vector<int>&` and copying it internally. Passing by value allows the *caller* to optimize. If the caller passes a temporary array or uses `std::move()`, the compiler will completely elide the copy. An internal copy forces a memory allocation every single time.
    * **Elegant STL:** Discovered `std::adjacent_find(nums.begin(), nums.end()) != nums.end()` as the cleanest way to check for adjacent duplicates after sorting.