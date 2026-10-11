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

### ⭐ My Heap Cheat Sheet (the click from K Closest Points, in my own words)
After three problems of mixing up min and max, this is the summary that finally made it stick. **Reread this first.**

**Which heap for top-K:**
* **K largest elements:** a **min-heap of size K** (the smallest of the big ones sits at the door, ready to be kicked out when someone bigger arrives), **or** dump everything into a **max-heap** and pop K times.
* **K smallest elements:** a **max-heap of size K** (the biggest of the small ones sits at the door), **or** dump everything into a **min-heap** and pop K times.
* The size-K version is the one to use: O(N log K) time, **O(K)** memory, and it works on a stream. Dump-and-pop costs O(N + K log N) time and **O(N)** memory (the heap holds a copy of everything).

**What the comparator means:**
* **`std::less` is `data_1 < data_2` → max-heap.** What it really asks: *"should `data_1` be placed **below** `data_2`?"* Smaller ones sink, so the **largest** stays on top.
* **`std::greater` is `data_1 > data_2` → min-heap.** Same question, *"should `data_1` be placed below `data_2`?"*, but now larger ones sink, so the **smallest** stays on top.
* The same applies to a custom lambda: whatever operator it uses on the key, read it as "**who sinks**". `<` → max-heap, `>` → min-heap. (`std::sort` reads the same comparator as "who comes **first**", which is why the two feel opposite.)

**A self-check that works on any code:** look at the eviction condition. `if (new < top()) { pop(); push(new); }` only makes sense if `top()` is the **largest** in the heap (you can't be smaller than the minimum and still have something to evict), so it's a max-heap, whatever the variable is called.

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

> First heap problem, and it took a while, not for the code but for the *why*: "I want the k-th **largest**, so why a **min**-heap?" Four versions: `756431b` (push everything, trim on `add`), `c7c3426` (constructor reuses `add`), `8d9c070` (checks before pushing, the "bouncer"), and the current file, which builds the heap with the iterator constructor in the initializer list (see the last Q&A below: measured, that change is a step *back* for small k).

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

* **Q&A (follow-up from Last Stone Weight): "build the heap in O(N) in the initializer list" — measured, it's slower here.** The other LLM suggested replacing the `add` loop with `minHeap_(nums.begin(), nums.end())` in the initializer list (heapify in O(N)), then `while (size() > k) pop();`. **The answer to my earlier "couldn't call the constructor because it's a class member" is the initializer list:** members are constructed there, before the body runs, so any member constructor can be called. But here it doesn't pay:
    * **Heapify is O(N), but the trim isn't.** Getting from N elements down to k is **(N − k) pops at O(log N) each**. The `add` loop never holds more than k elements, and with the bouncer most values are rejected in O(1).
    * **Measured in review** (N = 10⁶ random values, `-O2`, this machine):

        | k | `add` loop (`8d9c070`) | heapify + trim (current) |
        | -- | -- | -- |
        | 10 | 0.6 ms | 105 ms |
        | 1000 | 1.0 ms | 102 ms |
        | 500,000 | 39 ms | 53 ms |
        | 999,990 | 11 ms | 6.7 ms |

    * **Rule:** heapify + trim wins only when k is close to N, i.e. few pops. For the usual top-k case (k ≪ N) the size-k loop is far better, and it also uses O(k) memory instead of O(N). LeetCode 703's limits (≤ 10⁴ numbers) are too small to notice either way, but in an interview, the `add` loop is the one to defend.
    * **The LLM's "saves a double allocation" reason is also off:** an empty `vector` doesn't allocate at all. The real allocation difference is that the iterator constructor sizes its buffer once, while repeated pushes grow it geometrically (a handful of reallocations).
    * **Two warnings in the current constructor (`-Wall -Wextra`, verified):**
        * **`-Wreorder`:** the list says `k_(k), minHeap_(…)`, but `minHeap_` is **declared** first, so it is constructed first anyway. Harmless here; write the list in declaration order so it reads the way it runs.
        * **`-Wsign-compare`:** `while (minHeap_.size() > k)` compares against the `int` parameter. Use `k_` (already `size_t`).
    * Kept from this change: `const std::vector<int>&` (the constructor only reads `nums`) and `size_t k_` (silences the earlier `size() < k_` warning).

* **Time & Space Complexity:**
    * **Constructor:** O(N log k) by feeding each value through `add` (heapify + trim: O(N + (N − k) log N), see the table above).
    * **`add`:** O(log k) when the value gets in; **O(1)** when the bouncer rejects it.
    * **Space:** O(k). The heap never holds more than k values, however long the stream.

### [2] Last Stone Weight

> A direct heap simulation. Quick to code; the learning was all in the follow-up questions: why building from the vector is O(N), whether to stop pushing in loops, and how the constructor arguments work. First pass `1bc465e`; the current file is the cleaned-up version.

* **The Core Pattern:** Repeated "take the best, put something back" (§2 Pattern 2). Every round needs the **two heaviest** stones, and the result (if any) goes back into the pile. A **max-heap** gives the heaviest in O(1) and removal in O(log N), so each round is two pops plus at most one push. Stop when at most one stone is left: return its weight, or `0` if the heap is empty.

* **What I got right on the first pass:**
    * **Built the heap from the vector in one go** instead of a push loop (O(N) heapify, see the Q&A).
    * **`while (size() > 1)`** guarantees there are two stones before two `pop`s, so there's never a `top()` on an empty heap.
    * **The `empty() ? 0 : top()` return** covers the case where the last two stones destroy each other.

* **The Struggle & Insights:**
    * **`std::abs` was unnecessary.** `first` is popped before `second` from a max-heap, so `first ≥ second` is guaranteed and `first - second` is never negative. Using `abs` there isn't wrong, but it hides an invariant you should be able to state: **the order you pop from a heap *is* sorted order.**
    * **Q&A: how does building from the vector make it O(N) instead of O(N log N)?** A push loop inserts each value at the bottom and sifts it **up**. Late arrivals land at the bottom of a tall tree, so the total is O(N log N). Heapify (`std::make_heap`, which the vector and iterator constructors call; also known as Floyd's algorithm) puts all N values in the array first, then sifts **down** each non-leaf node, from the last one back to the root. Half the nodes are leaves and don't move, a quarter move at most 1 level, an eighth at most 2… The total work sums to less than N. **Most nodes live near the bottom, and the bottom ones barely move.** (Detail in §1, "Building a heap from N elements".)
    * **Q&A: "Should I avoid inserting into a heap with a for loop?"** Only when **all the data is available up front.** Then heapify (O(N)) beats N pushes (O(N log N)). When values arrive **one at a time** (a stream, like `add` in `[1]`), `push` is the only option, and that's fine. And O(N) heapify isn't automatically the faster *overall* plan. In `[1]`, heapify followed by trimming to k was measured up to ~170× slower than a size-k push loop (see `[1]`'s last Q&A). **Count the pops that come after the build.**
    * **Q&A: iterators, and where the comparator goes.**
        * **First pass, `priority_queue<int> maxHeap(std::less<int>(), stones);`:** this is the **(comparator, container)** constructor. It copies `stones` into the heap's vector, then heapifies. Valid, but the comparator *object* has to be spelled out because it's the first parameter.
        * **Current, `(stones.begin(), stones.end())`:** the **iterator** constructor. It copies the range and heapifies, the same O(N) cost. The comparator object defaults to a default-constructed one of the type named in the template arguments.
        * **"For a min-heap, does `greater` go third, after `.end()`?"** No need. The comparator **type** in the template arguments (`priority_queue<int, vector<int>, greater<int>>`) is what makes it a min-heap. The constructor creates a `greater<int>` by itself. You pass a comparator **object** as the third argument, `(first, last, cmp)`, only when it can't be default-constructed. Verified: a **captureless lambda** can be default-constructed in **C++20**, so `priority_queue<int, vector<int>, decltype(cmp)> pq(v.begin(), v.end());` compiles. In **C++17** it fails ("use of deleted function"), so there you must pass `cmp`. Lambdas that **capture** variables always need the object passed in.
        * My current file spells out `std::less<int>` in the template arguments. It's redundant (it's the default), but it does make "this is a max-heap" explicit.
    * **Verified:** both versions match a sort-based brute force on 50,000 random inputs (n ≤ 30, weights ≤ 1000).

* **Beyond the textbook:** weights are capped at 1000, so a **count array** indexed by weight (walk down from the heaviest non-empty bucket) solves it in O(N + W) without a heap. Overkill at n ≤ 30, but it's the standard trick when the value range is small.

* **Time & Space Complexity:**
    * **Time O(N log N):** O(N) to heapify, then at most N − 1 rounds (each round removes at least one stone), each with 2 pops and ≤ 1 push at O(log N).
    * **Space O(N):** the heap holds a copy of `stones` (the input is `const&`, so it can't be heapified in place; with a mutable vector, `std::make_heap` on the input itself would need O(1) extra).

### [3] K Closest Points to Origin

> The problem where min-vs-max finally clicked, and where I wrote my first lambda comparator. First pass `64180e1` (min-heap of all N points, pop K); optimal `cb3d81c` (max-heap capped at K, the bouncer). The click is written up at the top of the guide: **§1, "My Heap Cheat Sheet"**.

* **The Core Pattern:** top-K with the **opposite** heap (§2 Pattern 1). "Closest" means the **K smallest** distances, so keep a **max-heap of size K** keyed on squared distance `x² + y²`. Its `top()` is the **farthest** point still inside, the bouncer. For each point: club not full → push; closer than the bouncer → pop the bouncer, push the point; otherwise reject without touching the heap. At the end, the heap holds exactly the K closest. Order doesn't matter, so draining it straight into the result is fine.
    * **Squared distance, no `sqrt`:** `√` only increases with its input, so comparing `x² + y²` gives the same order as comparing the true distances. It's also exact integer math with no float rounding, and it avoids a slow `sqrt` per comparison.

* **My first approach (`64180e1`), and why it isn't the answer:** a min-heap built from **all** points with the iterator constructor (O(N) heapify), then pop K times. **Correct** (verified), and the smallest distance really is at the top. But the heap is a full copy of the input: **O(N) memory** to answer a question about K points. With 10⁷ points and K = 5, that's 10⁷ `vector<int>` copies just to read five.

* **The wall: "is this a min-heap or a max-heap?"** I wrote the optimal version, named it `minHeap`, and couldn't tell which one it was. It was a **max-heap** (comparator `<`), and a max-heap is what the problem needs. Two ways to see it without guessing:
    * **Read the eviction condition:** `if (dist(point) < dist(top())) { pop(); push(point); }`, i.e. "if the new point is closer than the top, kick the top out". That only makes sense if the top is the **farthest** point inside. So `top()` = max → max-heap.
    * **Read the comparator as "who sinks":** `dist(p1) < dist(p2)` returns true when `p1` is closer, so `p1` goes **below** `p2`. Closer points sink, the farthest floats up → max-heap.
    * **Then I flipped the question myself: "can't I use a min-heap of size K?"** No. With `{10, 20, 50}` in a min-heap, `top()` = 10. A new point at 15 belongs in the top 3 (it should replace 50), but 50 is buried at the bottom and a heap can only touch its top (§1 Complexity Summary: finding anything else is O(N)). **A min-heap can't see the element you need to evict.** It only works with no size cap, which is exactly my first pass.
    * The rule in one line: **the top of the heap must be the element you'd evict next.** For K smallest, that's the largest of them → max-heap.

* **The Struggle & Insights:**
    * **First lambda: `'this' cannot be implicitly captured`.** `calculateDistance` is a member function, so calling it from inside the class really means `this->calculateDistance(...)`. A lambda is its own anonymous class with an `operator()`. It can't see the enclosing object unless you hand it `this`, which is what `[this]` does (it copies the pointer into the lambda object). Better, now that I know: `calculateDistance` doesn't use any member data, so make it `static` (or a free function). Then the lambda captures nothing, `[]` works, and it's a plain function with no hidden pointer.
    * **The template typo: `priority_queue<int, vector<vector<int>>, …>`.** The first template argument is the **element type**, and it has to match what the container holds and what the comparator takes: `vector<int>`, not `int`. Also missing: the `>` closing the template list before the variable name. The error ("unknown type name `minHeap`") pointed past the real mistake, as template errors usually do.
    * **The SEGV at address `0x8`: an empty heap and a backwards `assert` again.** One draft never pushed anything into the heap and then called `top()` K times. `top()` on an empty `priority_queue` is **undefined behaviour**: it reads element 0 of an empty vector, whose data pointer is null, so AddressSanitizer saw a read near the zero page while copying "the vector at `nullptr`". The `assert(minHeap.empty() && …)` should have caught it, but it was backwards (the same bug as `[1]`): it asserted the heap *was* empty, so it passed. **`assert` states what must be true: `!empty()`.**
    * **Capturing lambdas must be passed to the constructor.** `decltype(compare_coordinates)` names the lambda's type, and `minHeap(points.begin(), points.end(), compare_coordinates)` passes the object. Because this lambda captures `this`, it can't be default-constructed, even in C++20 (`[2]`'s Q&A). With a `static` helper and `[]`, C++20 would let you skip it.
    * **`long long` return type, but the math is still `int`.** `point[0] * point[0] + point[1] * point[1]` is computed in `int`, and only the **result** is widened to `long long`. If the multiply overflowed, the UB would already have happened. To actually get 64-bit math, widen an operand: `1LL * point[0] * point[0]`. Here it doesn't matter: coordinates are within ±10⁴, so the max is 2·10⁸, well inside `int`.
    * **Stale comments.** The optimal file still says `// We need a min heap` and `// Push elements to min heap` above max-heap code. That's the exact confusion this problem was about, and comments like that are what would cause it again on a reread.

* **Beyond the textbook:**
    * **Every push copies a `vector<int>`, and every copy is a `malloc`.** A `vector<int>` is a pointer + size + capacity on the stack, with its two ints in a separate heap allocation. `push(point)` deep-copies it, and `push_back(top())` copies it again (`top()` is `const&`). Sifting is cheap (swapping vectors only swaps the three words), but the allocations aren't. The standard fix: store `pair<int, int>` = **(squared distance, index into `points`)**. It needs no allocations, compares lexicographically so the default `std::less` is already the max-heap you want (§1: pairs order by `.first`), and each distance is computed **once** instead of on every comparison. Copy the K winning points out at the end.
    * **Quickselect, `std::nth_element`:** partially sorts the range so the K closest end up in the first K slots (in no particular order), **O(N) average**. The other LLM called it O(1) space, but `points` is `const&` here, so you need a copy first, which is O(N) memory. It's the faster choice when the whole array is in memory, and the size-K heap is the one that works on a stream.
    * **Measured** (N = 10⁶ random points, `-O2`): first pass 76 ms (k = 10) / 82 ms (k = 1000); size-K max-heap **2.2 ms / 3.6 ms**. Both versions match a sort-based brute force on 20,000 random inputs. `-Wall -Wextra` flags `maxHeap.size() < k` (`-Wsign-compare`, `size_t` vs `int`), the same as `[1]`.

* **Time & Space Complexity:**
    * **First pass (min-heap of all):** O(N) heapify + O(K log N) pops; **O(N) space**.
    * **Optimal (max-heap of size K):** O(N log K) worst case, and **O(1)** for each rejected point; **O(K) space** for the heap (plus the O(K) output).

### [4] Kth Largest Element in an Array

> The first heap problem I solved straight from the cheat sheet in §1, with no confusion about min vs max. First pass `d420017`. It's the same bouncer loop as `[1]`'s `add`; the difference is that **the whole array is available up front**, which unlocks options a stream never has (see "Should I optimize further?").

* **The Core Pattern:** top-K with the opposite heap (§2 Pattern 1). K **largest** → **min-heap of size K**. Its `top()` is the weakest of the K largest seen so far, which is exactly the K-th largest. Club not full → push; bigger than the bouncer → pop, push; otherwise reject in O(1). After the loop, the answer is `top()`.

* **What I got right on the first pass:** chose the min-heap (`std::greater<int>`) straight from "K largest → min-heap", capped the size at K, and checked the bouncer **before** pushing (the `[1]` lesson), so rejected values never touch the heap.

* **Corrections to the other LLM's review:**
    * **"Memory reallocation prevented" by popping before pushing:** not really. The heap is at size K before *and* after every swap, and `[1]`'s measurement showed a vector reallocates only while it first fills, never once it's at steady size. Pop-then-push matters for skipping work on rejects, not for memory.
    * **"Returning `INT_MIN` on an empty heap is great defensive programming":** it's debatable. The constraints guarantee `1 ≤ k ≤ n`, so the heap can't be empty, and if the contract were broken, `INT_MIN` looks like a real answer and the caller can't tell. A sentinel is only safe when it can never be a valid result. Here an `assert(!minHeap.empty())` states the precondition more honestly. (Values are within ±10⁴, so `INT_MIN` happens to be unambiguous in practice. That's luck, not design.)
    * **"Strict O(N)" for Quickselect:** wrong. Quickselect is O(N) **on average**, but O(N²) in the worst case. `std::nth_element` in libstdc++ is **introselect**: quickselect that falls back to a heap-based method when partitions go badly, giving an O(N log N) worst case. It's also not free here: `nums` is `const&`, so you have to copy it (O(N) memory) before shuffling it.
    * **The `-Wsign-compare` warning is back** (`minHeap.size() < k`, `size_t` vs `int`), the same as `[1]` and `[3]`.

* **Should I optimize further? Measured** (random values in ±10⁴, `-O2`, avg of 20 runs; all five methods verified against `sort` on 20,000 random inputs):

    | N | k | my heap | heap, smaller side | heapify all + pop | `nth_element` (copy) | counting |
    | -- | -- | -- | -- | -- | -- | -- |
    | 10⁵ | 1 | 0.05 ms | 0.06 | 0.59 | 0.55 | 0.06 |
    | 10⁵ | 50,000 | 2.95 ms | 2.97 | 4.00 | **0.54** | **0.06** |
    | 10⁵ | 100,000 | 1.29 ms | **0.05** | 7.05 | 0.31 | 0.06 |
    | 10⁶ | 10 | 0.52 ms | 0.52 | 6.21 | 5.43 | 0.57 |
    | 10⁶ | 500,000 | 37.7 ms | 37.7 | 50.8 | **6.98** | **0.57** |

    * **Small k (the usual "top 10" case): my heap is already as fast as anything,** because almost every value is rejected in one comparison. Nothing to gain there.
    * **k near N/2 is the heap's worst case:** O(N log K) with K ≈ N/2, and many values get accepted. Quickselect wins by ~5×.
    * **"Smaller side" trick (a free win for k close to N):** the k-th largest is also the **(N − k + 1)-th smallest**. When `k > N/2`, keep a **max**-heap of size `N − k + 1` instead. The cost becomes O(N log min(k, N − k)). At k = N: 1.29 ms → 0.05 ms.
    * **Counting (exploits the constraint):** values are bounded to ±10⁴, so keep a count array of 20,001 buckets, tally every value, and walk down from the top subtracting counts until k is used up. O(N + R) with R = 20,001, flat ~0.06 ms at N = 10⁵ whatever k is. The count array is 80 KB, which fits in L2 cache, and the pass over `nums` is purely sequential. It only works because the value range is small. With full 32-bit ints it's useless, which is why it's a "read the constraints" trick, not the general answer.
    * **Verdict:** for an interview, the heap is the answer to write and defend: O(N log K), O(K) memory, and it also works on a stream. The follow-up to **prepare for** is "can you do it in O(N) average?", and interviewers usually want you to **explain or hand-write Quickselect**, not just call `nth_element`. Hand-writing it is the worthwhile next pass.

* **Quickselect from scratch (for the follow-up):**
    * **The idea:** it's quicksort's partition step, but you only recurse into **one** side. Pick a pivot and partition so that everything smaller is on the left and everything larger on the right. The pivot is now at its final sorted index `p`. The answer lives at index `N − k` (ascending order). If `p == N − k`, done. If `p < N − k`, repeat only on the right part; otherwise only on the left. Throw the other side away.
    * **Why O(N) on average:** a reasonable pivot halves the range each time, so the total work is `N + N/2 + N/4 + … ≈ 2N`. Quicksort is O(N log N) because it has to recurse into **both** sides.
    * **Why O(N²) in the worst case:** if the pivot is always the min or max (e.g. "always pick the last element" on sorted input), each step removes just one element: `N + (N−1) + …`. A **random pivot** makes that extremely unlikely.
    * **The duplicates trap:** with a plain 2-way partition, an array of all-equal values degenerates to O(N²) even with random pivots, because every element lands on the same side. A **3-way partition** (`< pivot | == pivot | > pivot`) fixes it: if `N − k` falls inside the `==` block, that's the answer. LeetCode 215 is known for tests that punish naive quickselect this way.
    * **Heap vs Quickselect, in one line:** the heap reads each value once and keeps K of them (streams, small K, O(K) memory). Quickselect needs the whole array in memory and rearranges it (array only, O(N) average, O(1) extra if you're allowed to modify the input).

* **Time & Space Complexity:**
    * **Heap (current):** O(N log K) time, O(K) space; O(N log min(K, N − K)) with the smaller-side trick.
    * **Quickselect:** O(N) average, O(N²) worst (O(N log N) worst for introselect); O(1) extra in-place, O(N) here because of the copy.
    * **Counting:** O(N + R) time, O(R) space, R = value range.
