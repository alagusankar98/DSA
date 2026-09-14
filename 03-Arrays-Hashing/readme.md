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

### C++ Quirks & Best Practices
* **NRVO (Named Return Value Optimization):** Do not use `std::move` when returning a local variable (e.g., `return std::move(result);`). The C++ compiler automatically performs NRVO, meaning it constructs the local variable directly in the caller's memory space, completely eliding the copy or move. Using `std::move` disables this optimization and forces a move operation, which is strictly worse.
* **Vector Initialization (Size vs Capacity):** Initializing a vector with a size (e.g., `vector<int> res(nums.size(), 1);`) allows direct index access (`res[i] = ...`) rather than using `.reserve()` and `.push_back()`. It also automatically value-initializes the elements (e.g., to `1`).
* **`std::string_view` Null-Termination:** A `string_view` is a lightweight, non-owning view over a character sequence. It is **not** guaranteed to be null-terminated. This means you cannot safely pass `.data()` to legacy C-functions or standard functions like `std::stoull()` without risking buffer overruns. 
* **High-Performance Parsing (`std::from_chars`):** Instead of `stoull()`, use `<charconv>`'s `std::from_chars()`. It takes a start pointer, an end pointer, and a stack-allocated output variable. It returns a result struct containing `.ptr` (pointing to the first unparsed character). It performs no heap allocations and throws no exceptions.
* **String Concatenation (`+` vs `.append()`):** Using the `+` operator (e.g., `s = s + "a"`) constructs a brand new temporary string on the stack on every invocation. When building strings in a loop, always use `.append()` or `+=` (ideally combined with `.reserve()`) to modify the string directly in memory without unnecessary allocations.
* **In-Place Construction (`emplace_back`):** You can construct a `std::string` directly inside a `std::vector` to bypass temporary objects entirely. For example, `vec.emplace_back(ptr, length)` uses the `char*` and `size_t` directly to construct the string at its final destination.

### Priority Queue / Heaps (`std::priority_queue`)
* **Under the Hood:** Implemented as a complete binary tree layered over a flat `std::vector`. A parent node strictly has higher priority than its children (e.g., in a Max Heap, parent > children), but there is no specific ordering between the left and right children.
    * Mathematical mapping: Given a parent at index `i`, its left child is at `2*i + 1` and right child is at `2*i + 2`.
* **Performance:** 
    * **Access:** $O(1)$ to get the highest priority element using `.top()` (always at the `0`th index).
    * **Insertion:** $O(\log N)$ using `.push()` or `.emplace()`. The element is added to the end of the array and "sifts up" the tree to its correct position.
    * **Deletion:** $O(\log N)$ using `.pop()`. The root is removed, replaced by the last element, which then "sifts down".
* **Visualization (Max Heap):**
    ```text
    Tree Structure:          Flat Vector Array:
         [10]                Index:   0   1   2   3   4   5
        /    \               Value: [10,  7,  8,  2,  4,  5]
      [7]    [8]
     /  \    /
   [2]  [4][5]
    ```

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

### [2] Valid Anagram

* **The Core Pattern:** Frequency Counter using a Fixed-Size Array. Increment counts for the first string, decrement for the second, and verify all counts remain exactly zero.
* **The "Gotcha":**
    * **The Length Guard:** Always start with `if (s.size() != t.size()) return false;`. This $O(1)$ check frequently prevents unnecessary $O(N)$ traversals.
    * **`std::string_view`:** For read-only string parameters, C++17's `std::string_view` is strictly superior to `const std::string&`. It is a lightweight non-owning view that completely avoids accidental heap allocations or copies, especially when string literals are passed.
    * **Array vs. Map:** Since the key domain is bounded to exactly 26 lowercase English letters, `std::unordered_map` is massive overkill. It allocates nodes on the heap and ruins data locality. Using a zero-initialized stack array (`std::array<int, 26> counts = {0};`) is infinitely faster and allows the CPU prefetcher to operate perfectly.
    * **Modern C++ Algorithms:** Instead of writing a manual `for` loop to verify if all array elements are `0`, use C++20's `std::ranges::all_of`. It takes the container and a lambda: `std::ranges::all_of(counts, [](int c){ return c == 0; });`. It perfectly signals your intent to anyone reading the code.
* **Time & Space Complexity:** O(N) Time / O(1) Space (since the array is always exactly 26 elements, space is constant regardless of string size).
* **The Struggle & Insights:**
    * Had the correct conceptual approach (frequency counting) but initially fell into the trap of using a heavy, general-purpose container (`unordered_map`) for a strictly bounded problem.
    * Missed the lowest-hanging optimization (the early-exit size check).
    * Learned how to leverage modern C++ paradigms (`string_view` and `ranges::all_of`) to write code that is both highly optimized at the hardware level and incredibly readable.

### [3] Two Sum

* **The Core Pattern:** One-Pass Hash Map. Iterate through the array, calculate the needed complement (`target - nums[i]`), and check if it exists in the map. If it does, you have your pair. If not, add the current number and its index to the map and continue.
* **The "Gotcha":**
    * **Inserting the Wrong Value:** A classic brain-slip is inserting the complement (`target - nums[i]`) into the map instead of the actual number (`nums[i]`). The map must act as a historical record of numbers you have *actually seen*, not numbers you are hoping to find.
    * **Modern C++ Insertion (`try_emplace`):** Using `.insert(std::make_pair(...))` is an older C++ pattern. In modern C++ (C++17+), prefer `.try_emplace(key, value)` for conditional insertions. It checks for the key first and completely avoids constructing the value object if the key already exists. If you strictly want to insert-or-overwrite, use `map[key] = value`.
    * **Missing `.reserve()`:** Just like with `unordered_set`, an `unordered_map` needs its capacity reserved to avoid rehashing. Since the worst-case scenario stores every element, call `map.reserve(nums.size())` before the loop.
    * **Type Casting:** Again, rely on `std::ssize(nums)` to get a signed integer for loop indices, rather than cluttering the code with `static_cast<int>(nums.size())`.
* **Time & Space Complexity:** $O(N)$ Time / $O(N)$ Space.
* **The Struggle & Insights:**
    * Successfully recognized the one-pass optimization: checking for the complement and inserting the current number can happen in the exact same loop.
    * Optimized the map lookup by capturing the iterator from `.find()` (`auto it = map.find(...)`) rather than using `.contains()` followed by a redundant index lookup (`map[...]`).

### [4] Group Anagrams

* **The Core Pattern:** Custom Hash Function + Frequency Array. Instead of sorting strings to use as keys, create a `std::array<uint8_t, 26>` to count character frequencies. Use a custom hash functor to hash this array, allowing $O(K)$ key generation instead of $O(K \log K)$.
* **The "Gotcha":**
    * **Custom Hash Functor `const`ness:** `std::unordered_map` requires the hashing functor's `operator()` to be marked as `const`. If you forget it (e.g., writing `size_t operator()(const array& a)` instead of `size_t operator()(const array& a) const`), the compiler will throw a massive template error.
    * **Operator Precedence in Hashing:** A standard string-style hash algorithm is `hash = hash * 31 + val`. Writing `hash *= 31 + val` actually evaluates as `hash = hash * (31 + val)`, completely changing the hash distribution and potentially causing massive collisions.
    * **MATLAB Syntax Bleed:** In C++, `~` is the bitwise NOT operator, not a placeholder for ignored variables. To ignore values in C++ (like the key when iterating a map), you either just name it a dummy variable (e.g., `for (auto& [ignore, group] : map)`) or use `std::ignore` with `std::tie` if not using structured bindings.
    * **Optimal Sizing:** If the problem constraints state that strings are at most 100 characters long, an 8-bit unsigned integer (`uint8_t`, max 255) is the perfect size for the frequency array. Using `uint16_t` or `int` doubles or quadruples the memory footprint of every key for no reason.
    * **Move Semantics vs. `const`:** You cannot move from a `const` reference. If the input array is `const vector<string>&`, you must copy the strings into your map. However, when moving the grouped vectors *out* of your map into the final `vector<vector<string>>`, you absolutely should use `std::move(pair.second)` since the map is a local mutable variable.
* **Time & Space Complexity:** $O(N \cdot K)$ Time (where $N$ is the number of strings, $K$ is the maximum string length) / $O(N \cdot K)$ Space.
* **The Struggle & Insights:**
    * Identified that sorting strings is suboptimal and pivoted to frequency arrays. Since C++ doesn't natively hash `std::array`, writing a custom hash struct was necessary.
    * The custom hash loop setup: Start with a prime base (e.g., 17), multiply by another prime (31), and add the value. 
    * Realized the importance of using range-based for loops (`for (char c : s)`) over standard indexed loops for cleaner, less error-prone iteration when you only need the values.

### [5] Top K Frequent Elements

* **The Core Pattern:** Frequency Counter + Min Heap. 
    1. Build a hash map of `number -> frequency`. 
    2. Iterate through the map and push pairs of `[frequency, number]` into a Min Heap. 
    3. Keep the heap size strictly at `k` by popping the top (smallest frequency) element whenever the size exceeds `k`. 
    4. Extract the remaining `k` elements from the heap into the final vector.
    * *Alternative Approaches Considered:* 
        * **Bucket Sort:** An array of vectors where the index represents frequency. $O(N)$ time.
        * **Ordered Map:** `std::map<int, std::vector<int>>` mapping frequencies to numbers, iterating backwards from the largest key.
* **The "Gotcha":**
    * **Max vs Min Heap Defaults:** By default, `std::priority_queue` uses `std::less`, which actually creates a **Max Heap** (largest element has highest priority). To create a Min Heap, you must explicitly use `std::greater`.
    * **Template Instantiation:** When providing a custom comparator to the template parameters, pass the *type* without parentheses: `std::priority_queue<T, vector<T>, std::greater<T>>`, NOT `std::greater<T>()`.
    * **Push vs. Emplace:** Use `.emplace(freq, num)` instead of `.push({freq, num})`. `.emplace()` constructs the object directly inside the underlying container, avoiding redundant stack allocations and copies.
    * **No `.reserve()`:** Even though `std::priority_queue` is backed by a `std::vector`, it does *not* expose a `.reserve()` method. You cannot pre-allocate its capacity.
    * **Verbose Types:** Using `std::pair<int, int>` everywhere gets messy. Use a type alias like `using Pair = std::pair<int, int>;` to keep the template parameters and code clean.
* **Time & Space Complexity:** 
    * Time: $O(N \log K)$ — $O(N)$ to build the frequency map, and inserting up to $N$ unique elements into a heap of max size $K$ takes $O(N \log K)$.
    * Space: $O(N)$ — $O(N)$ for the hash map to store frequencies, and $O(K)$ for the heap.
* **The Struggle & Insights:**
    * **Understanding Heaps:** Learned that a heap is essentially a tree conceptually, but mathematically mapped to a flat array/vector for incredible performance and cache locality. 
    * **Efficient Pruning:** Realized the power of actively pruning a Min Heap to size `k` during insertion. This guarantees that only the top `k` most frequent elements remain, significantly optimizing time compared to sorting all frequencies.

### [6] Encode and Decode Strings

* **The Core Pattern:** Length-Prefixed Serialization. Treat the array of strings like a network payload. Encode them by prepending the length of the string followed by a delimiter (e.g., `5#hello4#word`). Decode by reading the numerical length, skipping the delimiter, extracting the string using pointer arithmetic, and advancing the pointer.
* **The "Gotcha":**
    * **The Null-Termination Trap:** When decoding, using `std::string_view` is optimal for performance, but it breaks standard parsing tools like `std::stoull()` because the view is not null-terminated.
    * **Mastering `std::from_chars`:** Because `stoull()` is off the table, you must use `std::from_chars(start_ptr, end_ptr, out_val)`. You need to calculate the end pointer using `view.data() + view.size()`. The function writes the parsed number into `out_val` and returns a struct containing a pointer to the delimiter (the first non-numerical character).
    * **String Accumulation Penalty:** When encoding, avoid using the `+` operator to combine lengths, delimiters, and strings. Pre-calculate (or safely estimate) the total required length, call `.reserve()` on the accumulator string, and use `.append()` to construct the encoded result in-place.
* **Time & Space Complexity:** 
    * **Encode:** $O(N)$ Time (where $N$ is total characters) / $O(1)$ Space (excluding the output string).
    * **Decode:** $O(N)$ Time / $O(N)$ Space (for the output array of strings).
* **The Struggle & Insights:**
    * **First Pass Inefficiencies:** Initially used `std::string::find()`, `std::stoull()`, and `std::string::substr()`. While logically correct, it created multiple unnecessary string copies and heap allocations.
    * **Pointer Arithmetic Elegance:** Refactored the solution to use direct pointer manipulation (`.data()`), `from_chars`, and `vector::emplace_back(ptr, len)`. This approach works directly with memory, bypassing all temporary string constructions and bringing the runtime extremely close to the metal.

### [7] Product of Array Except Self

* **The Core Pattern:** Prefix and Suffix Arrays (Two-Pass Optimization). For any index `i`, the answer is the product of all elements to its left (prefix) multiplied by all elements to its right (suffix). 
    1. **Prefix Pass:** Traverse left-to-right. Store the running prefix product directly in the output array.
    2. **Suffix Pass:** Traverse right-to-left. Use a single scalar variable to track the running suffix product, multiply it with the existing value in the output array, and then update the suffix variable.
* **The "Gotcha":**
    * **Avoid `std::move` on Return:** When returning the result vector, simply `return res;`. Writing `return std::move(res);` disables Named Return Value Optimization (NRVO), forcing a move constructor rather than letting the compiler construct the vector directly in the caller's memory space.
    * **Vector Initialization:** Initialize the output vector with its size and a default value of `1`: `std::vector<int> res(nums.size(), 1);`. This allocates memory and default-initializes everything upfront, allowing direct index assignment (`res[i] = ...`) or `.front() = 1` rather than juggling `.reserve()` and `.push_back()`, making the two-pass logic trivial.
    * **Initial States:** The prefix product before index 0 is `1` (which can be initialized using `res.front() = 1`). The suffix product after the last index is also `1`.
* **Time & Space Complexity:** 
    * Time: $O(N)$ — Exactly two passes over the array.
    * Space: $O(1)$ extra space (since the output array does not count towards auxiliary space).
* **The Struggle & Insights:**
    * **The Condensation Trap:** Spent a long time trying to condense the logic into a single complex equation or loop. Realizing that the problem naturally decouples into "everything before" and "everything after" was the key breakthrough.
    * **Index Alignment:** Visualizing a size 4 array helped: index 3 strictly needs the accumulated product of indices `[0, 1, 2]`. This clarified why the running product is accumulated *after* assigning it to the result array for the current index, or running iteration backwards from $N-1$ to update previous indices with the trailing product.

### [8] Valid Sudoku

* **The Core Pattern:** Bitmasking for State Tracking. A Sudoku board requires checking rows, columns, and 3x3 boxes for duplicates. Instead of using nine 9x9 boolean arrays (or hash sets) for tracking, use three arrays of 9 `uint16_t` integers: `uint16_t rows[9]`, `cols[9]`, `boxes[9]`. Since `uint16_t` has 16 bits, it acts as an ultra-fast, memory-dense hash set for digits 1-9 by setting the $N$-th bit.
* **The "Gotcha":**
    * **Constraint Guarantees:** If the problem strictly defines the input domain (e.g., characters are strictly `.` or `'1'` to `'9'`), don't over-engineer guards like `if (c >= '1' && c <= '9')`. Simply `if (c == '.') continue;` and assume the rest are valid digits.
    * **Zero-Indexing the Bitmask:** To map characters to bits, use `board[i][j] - '1'` rather than `- '0'`. This perfectly maps `'1'` to bit 0 and `'9'` to bit 8, making full use of the bit space without leaving bit 0 empty.
    * **Pre-compute and Reuse Masks:** Don't calculate `(1 << digit)` three separate times for the row, col, and box. Calculate it once (`uint16_t mask = 1 << digit;`) and reuse it to save CPU cycles.
    * **Combine Bitwise Checks:** Instead of writing three separate `if` statements to check if the bit is set in the row, column, and box, combine them using bitwise OR: `if ((rows[i] | cols[j] | boxes[box_idx]) & mask) return false;`. This checks if *any* of the regions have the bit set in a single, blazing-fast hardware instruction, reducing branching.
* **Time & Space Complexity:** 
    * Time: $O(1)$ — The board is strictly 9x9, meaning $81$ iterations, which is constant time.
    * Space: $O(1)$ — Three arrays of 9 `uint16_t` integers consume exactly 54 bytes total. This effortlessly fits entirely inside a single L1 CPU cache line.
* **The Struggle & Insights:**
    * Recalling past struggles with the 3x3 box indexing math `(i / 3) * 3 + (j / 3)` paid off, making the matrix traversal smooth.
    * **Code Review Epiphanies:** Realized how much cleaner and faster the code becomes when leveraging bitwise OR `|` to flatten multiple condition checks, and how reusing calculated values (like the bitmask) is a critical optimization habit.

### [9] Longest Consecutive Sequence

* **The Core Pattern:** Hash Set and Sequence Building. Dump the array into a `std::unordered_set` to eliminate duplicates and enable $O(1)$ lookups. Iterate over the set, and for each number, try to build a sequence by repeatedly checking if `num + 1` exists, keeping track of the maximum length found.
* **The "Gotcha":**
    * **The $O(N^2)$ Trap (Time Limit Exceeded):** If you try to build a sequence starting from *every* number, a massive contiguous array like `[1, 2, ..., 19999]` will cause the inner loop to run 19999 times for `1`, then 19998 times for `2`, then 19997 times for `3`, etc. This degenerates into $O(N^2)$ time complexity.
    * **The Sequence Starter Check:** To achieve strict $O(N)$ time, only attempt to build a sequence if the current number is the absolute *start* of a sequence. You can mathematically verify this by checking if `num - 1` does *not* exist in the set (`if (!set.contains(num - 1))`). This ensures the inner `while` loop only ever processes each number in a sequence exactly once.
* **Time & Space Complexity:** 
    * Time: $O(N)$ — Creating the set takes $O(N)$. Iterating through the set takes $O(N)$. Because of the sequence starter check, the inner `while` loop strictly visits each element across all sequences exactly once. Therefore, total time is bounded by $O(N)$.
    * Space: $O(N)$ — To store the unique numbers in the `unordered_set`.
* **The Struggle & Insights:**
    * Initially blamed a failed large test case on a missing `.reserve()` optimization for the hash set, failing to realize the core logic was fundamentally $O(N^2)$.
    * **The Breakthrough:** Learned how a single $O(1)$ look-behind check (`num - 1`) can mathematically transform a nested loop from quadratic $O(N^2)$ time into linear $O(N)$ time by preventing redundant work.