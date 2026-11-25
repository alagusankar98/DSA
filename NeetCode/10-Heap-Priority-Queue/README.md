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

### [5] Task Scheduler

> Long and murky. I had the counting and the max-heap from the start; the wall was **idle time**: when can the next round start, and how do you count the gaps? Four broken versions before the one that works, first pass `4986d50`. It is correct (verified below), and there's an O(1)-after-counting formula that answers my own question: *"only the task frequency should matter, right?"* Yes, and that's the optimal solution.

* **The Core Pattern:** Heap + waiting area (§2 Pattern 4), simulated in **frames of `n + 1` slots**.
    * **Why `n + 1`:** a task that runs at slot `t` can run again at `t + n + 1` at the earliest. So in any window of `n + 1` consecutive slots, **each task can appear at most once**, and a task run in one frame is free again in the next.
    * **One frame:** pop up to `n + 1` **distinct** tasks from the max-heap (most remaining first), run each once, park the leftovers (count − 1) in a waiting room, then push the waiting room back for the next frame.
    * **Counting time:** if anything is left for a later frame, this frame costs the full `n + 1` (any slots not filled by tasks are idle). If nothing is left, it's the final frame, so stop right after the last task: add only the number executed, with no trailing idle.
    * **Why most-frequent first:** the task with the most copies left is the one that will run out of frames. Spending a frame slot on it now keeps the tail short. Rare tasks are **filler**: they're best spent in slots that would otherwise be idle.

* **The labels don't matter, only the counts.** `taskCount[task - 'A']` throws the letters away, and the heap only holds frequencies. That was the right instinct, and it leads to the formula below.

* **The four wrong versions, and what each one taught:**
    1. **Push the decremented count straight back during the same round.** The heap re-sorts immediately, so `A` (10 → 9) is the top again and runs **twice in a row**, ignoring the cooldown. *Lesson:* a task that just ran must be **out of the heap** until its cooldown expires. That's what the waiting room is for.
    2. **Waiting room, but each round pops the whole heap (`currentHeapSize`).** No back-to-back runs any more, but a round now runs **every** distinct task once, and `A` sits in the waiting room long after its cooldown has expired. On the example (`A×3, B×3`, 9 singletons C–K, `n = 7`) it returns **21; the right answer is 18**. All nine singletons get burned in round 1, so later rounds have nothing to fill the gaps between `A`s and `B`s, and those gaps become idle. Optimal: `A B C D E F G H | A B I J K _ _ _ | A B`. *Lesson:* a frame is **`n + 1` slots**, not "everything available". It's the cooldown that sets the frame size, not the heap size.
    3. **Frame of `n + 1`, but `minCount++` only when a task runs.** The loop exits early when the heap is empty, so idle slots are never counted. `A×2, n = 2` returns 2 instead of 4 (`A _ _ A`). *Lesson:* idle slots cost time too, and the code has to count them somewhere.
    4. **Full-frame charge decided by `!maxHeap.empty()`.** The question being asked is "is there **any** work left after this frame?" Work left lives in two places: tasks **still in the heap** (not reached this frame) and tasks **in the waiting room** (ran this frame, have copies left). After the frame loop, the heap is usually empty *because* everything is parked in the waiting room, so `!maxHeap.empty()` says "done" while work remains, and the idles get dropped. Wrong on 1,406 of 3,000 random small cases (e.g. `A×2, n = 2` → 2, should be 4).
    * **Why checking only `taskQueue` is enough** (the fix in `4986d50`): the full condition is "heap not empty **or** waiting room not empty". But if the heap is still non-empty after the loop, the loop must have stopped at `i == n + 1`, so `executedCount == n + 1`, and both branches add the same amount. So the heap half of the condition never changes the result, and checking only the waiting room is enough.
    * **Q&A: why does `time += pq.empty() ? taskCount : n + 1;` *after* dumping the waiting room back work?** (When I tried it, it failed only because I forgot to increment the count inside the loop, not because of the check.) My mental block: the loop stops for two different reasons (frame full, or heap ran dry), so how can one check placed after the dump cover both?
        * **The line doesn't ask "why did the loop stop?" It asks "is there another frame after this one?"** A frame costs exactly one of two amounts: `n + 1` if another frame follows (any unused slots are real idle, because some task is waiting out its cooldown), or `taskCount` if it's the last (the CPU stops after the last task, so no trailing idle). The reason the loop stopped is already in `taskCount`.
        * **Frame full (`taskCount == n + 1`):** both branches give `n + 1`, so the condition is irrelevant. "Last frame?" only matters when the frame has empty slots.
        * **Heap ran dry early (`taskCount < n + 1`):** the heap was empty at loop exit, so after the dump **the heap holds exactly the waiting room**. `pq.empty()` therefore asks: "did any task that ran this frame have copies left?" Yes → those copies are stuck in cooldown, so the empty slots are forced idle → charge `n + 1`. No → nothing is waiting, so this was the final frame → charge `taskCount`.
        * **Trace, `A×2, n = 2`:** frame 1 runs `A` (`taskCount = 1`) and parks `A×1`; after the dump the heap is `{1}`, not empty → `+3` (`A _ _`). Frame 2 runs `A` and parks nothing; after the dump the heap is empty → `+1`. Total 4 ✔.
        * **Why it has to come after the dump:** that's the first point where all remaining work is in one container. Before the dump, this frame's leftovers are still in the waiting room, so `pq.empty()` would answer the wrong question (measured: 18,554 / 50,000 random inputs wrong). Checking the heap **after** the dump and checking the waiting room **before** it (my `4986d50`) are the same question, asked at two different points in the loop.
        * **The frames are a real schedule, not just a counting trick:** the literal sequence the algorithm builds (frame by frame, padded with idles) never violates the cooldown (checked on 20,000 random cases).

* **The optimal solution: the formula (frequencies only).** Let `f` = the highest frequency, and `m` = how many tasks share that frequency.
    * **Mental model, the skeleton:** lay out the most frequent task with the minimum gap: `A _ _ A _ _ A` (`n = 2`, `f = 3`). That's `f − 1` full frames of `n + 1` slots, plus a last frame holding just `A`. Any other task that also has count `f` joins the last frame too: `A B _ A B _ A B`, so the last frame holds `m` slots.
    * Every other task has fewer copies than there are frames, so it can always go into the gaps without breaking its own cooldown (one copy per frame).
    * **If the gaps fill up and there are still tasks left**, widen the frames: there's no idle at all, and the answer is just the number of tasks.
    * **`answer = max(tasks.size(), (f − 1) · (n + 1) + m)`**
    * **The example:** `f = 3`, `m = 2` (A and B), `n = 7` → `(3 − 1) · 8 + 2 = 18`, and `18 > 15` tasks, so 18 ✔.
    * Verified: the formula, my heap version, and a slot-by-slot simulation all match an exhaustive search (BFS over every possible schedule) on 3,000 small inputs. My heap version also matches the formula on 50,000 random inputs up to 200 tasks, 26 letters, `n ≤ 100`.

* **Code notes on `4986d50`:**
    * **The two comments are swapped.** `// No more tasks in queue` sits on the branch where the queue is **not** empty, and `// Tasks still in heap…` sits on the final-frame branch. On a murky problem, a wrong comment will mislead the reread. It should say: queue non-empty → more frames to come → charge `n + 1`; otherwise final frame → charge `executedCount`.
    * **`taskQueue` is used as a stack** (`push_back` / `back` / `pop_back`). That's fine, because the whole waiting room goes back into the heap at once and the heap re-sorts anyway, so the order doesn't matter. The name just suggests FIFO.
    * **`uint16_t` counts:** at most 10⁴ tasks fits (max 65,535), and the 26-entry `std::array` sits on the stack with no allocation. Nice embedded instinct. `std::less<uint16_t>` is the default, so spelling it out is optional.
    * `tasks` is only read, so `const std::vector<char>&` would document that (LeetCode's signature uses a non-const ref).

* **Is the heap even needed?** For this problem, no: the formula needs one counting pass plus a scan of 26 counts. The heap simulation is still worth knowing. It's the version that **generalizes** (if the problem asks for the actual schedule, or tasks have different durations, the formula is gone), and it's the shape of a real scheduler: ready queue = max-heap, blocked tasks = waiting room. That's how an RTOS ready list and a timer list work together. Measured at 10⁴ tasks: heap ~0.09 ms, formula ~0.006 ms. Both are instant at these limits.

* **Time & Space Complexity:**
    * **Heap simulation:** each frame does at most `min(n + 1, 26)` pops/pushes on a heap of ≤ 26 elements, so O(log 26) = O(1) per task. **O(N) total** (N = number of tasks), **O(1) space** (26 counts, ≤ 26 heap entries).
    * **Formula:** O(N) to count, O(26) to find `f` and `m`. **O(1) space.**

### [6] Design Twitter

> First design problem. I built the follow map on my own, then stalled: *"where does the heap go?"* An LLM conversation got me from there to the K-way merge. Two bugs along the way: I swapped follower and followee when building the map, and then lost a long time to a feed that pushed **the wrong tweet IDs even though the heap held the right ones**. First pass `559d6d0`, final `d9fdeb7` (the hard-coded `10` became a member). Verified against a brute-force feed on 20,000 random operation sequences, with 0 mismatches.

* **The Core Pattern:** K-way merge (§2 Pattern 5), newest first, so a **max-heap**.
    * **Store tweets per user, not globally.** Each user gets their own `vector<(time, tweetId)>`. A global counter `time++` on every post means each vector is **already sorted by time** for free: the newest tweet is always `back()`.
    * **The feed = merge the K sorted vectors** belonging to the user and everyone they follow. Seed the heap with **one** entry per source (its `back()`), pop the newest, and push the **next-older tweet from the same source**. Stop at 10 tweets or when the heap is empty.
    * **The heap entry is a bookmark:** `(time, tweetId, userId, index)`. `time` comes first, so tuple comparison orders by it with no custom comparator. `userId` + `index` say *where to reload from*: after popping index `i` of user U, push U's index `i − 1` if `i > 0`. The source vectors are never touched.

* **Where I was stuck, and why the first ideas failed:**
    * **"Track tweets with a time counter."** Right, and it fixes the real worry: the problem only says tweet IDs are *unique*, not increasing, so the ID can't be used as a timestamp. The counter can.
    * **One global max-heap of `[time, (user, tweetId)]`, filtered per query.** I rejected this myself: a heap can't be iterated, so filtering means popping (and later re-pushing) every irrelevant tweet in the whole system on every feed call. If I follow 2 people out of millions, almost all of that work is wasted. The fix is to **not mix the tweets in the first place**, so only relevant sources ever enter the heap.
    * **"Won't A's 100 old tweets clog the feed?"** (A follows B and C; A tweeted 100 times two days ago, B and C 30 times each today.) No. Each source has **only one** entry in the heap at a time, its current newest. A's best entry sits at the bottom while B and C keep winning and reloading; it only rises once their newer tweets run out. The heap compares *heads of lists*, not whole lists. (The feed does include your own tweets; that's in the problem statement.)
    * **"How do I iterate over the user plus their followees?"** Copy them into one flat `relevantUsers` vector first (self + followees), then seed from that. Simple and cheap.
    * **"How do I advance without popping the source vector?"** That's what the `index` field is for (above). Never mutate the timelines to build a feed.

* **Bug 1: follower vs followee swapped.** `follow(followerId, followeeId)` means *follower now follows followee*, so the map must be **follower → set of people they follow** (`map[followerId].insert(followeeId)`). The feed reads `map[userId]` = "who do I follow". Swapping them makes the feed show the people who follow *you*. The names in the final code still pull the wrong way: the member is `followersMap_`, but it holds **followees**, and the loop variable `followerId` in `getNewsFeed` is actually a followee. Renaming them (`followeesMap_`, `followeeId`) makes the swap hard to repeat.

* **Bug 2 (the long debug): `auto& [...] = maxHeap.top(); maxHeap.pop();`**
    * **What happened:** `top()` returns a `const&` to **slot 0 of the heap's internal vector**. `auto&` binds the structured-binding names to *that slot*, not to a copy of the tuple. `pop()` then does `pop_heap`: it swaps slot 0 with the last slot, sifts the new slot-0 element down, and `pop_back`s the old top. My names still point at slot 0, which **now holds a different tweet**.
    * **The trace from my debugger:** heap `{(1,6,2,0), (0,5,1,0)}`. After `pop()` the only element left, `(0,5,1,0)`, sits in slot 0, so `tweetId` reads **5** where it should read **6**. That's why "the heap has the right elements but the feed is wrong": the heap *was* right; my names were looking at the wrong element. Reproduced exactly in review.
    * **Correction to the other LLM: it's not a use-after-free, it's aliasing.** Slot 0 is still a live element, and the memory was never freed. **ASan stays silent** (checked, even in the 1-element case where the popped slot is destroyed: the vector keeps its capacity, so no memory is returned). No sanitizer will catch this. Only the habit will.
    * **The fix:** `auto [...] = maxHeap.top();` copies 16 bytes onto the stack before `pop()`. **Rule:** anything read from `top()` / `front()` / `back()` that's still needed after a `pop` must be a **copy**. Same family as holding a reference into a `vector` across a `push_back` that reallocates. Putting both statements on one line made it look atomic, but they're two separate steps.

* **Corrections to the other LLM's interviewer review:**
    * **"Time O(U + 10 log U)":** seeding pushes U entries one at a time, which costs **O(U log U)**, not O(U). To get O(U), build the seed entries in a `vector` and construct the heap from it (O(N) heapify, see §1). The 10 pops/pushes add O(10 log U) on top.
    * **"Space drops to O(1)":** no. `relevantUsers` and the heap are both **O(U)** per call.
    * **"`if`-with-initializer prevents use-after-free":** it limits the iterator's scope, which is good hygiene, but it doesn't prevent anything memory-related. Today's actual memory bug was somewhere else.
    * **"Strong hire" on its own isn't a review.** The useful parts were the push vs pull follow-up and the magic number, below.

* **Code notes on `d9fdeb7`:**
    * **`maxTweets` as a runtime member:** every `Twitter` object now stores 4 bytes, initialised in the constructor, for a value that never changes. `static constexpr int kMaxFeed = 10;` costs no storage, is a compile-time constant, and says *"this is fixed by the spec"*.
    * **`followersMap_[userId]` in `getNewsFeed`** (twice) uses `operator[]`, which **inserts** an empty set for a user who follows no one. Harmless, but it means a read-only query allocates. **The loop isn't the problem:** it correctly runs zero times. The insert happens one step earlier, in the *lookup*: `map[key]` must return a reference to *something*, so for a missing key it creates the entry first. That's also why `getNewsFeed` couldn't be marked `const` (`operator[]` has no `const` overload). `find` once, then reuse the iterator, like I already do for `tweetListMap_`. The same applies to `tweetListMap_[user]` in the reload: the key is guaranteed to exist there, so `.at()` or a saved reference states that.
    * **Self-follow:** `follow(1, 1)` puts user 1 into `relevantUsers` twice, so every tweet of theirs appears **twice** in the feed (measured: one tweet → feed of size 2). LeetCode accepted the code, so its tests don't seem to hit this, but one compare in `follow` (`if (followerId == followeeId) return;`) closes it.
    * **Mixed signedness:** time is `unsigned` in the counter and the tuple, but `int` in the stored `pair<int, int>`. Pick one type for it.
    * **Kept from the review, and correct:** `std::ssize` (no `size() - 1` underflow), `emplace` / `emplace_back` building in place, and tuple ordering by its first member instead of a custom comparator. `_` as a name: in C++26 it becomes a real "don't care" placeholder; before that, it's an ordinary variable.

* **Follow-up fixes (after review):** `static constexpr` cap, self-follow guard, `.at()` in the reload, `find` instead of `operator[]`, and `getNewsFeed` is now `const` (the compiler confirms nothing in it writes). **The `reserve` question:** I thought moving the lookup into the `if` cost me the `reserve`. It doesn't. Found → `reserve(size + 1)`, then the followees, then `userId`, with no reallocation. Not found → a single `push_back` into an empty vector allocates exactly once, which is what `reserve(1)` would cost anyway. Pushing `userId` **last** matters: pushed first, it would allocate a 1-slot buffer that the `reserve` then throws away. The order inside `relevantUsers` doesn't matter, since the heap sorts by time.
* **The system-design follow-up (worth knowing for HLD):** this is the **pull** model (fan-out on read): the feed is assembled when it's requested. That's cheap for writes, but the cost of a read grows with the number of followees. The alternative is **push** (fan-out on write): each post is copied into every follower's precomputed feed, so reads are cheap and writes are expensive for accounts with millions of followers. Real systems mix them: push for normal accounts, pull for celebrities, merged at read time. The structure is the same producer/consumer trade-off as polling vs interrupts: do the work when data arrives, or when someone asks for it.

* **Time & Space Complexity** (U = followees + 1, T = total tweets):
    * `postTweet`, `follow`, `unfollow`: **O(1)** average (hash map / hash set + `emplace_back`).
    * `getNewsFeed`: **O(U log U)** to seed (O(U) with heapify) + **O(10 log U)** for the merge. **O(U)** extra space.
    * Storage: **O(T + total follow edges)**.

### [7] Find Median from Data Stream

> My first Hard. I built the intuition last night with an LLM and coded it this morning, first pass `95103f6`. I mirrored the standard solution (the **min**-heap holds the extra element), which forced me to work out the rebalancing rules myself instead of copying them. The overflow fix in `findMedian` came after review. Verified against a sorted brute-force median: 20,000 random streams, 608,003 `findMedian` checks, 0 mismatches.

* **The Core Pattern:** two heaps (§2 Pattern 3). The median only needs the **two numbers at the middle** of the sorted list. Split the sorted list in half: the middle values are the **largest of the lower half** and the **smallest of the upper half**. Those are exactly the two things heaps give you in O(1).
    * **Lower half → max-heap** (its top is the biggest of the smalls). **Upper half → min-heap** (its top is the smallest of the bigs).
    * **Two invariants, both required:** (1) **order**: everything in the max-heap ≤ everything in the min-heap; (2) **balance**: `minHeap.size() − maxHeap.size()` is **0 or 1** (my version; the standard one lets the max-heap hold the extra).
    * **Median:** equal sizes → average the two tops. Unequal → the top of the bigger heap (the min-heap, in my version).

* **Where I was stuck, and how each one resolved:**
    * **"This is just a vector, isn't it?"** I suspected a catch, and it's the word *sorted*. Unsorted push + `std::sort` per query is O(N log N) per `findMedian`. Keeping a vector sorted means O(N) shifting per insert. Heaps get O(log N) insert and O(1) query because they never sort what isn't in the middle.
    * **I described the halves backwards at first.** For `[9,8,7,6,5]` I said "min-heap `[5,6]`, max-heap `[9,8,7]`". It's the reverse: the **max-heap holds the small numbers**. The name tells you what sits on *top*, not what the heap holds. Same trap as in K Closest Points `[3]`: say "small half → max-heap" before typing.
    * **"How can I know the pivot before the heaps are built?"** You don't need to. **The pivot is just the top of the primary heap**, and it moves as numbers arrive. Each new number is routed by comparing it with that top. If I balance by size alone (alternating heaps), the order invariant breaks and I get scrambled halves like `[7, 9]` / `[8, 6, 5]`.
    * **Rebalance in `addNum` or in `findMedian`?** In `addNum`, every time, for two reasons. **Correctness:** routing reads the top, so if balance is allowed to drift, the top stops being the middle and later numbers go to the wrong side, breaking the order invariant. A lazy fix in `findMedian` would have to repair an unbounded mess. **Cost:** fixed immediately, the imbalance is never more than 1, so the fix is at most one pop + one push.

* **Why rebalancing needs two conditions (my "can't convince myself" question):** look at the size difference `d = min.size() − max.size()`.
    * **Before an add, `d` is 0 or 1** (the invariant).
    * **An add changes `d` by exactly 1:** routed to min → `d + 1`; routed to max → `d − 1`.
    * So after routing, `d ∈ {−1, 0, 1, 2}`. There are **two** illegal values, **one at each end**: `−1` (max got bigger) and `2` (min ran ahead by two). Each end needs its own check, and a single condition guards only one side. With only my `max > min` check, `d = 2` goes uncaught, so min keeps growing and the max-heap never gets anything.
    * **One move fixes either case:** moving a top across changes `d` by 2. `−1 → 1`, `2 → 0`. Both are back in range.
    * **`else if` is safe:** `d` can't be `−1` and `2` at the same time, so at most one fix runs.
    * **Moving the *top* keeps the order invariant:** the max-heap's top is the largest of the small half, so on the other side it's still ≤ everything in the min-heap. The same holds in reverse.

* **Code notes on `95103f6` (+ the working-tree fix):**
    * **The empty-check bootstrap is exactly right.** Because `d ≥ 0` always, an empty min-heap implies both are empty, so `minHeap_.empty()` means "first number". It could also be folded into the routing (`if (minHeap_.empty() || num > minHeap_.top())`), after which the rebalance does nothing, but the early return reads clearly.
    * **Ties (`num == minHeap_.top()`) go to the max-heap.** That's fine: `num ≤` every element in the min-heap, so the order invariant holds.
    * **Cost per `addNum`:** 1 push, plus at most 1 pop + 1 push. The common alternative ("always push to max, move its top to min, then rebalance") always does 3 operations. Routing first skips the move when the number already lands on the right side.
    * **Overflow fix:** `static_cast<double>(a + b)` adds in `int` **first**. Casting one operand first (`static_cast<double>(a) + b`) makes the addition happen in `double`. Measured: two `2'000'000'000`s give **−147483648.0** with the old line (UBSan: *"signed integer overflow"*) and `2000000000.0` with the fix. **Correction to the other LLM:** signed overflow is **undefined behaviour**, not a guaranteed wrap to negative. The wrap is just what x86 happens to do. And on LeetCode it can't happen at all (`|num| ≤ 10⁵`). The fix is still the right habit: it's the same bug as `(lo + hi) / 2` in Binary Search.
    * **Small cleanups:** `findMedian` can be `const` (it only reads tops). The empty constructor can go (or `= default`). The comment *"Max heap should always be <= min heap"* is about **size**, but reads like the **order** invariant. Say "size" to keep the two invariants apart.
    * **"Beats 5% on LeetCode": is it slow?** No. Measured at the problem's limit (25,000 adds + 25,000 medians, `-O2`): **mine 0.99 ms**, the textbook "always push to max, move its top to min" version **2.04 ms**, a sorted `vector` with `insert` 9.1 ms. Mine is already ~2× the standard solution, because routing first skips work. Results that small are swamped by LeetCode's harness and machine noise (resubmitting moves the percentile), so the percentile says nothing about the algorithm. An interviewer judges complexity, correctness, and how you explain them, and that's O(log N) / O(1), which is optimal.
    * `findMedian` on an empty stream is UB (`top()` on empty). The problem guarantees at least one number first, so no check is needed, but it's worth saying out loud in an interview.

* **Follow-ups an interviewer asks (from the LeetCode problem):**
    * **All numbers in `[0, 100]`:** drop the heaps. Use a `std::array<int, 101>` of counts plus a running total: `addNum` is O(1), and `findMedian` walks the counts to the middle in O(101) = O(1). That's the histogram trick, a fixed-size table with no allocation, very embedded.
    * **99% in `[0, 100]`:** same buckets, plus two counters for "below 0" and "above 100" (or small heaps for those outliers). The median almost always lands in a bucket.
    * **Sliding-window median** (last W samples only, the real sensor-filter version): now numbers must **leave**, and heaps can't remove arbitrary elements. You need lazy deletion (§2 Pattern 6) or a `std::multiset` with an iterator at the middle.

* **Time & Space Complexity:**
    * `addNum`: **O(log N)** (≤ 3 heap operations). `findMedian`: **O(1)**.
    * Space: **O(N)**: every number is stored once, split across the two heaps.
