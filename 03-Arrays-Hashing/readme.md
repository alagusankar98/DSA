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

### API Contracts & Quirks (Strings & Views)
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