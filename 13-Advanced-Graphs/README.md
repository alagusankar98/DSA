# Advanced Graphs - Topic Guide

## 1. Core Fundamentals

Topic 12 was almost all **unweighted**: every edge cost the same, so "fewest edges" was the whole question and BFS answered it. Advanced Graphs adds **weights**: each edge carries a cost (time, money, height, distance), and the question becomes **"cheapest"**, not "fewest". It also adds two structural questions that BFS/DFS alone don't answer: **"connect everything as cheaply as possible"** (minimum spanning tree) and **"use every edge exactly once"** (Eulerian path). Almost every tool here is a small change to something already built in Graphs: BFS, the heap from topic 10, Union-Find, and Kahn's topological sort.

### The Vocabulary
* **Weighted graph:** each edge `u → v` has a weight `w`. Stored as an adjacency list of `(neighbour, weight)` pairs instead of plain neighbours (the CSR layout from Graphs `[8]` works the same, with a parallel weights array).
* **Path cost:** the sum of the weights along the path. "Shortest path" now means **lowest total cost**, which can use *more* edges than the fewest-edges path.
* **Relaxation:** the one operation every shortest-path algorithm repeats. If going through `u` gives `v` a cheaper cost than `v` currently has (`dist[u] + w < dist[v]`), update `dist[v]`. The algorithms differ only in **which edges they relax, and in what order**.
* **Negative weights:** a cost that *lowers* the total (a refund, a downhill). They break Dijkstra (below). A **negative cycle** (a loop whose total is below 0) makes "cheapest" meaningless, since you could go round forever.
* **Spanning tree:** a subset of edges that connects all V nodes with no cycle, so exactly **V − 1 edges** (the Graph Valid Tree `[10]` fact). **Minimum spanning tree (MST):** the spanning tree with the lowest total weight.
* **Eulerian path:** a walk that uses **every edge exactly once**. (Not to be confused with a Hamiltonian path, which visits every *node* once and is NP-hard.)

### Dijkstra: BFS with a Priority Queue
**BFS is Dijkstra where every edge weighs 1.** BFS processes nodes in order of distance because a FIFO queue naturally holds distances `d, d, …, d+1, d+1`. With weights, a node discovered later can still be **cheaper** (one expensive edge vs three cheap ones), so FIFO order is wrong. Replace the queue with a **min-heap keyed on cost**: always pop the cheapest node not yet finalised.
* **The algorithm:** `dist[source] = 0`, push `(0, source)`. Pop the smallest `(d, u)`. If `d > dist[u]`, it's a **stale entry**, so skip it. Otherwise, relax each edge `u → v`: if `d + w < dist[v]`, set `dist[v]` and push `(dist[v], v)`.
* **Why the first pop of a node is final:** with **non-negative** weights, any other route to `u` passes through something still in the heap, which costs at least `d` already, and adding more non-negative edges can't make it cheaper. **One negative edge breaks this argument**, and that's why Dijkstra requires weights ≥ 0.
* **Lazy deletion again** (Minimum Interval to Include Each Query, Intervals topic): `std::priority_queue` can't lower a key in place, so you push a fresh entry and let the old one go stale. The `d > dist[u]` check discards it when it surfaces.
* **`std::priority_queue` is a max-heap.** For a min-heap: `std::priority_queue<P, std::vector<P>, std::greater<P>>` with `P = std::pair<int,int>` as `(cost, node)`, so pairs compare by cost first (the Heap topic lesson from Meeting Rooms II).
* **Cost:** O(E log E) with lazy deletion (each edge pushes at most one entry). For a dense graph, an **O(V²) array version** (scan an array for the cheapest unfinished node, with no heap) can be faster.
* *Use for:* Network Delay Time (Dijkstra from the source; the answer is the **largest** `dist`, or −1 if any node is unreachable).

### Variant: "Minimise the Worst Step" (minimax / bottleneck paths)
Sometimes a path's cost isn't the **sum** of its edges but the **largest** one: "the highest point you have to cross", or "the weakest link". Dijkstra still works if you change what a path costs: going from `u` to `v` costs `max(dist[u], weight)` instead of `dist[u] + w`. The proof still holds, because `max` can't make a path cheaper as it gets longer, just as adding non-negative weights couldn't.
* **Two other ways to solve the same problem:**
    * **Binary search on the answer + flood fill:** "can I get across using only cells ≤ T?" is a yes/no check that's monotonic in T, so binary-search T (the Koko Eating Bananas shape from Binary Search) and BFS for each guess.
    * **Union-Find in sorted order:** add cells from lowest to highest and stop when the start and end share a leader (Kruskal's idea, below).
* *Use for:* Swim in Rising Water (the grid is the graph, Pacific Atlantic style; the cost of a path is its highest cell).

### Bellman-Ford: Relax Everything, Round by Round
Instead of choosing the order cleverly, **relax every edge, V − 1 times**. After round `k`, `dist[v]` is the cheapest cost using **at most `k` edges**. A shortest path without a negative cycle has at most V − 1 edges, so V − 1 rounds are enough.
* **Handles negative weights.** A V-th round that still improves something proves a **negative cycle** exists.
* **Cost:** O(V · E). Slower than Dijkstra, but **every round has a meaning: "the best cost with at most k edges"**. That's exactly what a hop-limited question needs.
* **The hop-limit trick: copy before each round.** If round `k` reads values that were already updated *in the same round*, a path can sneak in 2 edges in one round, and the hop count is wrong. So each round reads from a **copy of the previous round's `dist`** and writes into the current one. (It's Pacific Atlantic's lesson, "don't let the pass you're running read its own writes", in a new form.)
* *Use for:* Cheapest Flights Within K Stops (**K stops = K + 1 edges**, so K + 1 rounds). **Why plain Dijkstra fails here:** the cheapest route may use too many stops. Dijkstra would finalise a node at its cheapest cost and throw away a pricier route that uses fewer stops, which might be the only one that fits within K.

### Minimum Spanning Tree: Prim and Kruskal
"Connect every point, with the lowest total cable length." Both algorithms rest on one fact, the **cut property**: split the nodes into any two groups, and the cheapest edge crossing between them is safe to use in an MST.
* **Kruskal (edge-first):** sort **all edges** by weight, then go through them with **Union-Find** (Graphs §1). `unite` succeeds → keep the edge, because it connects two separate groups. `unite` fails → skip it, because it would close a cycle. Stop after V − 1 accepted edges. That's Redundant Connection `[12]`'s "reading 1" plus a sort. **Cost O(E log E)**, dominated by the sort.
* **Prim (node-first):** grow one tree from any start node. Repeatedly add the **cheapest edge leaving the tree**. It's Dijkstra's loop, except the key is **the single edge's weight**, not the total path cost. With a heap that's O(E log E). With a plain array of "cheapest edge into the tree so far" and a linear scan per step, it's **O(V²) with no heap and no edge list**.
* **Which one:** for sparse graphs given as an edge list, Kruskal. For a **complete graph** (every pair of points is an edge, E ≈ V²/2), **array Prim**: the edges never need to exist in memory. Compute each weight on the spot, the way Word Ladder generated neighbours instead of building them.
* *Use for:* Min Cost to Connect All Points (complete graph on up to 1000 points with Manhattan distances: 500k edges for Kruskal to sort, compared with array Prim's 1M distance calculations and no allocation beyond two arrays).

### Topological Sort When the Edges Are Hidden
Kahn's algorithm (Course Schedule `[8]`) works unchanged. The work is **extracting the edges**. In Alien Dictionary, the words are sorted in an unknown alphabet. Compare each **adjacent pair** of words: their **first differing letter** gives one edge (`a` comes before `b`), and nothing after that position tells you anything.
* **Traps:** only the *first* difference counts. A longer word followed by its own prefix (`"abc"` then `"ab"`) is an **invalid** ordering, so return `""`. Letters that appear in the words but in no edge are still part of the answer (in-degree 0). A cycle means no valid alphabet, so return `""` (Kahn's `read < number of letters`).
* **Size:** 26 letters at most, so a `26 × 26` boolean matrix or 26 small lists is fine here. It's the rare case where an adjacency **matrix** is the right choice (tiny V, and you need to ignore duplicate edges).

### Eulerian Path: Hierholzer's Algorithm
"Use every ticket exactly once" means a path through **every edge** once. Two facts:
* **When does one exist (directed graph):** every node has in-degree = out-degree, except possibly a start node with one extra **out**-edge and an end node with one extra **in**-edge.
* **Hierholzer:** DFS from the start, **consuming each edge as you take it** (delete it so it can't be reused). When a node has no edges left, **append it to the result on the way back** (post-order). Reverse the result at the end. Why post-order: a greedy walk can enter a dead end early, and appending on the way back puts that dead end *last* in the final path, with the detour spliced in before it.
* **Greedy forward walking fails:** taking the lexically smallest ticket at every step can strand you before every ticket is used. Hierholzer handles that without any backtracking search.
* *Use for:* Reconstruct Itinerary (start at `"JFK"`; visit destinations in **lexical order**, so sort each node's edge list or keep them in a min-heap, and with post-order + reverse, the result is the lexically smallest valid itinerary).

### Complexity Cheat Sheet
| Algorithm | Time | Weights | Answers |
| -- | -- | -- | -- |
| BFS | O(V + E) | all equal | fewest edges |
| Dijkstra (binary heap) | O(E log E) | **≥ 0** | cheapest path from one source |
| Dijkstra (array, dense) | O(V²) | ≥ 0 | same, better when E ≈ V² |
| Bellman-Ford | O(V · E) | any (detects negative cycles) | cheapest with **at most k edges** after k rounds |
| Kruskal | O(E log E) | any | MST (edge list + Union-Find) |
| Prim (heap / array) | O(E log E) / O(V²) | any | MST (grow from a node) |
| Kahn's topological sort | O(V + E) | n/a | an order for a DAG, or "cycle" |
| Hierholzer | O(E log E) with sorted edges | n/a | path using every edge once |

### The Hardware Reality
* **Heap entries are small, so keep them small.** `(int cost, int node)` is 8 bytes. A heap of 8-byte pairs in a `std::vector` is contiguous and cache-friendly. Avoid putting strings or big structs in it: push indices, and keep the payload in side arrays.
* **`dist` as a flat `vector<int>`, initialised to a large value** (`INT_MAX` or a constant). Watch the **overflow** in `dist[u] + w` when `dist[u]` is `INT_MAX`: check reachability before adding, or use a sentinel well below `INT_MAX`.
* **The weighted CSR layout:** one `offsets` array, one `targets` array and one parallel `weights` array (or interleaved `(target, weight)` pairs). The same counting-sort build as Course Schedule `[8]`.
* **Dense graphs: compute, don't store.** A complete graph on 1000 points is 500k edges, which is 4–8 MB as pairs. Array Prim never builds them. It's the Word Ladder lesson in a new form.
* **Bellman-Ford's copy per round** is one V-sized `memcpy` per round. That's cheap and sequential, and it keeps the hop count correct.

### Where These Show Up in the Real World
Routing: **OSPF and IS-IS run Dijkstra** on link costs, while **RIP and BGP-style distance-vector protocols are Bellman-Ford**, spread over many routers and with hop limits. Map and route planners (Dijkstra/A*). Network and PCB design: MST for the cheapest cabling or trace layout, and clustering. Build systems and compilers (topological order from hidden dependencies). Circuit and DNA-sequence reconstruction (Eulerian paths: de Bruijn graphs in genome assembly). Minimax paths: the maximum vehicle height a route allows, or the widest-bandwidth path in a network.

---

## 2. General Summary / Quick Reference

### Pattern 1: Single-Source Shortest Path, Non-Negative Weights → Dijkstra
Min-heap of `(cost, node)`, skip stale entries, relax neighbours.
* *Use for:* Network Delay Time.
* *Discipline:* `greater<>` for a min-heap, `if (d > dist[u]) continue;` before relaxing, and check that `INT_MAX + w` can't overflow.

### Pattern 2: Minimax / Bottleneck Path → Dijkstra with `max` instead of `+`
The cost of a path is its worst edge or cell. Same loop, with `newCost = max(cost, weight)`.
* *Use for:* Swim in Rising Water.
* *Alternatives:* binary search on the answer + BFS, or Union-Find over cells in sorted order.

### Pattern 3: Shortest Path with a Hop Limit → Bellman-Ford, K + 1 Rounds
Each round relaxes every edge, reading from **the previous round's copy**.
* *Use for:* Cheapest Flights Within K Stops.
* *Discipline:* K stops = K + 1 edges. Copy `dist` before each round. Plain Dijkstra is wrong here: it finalises a node at its cheapest cost, which may use too many stops.

### Pattern 4: Connect Everything Cheaply → MST
Kruskal (sort edges + Union-Find) for edge lists. Array Prim for complete or dense graphs.
* *Use for:* Min Cost to Connect All Points.
* *Discipline:* an MST has exactly V − 1 edges, so stop when you have them. Don't build a complete graph's edge list if you can compute weights on demand.

### Pattern 5: Ordering from Comparisons → Extract Edges, then Kahn's
The first differing letter of each adjacent pair gives one edge. Then topological sort.
* *Use for:* Alien Dictionary.
* *Discipline:* a word followed by its own prefix is invalid, include letters that have no edges, and a cycle means return `""`.

### Pattern 6: Use Every Edge Once → Hierholzer (DFS, post-order, reverse)
Consume edges as you take them, append nodes on the way back, reverse at the end.
* *Use for:* Reconstruct Itinerary.
* *Discipline:* visit neighbours in lexical order, and never mark *nodes* as visited (airports repeat). It's the *edges* that get used up.

### The Non-Negotiable Discipline
* **Weighted? Negative weights? Hop limit? Is the cost a sum or a max?** These four answers pick the algorithm.
* **Dijkstra only with weights ≥ 0.** Negative weights or a hop limit → Bellman-Ford.
* **Lazy deletion:** push a new entry, skip stale ones on pop.
* **Overflow:** never compute `INT_MAX + w`.
* **Spanning tree = V − 1 edges** (from Graph Valid Tree), and Union-Find is the cycle check (from Redundant Connection).

---

## 3. Problem Strategies & Patterns

<!-- NeetCode 150 Advanced Graphs (6): Reconstruct Itinerary · Min Cost to Connect All Points · Network Delay Time · Swim in Rising Water · Alien Dictionary · Cheapest Flights Within K Stops -->
