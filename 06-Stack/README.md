# Stack - Topic Guide

## 1. Core Fundamentals

A Stack is a Last-In, First-Out (LIFO) data structure. In modern C++, you have two primary ways to implement this, and choosing between them is a classic systems engineering tradeoff.

### Option 1: The `std::vector` (The Bare-Metal Choice)
* **Interface:** `push_back()`, `pop_back()`, `back()`, `empty()`.
* **Why use it?** A `std::vector` is a single contiguous block of memory. It offers the absolute best L1 cache locality. Furthermore, because it's just a vector, you can iterate over it (`for (int x : stack)`) if you get stuck in an interview and need to print your state. 
* **The Catch:** It doesn't strictly enforce the LIFO contract. You have to enforce it through discipline.

### Option 2: `std::stack` (The Semantic Choice)
* **Interface:** `push()`, `pop()`, `top()`, `empty()`.
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

*(Problems and your insights will be added here as you solve them)*
