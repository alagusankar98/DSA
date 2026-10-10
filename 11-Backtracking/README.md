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

### [1] Word Search

> Solved **before** Tries `[3]` (Word Search II) on purpose: II is this grid search with a trie steering it, so this is the prerequisite. About 30 minutes in I was blocked on **how to make the recursive calls**, and looked at the solution. From there the structure came together: reject every bad case at the top of the helper, then fan out in four directions. The first pass used `std::set<pair<int,int>>` as "seen"; the second (committed) swapped it for a `vector<vector<bool>>`.

* **The Core Pattern:** Grid path search (§2 Pattern 5), in two parts:
    * **The scanner:** a nested loop in `exist` that tries **every** cell as a possible start, and returns `true` the moment one start works.
    * **The explorer:** a recursive helper that answers one precise question. It's choose → explore → un-choose (§1): mark the cell as used, ask the four neighbours, unmark.
* **The question that unblocks the recursion:** the helper's contract is
  **"starting *at* cell `(i, j)`, can I spell `word[k..]` without reusing a cell on the current path?"**
  Once that sentence is fixed, the recursive calls write themselves. If this cell matches `word[k]`, the rest of the word must be spelled starting at one of the four neighbours, i.e. `word[k+1..]` from `(i-1, j)`, `(i+1, j)`, `(i, j-1)` or `(i, j+1)`. That's the same question asked again with `k + 1`. Being blocked on "how do I recurse" usually means the function's contract isn't pinned down yet. Write the sentence first, then the code.

* **The Struggle & Insights:**
    * **Check on entry, not before calling.** My first approach matched `word[0]` in `exist` and used the helper only for the *next* letters. That forced me to validate four neighbour coordinates (bounds, seen, letter) *before* each of the four calls, which is four copies of the same checks and messy. The fix was to **trust the helper**: every call, including the very first one from the scanner, checks **only its own cell** at the top and returns `false` if anything is wrong. Callers just call. An out-of-bounds call costs one function entry and an immediate return, which is cheap. Same lesson as Tries `[2]`, where deleting the call-site null checks and letting the base case handle `nullptr` cleaned up the code: **one check, in one place, at the top of the callee.**
    * **The order of the guards matters: "word finished" comes first.** The helper starts with `if (k >= word.size()) return true;`, **before** the bounds check. That's because success is only recognised *one call after* the last letter matched, and that call is aimed at a neighbour, which may be off the board. Put the bounds check first and a fully matched word whose last letter sits on an edge gets rejected. Verified in review: with the two checks swapped, a `1×1` board `{'A'}` searching `"A"` returns `false`, and so do all three of LeetCode's sample cases.
    * **Bugs found in my own first-draft review:**
        * **Inner loop bound:** `j < std::ssize(board)` compared the column index against the **row count**. That's only right on a square board. On a 3×4 board the last column is never tried as a start; on a 4×3 board `j` runs one past the end of a row. Fixed to `std::ssize(board[i])`.
        * **No negative bounds check:** stepping up from row 0 calls with `i = -1`. In `i >= board.size()`, the `int` is converted to `size_t` (unsigned) before comparing, and `-1` becomes `SIZE_MAX`, so the check *happened* to reject it. That works only because of an implicit conversion, which is fragile. I added explicit `i < 0 || j < 0`. With those first, the remaining `i >= board.size()` comparisons are correct, but they still mix signed and unsigned (`-Wall -Wextra` flags both with `-Wsign-compare`); comparing against `std::ssize(board)` / `std::ssize(board[i])` would make them warning-free.
    * **Short-circuit `||` gives the early exit for free.** `a || b || c || d` stops calling directions as soon as one returns `true`. Storing the result in `resultFlag` *before* unmarking means the un-choose step still runs on the way out.

* **Q&A: where should the `coordinates` alias live?** I needed it in both the private helper and the public method, with private written above public, so I put it outside the class. That works, but it puts `coordinates` into the global namespace for every file that includes this code. The confusion was about **access**, but the actual rule is about **declaration order**:
    * **Access doesn't restrict members.** `private` only stops code *outside* the class from naming something. Every member function, public or private, can use a private alias.
    * **Order matters only in signatures.** A type used in a member function's **parameter list or return type** must be declared *above* it in the class. Inside function **bodies** the whole class is visible, even names declared further down. Verified by compiling: a body using an alias declared below compiles; a signature using it fails with "has not been declared".
    * **So:** make `using coordinates = std::pair<int, int>;` the first line of the `private:` section. Every member below can use it, and it doesn't leak. Put it under `public:` only if outside code needs to *name* the type. Callers can still pass `{i, j}` without naming it.

* **Q&A: `std::set` vs `vector<vector<bool>>`, which is actually optimal?** I wanted the board read-only (a realistic interviewer constraint, and it's why my signature takes `const&`). `std::unordered_set<pair<int,int>>` doesn't compile without a custom hasher (the standard library has no `std::hash` for `pair` or for my own struct), so the first pass used `std::set`, which only needs `<`. Measured in review on the worst case (6×6 board of all `'A'`, searching `"AAAAAAAAAAAAB"` so nothing matches and every path is explored, `-O2`, this machine):

    | "seen" storage | Cost per mark / check | Time |
    | -- | -- | -- |
    | `std::set<pair<int,int>>` | red-black tree: O(log k) comparisons, **a heap allocation on every insert and a free on every erase** | **66 ms** |
    | `vector<vector<bool>>` (my 2nd version) | O(1) index, no allocation during the search | 9 ms |
    | flat `vector<char>(rows * cols)` indexed `i * cols + j` | O(1), one contiguous block | 9 ms |
    | `uint64_t` bitmask, bit `i * cols + j` (fits: LeetCode caps the board at 6×6 = 36 cells) | one AND / OR in a register | 9 ms |
    | in-place: overwrite `board[i][j]` with `'#'`, restore after | O(1), **zero extra memory**, but the board must be writable | 8 ms |

    * **Verdict:** for a read-only board, `vector<vector<bool>>` is the right answer. The set is ~7× slower, all from per-node `malloc`/`free` and pointer-chasing through a tree. Everything array-based ties at this board size. If mutation is allowed (LeetCode's actual signature is a non-const `vector<vector<char>>&`), in-place marking is the standard interview answer: same speed, O(1) extra space.
    * **The embedded aside:** `vector<bool>` is a special case in the standard: it **packs 8 flags per byte**, so `seen[i][j]` isn't a real `bool&` but a proxy object. A read is load + shift + mask, and a write is a read-modify-write, exactly like setting one bit in a GPIO register. That's why it's memory-tight but can't hand out a `bool*`. On a 36-cell board it made no measurable difference.
    * Big-O time is the same for all of them. The choice only changes the constant factor and the extra space.

* **Q&A: initialising the 2-D `seen` grid.** My plan was to `reserve` the outer vector and then loop to set up each row. The trap: **`reserve` only allocates capacity; `size()` stays 0**, so `seen[i]` after a `reserve` indexes elements that don't exist (undefined behaviour). You'd need `resize` or `push_back` instead. The one-liner is the **fill constructor** `vector(count, value)`, used twice: `std::vector<std::vector<bool>> seen(rows, std::vector<bool>(cols, false));`. It builds one row of `cols` `false`s, then copies it `rows` times. `seen` is allocated once in `exist` and shared across every start cell. That's safe because every path un-marks what it marked, so the grid is all `false` again whenever control is back in the scanner.

* **Time & Space Complexity:** with board `m × n` and word length `L`:
    * **Time O(m · n · 3^L):** every cell can be a start. The first step has 4 directions; every later step has at most **3**, because the cell you came from is marked. Precisely `4 · 3^(L-1)` per start, which is O(3^L).
    * **Space O(L)** recursion depth (at most 15 frames on LeetCode, so trivial), **plus O(m · n)** for the `seen` grid. The set version is O(L) for the set (it only holds the current path), but slower. The in-place version is O(L) total.
    * **Known extra pruning, not used here:** before searching, count letters on the board and return `false` if the word needs more of some letter than exists. And if the word's last letter is rarer on the board than its first, search the reversed word, so fewer starts survive the first check.
