# Advanced Graphs - Topic Guide

## 1. Core Fundamentals

Topic 12 was almost all **unweighted**: every edge cost the same, so "fewest edges" was the whole question and BFS answered it. Advanced Graphs adds **weights**: each edge carries a cost (time, money, height, distance), and the question becomes **"cheapest"**, not "fewest". It also adds two structural questions that BFS/DFS alone don't answer: **"connect everything as cheaply as possible"** (minimum spanning tree) and **"use every edge exactly once"** (Eulerian path). Almost every tool here is a small change to something already built in Graphs: BFS, the heap from topic 10, Union-Find, and Kahn's topological sort.

### The Vocabulary
* **Weighted graph:** each edge `u → v` has a weight `w`. Stored as an adjacency list of `(neighbour, weight)` pairs instead of plain neighbours (the CSR layout from Graphs `[8]` works the same, with a parallel weights array).
* **Path cost:** the sum of the weights along the path. "Shortest path" now means **lowest total cost**, which can use *more* edges than the fewest-edges path.
* **Relaxation:** the one operation every shortest-path algorithm repeats. If going through `u` gives `v` a cheaper cost than `v` currently has (`dist[u] + w < dist[v]`), update `dist[v]`. The algorithms differ only in **which edges they relax, and in what order**.
* **Negative weights:** a cost that *lowers* the total (a refund, a downhill). They break Dijkstra (below). A **negative cycle** (a loop whose total is below 0) makes "cheapest" meaningless, since you could go round forever.
* **Spanning tree:** a subset of edges that connects all V nodes with no cycle, so exactly **V − 1 edges** (the Graph Valid Tree `[10]` fact). **Minimum spanning tree (MST):** the spanning tree with the lowest total weight.
* **Degree.** Undirected: a node's **degree** is the number of edges touching it. Directed: each node has an **in-degree** (edges *arriving*) and an **out-degree** (edges *leaving*). It counts **edges, not neighbour nodes**: two `JFK → ATL` tickets give JFK out-degree 2, even though ATL is one node. (An undirected self-loop adds 2 to its node's degree, because both of its ends touch it.)
* **Eulerian path:** a walk that uses **every edge exactly once**, from some start node to some end node. **Eulerian circuit:** an Eulerian path that **ends where it started**. (Not to be confused with a Hamiltonian path, which visits every *node* once and is NP-hard.)

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
"Use every ticket exactly once" means a path through **every edge** once.

**When does one exist?** It's a counting argument. Every time a walk passes *through* a node, it uses one edge in and one edge out. So every middle node is balanced, and only the start and end can be off, by exactly one each.

| | Undirected | Directed |
| -- | -- | -- |
| **Eulerian circuit** | **every** node has even degree. Start anywhere; you end at the same node. | **every** node has in = out. Start anywhere; you end at the same node. |
| **Eulerian path** (not closed) | **exactly 2** nodes have odd degree. Start at one, end at the other (either direction works). | **exactly one** node has out − in = 1 (**the start**) and **exactly one** has in − out = 1 (**the end**). All the others have in = out. |
| **Both also need** | all nodes with degree > 0 in **one connected component** | all nodes with degree > 0 in one component, **ignoring edge direction** ("weakly connected") |

* **"At most two odd" is really "0 or 2".** Every edge adds 2 to the total degree (one per end), so the total is even, and an even total can't have an odd number of odd terms. **Exactly 1 odd-degree node is impossible.** 0 odd means a circuit, 2 odd means a path, 4 or more means neither.
* **The directed imbalances come as a pair.** Each edge adds 1 to the total out-degree and 1 to the total in-degree, so Σ(out − in) = 0. A node with +1 forces a node with −1. You can't have one without the other. Both 0 means a circuit.
* **A circuit is also a path.** By the definition above, a circuit is just a path whose end is its start, so every circuit counts as a path, and the reverse isn't true (`A → B` is a path, not a circuit). Some books use "Eulerian path" (or "trail") to mean *only* the non-closed kind. With that reading, the two cases in the table don't overlap.
* **Connectivity is about edges, not every node.** A node with no edges at all (degree 0) doesn't stop a path or circuit existing, because it has no edge you'd need to reach. What fails is edges split into two separate components: then no single walk can cover them all. In LeetCode problems every node comes from an edge, so there are no edgeless nodes.
* **Reconstruct Itinerary:** the problem guarantees a valid itinerary from `"JFK"`, so you skip all of these checks. Without the guarantee, the directed column gives you the start (out − in = 1) and the end (in − out = 1) from the degrees alone. The end is also where a greedy walk first gets stuck, which is why Hierholzer records it first.
* *Verified* by brute force on 20,000 random small multigraphs (every condition in the table matched an exhaustive search, for both kinds of graph and both kinds of walk). The odd-degree counts that came up were only 0, 2 and 4, never odd.
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

### [1] Network Delay Time

> Four commits, one algorithm. First pass `588744a`: array Dijkstra (linear scan for the cheapest unsettled node) on a weighted CSR, with a final max loop. Then `dc749a3`: early return on "no node found", with the answer read off the last settled node. Then the heap, built twice: `94bb441` with `seen` only, and the final `34cc1db` with `dist` only. The first weighted graph. The walls: what an adjacency list actually stores, a segfault I first blamed on edge count, a reversed guard, the heap version not clicking the first time, and a stale check I deleted because I was sure it could never fire.

* **The Core Pattern:** single-source shortest path with weights ≥ 0, so Dijkstra (§2 Pattern 1). The answer is the time the **last** node hears the signal = the largest shortest-path distance, or −1 if any node is unreachable. Every version is the same loop: **pick the cheapest unsettled node, settle it, relax its out-edges** (`dist[u] + w < dist[v]` → lower `dist[v]`). The versions differ only in *how the cheapest node is found*.
    * **The graph is a weighted CSR**, the Course Schedule II build (Graphs `[9]`), with `(destination, time)` pairs in `edges`: count out-degrees into `offset[src]`, prefix-sum in place, then fill backwards with `edges[--offset[src]]`. When the fill is done, `offset[u]` has walked down to the start of u's slice and `offset[n]` still holds E, so u's edges are `[offset[u], offset[u + 1])`, last node included. One flat array of 8-byte structs, contiguous per node.
    * **The 1-based labels are converted once, at the boundary.** The `- 1`s live only in the three lines that read `times`. Everything after that (`offset`, `edges`, `dist`, the heap) is pure 0-based. This is the answer to "1-based or n + 1 slots?" in an interview: normalise at ingestion and say so. What costs points is mixing the two schemes.

* **The "Gotcha":**
    * **An adjacency list never stored "edges".** I thought my earlier lists held edges and wondered where a weight would go. They held the **neighbour**, and the edge was implied by which list it sat in (`v` in `adj[u]` means `u → v`). A weight is just a second field next to the neighbour. I named the field `edge` at first, then `destinationNode`, because the *struct* is the edge.
    * **The segfault was reachability, not edge count.** My first pass crashed on `offset[-1]`: when every unsettled node is still at `INT_MAX`, nothing passes `< minDistance`, so `minIdx` stays −1. I blamed "fewer edges than vertices". The real cause is a node nothing can reach, and a graph with a million edges can still have one. My first fix skipped the round. Then I saw myself that "no candidate found" **is** the answer: everything left is unreachable, so `return -1` on the spot. That return also removes the overflow trap from §1 by construction: you only relax from a node you actually picked, so `INT_MAX + w` can never happen.
    * **The reversed guard.** My early-return version shipped `if (minIdx != -1) return -1;`, which returns −1 on round 1 of every input, example 1 included. One dry run of the example would have caught it.
    * **The last settled distance *is* the answer.** Dijkstra settles nodes in non-decreasing order of distance, so the final max loop was rediscovering a number the loop already had. `lastSettledTime`: declared before the loop (it has to outlive it), initialised to `0` (a true value, since the source settles at 0, not a sentinel that looks like it might escape), and updated on the same line as the settle. Reusing `minDistance` for this would give one variable two meanings in the same round. Also a leftover `int read = 0;` sat above a loop that declared its own `read`: `-Wall` flags it as unused and `-Wshadow` as shadowing. A warning in a three-line edit is a review stop.
    * **"Isolated `k`: empty heap, so −1 or the answer?"** An empty heap only means "nothing more is reachable". Whether *everything* was reached is a separate question, and the cheapest answer is an `int`: count real settles, and at the end `settledNodes == n ? lastSettledTime : -1`. `n = 1` gives 0, correct. The same counter also gives the early exit (below).
    * **I deleted the stale check because "we're the ones pushing".** True at **push** time: `(d, v)` is pushed right after `dist[v] = d`, so they always match. But the check runs at **pop** time, and in between a cheaper path can lower `dist[v]`. The heap entry doesn't change, the array does. Trace: A→B (5), A→C (1), C→B (1), A→D (10). `(5,B)` goes in, then C lowers `dist[B]` to 2 and pushes `(2,B)`. `(2,B)` pops first, then `(5,B)` surfaces with `5 > 2`. **Without the check this returned 5 instead of 10.** The distances were fine (relaxing from the stale 5 can't beat what 2 already set). It was the **counters** that broke: the stale pop counted as B's second settle, `settledNodes` hit n before D ever popped, and `lastSettledTime` held 5. **The model:** lowering `dist[v]` is what makes the older entries stale, without touching them. `dist` is the source of truth, and heap entries are claims checked against it on pop. It catches *every* older duplicate (5 → 3 → 2 rejects both 5 and 3). It relies on relaxing with a strict `<`: with `<=` and zero-weight edges (times can be 0), two equal entries would both pass.

* **The Struggle & Insights:**
    * **What the heap actually replaces.** The scan answers "which unsettled node is cheapest?" by looking at all n, every round. A min-heap of `(distance, node)` hands you the cheapest in O(log). The catch is that `std::priority_queue` can't lower an entry's key in place. So a better path **pushes a second entry**, the old one stays and goes stale, and since the better entry is smaller it pops first. That's lazy deletion (also in Minimum Interval to Include Each Query): garbage is allowed to exist, and only one entry per node ever gets *processed*. Distance goes first in the pair because `pair` compares `.first` first. `std::greater` makes it a min-heap, which means spelling out the container argument too.
    * **The heap doesn't remove the per-node array. You need one, and you get to choose which.** I thought the point of the heap was to drop `dist`. `seen` only (push `d + w` for every unseen neighbour, skip already-seen nodes on pop) is **correct**: the first pop of a node is its shortest, as before. `dist` only (push only on a strict improvement, skip `d > dist[node]` on pop) is the textbook version. Both together are redundant. Either way, each node settles once and each edge is examined once, so the heap never holds more than **E + 1** entries, which is the reserve bound.
    * **The early exit: `while (!minQueue.empty() && settledNodes < n)`.** Without it, the loop only stops when the heap is empty, so after the last real settle it keeps popping, and every one of those pops is garbage. Why every one: each node settles exactly once, so once `settledNodes == n` every entry still in the heap belongs to a node that's already settled. Each will fail the stale check (`seen` set, or `d > dist[node]`), and draining them costs O(log E) per pop with no possible effect on the answer. Measured in the `seen` version: **2,777 of 2,946 stale pops came after all n nodes had settled**, so over 90% of its pops were cleanup work. The 169 that remain are stale entries that happened to sit below a real one while nodes were still unsettled, which lazy deletion can't avoid.
        * **Why the check belongs in the loop header:** the n-th settle still runs its relaxation loop before the condition is tested again, and that's harmless. In the `seen` version every destination is already seen, so nothing gets pushed. In the `dist` version every `dist` is final, so the strict `<` fails. Putting it in the header keeps one exit, and the same `settledNodes == n ? lastSettledTime : -1` after the loop covers both ways of leaving: all settled, or the heap ran dry first (unreachable nodes).
        * **It's the heap version of the scan's counted loop.** The scan ran exactly n rounds because every round settled one node or returned. The heap can't count rounds (stale pops add extra ones), so it counts settles instead.
    * **Copying before `pop()`:** `auto [d, u] = top();` copies 8 bytes. With `auto&`, the bindings would point into the heap's storage, and `pop()` moves another element into that slot.
    * **Reserve-then-move:** build a `std::vector` with E + 1 capacity and hand it to the `priority_queue(Compare, Container&&)` constructor. A moved vector keeps its capacity, and inspecting the underlying container confirmed it: 6,001 slots at the start, unchanged through the whole run, so 0 growth reallocations (versus ~12 doublings to reach a 2,877-entry peak). After the `dist` check, the peak is only ~300, so the reserve is a ~20× over-allocation. Keep it anyway: E + 1 is the only bound you can *guarantee* (an adversarial graph can make most relaxations succeed), and capacity you never write never enters the cache.
    * **Measured** (n = 100, E = 6000 random dense graphs, weights 0–100; all four versions fuzzed against Bellman-Ford on 3,000 random graphs under ASan/UBSan, 0 wrong):

        | | Array scan `dc749a3` | Heap, `seen` only `94bb441` | **Heap, `dist` only `34cc1db`** |
        | -- | -- | -- | -- |
        | pushes | n/a | 3,024–3,045 | **370–391** (~8× fewer) |
        | stale pops | n/a | 2,946 → **169** with the early exit | **55–63** |
        | time per call (`-O2`) | 36–39 µs | 90–94 µs | **40–41 µs** |

        (The early exit's share of this is in its own bullet above.) **A correction I had to absorb:** I'd been told the scan "wins" at these limits because O(V²) = 10⁴ beats the heap's ~78k. That 78k is the heap's *worst case*, with every edge pushed. With the `dist` check, pushes drop to ~6% of E, and the two **tie** (all three versions also pay the same fixed cost of building the CSR from `times`). The interview line: the array scan is O(V²) and best on dense graphs, while the heap is O(E log E) and pulls away as graphs get bigger and sparser. Present the `dist` version, and have the "do you even need `dist`?" answer (`seen` only works, at ~8× the pushes) ready.

* **Time & Space Complexity:**
    * **Time:** array scan **O(V² + E)**. Heap **O(E log E)** (≤ E + 1 pushes and pops). Both add O(V + E) for the CSR build.
    * **Space O(V + E):** `offset` and `edges`, one per-node array, and the heap (≤ E + 1 entries) in the heap versions.
