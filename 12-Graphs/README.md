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
* *Discipline:* `size = q.size()` per round when you need to count the rounds.

### Pattern 3: Search Backwards from the Border (reverse the question)
Instead of asking "can this cell reach the edge?" from every cell (expensive), start from the **edge** and mark everything that can reach it, in one traversal per border.
* *Use for:* Pacific Atlantic Water Flow (two searches inward from the two oceans; the answer is the cells reached by both), Surrounded Regions (`O`s connected to the border survive, everything else gets flipped).

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

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*

<!-- NeetCode 150 Graphs (13): Number of Islands · Max Area of Island · Clone Graph · Walls and Gates (Islands and Treasure) · Rotting Oranges · Pacific Atlantic Water Flow · Surrounded Regions · Course Schedule · Course Schedule II · Graph Valid Tree · Number of Connected Components in an Undirected Graph · Redundant Connection · Word Ladder -->
