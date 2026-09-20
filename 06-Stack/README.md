# Stack - Topic Guide

## 1. Core Fundamentals

A Stack is a Last-In, First-Out (LIFO) data structure. In modern C++, you have two primary ways to implement this, and choosing between them is a classic systems engineering tradeoff.

### Option 1: The `std::vector` (The Bare-Metal Choice)
* **Interface:** `push_back()`, `pop_back()`, `back()`, `empty()`.
* **The Empty Trap (UB):** Calling `.pop_back()` or `.back()` on an empty vector is **Undefined Behavior**. You must *always* verify `!stack.empty()` before peeking or popping.
* **Why use it?** A `std::vector` is a single contiguous block of memory. It offers the absolute best L1 cache locality. Furthermore, because it's just a vector, you can iterate over it (`for (int x : stack)`) if you get stuck in an interview and need to print your state. 
* **The Catch:** It doesn't strictly enforce the LIFO contract. You have to enforce it through discipline.

### Option 2: `std::stack` (The Semantic Choice)
* **Interface:** `push()`, `pop()`, `top()`, `empty()`.
* **The Empty Trap (UB):** Exactly like vectors, calling `.pop()` or `.top()` on an empty stack is **Undefined Behavior**. It will not throw a safe exception; it will simply crash or corrupt memory. Always check `!stack.empty()`.
* **Why use it?** It strictly enforces the LIFO contract. You cannot accidentally index into the middle of it. It immediately signals your algorithmic intent to the interviewer.
* **The Catch:** It is a "Container Adaptor", meaning it wraps another container. By default, it wraps a `std::deque`, which allocates in chunks and has slightly worse cache locality. Furthermore, you **cannot iterate over it** to print or debug your state without destroying it.

### The "Senior Flex": `std::stack<int, std::vector<int>>`
If you want to use the strict `std::stack` API but maintain the bare-metal performance of a `std::vector`, you can explicitly tell the STL to back the stack with a vector:
```cpp
std::stack<int, std::vector<int>> st;
```
This gives you the semantic clarity of `push()`/`pop()` while mathematically guaranteeing single-block contiguous memory.

### Systems Design: Why is `deque` the default instead of `vector`?
If `vector` offers better L1 cache locality, why did the C++ Standards Committee make `std::deque` the default backing for `std::stack`? It comes down to **Reallocation Penalties** and **Memory Waste**:
1. **The $O(N)$ Reallocation Spike:** A `vector` must be a single unbroken block of RAM. When it fills up, it must allocate a larger block, copy/move *every single element* over, and delete the old block. If your stack has 10 million elements, that single `push` causes a massive latency spike. A `deque` grows by just allocating a new tiny "chunk" of memory. It *never* copies old elements when growing, meaning pushes never suffer massive latency spikes.
2. **Pointer Invalidation:** Because a `vector` physically moves elements in RAM during reallocation, any references/pointers to existing elements instantly become dangling. A `deque` never moves existing elements, so pointers to them remain perfectly valid.
3. **Memory Hoarding:** Vectors typically double their capacity when growing. If you grow from 1GB to 2GB but only add a few elements, you waste nearly 1GB of RAM. Deques only allocate small chunks exactly as needed.
* **The Verdict:** Use a `vector` backend if you care about raw, average-case CPU speed (or can `.reserve()` the capacity upfront). Use the default `deque` backend if you need to avoid unpredictable latency spikes or memory hoarding.

### The Monotonic Stack Concept
You just got a taste of this with the Monotonic Deque! A Monotonic Stack is simply a stack whose elements are strictly increasing or strictly decreasing.
* If you need to find the "Next Greater Element", you maintain a decreasing stack. When a new element arrives that is *larger* than the top of the stack, the top element has found its "Next Greater", so you pop it and resolve it.
* Just like the deque, you often push the *indices* of the elements onto the stack, not the values themselves, so you can easily calculate distances (like days passed or width of a rectangle).

### Systems-Level String Parsing (`<charconv>`)
When parsing numbers from strings, avoid `std::stoi` (which throws expensive exceptions and requires null-termination). Systems engineers use `std::from_chars`, but it comes with strict low-level rules:
* **Raw Pointers Only:** It requires `const char*` bounds. You cannot use iterators like `.begin()`. You must pass the base memory address `str.data()` and the end boundary `str.data() + str.size()`.
* **The `+` Quirk:** It parses negative numbers (`-2`) perfectly, but natively fails on explicit positives (`+2`). You must manually advance the pointer: 
  ```cpp
  const char* first = str.data();
  const char* last = str.data() + str.size();
  if (first != last && *first == '+') ++first;
  ```
* **The Range Guard:** Why `first != last`? Because in modern C++ range logic (like with `string_view`), an empty string means `first` and `last` are the exact same address. Dereferencing `*first` without checking `first != last` can result in out-of-bounds Undefined Behavior if the payload isn't null-terminated.
* **Asserting Success:** Always assert the error code to guarantee valid parsing: `assert(res.ec == std::errc());`.

---

## 2. General Summary / Quick Reference

When tackling Stack problems, keep these core patterns in mind:

### Variation 1: The String/Syntax Parser
Used for matching parentheses, validating paths, or evaluating postfix notation.
* Every time you see an "opening" symbol, `push_back()`.
* Every time you see a "closing" symbol, check if the stack is `empty()`. If not, verify the `back()` element matches your closing symbol, then `pop_back()`.

### Variation 2: The Monotonic Stack
Used for finding the "Next Greater" or "Previous Smaller" element, often for histograms or temperature span problems.
* While the stack is not empty and the current element breaks the monotonic rule (e.g. current is `>` than `stack.back()`), you `pop_back()` the old element and process its result using the current index.
* After crushing the weak elements, `push_back(i)` the current index.

---

## 3. Problem Strategies & Patterns

### [1] Valid Parentheses

* **The Core Pattern:** String/Syntax Parser. Use a Stack (`std::vector`) to keep track of unclosed brackets. When you encounter a closing bracket, verify it matches the most recently opened bracket at the top of the stack.
* **The "Gotcha":**
    * **The Empty Stack Trap (UB):** You *must* check if the stack is `.empty()` before calling `.back()` or `.pop_back()`. If you get a closing bracket `]` as your very first character, checking `.back()` immediately causes Undefined Behavior and will crash.
    * **Odd Length Early Return:** A valid pairing requires an even number of characters. `if (s.size() % 2 != 0) return false;` instantly drops impossible cases without processing a single character.
    * **Push the Counterpart:** Instead of pushing the opening bracket `(` onto the stack and looking up its counterpart later, push the *expected closing bracket* `)` right now. When you actually encounter a closing bracket in the string, the verification becomes a trivial `if (stack.back() != currentChar)`.
* **Time & Space Complexity:** $O(N)$ Time / $O(N)$ Space.
* **The Struggle & Insights:**
    * **Hash Maps vs `switch`:** My first instinct was to use a `std::unordered_map` to link `(` to `)`. In systems C++, a `switch` statement completely bypasses hash computation and evaluates in bare-metal $O(1)$ time via a jump table.
    * **The `reserve()` Micro-Optimization:** Since we are only pushing expected closing brackets, the maximum possible size the stack can reach for a valid string is exactly `s.size() / 2`. By calling `stack.reserve(s.size() / 2)` upfront, you mathematically eliminate all dynamic memory reallocation overhead during the loop.

### [2] Min Stack

* **The Core Pattern:** Dual-State Tracking. To retrieve the minimum element in $O(1)$ time without searching, you must store the historical minimum *alongside* every single value. When you push, you calculate the new minimum. When you pop, both the value and its corresponding minimum are removed, instantly rolling the state back.
* **The "Gotcha":**
    * **Cache Locality (Struct vs Parallel Vectors):** Using two separate vectors (`dataStack` and `minStack`) fragments memory. Creating a single `std::vector<MinData>` where `struct MinData { int val; int min; };` guarantees that both the value and the minimum are pulled into the L1 cache simultaneously.
    * **Ternary Initialization:** Calculating the minimum to push can get messy with `if/else` logic. A clean ternary operator handles it branchlessly: `int currentMin = stack.empty() ? val : std::min(val, stack.back().min);`.
    * **Defensive Assertions (`<cassert>`):** Even if a problem guarantees valid inputs, never call `.pop_back()` or `.back()` blindly. Use `assert(!stack.empty());` to prove to the interviewer that you understand the Undefined Behavior risks and are actively guarding against them.
    * **Constructor Pre-allocation:** The constraints explicitly state a maximum of 30,000 operations. Adding a constructor that calls `stack.reserve(30000)` instantly drops all reallocation latency to zero, making every operation strictly $O(1)$.
* **Time & Space Complexity:** $O(1)$ Time for all operations / $O(N)$ Space.
* **The Struggle & Insights:**
    * **Overcomplicating with Deques:** Initially tried to shoehorn yesterday's Monotonic Deque logic into this, forgetting that a strict Stack interface doesn't allow removing expired minimums from the middle or back. Duplicating the minimum alongside the data is the LIFO-compliant way.
    * **C++ Naming Conventions:** A struct definition is a user-defined type, so it should follow PascalCase (`MinData`), not camelCase (`minData`), to distinguish it from variables.

### [3] Evaluate Reverse Polish Notation

* **The Core Pattern:** The Accumulator Stack. The algorithm is incredibly rigid: if it's a number, push it. If it's an operator, pop two numbers, evaluate them, and push the result back.
* **The "Gotcha":**
    * **The Order of Operands:** When you hit an operator and pop twice, the *first* number you pop is the **Right** operand, and the *second* number is the **Left** operand. Reversing this will cause subtraction and division to fail completely.
    * **The Mathematical `reserve()`:** An RPN expression of length $N$ tokens contains exactly $n$ numbers and $n-1$ operators (because every operator consumes 2 numbers and returns 1). Therefore, $N = 2n - 1$, which means $n = (N + 1) / 2$. You can pre-allocate the exact maximum capacity your stack will ever need via `stack.reserve((tokens.size() + 1) / 2)`.
    * **Switching on Strings:** You cannot pass a `std::string_view` into a `switch` statement. Since all valid operators are single characters, use `if (str.size() == 1 && !std::isdigit(str[0]))` to confirm it's an operator, then run a `switch (str[0])` for blazing fast $O(1)$ evaluation.
    * **Relentless Assertions:** 
        * Assert the stack has at least 2 elements before operating (`assert(stack.size() >= 2)`).
        * Assert `std::from_chars` succeeded (`assert(res.ec == std::errc{})`).
        * Assert the stack ends with exactly 1 element (`assert(stack.size() == 1)`). 
* **Time & Space Complexity:** $O(N)$ Time / $O(N)$ Space (technically bounded to $N/2$ elements).
* **The Struggle & Insights:**
    * **Why a Stack?** Initially tried to solve this with just two variables, completely failing to realize that RPN expressions can stack an arbitrary number of operands before ever encountering a single operator (e.g., `5 4 3 2 + + +`).
    * **The `std::from_chars` Nuances:** Struggled with the pointer arguments until realizing `.data()` provides the base memory address. Got blocked by its refusal to parse `+2` natively, which forced me to write the memory-safe pointer advancement check (`first != last && *first == '+'`).

### [4] Daily Temperatures

* **The Core Pattern:** The Monotonic Stack (Decreasing). The stack acts as a "waiting room" for unresolved indices. As you iterate, the current element acts as a resolver. While the current temperature is hotter than the temperature of the day sitting at the top of the stack, the current day is the answer for that waiting day. Pop the waiting day, calculate the distance (`current_index - popped_index`), and repeat the chain reaction. Finally, push the current day into the waiting room.
* **The "Gotcha":**
    * **The Implicit Zero Cleanup:** If you encounter a strictly descending array (e.g., `[50, 40, 30]`), no day will ever find a warmer day. Instead of writing a second loop at the end to pop leftover stack elements and set them to `0`, simply initialize the entire result array with zeros upfront: `std::vector<int> res(n, 0);`. Unresolved days are inherently handled.
    * **Logic Flattening:** Don't wrap the logic in `if/else` blocks based on whether the stack is empty or the current day is hotter/colder. Every single day *must* enter the waiting room. Just run a `while (!stack.empty() && temp > temps[stack.back()])` loop to resolve old days, and then unconditionally `stack.push_back(i);`.
    * **The Worst-Case Pre-allocation:** In the worst-case scenario (a descending array), every single element will sit in the stack at once. Therefore, you can safely `stack.reserve(n)` to mathematically guarantee zero reallocations.
* **Time & Space Complexity:** $O(N)$ Time / $O(N)$ Space.
* **The Struggle & Insights:**
    * **Breaking the Sliding Window Habit:** Stared at the problem initially trying to shoehorn a sliding window/deque into it. Realized it fundamentally failed because a window expects fixed boundaries, whereas this problem demands resolving a "Next Greater Element" at an unknown future distance.
    * **The "Waiting Room" Epiphany:** The visual of the stack as a waiting room clicked perfectly. The values naturally sort themselves in strictly decreasing order because any hot day instantly annihilates all colder days before entering the room itself.

### [5] Car Fleet

* **The Core Pattern:** Sorting + Bottleneck Tracking (Simulated Stack). To know if cars collide, evaluate them from the target backwards. Pair the starting `position` with the `timeToTarget` (`(target - pos) / speed`). Sort the cars by position. A car coming from behind will only form a *new* fleet if its `timeToTarget` is strictly greater than the bottleneck time of the fleet ahead of it.
* **The "Gotcha":**
    * **The Integer Division Trap:** `(target - position) / speed` silently truncates decimal times if both variables are integers, ruining the collision mathematics. You must promote the divisor: `static_cast<double>(speed)`.
    * **The Reverse Iterator Bypass:** Evaluating cars closest to the target first implies a reverse loop (`rbegin()` to `rend()`). Reverse iterators can be cognitively messy. Instead, sort the array *descending* via `std::greater<std::pair<int, double>>()`. The car closest to the target is now at index `0`, allowing a clean, standard forward iteration.
    * **The Empty Stack Crash:** If using an actual stack (or a vector with `pop_back()`), failing to write `!empty()` in the inner destruction loop instantly crashes the program when duplicates cause the stack to fully drain.
* **Time & Space Complexity:** $O(N \log N)$ Time (due to sorting) / $O(N)$ Space (for the pairs array).
* **The Struggle & Insights:**
    * **Why is this a Stack Problem?** Initially tried to physically `pop()` merged fleets. Then realized a physical stack isn't actually needed. Tracking `double prevBottleNeckTime = std::numeric_limits<double>::min();` completely mimics the `.top()` of a stack without needing memory allocation or pop loops. A variable and a counter achieve the exact same state machine as a full stack.
    * **Sorting Paired Data:** Learned how to safely tie derivative data (`hoursRemaining`) to its original sorting key (`position`) by packing them into a `std::vector<std::pair<int, double>>`. Since `std::sort` inherently evaluates `.first`, the pairing is perfectly maintained.
