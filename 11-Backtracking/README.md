# Backtracking - Topic Guide

## 1. Core Fundamentals

**Backtracking** is a way to search through *every possible sequence of choices* by building candidates one decision at a time, and **undoing the last decision** as soon as you've finished exploring it (or as soon as you know it can't lead anywhere). It's DFS, but the tree being searched isn't stored in memory. It's the **tree of decisions**, generated on the fly by the recursion itself.

### The Mental Model: the decision tree you never build
Take "all subsets of `[1, 2, 3]`". At each element you make one decision: **take it or skip it**.
```text
                          []                      ← depth 0: decide about 1
                 take 1 /    \ skip 1
                    [1]        []                 ← depth 1: decide about 2
                   /   \      /   \
              [1,2]   [1]   [2]    []             ← depth 2: decide about 3
              /  \    / \   / \    / \
       [1,2,3][1,2][1,3][1][2,3][2][3] []         ← leaves = the 8 answers
```
* **Each node is a *partial* answer; each edge is one choice.** The recursion call stack *is* your current position in this tree: depth = how many decisions you've made so far.
* **There's only ever one path in memory.** You don't keep the whole tree (it's exponential). You keep one mutable `path` vector, and walk it down and back up. Going down = `push_back`, coming back up = `pop_back`. That "coming back up and undoing" is the *backtrack*.
* **Compared with Trees:** in Trees, the nodes already existed and DFS visited them. Here, the "children" of a node are *the choices still available*, computed on the spot. Same recursion shape, but the tree is virtual.

### The Three-Step Template: choose → explore → un-choose
Almost every backtracking problem is this skeleton with different answers to "what are my choices?" and "when am I done?":
```cpp
void backtrack(State& state) {
    if (isComplete(state)) { record(state); return; }   // 1. base case: a leaf
    for (Choice c : choicesFrom(state)) {
        if (!isValid(state, c)) continue;               // 2. prune: skip dead branches
        apply(state, c);                                // choose
        backtrack(state);                               // explore the subtree
        undo(state, c);                                 // un-choose: restore EXACTLY what apply() changed
    }
}
```
* **The un-choose step must exactly reverse the choose step.** If you `push_back`, you `pop_back`. If you mark a cell visited, you unmark it. If you increment a counter, you decrement it. When the loop moves to the next choice, `state` must look *exactly* as it did before the previous choice was tried. Nearly every backtracking bug is a leaked change: something set during exploration that never got undone, so the sibling branches see stale state.
* **Why mutate-and-restore instead of passing copies:** passing `path` by value is correct (each call gets its own copy, so nothing needs undoing), but every call then pays an O(n) copy and a heap allocation. One shared `path` passed by `&` with `push_back`/`pop_back` costs O(1) per step. Copy only at the leaves, when you `record` a finished answer into the results.

### Pruning: the whole point of "backtracking" vs brute force
Pure brute force generates every full candidate and then checks it. Backtracking checks **while building**: the moment a partial candidate breaks a constraint, return immediately, and the *entire subtree under it* is skipped.
* Example: if a running sum already exceeds the target and all numbers are positive, no extension can fix it, so stop. One `if` cuts off a whole subtree that might hold thousands of leaves.
* **Sorting the input first** often makes pruning stronger (once one candidate is too big, every later one is too, so `break` instead of `continue`), and it's also the standard way to skip duplicates (equal values become neighbours).
* Pruning doesn't change the worst-case big-O (that's still exponential), but in practice it's the difference between "instant" and "time limit exceeded".

### Four ways to describe the choices (pick one before coding)
| Shape | What each level decides | Typical problems |
| -- | -- | -- |
| **Include / exclude** | "Do I take element `i`?" → 2 branches per level | Subsets |
| **Pick the next from a `start` index** | "Which of the elements from `start` onwards comes next?" → loop `i = start..n-1`, recurse with `i + 1` (or `i` if reuse is allowed) | Combination Sum, Subsets II |
| **Pick any unused** | "Which element not yet used goes in this position?" → loop over all, skip the `used[i]` ones | Permutations |
| **Grid walk** | "Which neighbour cell do I step into next?" → up to 4 directions | Word Search |

The `start` index is what separates **combinations** (order doesn't matter: `[1,2]` = `[2,1]`, so never look backwards) from **permutations** (order matters, so every unused element is a candidate at every position).

### Grid Backtracking (the Word Search shape)
On a 2-D grid, the "choices" from a cell are its neighbours, and "state" includes **which cells the current path has already used**.
* **Direction arrays** keep the four moves in one loop instead of four copy-pasted calls: `int dr[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};`.
* **Bounds check first, then look at the cell.** Reading `board[r][c]` with `r = -1` is undefined behaviour in C++ (for a `vector`, `operator[]` doesn't check). Check `0 <= r < rows` and `0 <= c < cols` before reading.
* **Visited-tracking options:** a separate `vector<vector<bool>>` (clean, costs rows × cols extra memory), or **mark the board cell in place** with a sentinel character and restore it on the way back up (zero extra memory, the usual interview answer). The in-place mark is exactly the choose / un-choose rule above: change one byte going down, put it back going up.
* **This is not BFS/flood-fill "visited".** In a graph traversal (topic 12), a visited cell stays visited forever. In backtracking, "visited" means "used *by the current path*", so it must be cleared when the path retreats, because a different path may legitimately pass through that cell later.

### Complexity: exponential is expected, so say it precisely
Backtracking problems usually ask for *all* answers, and the number of answers is itself exponential. You can't beat the output size.
* General shape: branching factor **b**, depth **d** → about **O(b^d)** nodes in the decision tree, times the work per node.
* Subsets: 2 choices × n levels → **2ⁿ** subsets, O(n) to copy each → **O(n · 2ⁿ)**.
* Permutations: n, then n−1, then n−2 … choices → **n!** leaves → **O(n · n!)**.
* Grid walk for a word of length L: 4 choices for the first step, then at most **3** for each later step (you can't step back onto the cell you came from, since it's marked) → about **O(rows · cols · 3^L)**.
* **Space:** O(d) for the recursion stack + `path`, *not counting* the output. Say both numbers in an interview: "O(n) auxiliary, plus the output."

### The Hardware Reality
* **Recursion depth = decision-tree depth, which is usually small.** For these problems it's ≤ n or ≤ word length, often under 20. Each frame is a few dozen bytes, so the stack is never the problem here (unlike a 10⁵-deep skewed tree in Trees). On a Cortex-M with a 1–2 KB task stack it's still worth knowing that depth × frame size is the real budget.
* **The real cost is the node count, and it's dominated by allocation.** A path stored in a shared `std::vector` with `reserve(n)` once does zero allocations during the search. Copying `path` at every call allocates at every node of an exponential tree. That's the difference between a cache-hot loop and `malloc` thrash.
* **Mutate-in-place is the embedded instinct.** Changing one byte and restoring it is the same pattern as saving and restoring a register around a call: the callee must leave state exactly as it found it.

### Where Backtracking Shows Up in the Real World
Constraint solvers (Sudoku, scheduling, register allocation in compilers), SAT solvers (DPLL is backtracking with very smart pruning), regex engines that support backreferences, Prolog's execution model, and puzzle and game-tree search.

---

## 2. General Summary / Quick Reference

When tackling Backtracking problems, keep these core patterns in mind:

### Pattern 1: Subsets / Combinations (`start` index, never look back)
Loop from `start` to the end, push, recurse with `i + 1`, pop. Every node of the decision tree is an answer for subsets; only nodes that hit the target are answers for combination problems.
* *Use for:* Subsets, Combination Sum (recurse with `i`, not `i + 1`, because an element can be reused), Combination Sum II, Letter Combinations of a Phone Number.
* *Discipline:* the `start` index is what prevents `[1,2]` and `[2,1]` both appearing.

### Pattern 2: Permutations (any unused element, every position)
Loop over all elements, skip the ones already in the path (`used[i]`), mark, recurse, unmark.
* *Use for:* Permutations.
* *Discipline:* the `used` array is state, so it's undone on the way back like everything else.

### Pattern 3: Duplicates in the Input (sort, then skip equal siblings)
Sort first. At a single level of the loop, if `nums[i] == nums[i-1]` and `i > start`, skip it: choosing the same value twice *as siblings* produces the same subtree twice.
* *Use for:* Subsets II, Combination Sum II.
* *Discipline:* skip equal **siblings** (same loop level), not equal elements *down* the path. `[1,1,2]` can still contain both `1`s.

### Pattern 4: Constraint Building (prune while building)
Track what the partial answer has used so far (counters, sets, or flags), and only make choices that keep it valid.
* *Use for:* Generate Parentheses (open count vs close count), Palindrome Partitioning (only cut after a palindromic prefix), N-Queens (columns + both diagonals already taken).
* *Discipline:* check validity *before* recursing, so invalid partial answers never get a subtree.

### Pattern 5: Grid Path Search (4 directions, mark the cell, restore it)
Start from every cell that could begin the path; from each, DFS into neighbours that match the next requirement, marking the current cell as used for this path and restoring it on return.
* *Use for:* Word Search, and the Trie-guided version in Word Search II (Tries `[3]`).
* *Discipline:* bounds check before reading the cell, and the restore must happen on **every** return path out of the cell, including an early `return true`, if the board must end up unchanged.

### The Non-Negotiable Discipline
* **Draw the decision tree first** (two or three levels on paper). Know what a level decides and what a leaf looks like before typing.
* **Every choose has an un-choose.** Check them in pairs before running anything.
* **Shared state by `&`, copy only at the leaves.**
* **Prune as early as the information allows.**
* **State the complexity as exponential × copy cost, plus O(depth) auxiliary.**

---

## 3. Problem Strategies & Patterns

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*

<!-- NeetCode 150 Backtracking (10): Subsets · Combination Sum · Combination Sum II · Permutations · Subsets II · Generate Parentheses · Word Search · Palindrome Partitioning · Letter Combinations of a Phone Number · N-Queens -->
