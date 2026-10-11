# Heap / Priority Queue - Topic Guide

## 1. Core Fundamentals

A **priority queue** is a container with one promise: **"give me the most important element, fast."** Push anything in any order; at any moment you can look at or remove the current **best** element (largest, smallest, earliest deadline, closest point). It makes no promise about the order of anything else.

A **heap** is the data structure almost every priority queue is built on. The two names get used interchangeably: *priority queue* is the interface (push / top / pop), *heap* is the implementation.

> **Name clash:** this "heap" has nothing to do with **heap memory** (`new` / `malloc`). Same word, unrelated ideas.

### Why not just keep things sorted?
| Approach | Get best | Insert | Remove best |
| -- | -- | -- | -- |
| Unsorted array | O(N) scan | O(1) | O(N) |
| Sorted array | O(1) | O(N) (shift everything) | O(1) from the end |
| Balanced BST (`std::set` / `std::multiset`) | O(log N) (or O(1) via `begin()`) | O(log N) | O(log N) |
| **Binary heap** | **O(1)** | **O(log N)** | **O(log N)** |

* A sorted array does too much work: it keeps *every* element in order, when you only ever ask for the best one.
* A BST matches the heap's big-O, but every element is a separate node allocation with three pointers, scattered across memory. A heap does the same job in **one flat array with no pointers at all.**
* **The heap's trick is being lazy:** it keeps just enough order to know the best element, and nothing more.

### The Mental Model: a tournament bracket, stored in an array
A **max-heap** is a binary tree with two rules:
1. **Heap property:** every parent is **≥ both of its children**. So the root is the maximum. (A **min-heap** flips it: parent ≤ children, root is the minimum.)
2. **Shape rule:** the tree is **complete**: every level is full except possibly the last, which fills **left to right** with no gaps.

```text
Tree view (max-heap):               Array view (the real storage):
            [10]                     index:  0   1   2   3   4   5
           /    \                    value: [10,  7,  8,  2,  4,  5]
        [7]      [8]
       /   \     /
     [2]   [4] [5]
```
* **Siblings are not ordered.** `7` is left of `8` for no reason. Only parent-vs-child is guaranteed. So a heap is **not sorted**: reading the array left to right gives `10, 7, 8, 2, 4, 5`.
* **No pointers.** Because the tree is complete (no gaps), you can number the nodes level by level and store them in an array. Parent and children are found with **index arithmetic**:
    * children of `i`: `2*i + 1` and `2*i + 2`
    * parent of `i`: `(i - 1) / 2` (integer division)
    * Check it: `7` is at index 1, so its children are at 3 and 4 (`2`, `4`). ✔ The parent of index 5 (`5`) is `(5-1)/2 = 2` (`8`). ✔
* **Height is always ⌊log₂ N⌋.** The shape rule forbids the skewed, linked-list-like trees that made the Trees topic's worst cases O(N). A million elements is only 20 levels deep. That's where every O(log N) below comes from.

### The Two Moves Everything Is Built From
Every operation breaks the heap property in exactly one spot, then repairs it by walking that one element up or down a single root-to-leaf path.

**Sift up** (after `push`): put the new element at the **end of the array** (the next free slot keeps the tree complete), then while it's bigger than its parent, **swap it with the parent**.
```text
push(9):   [10, 7, 8, 2, 4, 5, 9]      9 lands at index 6, parent = index 2 (8)
           9 > 8 → swap → [10, 7, 9, 2, 4, 5, 8]   now parent = index 0 (10)
           9 < 10 → stop.
```

**Sift down** (after `pop`): the root (the answer) is removed. Move the **last element** into the root's slot (again keeps the tree complete), then while it's smaller than one of its children, **swap it with the *larger* child**.
```text
pop():     remove 10, move last (5) to the root → [5, 7, 8, 2, 4]
           children 7, 8 → larger is 8 → swap → [8, 7, 5, 2, 4]
           5 has no children left → stop.   New top = 8 ✔
```
* **Why the *larger* child?** It becomes the parent of the other one, so it must be ≥ it. Swapping with the smaller child would put a smaller value above a bigger one and break the property right there.
* Each move walks at most one path, so both are **O(log N)**.

### Building a heap from N elements: O(N), not O(N log N)
Pushing N elements one at a time costs O(N log N). **Heapify** does better: treat the unsorted array as a broken heap, and sift down every non-leaf node, starting from the last one and working back to the root.
* Why it's only O(N): half the nodes are leaves (no work), a quarter sift down at most 1 level, an eighth at most 2… The sum `N/4·1 + N/8·2 + N/16·3 + …` converges to under N. Most nodes are near the bottom, and nodes near the bottom only have a short way to go.
* In C++, `std::make_heap(v.begin(), v.end())` does this, and so does constructing a `priority_queue` from an existing vector (below).

### Complexity Summary
| Operation | Cost |
| -- | -- |
| `top()` (peek at the best) | O(1) |
| `push` | O(log N) |
| `pop` (remove the best) | O(log N) |
| Build from N elements (heapify) | O(N) |
| Find / remove an arbitrary element | **O(N)**: there's no index, you'd have to scan |
| Pop everything in order (heapsort) | O(N log N) |

The last-but-one row matters. **A heap can only touch its top.** "Remove element X" or "change X's priority" isn't supported. That's the source of the *lazy deletion* trick in §2.

### `std::priority_queue` in C++
It's a thin wrapper (a **container adaptor**) over a `std::vector` that calls the heap algorithms for you. Only `push` / `emplace`, `top`, `pop`, `size`, `empty`. No iteration, no indexing, no search.

* **The default is a MAX-heap:** `std::priority_queue<int> pq;` gives the largest on `top()`.
* **Min-heap:** pass the container and the comparator *type* explicitly:
  `std::priority_queue<int, std::vector<int>, std::greater<int>> pq;`
  Note `std::greater<int>`, a type, not `std::greater<int>()`.
* **The comparator rule that confuses everyone:** the comparator answers **"does `a` have *lower* priority than `b`?"**, i.e. "should `a` sit *below* `b`?". The default `std::less` (`a < b` → smaller sinks) gives a **max**-heap, and `std::greater` gives a **min**-heap. It feels backwards because `std::sort` with `std::less` gives *ascending* order. With a heap, think "who sinks", not "who comes first".
* **Custom ordering with a lambda:** the lambda's type has no name, so use `decltype`, and pass the lambda to the constructor:
  ```cpp
  auto cmp = [](const P& a, const P& b) { return a.second > b.second; };   // ">" → MIN-heap on .second
  std::priority_queue<P, std::vector<P>, decltype(cmp)> pq(cmp);
  ```
* **Pairs and tuples compare lexicographically** (first member, then second), so `priority_queue<pair<int,int>>` orders by `.first` for free. Put the key you want ordered on *first*, and you often don't need a custom comparator at all.
* **`top()` or `pop()` on an empty queue is undefined behaviour.** No exception, no assert in release builds. Check `empty()` first.
* **`top()` returns a `const&`.** You can't modify the top element in place. To change it, `pop()` it and `push()` the new value.
* **`emplace(args…)` vs `push(obj)`:** `emplace` builds the element directly in the vector's storage from constructor arguments. `push({a, b})` builds a temporary first, then moves it in. For `pair<int,int>` the difference is negligible. It matters for heavier types.
* **No `reserve()`.** The adaptor hides it. To pre-allocate, reserve a vector yourself and **move it in**: `std::priority_queue<int> pq(std::less<int>{}, std::move(buf));`. The same constructor with a filled vector **heapifies in O(N)**.
* **The raw algorithms** (`<algorithm>`): `std::make_heap`, `std::push_heap` (call *after* `push_back`), `std::pop_heap` (moves the top to the *back*, then you `pop_back`). Use these when you need the underlying vector itself, e.g. to iterate over it or keep it as a class member you control.

### The Hardware Reality
* **One contiguous array, zero pointers, zero per-element allocations.** That's the heap's big advantage over a BST (`std::set` allocates a node, with 3 pointers and a colour bit, for every element). After the vector stops growing, `push` / `pop` never call `malloc`.
* **But sift-down jumps.** Going from `i` to `2i+1` doubles the index. The top few levels share a few cache lines and stay hot, but deeper levels land on a new cache line every step. On huge heaps, the cost of `pop` is dominated by cache misses, not comparisons. That's why some high-performance systems use **4-ary heaps** (children at `4i+1 … 4i+4`): half the height, and the 4 children usually sit in one cache line.
* **Embedded instinct:** a fixed-capacity heap in a static array (`std::array<T, N>` plus a `size` counter) needs no allocator at all. RTOS **timer queues** and **schedulers** often work exactly like this: the top is "the next deadline to fire" or "the highest-priority ready task".

### Where Heaps Show Up in the Real World
OS schedulers (next task to run), timer queues and event simulations (next event by time), Dijkstra's shortest path and Prim's minimum spanning tree (Advanced Graphs, topic 13), merging K sorted streams (log-structured databases merging sorted files), "top 10" dashboards over endless data, and Huffman coding (repeatedly merge the two least-frequent symbols).

---

## 2. General Summary / Quick Reference

When tackling Heap problems, keep these core patterns in mind:

### Pattern 1: Top-K with a Size-K Heap (use the *opposite* heap)
To keep the **K largest** of a stream, use a **min-heap** capped at size K: push each element, and if the size exceeds K, pop. The top is then the **K-th largest**, the weakest of the current top K, which is the one to throw out when something better arrives.
* *Use for:* Kth Largest Element in a Stream, Kth Largest Element in an Array, K Closest Points to Origin (closest = smallest distance → **max**-heap of size K), top-K frequent anything.
* *Why the opposite heap:* the top of the heap must be the element you'd **evict next**. O(N log K) instead of sorting everything in O(N log N), and only O(K) memory, so it works on streams too big to store.
* *Discipline:* say out loud "K largest → min-heap", "K smallest → max-heap" before typing.

### Pattern 2: Repeated "Take the Best, Put Something Back" (simulation)
When a process repeatedly takes the current max or min, transforms it, and maybe re-inserts the result, a heap gives you the next best in O(log N) each round.
* *Use for:* Last Stone Weight (smash the two heaviest, push the difference back).
* *Discipline:* check `size()` before each `pop`. Simulation loops are where empty-heap UB hides.

### Pattern 3: Two Heaps (keep a running middle)
Split the data into a **max-heap of the lower half** and a **min-heap of the upper half**. Their two tops sit on either side of the middle, so the median is available in O(1). Each insert goes to one side, then **rebalance** so the sizes differ by at most 1.
* *Use for:* Find Median from Data Stream.
* *Invariants to maintain:* every element in the lower heap ≤ every element in the upper heap, and `|size difference| ≤ 1`.

### Pattern 4: Heap + Waiting Area (things that can't be used yet)
When a chosen item has to **cool down** before it can be picked again, pop it from the heap, park it in a **queue** with the time it becomes available, and move it back into the heap when that time arrives.
* *Use for:* Task Scheduler (counting / greedy shortcuts also exist).
* *Discipline:* simulate with an explicit clock; decide exactly when parked items re-enter.

### Pattern 5: K-Way Merge (one candidate per source)
Merging K sorted sources: seed a min-heap (or max-heap, for newest-first) with the **first** element of each source, tagged with which source it came from. Pop the best, output it, and push the **next** element from *that same* source. The heap never holds more than K items.
* *Use for:* Design Twitter (newest tweets across the users you follow), merging K sorted lists or arrays.
* *Cost:* O(total · log K) time, O(K) space.

### Pattern 6: Lazy Deletion (when you need "remove X")
Heaps can't remove arbitrary elements. Instead, record that X is dead (in a hash map or counter), leave it in the heap, and **discard it when it surfaces at the top**.
* *Use for:* sliding-window max/median variants, and any time you'd want `decreaseKey` (the Dijkstra version: push the improved distance as a new entry and skip stale entries when popped).

### The Non-Negotiable Discipline
* **Know which heap you have:** `std::priority_queue` defaults to **max**. Say "max or min?" before writing the type.
* **Comparator = "who sinks".** `greater` → min-heap. A lambda returning `a > b` → min-heap.
* **Never `top()` / `pop()` an empty heap.**
* **Size-K heap for top-K; never sort the whole input unless you need all of it.**
* **State both costs:** O(log N) per push/pop, O(N) to heapify, and O(N) for anything that isn't the top.

---

## 3. Problem Strategies & Patterns

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*

<!-- NeetCode 150 Heap / Priority Queue (7): Kth Largest Element in a Stream · Last Stone Weight · K Closest Points to Origin · Kth Largest Element in an Array · Task Scheduler · Design Twitter · Find Median from Data Stream -->

### [1] Kth Largest Element in a Stream

> First heap problem, and it took a while, not for the code but for the *why*: "I want the k-th **largest**, so why a **min**-heap?" Three versions: `756431b` (push everything, trim on `add`), `c7c3426` (constructor reuses `add`), and the current file, which checks before pushing (the "bouncer").

* **The Core Pattern:** Top-K with a size-K heap of the **opposite** kind (§2 Pattern 1). Keep only the **k largest values seen so far**, in a **min-heap**. Its `top()` is the smallest of those k, and that's exactly the k-th largest overall. Each `add` either lets the new value in (evicting the current smallest) or rejects it. The answer is always `top()`.

* **The question that didn't click: why min-heap, not max-heap?**
    * **My first approach (max-heap) and why it fails:** put everything in a max-heap and, on each `add`, pop k times to reach the k-th largest. The pops are **destructive**: after the first query, the top k−1 values are gone, so the next query is wrong (and with few enough elements it hits an empty heap). I spotted the fix myself: push those k−1 back after every query. That works, but it's O(k log N) per `add`, and it re-does the same work every time.
    * **What the question actually needs:** "the k-th largest" is a **threshold**: the weakest member of the top-k group. Values below it never matter again. Values above it push it out and become part of the top k.
    * **So keep the top-k group, ordered so its *weakest* member is at the door.** That's a min-heap of size k. A max-heap puts the *strongest* at the top, which is the one element you never need to look at.
    * **The bouncer model (from the transcript):** a club with room for k. When someone new arrives, the bouncer only compares them with the **poorest person inside**. Richer → the poorest is kicked out and the new one gets in; otherwise, rejected. The richest member is buried inside and never consulted.
    * **Trace, k = 3, stream `100, 50, 200, 10, 75`:** `[100]` → `[50, 100]` → `[50, 100, 200]`, now full, top = 50 = 3rd largest ✔ → `10` < 50, rejected, top stays 50 → `75` > 50, evict 50 → `[75, 100, 200]`, top = 75 ✔.
    * **"The size is constant"** was the right instinct. After the club fills, the heap size never changes, which is why each step is O(log k), and k is fixed. The total number of values seen doesn't matter.

* **The Struggle & Insights:**
    * **The backwards `assert`.** `assert(maxHeap_.empty() && "Max heap should not be empty")` aborted on every non-empty heap.
        * **`assert(cond)` states what must be TRUE.** If `cond` is false, the program prints the expression and aborts. So write the condition you *expect*: `assert(!maxHeap_.empty() && "…")`.
        * **The `&& "message"` trick:** a string literal turns into a pointer that's never null, so it always counts as `true`. `cond && "msg"` has exactly `cond`'s truth value, and the text just shows up in the failure output. It's documentation, not logic.
        * `assert` disappears entirely when compiled with `NDEBUG` (typical release builds), so it's a debugging aid, never a substitute for a real check.
        * The heap it was guarding *would* have gone empty: the pop-k-times approach empties it within a few queries. The assert was pointing at the real problem.
    * **`k_` never assigned** (one transcript version). An `int` member with no initializer holds garbage, so `size() > k_` compared against random memory. Set it in the **member initializer list**, `KthLargest(int k, …) : k_(k) {…}`, so it holds the right value before the constructor body runs. That matters here because the body calls `add`, which reads `k_`.
    * **"WTF is `add` and how does it trim?"** `add` is just the class's own method, and the constructor may call it like any function. Feeding each initial number through `add` means the size-k cap is enforced from the very first element, so the heap never holds more than k (k+1 for a moment, in the push-then-pop version). My `756431b` instead pushed all N values, then let the first `add` trim with a `while` loop. That's also correct (verified), just O(N log N) up front and N values in memory.
    * **The bouncer check — my own idea: "can't I check the top *before* pushing?"** Yes. `push` knows nothing about my size cap: it always sifts the new value up, and the following `pop` sifts back down. Two O(log k) passes, just to throw away a value that was never going to get in. Checking first:
        * not full → push;
        * full and `val > top()` → pop, then push;
        * otherwise, reject in **O(1)**, without touching the heap.
        * For a stream where most values are too small to matter (the common case for top-k), most `add`s become a single comparison.
    * **Correction to the other LLM's "reallocation" claim.** It said popping before pushing avoids the vector **reallocating on every `add`**. That's not true. A `vector` grows geometrically and **`pop` never shrinks its capacity**. Measured in review: a million push-then-pop `add`s with k = 1000 changed the underlying capacity 11 times (1 → 2 → … → 1024), all while the heap was first filling, and **never again**. Pop-before-push saves at most one extra growth, when the capacity happens to be exactly k. The real gain from the bouncer is skipping the two sifts on rejects, not memory.
    * **Beyond the textbook:** an accepted value still costs `pop` + `push`, i.e. two sifts. A hand-rolled heap could **overwrite the root and sift down once** ("replace top"), saving half the work. `std::priority_queue` has no such operation.

* **C++ notes from this problem:**
    * **Min-heap type:** `std::priority_queue<int, std::vector<int>, std::greater<int>>`. All three template arguments are required, because the comparator is the third one.
    * **`size() < k_` compares `size_t` with `int`**, and `-Wall -Wextra` flags it (`-Wsign-compare`, verified). It's safe here since `k ≥ 1`, but storing `k_` as `size_t` would make it clean.
    * **Initializing the heap from the vector in the initializer list** (`minHeap_(nums.begin(), nums.end(), std::greater<int>())`, from the transcript) heapifies all N in O(N), then you pop down to k. Note that members are initialized in **declaration order**, not the order written in the list. The transcript listed `k_` first while `minHeap_` is declared first, which gets a `-Wreorder` warning. Harmless here (neither reads the other), but it's a real bug when one member's initializer uses another.

* **Time & Space Complexity:**
    * **Constructor:** O(N log k) by feeding each value through `add`.
    * **`add`:** O(log k) when the value gets in; **O(1)** when the bouncer rejects it.
    * **Space:** O(k). The heap never holds more than k values, however long the stream.
