# Graphs - Topic Guide

## 1. Core Fundamentals

A **graph** is a set of **nodes** (also called vertices) connected by **edges**. That's the whole definition. Everything you've used so far is a special case: a **linked list** is a graph where each node has one outgoing edge, and a **tree** is a graph with no cycles where every node except the root has exactly one parent. A graph drops those restrictions. A node can have any number of neighbours, edges can form **cycles**, and parts of the graph can be completely **disconnected** from each other.

### The Vocabulary (the words every problem uses)
* **Directed vs undirected.** In an **undirected** graph an edge `A — B` goes both ways (friendship, a road). In a **directed** graph `A → B` goes one way only (course A is a prerequisite of B, a web link). Course Schedule is directed; Number of Islands is undirected.
* **Neighbours / adjacent:** the nodes one edge away.
* **Degree:** how many edges touch a node. In a directed graph, split it into **in-degree** (edges coming in) and **out-degree** (edges going out). In-degree is the key quantity in topological sort.
* **Path:** a sequence of nodes where each consecutive pair is joined by an edge. **Cycle:** a path that comes back to its start.
* **Connected component:** a group of nodes that can all reach each other, with no edges leading out of the group. "Count the islands" = "count the connected components".
* **Weighted:** edges carry a cost (distance, time). This topic is almost all **unweighted**. Weights (Dijkstra and friends) are topic 13, Advanced Graphs.
* **DAG (directed acyclic graph):** directed, with no cycles. Prerequisites, build systems and task dependencies are DAGs. Only a DAG can be put in a valid order.

### How a Graph Is Stored (pick before coding)
A tree came with its structure built in (`left` / `right` pointers). A graph usually arrives as **raw data**, and the first step is turning it into something you can walk.
| Representation | What it is | Neighbours of `u` | Memory | Use when |
| -- | -- | -- | -- | -- |
| **Adjacency list** | `vector<vector<int>> adj(n)`, where `adj[u]` holds `u`'s neighbours | loop over `adj[u]`: O(degree) | O(V + E) | **The default.** Almost every problem here |
| **Adjacency matrix** | `n × n` grid, `m[u][v] = 1` if there's an edge | scan a whole row: O(V) | O(V²) | Small dense graphs, or "is there an edge u–v?" in O(1) |
| **Edge list** | `vector<pair<int,int>>` of edges, often the input format | must scan all edges | O(E) | Input format. Also what Union-Find consumes directly |
| **Grid (implicit graph)** | a 2-D array where each cell is a node and its 4 neighbours are the edges | 4 direction offsets | nothing extra | Islands, Rotting Oranges, Walls and Gates, Surrounded Regions |

* **Building an adjacency list from an edge list:** for each edge `(u, v)`: `adj[u].push_back(v)`, and for an **undirected** graph also `adj[v].push_back(u)`. Forgetting the second line is the classic bug.
* **Grids are graphs you never build.** You already did this in Word Search: the "neighbours" of `(r, c)` are `(r±1, c)` and `(r, c±1)`, computed on the spot with a direction array, plus a bounds check.

### Traversal: the Two Ways to Walk a Graph
Both visit every reachable node exactly once. They differ in **order**, which is the same split you saw in Letter Combinations `[9]` (cascade vs backtracking).
* **DFS (depth-first search):** go as deep as possible along one path, then back up and try the next branch. Implemented with **recursion** (the call stack) or an explicit **stack**. It's exactly the Trees DFS, plus a `visited` check.
* **BFS (breadth-first search):** visit everything 1 edge away, then everything 2 edges away, and so on, in rings. Implemented with a **queue** (`std::queue`). It's exactly the Trees level-order traversal, plus a `visited` check.
* **The one rule that changes from Trees: `visited`.** A tree can't loop back on itself, so you never revisit a node. A graph can (A → B → C → A), so without a `visited` record you loop forever, or recurse until the stack overflows. **Mark a node visited when you first discover it** (when you push it onto the queue or stack), not when you pop it. Marking on pop lets the same node be queued many times.
* **This `visited` is permanent, unlike Backtracking's.** In Word Search, "visited" meant "on the current path", so it was cleared on the way back. In graph traversal a visited node stays visited forever, because the question is "have I reached this node at all?", not "is it on my current path?". (Backtracking §1 "Grid Backtracking" makes the same point from the other side.)

### BFS Gives Shortest Paths (in unweighted graphs). DFS Doesn't.
BFS reaches nodes in order of distance: everything at distance 1 before anything at distance 2. So **the first time BFS reaches a node is along a shortest path**, measured in edges. That's why "minimum steps / minutes / moves" problems are BFS (Rotting Oranges, Walls and Gates, Word Ladder). DFS finds *a* path, not necessarily the shortest one.
* **Processing level by level:** take `int size = q.size();` at the start of a round and pop exactly that many. Each round is one more step of distance, the same trick as level-order traversal in Trees.
* **Multi-source BFS:** push **every** starting node into the queue before the loop (all rotten oranges, all gates). The rings then grow from all sources at once, and each cell gets the distance to its *nearest* source in a single pass. It's the key idea of two problems in this topic.

### Topological Sort (ordering a DAG)
"Course B requires A" is an edge `A → B`. A **topological order** lists every node so that every edge points forward: you never take a course before its prerequisites. It exists **only if there is no cycle** (A needs B and B needs A means neither can come first).
* **Kahn's algorithm (BFS on in-degrees):** count the in-degree of each node. Push every node with in-degree 0 (no prerequisites) into a queue. Pop one, append it to the order, and decrement the in-degree of each of its neighbours; any neighbour that hits 0 goes into the queue. **If the final order has fewer than V nodes, there was a cycle.** The nodes on the cycle never reach in-degree 0.
* **DFS with three colours:** white = unvisited, grey = on the current DFS path, black = finished. Reaching a **grey** node means you've found a cycle (you've looped back onto your own path). Adding nodes to a list as they turn black, then reversing it, gives a topological order.
* *Use for:* Course Schedule (is there a cycle?), Course Schedule II (give the order).

### Union-Find (Disjoint Set Union)
A different tool for one question: **"are these two nodes in the same group?"**, while edges are being added. Each node points to a **parent**. Following parents leads to the group's **root**, which is the group's ID.
* **`find(x)`:** follow parents up to the root. **Path compression:** on the way back, point every node directly at the root, so later finds are O(1).
* **`unite(a, b)`:** find both roots. If they're the same, `a` and `b` were **already connected**, so this edge closes a **cycle**. Otherwise, attach one root under the other (**union by rank/size:** attach the smaller tree under the bigger one, so the trees stay shallow).
* With both tricks, each operation is effectively **O(1)** (technically O(α(n)), where the inverse Ackermann function α is ≤ 4 for any input that fits in the universe).
* *Use for:* Redundant Connection (the first edge whose ends are already connected), Number of Connected Components, Graph Valid Tree (n − 1 edges and no cycle). It's an alternative to DFS/BFS for those.

### Complexity: O(V + E), and why
A full traversal visits each node once (O(V)) and looks at each edge once, or twice in an undirected graph, once from each end (O(E)). So BFS and DFS are **O(V + E)** time. Space is O(V) for `visited` plus the queue or recursion stack. **For a grid of R × C:** V = R·C and E ≈ 4·R·C, so it's **O(R · C)**.

### The Hardware Reality
* **Recursion depth is the real danger here, unlike in Backtracking.** A DFS on a 300 × 300 grid that's all land can recurse **90,000 frames deep** along a snake-shaped path. At ~50–100 bytes a frame, that's several MB, which can blow a default 8 MB Linux stack, and is far beyond a Cortex-M task stack of a few KB. **BFS with a queue, or DFS with an explicit `std::stack`, keeps the work on the heap** with no depth limit. Know both, and say this in an interview.
* **Adjacency list layout:** `vector<vector<int>>` means one heap allocation per node, and neighbour lists scattered across memory. The cache-friendly version is **CSR (compressed sparse row):** one flat `edges` array, plus an `offsets` array where node `u`'s neighbours are `edges[offsets[u] … offsets[u+1])`. It's how graph libraries and route planners store big graphs. Not needed for LeetCode sizes.
* **A grid is already a perfect layout:** contiguous memory, neighbours computed by arithmetic, no pointers. Marking visited **in place** (overwrite `'1'` with `'0'`) costs no extra memory at all, when you're allowed to modify the input.
* **`visited` as `vector<char>` or a bitset, not `vector<bool>`** for speed (see Backtracking `[10]`: `vector<bool>` packs bits, so every write is a read-modify-write).

### Where Graphs Show Up in the Real World
Networks and routing (OSPF and IS-IS run shortest-path algorithms over the router graph), build systems and package managers (`make`, CMake and `apt` topologically sort dependencies; a cycle is a build error), the OS (deadlock detection = a cycle in the wait-for graph), compilers (control-flow graphs, register allocation by graph colouring), garbage collectors (mark-and-sweep is a graph traversal from the roots), and the linker (resolving symbol dependencies between object files).

---

## 2. General Summary / Quick Reference

When tackling Graph problems, keep these core patterns in mind:

### Pattern 1: Grid Flood Fill (count / measure connected regions)
Loop over every cell. When you find an unvisited cell of the right kind, start a DFS or BFS from it that marks its whole region visited, and count one region (or measure its size).
* *Use for:* Number of Islands (count regions), Max Area of Island (largest region size).
* *Discipline:* bounds check **before** reading the cell, mark visited on discovery, and decide up front whether you may overwrite the input.

### Pattern 2: Multi-Source BFS (distance to the nearest source)
Push **all** sources into the queue first, then BFS level by level. Each level is one unit of distance.
* *Use for:* Rotting Oranges (minutes until everything rots; check for any fresh orange left at the end), Walls and Gates / Islands and Treasure (distance from every room to its nearest gate).
* *Discipline:* when you need to count the rounds, save `levelEnd = q.size()` at the start of each round and stop the moment the goal is met (Rotting Oranges: return when the fresh count hits 0, or a final empty round adds a fake minute). The line after the loop then always means "unreachable".

### Pattern 3: Search Backwards from the Border (reverse the question)
Instead of asking "can this cell reach the edge?" from every cell (expensive), start from the **edge** and mark everything that can reach it, in one traversal per border.
* *Use for:* Pacific Atlantic Water Flow (two searches inward from the two oceans; the answer is the cells reached by both), Surrounded Regions (`O`s connected to the border survive, everything else gets flipped).
* *Discipline:* seed **every** border cell, not just the corners. When several searches share one grid, give each its own bit, and never overwrite the input that the next search still needs to read.

### Pattern 4: Build the Graph, then Traverse (explicit graphs)
Turn the input (edge list, prerequisite pairs) into an adjacency list, then run DFS or BFS with `visited`.
* *Use for:* Clone Graph (traverse while mapping old node → new copy in a hash map, which doubles as `visited`), Number of Connected Components (count traversal starts), Graph Valid Tree (connected **and** no cycle).
* *Discipline:* undirected = add both directions. Check whether nodes are numbered `0…n−1` or `1…n`.

### Pattern 5: Topological Sort / Cycle Detection in a Directed Graph
Kahn's algorithm (in-degree queue) or DFS with three colours.
* *Use for:* Course Schedule (cycle → impossible), Course Schedule II (return the order).
* *Discipline:* in a **directed** graph, "visited" isn't enough to detect a cycle. You need "on the current path" (grey) vs "finished" (black). Kahn's avoids that by counting how many nodes get output.

### Pattern 6: Union-Find (connectivity while adding edges)
`find` with path compression, `unite` by rank or size. An edge whose two ends already share a root closes a cycle.
* *Use for:* Redundant Connection, Number of Connected Components, Graph Valid Tree.

### Pattern 7: Implicit Graph + BFS for Fewest Steps
The nodes aren't given. They're **states** (words, board positions), and an edge is "one legal move". BFS from the start state gives the fewest moves.
* *Use for:* Word Ladder (words are nodes, one-letter changes are edges).
* *Discipline:* generate neighbours on the fly and mark them visited when you enqueue them. The state space can be huge.

### The Non-Negotiable Discipline
* **Directed or undirected? Weighted? Can there be cycles? Is it connected?** Answer these four before writing code.
* **Choose the representation:** grid (nothing to build), adjacency list (build it), or edge list (Union-Find).
* **`visited` is permanent, and set on discovery.**
* **"Shortest / minimum steps" in an unweighted graph → BFS.** "Visit everything / count regions" → either works.
* **Deep recursion on big grids can overflow the stack.** Know the iterative version.
* **State the complexity as O(V + E)** (O(R · C) for grids).

---

## 3. Problem Strategies & Patterns

<!-- NeetCode 150 Graphs (13): Number of Islands · Max Area of Island · Clone Graph · Walls and Gates (Islands and Treasure) · Rotting Oranges · Pacific Atlantic Water Flow · Surrounded Regions · Course Schedule · Course Schedule II · Graph Valid Tree · Number of Connected Components in an Undirected Graph · Redundant Connection · Word Ladder -->

### [1] Number of Islands

> The first problem in Graphs. First pass `18e3418` (a recursive flood fill), then four review rounds to the final version: an iterative BFS on a reserved `vector` with a flat `seen`. I went in thinking it was a graph-algorithm problem on an adjacency matrix. It turned out to be Word Search's grid walk **without the un-choose step**. Every round after the first pass was about systems cost, not the algorithm: a stack overflow, a hidden allocator, and two index bugs.

* **The Core Pattern:** grid flood fill (§2 Pattern 1), in two parts:
    * **The scanner:** a nested loop over every cell. When it finds land that hasn't been claimed yet, it starts a flood from there, then does `count++`.
    * **The flood:** claims every land cell reachable from the start, and stops at water and at the grid edge. When it returns, the **whole island** is marked.
    * **The invariant that makes the count right:** a flood starts only on an **unseen** `'1'`, and it consumes the island it starts on. So **one flood = one island = count once**. Every later cell of that island is already `seen` when the scanner reaches it, so it's skipped.
    * **In graph terms:** each cell is a node, its 4 neighbours are its edges, and an island is a **connected component**. The scanner is "one traversal start per component".

* **The "Gotcha":**
    * **It isn't an adjacency matrix, and you aren't counting loops.** An adjacency matrix is n × n, with `m[u][v]` = "is there an edge between nodes u and v?". Here every *cell* is a node, and its edges are implicit (§1, "Grid (implicit graph)"). The question is "how many connected components?", not "how many cycles?".
    * **One flood from `(0,0)` doesn't visit the whole grid.** A traversal only reaches its own component, because water cuts it off from the others. The outer loop exists to give every island a start.
    * **`visited` here is permanent, unlike Word Search's.** Word Search un-marked a cell on the way back, because "visited" meant "on the current path". Here it means "already claimed by some island", and it's never cleared. That's the "backtracking without the backtracking part" feel: the same walk, no undo.
    * **Mark when you push, not when you pop.** With mark-on-pop, up to 4 neighbours can each push the same cell before it's processed, so the container holds duplicates. Mark the **start** cell too, on the line that pushes it. I forgot it once: the start got pushed a second time by its first neighbour. The count stays right (same island), but it's duplicate work, and it breaks the one rule.
    * **Flat index = `r * columns + c`. The stride is the row *length*, never the row count.** It's the framebuffer formula `y * width + x`. I wrote `i * rows` in the final round, and it is **invisible on square grids**. Tall grids (3 × 2: cell `(2,1)` → `2*3 + 1 = 7`, but only 6 slots) hit a heap-buffer-overflow under ASan. Wide grids are worse: two cells **share one slot**, so it's silently wrong with every access in bounds. `[["0","0","1"],["1","0","0"]]` gave 1 instead of 2. **Always test a tall and a wide grid.** It's the same family as Word Search's `j < ssize(board)` bug.

* **The Struggle & Insights:**
    * **First pass: recursion with a `bool` return that carried nothing.** I added a `bool` return to the helper and OR'ed the four directions, then noticed I only ever left through `return false`. In fact it **can never return `true`**: the base case returns `false`, and the other path returns an OR of calls that are all `false`. Worse, it's a trap. If any path ever returned `true`, `||` would **short-circuit** and skip the remaining directions, so the flood would stop halfway and split one island into several. Word Search used that short-circuit on purpose as its early exit, but a flood fill must visit every direction. The helper should return `void`. Getting from "only `false` gets me out" to "the count happens in the scanner, one per flood" was the click of the problem.
    * **The recursion overflowed the stack on a valid input.** On a 300 × 300 all-land grid (inside LeetCode's limits), the DFS went **90,000 frames deep, one per cell**. 8 MB / 90,000 ≈ 93 bytes per frame, right at the edge. It passed at `-O2`, **segfaulted at `-O0`**, and hit stack-overflow under ASan. A recursive version that differed only in two extra parameters segfaulted even at `-O2`. Surviving depended on the compiler's frame size, so it's a crash, not a pass. **Fix:** an explicit container, so the work lives on the heap with no depth limit. It also measured about **2× faster** (no call, return or frame setup per cell). On a Cortex-M with a few-KB task stack, the recursive version dies on a tiny grid.
    * **`idx > 0` instead of `>= 0` (in the iterative rewrite).** When I turned the reject guard (`i < 0 → return`) into an accept guard, I flipped it wrong. `!(idx < 0)` is `idx >= 0`. With `> 0`, a flood can never step **into** row 0 or column 0. In `[["0","1"],["1","1"]]`, the flood from `(0,1)` can't reach `(1,0)`, so the scanner counts it again and returns 2. That was wrong on 22,378 of 100,000 fuzz grids. **When inverting a guard, flip every comparison.**
    * **A `std::queue` constructed inside the scan loop is a hidden allocator.** `std::queue` sits on `std::deque`, and libstdc++'s `deque` **allocates on construction** (a block map plus a 512-byte block). It also frees and reallocates a block every 64 elements as the front moves through. One queue per island gave **90,302 allocations** on a checkerboard (45,000 one-cell islands) and 12,844 on a random grid. Hoisting it out of the loop alone halved the checkerboard time (1.34 → 0.70 ms).
    * **The fix: a reserved `vector` with a read cursor, not a `deque`.** Push with `emplace_back`, read `q[read++]`, and stop when **`read == q.size()`**: the reader has caught up with the writer. Nothing is ever removed, and `clear()` between islands keeps the capacity. It's a UART ring buffer with a read and a write pointer, and `read == write` means empty. It needs no wrap-around because **each cell is pushed at most once in the whole run**, so `reserve(rows * columns)` is a hard bound and nothing reallocates. That's the argument to give an interviewer. Elements are read in write order, so it's a real FIFO, i.e. **BFS**. With `back()`/`pop_back()` instead, the same vector is an iterative **DFS**. Both work for counting; name the variable for the one you wrote.
    * **Q&A: why a flat `vector<T>` beats `vector<vector<T>>`, even though it computes an index.** My worry was that `r * columns + c` adds a multiply on every access. It doesn't add one: the nested version already has one. What it removes is a pointer chase. Compiled at `-O2`, one read each:
        * **`seen[r][c]` on `vector<vector<char>>`: 7 instructions, two dependent loads.** The outer vector holds **`vector` objects** (24 bytes each: begin, end and capacity pointers), not the bytes. So it computes `r * 24` (two `lea`s), **loads that row's data pointer from memory**, and only then can it load the byte. The second load can't start until the first returns, about 4–5 cycles even from L1.
        * **`seen[r * columns + c]` on a flat `vector<char>`: 5 instructions, one load.** The `imul` (about 3 cycles) runs in parallel with loading the data pointer, which the compiler can keep in a register across a whole loop. There's nothing to chase.
        * **The other costs of nesting:**
            * **Allocations:** R + 1 instead of 1 (301 vs 1 here).
            * **Contiguity:** each row is a separate heap block, so the rows aren't laid out one after another. Walking across a row boundary means loading a new row pointer, and the hardware prefetcher loses the stream.
            * **Memory:** 24 bytes of bookkeeping per row, plus the allocator's header per block.
        * **Measured, 2000 × 2000 `uint8_t`, `-O2`:** a column-order walk took 2.97 ms nested vs **2.2 ms** flat, and 4 M random reads took 7.3 ms nested vs **5.7 ms** flat. That's **25–35% faster flat**, with the same values and the same order. (On Islands at 300 × 300 the time difference was only 3–10%, because the grid fits in L1/L2. The allocation count was the bigger win.)
        * **"Always" has exceptions; know them:**
            * **jagged rows** (different lengths per row), where a flat array needs a separate offsets table, like CSR in §1
            * **rows that grow or shrink independently**
            * **swapping or reordering whole rows**, which is O(1) for nested (a pointer swap) and a copy of the row for flat
        * For a fixed R × C grid, which is every grid problem in this topic, flat wins. **The rule:** one allocation, one stride (`columns`), and `r * columns + c` everywhere. Test it on a non-square grid (see the stride bug above).
        * Optional next step: store the flat index itself in the queue. Then the neighbours are `idx ± 1` and `idx ± columns`, which are just adds, but the left/right edge checks need care.
    * **Q&A: why `vector<uint8_t>` (or `char`) beats `vector<bool>` for `seen`.** `vector<bool>` is a **special case in the standard**: it packs 8 flags per byte. It isn't a container of `bool`s.
        * **Every access is bit arithmetic.** `seen[i]` returns a **proxy object**, not a `bool&`. Compiled, `seen[r][c]` on `vector<vector<bool>>` was **about 25 instructions**: the row-pointer chase, then `idx / 64` to find the 64-bit word, `idx % 64` to find the bit, sign fix-ups for a signed index, a shift, an AND, and a branch. A write is a **read-modify-write** of the whole word, exactly like setting one bit in a GPIO register without bit-banding. `vector<uint8_t>` is one byte load or store.
        * **Measured:** a sieve of Eratosthenes up to 10⁶ took 4.1 ms with `vector<bool>` vs **1.2 ms** with `vector<uint8_t>` (3.4×). Up to 10⁸ it took 466 ms vs **392 ms**. N-Queens (Backtracking `[10]`) saw about 30% from the same swap. On Islands and Word Search the difference was noise, because the flag access wasn't the bottleneck.
        * **It isn't a real container.** You can't take a `bool*` or a `bool&` to an element. `auto x = v[i]` gives you a proxy, not a copy, so changing `v` later changes what you see through `x`. And `vector<bool>` has no `.data()` you could hand to a C API or `memset`.
        * **It isn't thread-safe per element.** For every other `vector`, two threads writing **different** elements is fine. For `vector<bool>`, the standard explicitly excludes that guarantee: elements 3 and 5 live in the same word, so concurrent writes are a **data race**. That matters for the concurrency phase.
        * **When `vector<bool>` (or `std::bitset`, or a hand-rolled `uint64_t` mask) is the right call:** when **memory is the constraint**. It's 8× smaller (10⁸ flags = 12 MB vs 98 MB). That's the deciding factor on a microcontroller with a few hundred KB of RAM, or when the bit version fits in a cache level and the byte version doesn't. On this machine (320 MB L3) both sieves fit, so bytes won at both sizes, and I don't have a measured win for bits here. The true statement is "bytes for speed and simplicity, bits for footprint", not "bits are always worse".
    * **Mutate or not:** I kept the grid `const&` (a realistic interviewer constraint), so `seen` is needed. If mutation is allowed (LeetCode's real signature is non-const), overwriting `'1'` with `'0'` removes `seen` entirely. Say which one you chose, and why.
    * **Measured at 300 × 300, `-O2`:**

        | Version | all land | random 50% | checkerboard | Allocations |
        | -- | -- | -- | -- | -- |
        | First pass: recursive, `vector<vector<bool>>` | 1.31 ms (crashes at `-O0`/ASan) | 1.49 ms | 0.62 ms | 301 |
        | Iterative, `std::queue` per island | 1.18 ms | 1.57 ms | 1.34 ms | up to 90,302 |
        | **Final:** reserved `vector` + read cursor, flat `seen` | **0.74 ms** | **1.28 ms** | **0.30 ms** | **2** |

* **Time & Space Complexity:**
    * **Time O(R · C).** Each cell is scanned once, and each land cell is pushed once and checks 4 neighbours. That's optimal: every cell has to be read.
    * **Space O(R · C)** for `seen` (1 byte per cell), plus the container reserved to R · C, all on the heap. At runtime the BFS frontier is only about one diagonal "ring" wide, O(min(R, C)). An iterative DFS's stack can approach R · C. Recursion puts that same R · C on the **call stack**, which is the part that crashes.

### [2] Max Area of Island

> First pass `acc73ca`, **optimal as committed**. I recognised it as Number of Islands `[1]` with one extra number per flood, and reused the final `[1]` version directly: a reserved `vector` with a read cursor, a flat `seen`, a direction array, and marking on push. The review found nothing to fix.

* **The Core Pattern:** grid flood fill (§2 Pattern 1), the "measure each region" variant. The scanner and the flood are the same as in `[1]`. Instead of `count++` per flood, each flood measures its island's size, and the scanner keeps the running `max`.
    * **What changes from `[1]`:** one counter per flood, reset at the start of each flood and incremented once per cell **processed** (popped or read), then `maxArea = max(maxArea, area)` when the flood ends.
    * **Why count on read, not on push:** each cell is pushed exactly once and read exactly once, so either spot counts every cell exactly once. Counting on read keeps "one cell processed = +1" together in one place.

* **The "Gotcha":**
    * **The counter is already there: it's the read cursor.** When the flood ends, `read == bfsVector.size()` == the number of cells pushed in this flood == the island's area. That's because the container is cleared per island and every cell is pushed exactly once. So `currentArea` duplicates `read`. It's harmless (measured: no difference) but redundant, and seeing it shows you understand the container: **the BFS queue's length is the size of the component it just explored.**
    * **The grid is `int`, not `char`.** It's `vector<vector<int>>` with `0`/`1` values, not `'0'`/`'1'`. Comparing to `'1'` (49) out of habit from `[1]` would find no land at all. My scanner tests `!= 1` while the neighbour check uses truthiness (`grid[r][c]`). That's equivalent for 0/1 input, but pick one form and use it in both places.
    * **The answer for no land is 0**, which falls out of `maxArea` starting at 0.

* **The Struggle & Insights:**
    * **The payoff of the `[1]` grind.** Four review rounds on `[1]` (the recursion overflow, the queue per island, the `> 0` bound, the `i * rows` stride) meant this one had none of those bugs on the first pass, including the non-square test: tall and wide grids were both correct.
    * **Early exit when `maxArea == rows * columns`?** Measured: it saves about 9% on an all-land grid, and costs about 8–10% on a checkerboard (one extra compare per island, with 45,000 islands). It's a wash, so leave it out. The more general prune ("stop scanning when the cells left can't beat `maxArea`") has the same problem: it adds bookkeeping to every scanner step to save work only on rare inputs.
    * **Mutate or not:** same choice as `[1]`. LeetCode's signature is non-const, so you could overwrite `1` with `0` and drop `seen`. I kept the grid `const&`.

* **Time & Space Complexity:**
    * **Time O(R · C).** Each cell is scanned once, and each land cell is pushed and read once with 4 neighbour checks.
    * **Space O(R · C)** for `seen` plus the reserved container, all on the heap. At LeetCode's limit (50 × 50) that's 2,500 bytes plus 20 KB.

### [3] Clone Graph

> First pass `302c8a4`, after a lot of mental gymnastics. Then three rounds: drop `seen` and look the root up in the map, then a local map with one `try_emplace` (`3d2d03a` → `1e95c02`), then the array indexed by `val` (final commit). My two confusions going in: **where** to create a copy (after popping, or while scanning neighbours?), and **how to keep track of the root**. I hit one bug on the way, a node processed twice, and found the fix myself after an LLM hint.

* **The Core Pattern:** traverse the original (BFS here) while keeping an **original → copy map** (§2 Pattern 4). The map does three jobs at once:
    * **It's `visited`.** A node is in the map ⇔ its copy exists ⇔ it has been discovered. One fact, stored once.
    * **It's the creation point.** A copy is created the **first time a node is discovered, whoever discovers it**: the root by you, every other node as somebody's neighbour. "After popping or during the neighbour loop?" has one answer: **at discovery**, and the map is what tells you it's a discovery.
    * **It's the wiring table.** When node A's copy needs its neighbour B, `map[B]` gives B's copy whether B is new or was discovered long ago. Every edge is wired once, from the node being processed, so each copy's `neighbors` comes out in the original order.
    * **The root:** its copy is the first one created, and it never moves, so hold the pointer (or look it up at the end). The linked-list **dummy node** solves a different problem, a head that isn't known until the first append or that can change. Neither happens here, so no dummy is needed.

* **The "Gotcha":**
    * **Mark when you push, not when you pop.** This was my bug: `seen` was inserted on pop. A node sitting in the queue isn't processed yet, but it **is discovered**. If 10 nodes are queued and none is marked until it's popped, every other node pointing at one of them pushes it again. In Clone Graph a duplicate pop isn't just wasted work: that copy's `neighbors` gets **appended twice**, so the clone is wrong. It's the same rule as `[1]`, on a different data structure. Whenever several producers can feed the same item into a container before the consumer reaches it, mark at the producer.
    * **Working state must not outlive the call.** My first pass kept the map as a **member** of `Solution`. A second `cloneGraph` call on the same object returned the **first call's copies** and appended their neighbours again: the root went from 2 to 4 neighbours, and the clone was invalid. On a *different* graph it's worse: freed nodes' addresses get reused by `new`, so a fresh node can map to a stale copy. A function whose output depends only on its input keeps its state local.
    * **`operator[]` on a read inserts on a miss.** `return seen_[node]` was safe only because the key had to exist. Read-only lookups use `find` or `at`.
    * **`std::array<Node*, 101> seen{}` needs the braces.** `{}` value-initialises every slot to `nullptr`, which *is* the "not discovered" state. Without the braces, a local array of pointers is uninitialised garbage, and `!slot` reads it.

* **The Struggle & Insights:**
    * **`seen` and the map were the same fact.** `seen.contains(x)` and `map.contains(x)` were always equal, because a copy is created exactly when a node is discovered. Deleting `seen` removed a hash set and one lookup per edge. It's the same lesson as `[2]`, where `currentArea` duplicated the read cursor: **two variables that always agree are one variable.**
    * **Hash once, and use the answer you already computed.** In my second version the helper called `try_emplace` (which knows whether the key was new) but returned only the pointer, so the caller asked `contains` first. That's 2 hashes per neighbour, and the first version had 3 (`contains`, `find`, `operator[]`). The fix: the helper returns `{copy, isNew}`, and the push depends on that `isNew`. **21.5 → 11.9 µs** at 100 nodes, 1.8× from one lookup. At 100k nodes / 800k edges it barely moved (64 → 62 ms). There, each lookup is a cache miss into a random bucket plus a pointer chase to a heap-allocated map node, so *fewer* lookups help less than *cheaper* ones. `map.reserve(n)` didn't help either: it removes rehashing, not the per-entry allocation, and it measured 30% slower at 100k.
    * **The array indexed by `val` is a hash map with the identity hash.** My question: *"what if root 1 neighbours 201 and 2? How do I insert 201 by index?"* The answer is that it isn't a list you append to in discovery order. It's a **direct-address table**: slot `k` holds the copy of the node with `val == k`, written in any order, with `nullptr` meaning "not discovered". What it needs up front is the **largest `val`**, not n, and the slots must **exist**, so it's a sized table, not `reserve` (the same trap as Minimum Interval's scattered writes). LeetCode guarantees unique values in 1..100, so `std::array<Node*, 101>{}` covers every legal input, and 201 can't occur. **It's valid only under two conditions, and both must be said:**
        * **Values are small and dense.** Otherwise memory scales with the largest value: `{1, 2, 10⁹}` would be 8 GB of pointers for 3 nodes.
        * **Values are unique.** Two nodes with `val == 5` would share `copy[5]`, so the second would look already discovered and both would map to one copy. Pointer keys can't collide, which is why the general solution keys on `Node*`.
    * **The interview line:** "The general solution maps `Node*` to its copy. Given unique values in 1..100, I'd swap the map for a 101-slot array, which is 2× faster measured, and say why it doesn't generalise."
    * **Last details in the final version:**
        * `Node*& slot = seen[node->val]` names the slot once. It serves as the test, the write and the return. `new` is an opaque call, so without the reference the compiler has to reload `node->val` and the slot after it.
        * `nodeVector.reserve(100)`: with the bound known, it removes the 7 doublings. The 201 allocations left are exactly the output (100 nodes + 100 `neighbors` buffers) plus 1 queue.
        * `assert(node->val <= 100)` records the assumption (`std::array::operator[]` doesn't check bounds).
    * **Measured (100 nodes, 3,400 directed edges, `-O2`):**

        | Version | Time | Allocations |
        | -- | -- | -- |
        | Member map, no `seen` (`std::queue`, 2–3 hashes per edge) | 23.2 µs | 307 |
        | Local map, `contains` + `try_emplace` (`3d2d03a`) | 21.5 µs | 312 |
        | Local map, one `try_emplace`, its `isNew` drives the push (`1e95c02`) | 11.9 µs | 312 |
        | **Final:** `std::array<Node*, 101>` by `val`, reserved queue | **5.1 µs** | **201** |

* **Time & Space Complexity:**
    * **Time O(V + E).** Each node is discovered, copied and pushed once, and each directed edge is wired once. That's expected O(1) per edge with the map, guaranteed O(1) with the array.
    * **Space O(V)** for the map or array plus the queue, not counting the clone itself, which is the O(V + E) output.

### [4] Walls and Gates (Islands and Treasure)

> First pass `6c4a1e2` (a separate BFS per gate), then two rounds: accumulate all gates before the loop (`9b5bf08`), then drop the `unordered_set` and let the grid be `visited` (`e6a72d0`). After Rotting Oranges `[5]`, I revisited it to go **layer by layer** instead of storing a level in every queue entry (final commit). My initial instinct was to start from every land cell and search outward for the nearest gate. A nudge to flip the direction — start from the gates — was the click. The two rounds after the first pass were about removing redundant state: first `std::min` (unnecessary once the BFS is truly multi-source), then the entire `seen` set. The revisit removed a third copy: the level stored in each entry.

* **The Core Pattern:** multi-source BFS (§2 Pattern 2). Seed **every** gate (value `0`) into the queue before the loop starts. Then run one BFS. Each level is one more step of distance, and the first time a cell is reached is along a shortest path, so a direct assignment is correct — no `min` needed.
    * **Why multi-source works:** imagine dropping a stone into water at every gate simultaneously. The ripples expand one step at a time. The first ripple to reach any land cell is, by definition, the shortest distance from that cell to its nearest gate. A second ripple arriving later finds the cell already written and skips it.
    * **Every reachable cell is covered.** A cell is missed only if it's completely walled off from every gate — which is correct: those cells should stay `INT_MAX`.

* **The "Gotcha":**
    * **Per-gate BFS is not multi-source BFS.** My first pass looped over every gate and ran a full BFS from each, clearing the queue and `seen` set between gates. That visits cells multiple times (once per gate that can reach them), which forced a `std::min` on every write. True multi-source BFS seeds all sources into the queue *before* the loop, runs *once*, and each cell is dequeued exactly once. The `min` and the per-gate `clear()` both disappear.
    * **The grid is `visited`.** An unvisited land cell has value `INT_MAX`. The moment it's written (`grid[r][c] = level`), it's no longer `INT_MAX`, so the `== maxVal` check naturally rejects it on future visits. The grid itself is the visited set — no `unordered_set` needed. This is the same insight as Number of Islands' in-place `'1'` → `'0'` overwrite, but here you *must* write (the distance is the output), so it's free.
    * **Walls (`-1`) and gates (`0`) are excluded by the same check:** neither equals `INT_MAX`, so BFS never enqueues them and never overwrites them.

* **The Struggle & Insights:**
    * **The original instinct was backwards.** I wanted to start from every INF cell, search outward for the nearest gate, then backtrack updating distances. That's $O((R \cdot C)^2)$ in the worst case: potentially 62,000 cells each launching a full traversal. Flipping the direction — start from gates, expand outward — makes every cell $O(1)$ amortised work.
    * **"`std::min` is a code smell in BFS."** If you need `min` when writing a distance, it means the same cell can be reached by multiple BFS passes, which means you're running per-source BFS, not multi-source. In a true multi-source BFS, first visit = shortest distance, so it's always a plain assignment.
    * **Three states, one check.** The grid has three kinds of cells: walls (`-1`), gates (`0`), and land (`INT_MAX` initially, then a distance). The `== INT_MAX` guard handles all three: walls and gates fail it (never enqueued), already-visited land fails it (already written), and unvisited land passes it. One comparison does the work of `!= -1 && != 0 && !seen.contains(…)`.
    * **Where the distance comes from: three ways, all correct.**
        * **A level in every entry** (`e6a72d0`): `{i, j, level}`, 12 bytes. A neighbour gets `level` and is pushed with `level + 1`.
        * **Layer by layer** (final): entries are just `{i, j}`, 8 bytes. Save `levelEnd = size()` at the start of each layer and write `currentLevel` (starting at 1, because the gates are layer 0), then add one after the layer. It's the same loop shape as `[5]`. Unlike `[5]` there's no early exit, because every reachable room has to be written.
        * **Read it from the grid** (scratch only): when a cell is popped, its own distance is **already in the grid**. It was written when the cell was discovered, and gates hold `0`. So the neighbour's distance is `grid[r][c] + 1`, and neither the entry nor a counter needs to carry a level. This works only because the output *is* the distance map. In `[5]` the grid holds only `0/1/2`, so there's nothing to read the time back from.
    * **Measured** (3000 × 3000, 30 gates, 10% walls, `-O2`, best of 7, three runs; all three versions 0 wrong out of 20,000 fuzzed grids against a per-room brute force):

        | Version | Time |
        | -- | -- |
        | Level in every entry (`e6a72d0`) | ~135–140 ms |
        | **Final:** layer by layer | **~119–126 ms** |
        | Read the level from `grid[r][c]` | ~123–133 ms |

        Layering is about 10% faster for the same reason as in `[5]`: 8-byte entries instead of 12, so less memory traffic through the queue. Here it's the *only* change, because the grid was already the visited set and there's no wasted final layer. Reading from the grid ties with layering: one extra load per pop (of a cell written moments ago, so almost certainly still in cache) in exchange for no inner loop. **Default to layer by layer**, because it's the pattern that also works when the output isn't a distance (`[5]`, Word Ladder). In an interview, say the third option out loud: "the grid already holds each cell's distance, so the queue only needs coordinates."
    * **Last detail:** `static constexpr` on the direction table (see `[5]`). My first edit wrote `td::array`, so check that it compiles before you commit.

* **Time & Space Complexity:**
    * **Time O(R · C).** Every cell is enqueued at most once and checks 4 neighbours. The gate-seeding pass is also O(R · C).
    * **Space O(R · C)** for the BFS vector (reserved to `rows * columns`, 8 bytes per entry in the final version). No auxiliary visited structure — the grid is modified in place.

### [5] Rotting Oranges (Rotting Fruit)

> First pass `ed7a501` (minutes written into the grid with an offset), then `f890bb7` (minutes in each queue entry, plus a `freshFruit` counter), `889aaa4` (layer by layer), and the final `c48ad50` (a flat `uint8_t` buffer, and an early return when the last fruit rots). The approach carried straight over from Walls and Gates `[4]`: seed every rotten fruit, then run one BFS. The wall was **representation**. The grid already uses 0, 1 and 2, so where do the minutes go?

* **The Core Pattern:** multi-source BFS that **counts rounds** (§2 Pattern 2). Seed every `2` into the queue, and process one layer per minute. The answer is the number of layers that rotted something, or `-1` if any fresh fruit is left over.
    * **What changes from `[4]`:** in `[4]` the output is a distance **per cell**, so writing it into the grid is the job. Here the output is **one number** (the minute the last fruit rots) plus a yes/no (did every fruit rot?). The grid only has to hold **state**, never time.
    * **Mental model:** the same ripples as `[4]`, all starting together. The answer is the moment the last ripple lands, and `-1` means some fresh fruit sits behind a wall the ripples can't cross.

* **The "Gotcha":**
    * **One container, one job.** My first pass wrote `minutes + 1` into the grid. Without an offset, a fruit that rotted at minute 1 reads as `1` = fresh, so I started the minutes at 10. That worked, but the last line had to undo the encoding (`maxMinutes <= 2 ? 0 : maxMinutes - minuteOffset`), so two distant parts of the function were tied together by a magic number. And the queue entries (`IndexMinute`) **already carried the minutes**: the time was stored twice. The split: **the grid holds state** (write `2` on discovery; that's the visited mark, and `== 1` is the only check), and **the queue holds time**.
    * **Count the change where the change happens.** I first decremented `freshFruit` in the outer loop (on pop). Pops include the original rotten fruit, which were never fresh. The only `1 → 2` transition is in the neighbour loop, next to the write and the push, so the decrement goes there. Keep a counter on the same line as the state change it counts, and it can't drift.
    * **The last layer rots nothing.** A plain `minutes++` per layer overcounts by one: after the last fruit rots, the loop still processes those newly rotten fruit as one more layer, finds no fresh neighbours, and adds a minute. My fix, `if (read < bfsVector.size()) lastMinute++` (only count a layer that grew the queue), was correct but still **ran** the wasted layer. The final version returns as soon as `freshFruit` hits 0 after a layer. That also makes the increment safe to do unconditionally: the only layer that rots nothing while fresh fruit remains leaves the queue the same size, so the loop exits and returns `-1`, and the extra minute is never returned. So the "did this layer grow?" check is still there, but it's the outer `while` condition now.
    * **The line after the loop always means "stuck".** Every success returns earlier (`return 0` for no fresh fruit, `return lastMinute` when the count hits 0). So the last line is just `return -1`. Writing `return lastMinute` there returned the layer count instead: LC example 2 gave **5**, `[[1]]` gave **0**, and `[[2,0,1]]` gave **1**, all supposed to be `-1`. A `freshFruit > 0 ? -1 : lastMinute` ternary there is dead code, because its second branch can't be reached.
    * **No fresh fruit means 0, before any BFS.** Count fresh fruit during the first scan and return `0` straight away (`[[0]]`, `[[2]]`, `[[0,2]]`). The empty grid is the same case: zero fresh fruit, so `0`. My `if (grid.empty()) return -1` contradicted the rule a few lines below it.
    * **`int` for the counter, not `size_t`.** It's a count that goes down and gets tested with `> 0`. An off-by-one (like the decrement on pop) goes negative, which you can see. Unsigned, it wraps to about 1.8 × 10¹⁹, `> 0` is true, and the answer is a silent `-1`.

* **The Struggle & Insights:**
    * **`levelEnd`, not `levelSize`.** My first layer loop counted `levelSize` down while `read` counted up. They change together, so `read + levelSize` stays fixed for the whole layer, and that fixed value is `bfsVector.size()` at the start of the layer, i.e. where the layer ends. Two counters were tracking one boundary. Save `levelEnd = bfsVector.size()` and loop `while (read < levelEnd)`. Fruit that rots during the layer is pushed past `levelEnd`, so it's left for the next layer automatically, and both values are `size_t`, so the `static_cast` goes too.
    * **Flat `uint8_t` buffer instead of `vector<vector<int>>`.** That's 1 allocation instead of R + 1, a quarter of the bytes for 3 states, and one load per access instead of two dependent ones. The fill index is `i * columns + j`. Using `rows` is the stride bug from `[1]` (it goes unnoticed on square and 1-row grids). Copy and classify in the **same** loop, not two passes.
    * **`static constexpr` on the direction table.** `constexpr` builds it at compile time, and `static` makes it one object in `.rodata` (on an MCU, a `const` table in flash instead of an array copied onto the stack each call). Because the initialization happens at compile time, there's also no thread-safe init guard (`__cxa_guard_acquire`). Checked the assembly at `-O2`: the local version stores all 4 entries to the stack on every call before reading one, and the `static constexpr` version is a single indexed load from `.rodata`. In this loop (a range-`for` over all 4), GCC will most likely unroll it and turn the offsets into immediates either way. So the real point is that the declaration says what the table is.
    * **Is layer by layer faster than minutes per entry?** I measured it (3000 × 3000 all fresh, one rotten corner, answer 5998, `-O2`, best of 7):

        | Version | Time |
        | -- | -- |
        | `f890bb7`: `vector<vector<int>>`, 12-byte `{r, c, minutes}` entries, runs the final layer | ~112 ms |
        | Minutes per entry, but with the flat `uint8_t` buffer and early exit (scratch only) | ~78 ms |
        | **Final** `c48ad50`: layer by layer, flat buffer, early exit | **~73 ms** |

        Most of the 35% comes from **the layout and the early exit**. Layering by itself adds about 6%, from 8-byte entries instead of 12 (a 72 MB queue instead of 108 MB at this size). Algorithmically the two are the same. Layer by layer is the canonical answer: one counter, because every entry in a layer has the same distance, and it's the same shape as level-order traversal in Trees and Word Ladder. **A distance per entry is still the tool when distances don't advance in lockstep**: sources that start at different distances, or weighted edges (Dijkstra stores a distance with every entry). In `[4]` the per-cell distance lived in the grid.
    * **Left on the table (optional):** pad the buffer with a border of walls, `(R+2) × (C+2)`. Then every real cell's neighbours are in bounds, the four bounds comparisons per neighbour go away, a step is `idx ± 1` or `idx ± width`, and the queue can hold one `int` instead of a `pair`.

* **Time & Space Complexity:**
    * **Time O(R · C).** One scan to copy and classify. Every fruit is pushed once and checks 4 neighbours, and the early exit skips the final empty layer.
    * **Space O(R · C):** the `R · C`-byte state buffer plus the queue, reserved to `R · C` entries of 8 bytes each. At LeetCode's limit (10 × 10) that's 100 B + 800 B.

### [6] Pacific Atlantic Water Flow

> First pass `b5fa840`, corner de-duplication `44a6a14`, then two rounds of refactoring into the final version: a single `markOceanCoverage(mask)` lambda, answers collected during the BFS, and named bit masks. The idea of "seed from the target and go backwards" carried over from `[4]` and `[5]`. My first two ideas about **what** the targets were and **where** to record a result were both wrong, and the scratch tests showed exactly how.

* **The Core Pattern:** search backwards from the border (§2 Pattern 3), **once per ocean**. Water flows from a cell to a neighbour of **equal or lower** height. Traced backwards from the ocean, that means **climbing**: from a cell, move to neighbours with height `>=` it.
    * **Split "both" into two separate questions.** "Which cells reach the Pacific?" is a multi-source BFS seeded from **every** cell of the top row and left column. "Which reach the Atlantic?" is seeded from **every** cell of the bottom row and right column. The answer is the cells in **both** sets. Two O(R·C) passes, compared with O((R·C)²) for searching from every cell.
    * **One byte per cell, one bit per ocean.** `PACIFIC = 0b01`, `ATLANTIC = 0b10`, `BOTH = PACIFIC | ATLANTIC`. Each pass tests and sets **only its own bit**, so a cell the Pacific already reached is still new to the Atlantic. That independence is why two bits work and one shared `seen` wouldn't.
    * **Mental model:** pour dye into the Pacific and let it climb uphill. Then pour a second colour into the Atlantic. The answer is the cells that end up with both colours.

* **The "Gotcha":**
    * **The two corners are not the targets.** My first idea: `[0][C-1]` and `[R-1][0]` touch both oceans, so seed just those and keep any cell that connects to them. Wrong. Water can go into the Pacific at **any** top or left border cell, and into the Atlantic at a **different** border cell. Counterexample: `[[9,1,9],[9,5,9],[9,1,9]]`. The centre `5` drains up to the top `1` (Pacific) and down to the bottom `1` (Atlantic), but climbing from either corner stops at a 9. Adding all four corners doesn't help either. **A corner is one way into an ocean, not the only one.**
    * **Don't overwrite the heights.** My second idea: run the Pacific pass, mark the cells it can't reach as `-1` or `INT_MIN`, then run the Atlantic pass. It was wrong on **9,634 out of 200,000** random 3×3 and 3×4 grids. The heights are input to **both** passes. A `-1` misleads the second pass in both directions: **from** a `-1` cell every neighbour looks uphill, so cells that aren't answers get in, and **into** a `-1` cell nothing can climb, so answers get cut off. In the first counterexample the scratch test found, two Atlantic **seeds** on the bottom row were `-1`, so the Atlantic pass climbed from them into a cell that can't reach the Atlantic. "The Pacific can't reach this cell" says nothing about the Atlantic. Compare `[4]`, where writing into the grid *is* the output and nothing reads the old value afterwards.
    * **Trimming the duplicate corner: each ocean's corner is at a different end.** Each ocean's two seed loops share exactly one corner. For the Pacific it's `[0][0]`, **index 0** of both loops. For the Atlantic it's `[R-1][C-1]`, the **last** index of both. When I started both row loops at `j = 1`, the Pacific was right, but the Atlantic lost its **bottom-left** seed `[R-1][0]`, which nothing else adds. That's 32,888 wrong out of 100,000 (`[[1,2,3]]` returned 2 cells instead of 3), while the real duplicate was still added twice. The fix is to trim the **last** index of one Atlantic loop. The test for any trim: **which cell am I skipping, and which other loop still adds it?**
    * **`&` binds looser than `==`.** `seen[k] & 0b11 == 0b11` parses as `seen[k] & (0b11 == 0b11)` = `seen[k] & 1`, which tests only the Pacific bit and returns every Pacific cell. `-Wall` warns about it (`-Wparentheses`: "suggest parentheses around comparison in operand of `&`").
    * **Flat-index translation.** This is where I stalled after switching to a flat `seen`. There's one rule, `r * columns + c`, and every border variant is that rule with one coordinate fixed: left column `i * columns`, top row `j`, right column `i * columns + (columns - 1)`, bottom row `(rows - 1) * columns + j`. If unsure, write the full formula and substitute the constant, rather than simplifying in your head.
    * **`return {{}}` is not empty.** For the `[[]]` guard I wrote `return {{}};`. That's a vector holding **one** empty inner vector, i.e. one bogus "cell". An empty result is `return {};`.

* **The Struggle & Insights:**
    * **Collect on pop, not with a final scan.** First pass: after both passes, scan all R·C bytes for `== BOTH`. Final version: when a cell is **popped**, check whether both bits are set. In the Pacific pass this never fires (no Atlantic bits exist yet). In the Atlantic pass, every cell it reaches is popped exactly once, **seeds included**, so each answer is collected once. My first idea was to collect when the second bit is *set*. Collecting on pop is simpler, because the seeds need no special case. It's the "count on read" idea from `[2]`.
    * **Refactoring: separate seeding from draining.** I couldn't see how to generalize two BFS runs whose seeds were different. The answer is to put the function boundary **between** seeding and draining. The drain doesn't care where its seeds came from, and the only thing that varies is the mask. So the caller seeds, then calls `markOceanCoverage(PACIFIC)`; seeds again, then calls `markOceanCoverage(ATLANTIC)`. The helper also `clear()`s the queue, so the caller doesn't need to know that step exists.
    * **Masks, not bit positions.** My first named version had `PACIFIC = 0`, `ATLANTIC = 1` (positions) next to `BOTH = 0b11` (a mask), equal to `(1 << PACIFIC) | (1 << ATLANTIC)` only by coincidence. Masks everywhere means every `1 << x` disappears, `BOTH` is **built** from the other two, and every test has the same shape: `(seen[k] & mask) == 0`.
    * **Lambdas (my first one).** `[captures](parameters) { body }`. The compiler turns it into an **unnamed struct**: the captures are its data members, and the body is its `operator()`. That's why it's declared with `auto` (the type has no name you could write).
        * **What it can see:** only the **local variables it captures**, and only from the function it's written in (here `pacificAtlantic`, not its caller). `static`/`constexpr` locals and globals don't live on the stack, so it can use them **without capturing** them. That's why `directionArray` needs no capture.
        * **`[&]`** = capture every local you use **by reference** (the struct stores pointers). **`[=]`** = by value (it stores copies). **`[&, rows, columns]`** = by reference by default, except these two, which are copied. Checked: a `[&]` lambda using 3 locals is 24 bytes (3 pointers), `[=]` over two `int`s is 8 bytes, and a capture-less `[]` is 1 byte. After the outer `rows = 100`, the `[&]` lambda saw the new value and the by-value one kept the old one.
        * **Compared with function pointers:** only a **capture-less** lambda converts to a function pointer (`int (*fp)(int) = [](int x){...};` compiles). Once it captures something, it carries state, and a bare code pointer can't hold that. A lambda called directly has a type the compiler knows exactly, so the call is normally inlined. A call through a function pointer is an indirect jump unless the optimizer can work out the target.
        * **Danger:** `[&]` stores addresses of locals. If the lambda outlives the function (returned, stored, or handed to a thread), those pointers dangle. Here it's called and discarded inside the same function, so `[&]` is safe.
    * **Capture hot small values by value. Measured.** With `[&]`, every use of `rows` and `columns` in the bounds check is a load through the closure. Because `seen` is `uint8_t`, a character type that may alias anything, the compiler must reload them after every `seen[...] |=` write and every `emplace_back`. Capturing the two `int`s by value made the lambda as fast as the free function (200×200, all equal: ~2.42 ms with `[&]` vs ~2.26 ms with `[&, rows, columns]` vs ~2.28 ms for the free function, consistent over three runs). **Containers by reference, small values read in a hot loop by value.**
    * **Measured overall** (`-O2`, best of 15):

        | Grid | `44a6a14` (final scan) | Final (collect on pop, lambda) |
        | -- | -- | -- |
        | 200×200 random heights (5 answers) | ~30 µs | **~16 µs** |
        | 2000×2000 random (18 answers) | ~2.0–2.4 ms | **~0.4 ms** (about 5×) |
        | 2000×2000 all equal (4M answers) | ~200–240 ms | ~10–15% slower |

        When every cell is an answer, collecting during the BFS is slower. I haven't profiled it. My guess is that the 4 million result allocations are now interleaved with the queue reads instead of done in one batch, and the two compete for cache. With 4M answers the time is mostly the output anyway: `vector<vector<int>>` means one `malloc` per answer, which the signature forces. Sparse answers are the normal case.
    * **Measured and rejected:** removing `resultVector.reserve(R·C)` (no change on sparse input, **~40–65% slower** on dense input, because the 40k inner-vector headers get moved on each regrowth; untouched reserved pages cost almost nothing). A flat copy of `heights` (an extra 16 MB copy that loses when the search only touches the border region). Hoisting `heights[r][c]` (noise, since the row stays in L1). And **keep the height test first** in the `&&` chain: it rejects about half of all neighbours on random terrain, while the bit test almost always passes. Putting the bit test first cost about 20% in my refactor.

* **Time & Space Complexity:**
    * **Time O(R · C).** Each pass pushes each cell at most once and checks 4 neighbours. There's no final scan.
    * **Space O(R · C):** `seen` (1 byte per cell), the queue (8 bytes per entry, reserved R·C), and the output. On the heap, the output is the largest part: one small `vector<int>` allocation per answer.

### [7] Surrounded Regions

> First pass `77e6a85` (border seeds found by scanning every cell, plus a separate `cannotMark` buffer), then `3e33628` (two loops that visit only the border), then the final version after one review: safe cells marked **in the board** with `'#'` through a single `markAndPush` lambda, and a final pass through a raw row pointer. The hardest part was the wording: "surrounded" didn't make sense until I rephrased it as **"an `'O'` survives exactly when it's connected, through `'O'`s, to an `'O'` on the border."** After that, it's Pacific Atlantic `[6]` with one ocean.

* **The Core Pattern:** search backwards from the border (§2 Pattern 3). Don't ask every region "do you touch the border?". Start from **every border `'O'`**, flood through `'O'`s, and mark everything reached as safe. Then one sweep: unmarked `'O'`s are captured (`'X'`), and marked ones go back to `'O'`. O(R·C).
    * **What changes from `[6]`:** one source set (the whole border) instead of two, and the board is **mutable and is the output**. That's the condition from `[4]` that lets the board hold the visited state. In `[6]` it couldn't, because the second pass needed the heights.
    * **Mental model:** pour water in along the whole border. Every `'O'` it can reach through `'O'`s is safe. Whatever's left dry is enclosed, and gets captured.

* **The "Gotcha":**
    * **Border, not corners.** My comment and my description said "corner cells", but the code was right (it checks the whole border). In an interview, "corners" means four cells. It's the same confusion that sank the first Pacific Atlantic idea.
    * **Walking only the border without four loops.** Two loops cover it, **each handling two opposite edges**: one over the rows does `[i][0]` and `[i][columns-1]`, and one over the columns does `[0][j]` and `[rows-1][j]`. That's 2R + 2C cells instead of scanning all R·C. The corners are shared by both loops, so either trim the column loop to `1 … columns - 2`, or let a test before the push skip them. If you ever want an inner loop over just the edge indices `{0, n-1}`, loop `k < std::min(2, n)` with index `k * (n - 1)`. Don't step `j += n - 1`: when `n == 1` the step is 0 and the loop never ends (it needs a `max(1, …)` guard).
    * **Changing half the marking scheme is worse than changing none of it.** My first in-place attempt moved the **seeds** to `'#'`, but the **BFS** still set `cannotMark` and left the cells it found as `'O'`. The final pass only reads the board, so every safe cell found by the BFS was captured: **45,551 wrong out of 100,000** (`[[X,O,X],[X,O,X],[X,X,X]]` lost its middle `'O'`). Example 1 still passed, because its only survivor is a border seed. **There must be one source of truth for "visited", written in one place:** `markAndPush` for seeds and BFS alike.

* **The Struggle & Insights:**
    * **The board is the visited set.** Marking a safe cell `'#'` means "already visited" is just "isn't `'O'` any more". The BFS condition shrinks to the bounds check, `markAndPush`'s single `'O'` test both filters and de-duplicates, and the R·C `cannotMark` buffer (plus a second memory read per neighbour) disappears. As a bonus, the 1-wide duplicate pushes from `3e33628` (when `columns == 1`, `[i][0]` and `[i][columns-1]` are the same cell) go away: the second call sees `'#'` and skips it.
    * **The final pass does two jobs in one sweep:** `'O'` → `'X'` (captured) and `'#'` → `'O'` (restore).
    * **A `char` store can alias anything, so take the row pointer once.** `board[i][j]` on `vector<vector<char>>` is two dependent loads (the outer buffer, then row `i`'s begin pointer). `char` is allowed to point into **any** object, so after any `char` store the compiler must assume the row's begin pointer might have changed, and it reloads both pointers for **every cell**. Verified in the `-O2` assembly: the indexed loop has `movq (%rdi)…; movq (%rcx,%rsi)…` inside the loop, while the `int` version of the same loop hoists them by itself (an `int` store can't alias a pointer). The fix: `char* row = board[i].data();` once per row, then `row[j]`.
        * **`data()` returns the address of element 0, not its value:** `row == &board[i][0]`, `row[j]` is `*(row + j)`, the same byte as `board[i][j]`, readable and writable. It's a `uint8_t buf[]` on an MCU.
        * **`auto& row = board[i]` isn't enough.** `row[j]` still reads the begin pointer from inside the row object, which a `char` store might have changed. A local `char*` whose address is never taken can't be touched by any store, so it stays in a register.
        * **Take it fresh for each row** (each row is a separate heap block), and only while nothing resizes that row (iterator invalidation applies to raw pointers too).
        * **It applies to flat `vector<uint8_t>` too.** `s[k] |= 2` reloads the vector's begin pointer on every iteration and doesn't unroll. Via `s.data()` it loads once and unrolls ×2. It only matters in tight loops with nothing but memory operations. Inside a BFS, the `emplace_back` call forces reloads anyway. **Never keep the queue's `data()` across `emplace_back`**, because a reallocation would leave it dangling.
    * **Measured** (`-O2`, best of 11, two runs; this machine is noisy):

        | Board | `3e33628` (`cannotMark`, indexed final pass) | Final (in-place `'#'`, row pointer) |
        | -- | -- | -- |
        | 200×200 random 50% `'O'` | ~222–250 µs | **~155–172 µs** |
        | 2000×2000 random | ~19.6–19.9 ms | **~14.0 ms** |
        | 2000×2000 all `'O'` | ~41–67 ms | **~25–26 ms** |
        | 2000×2000 `X` border, `O` inside | ~3.3–5.2 ms | **~2.1 ms** |

        Separately, in-place marking measured ~10–25% and the row pointer ~20% on random boards. **Tried and rejected:** a branch-free final pass (a nested `?:` instead of `if / else if`) didn't improve consistently across runs.

* **Time & Space Complexity:**
    * **Time O(R · C).** O(R + C) seeding, each safe `'O'` pushed once with 4 neighbour checks, and one final sweep.
    * **Space O(R · C)** for the queue only (8 bytes per entry, reserved R·C for the all-`'O'` worst case). The visited state lives in the board.
