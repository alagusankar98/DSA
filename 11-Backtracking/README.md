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

### [2] Subsets

> Stuck for a long time. I read the solution in the morning and didn't understand it, then got an intuition from an LLM but couldn't turn it into code. In the end I coded it from a half-understood solution, mostly from memory. First pass `1231003`; the two `reserve` calls were added in the working tree. Two specific walls: **how can "include" come first when I can't `pop_back` from an empty vector?** and **I blanked on the iterative version.** Both are resolved below, so a reread should rebuild this from scratch.

* **The Core Pattern:** include / exclude (§1 "Four ways", row 1). Walk the array **one index at a time**, and at index `i` split into two universes: **take `nums[i]`** or **leave it**. Both universes move to `i + 1`. When `i == n`, every element has had its yes/no decision, so `temp` is one complete subset: copy it into the result and return. The tree is exactly the one drawn in §1: depth `i` decides `nums[i]`, and the 2ⁿ leaves are the answers.

* **My first model was the wrong one: a `seen` array.** I started with `temp` plus a `seen[n]` array of `false` and tried to "consume an index, mark it, then add the others", and it blurred (how many iterations? when to stop? what to unmark?). **`seen` is the Permutations tool** (§2 Pattern 2): there, order matters (`[1,2] ≠ [2,1]`), so every position may pick *any* element and you need to remember which are used. For subsets, order doesn't matter, so only ever **move forward**. If you never look back, there's nothing to remember. **Self-check:** "Does order matter?" No → a forward index (`i` or `start`), no `seen`. Yes → `used[]`.

* **The wall: "include first, but I can't pop an empty array".** The fear was that `pop_back` might run on an empty `temp`. It never can, because of one invariant:
    * **Every call returns `temp` exactly as it found it.** The base case doesn't touch it (it only copies it). The recursive case does `push(nums[i])` → call → `pop` → call. The first call returns `temp` unchanged (by the same invariant, one level down), so the `pop` removes **exactly the element this frame just pushed**, never anything else. The vector is never empty at that point: it holds at least the element this frame pushed.
    * So `push` → recurse → `pop` is a **matched pair inside one frame**, like a register save and restore around a call. The second call (exclude) then sees `temp` exactly as this frame received it. That *is* "not taking `nums[i]`", so the exclude branch needs no code of its own: it's just the second call.
    * **Trace, `[1, 2]`:** `f(0, [])` push 1 → `f(1, [1])` push 2 → `f(2, [1,2])` record `[1,2]` · pop → `[1]` → `f(2, [1])` record `[1]` · back in `f(0)`: pop → `[]` → `f(1, [])` push 2 → record `[2]` · pop → record `[]`. Output `[1,2], [1], [2], []`, and `temp` is back to `[]` at the end.
    * **Exclude first works just as well:** call → push → call → pop. Same 2ⁿ subsets, mirrored order (`[]` first). The order of the two branches is a free choice; only the matched push/pop pair is required.

* **The iterative versions I blanked on (two of them, both short):**
    * **Cascade (doubling):** start with `result = {[]}`. For each `num`, go over **only the subsets that existed before this `num`** (save `size()` first, because you're appending to the same vector), and add a copy of each with `num` appended. `[] → [], [1] → [], [1], [2], [1,2] → …`. Each element doubles the count: 1 → 2 → 4 → 2ⁿ. It's the include/exclude tree built level by level instead of path by path. **Gotcha:** use an index, not a range-for. `push_back` while iterating over the same vector invalidates the iterators.
    * **Bitmask (this one should click from Bit Manipulation):** n elements → 2ⁿ masks, and **each integer `0 … 2ⁿ − 1` *is* one subset**. Bit `j` set means "include `nums[j]`". Loop over `mask`, and for each `j` test `mask >> j & 1`. No recursion and no state to undo, just a counter. It's the same thing as enumerating every value of an n-bit register.
    * **The `start`-loop form (§2 Pattern 1)** is a third shape: record `temp` at **every** node, then `for (j = start; j < n; ++j) { push; recurse(j + 1); pop; }`. Same 2ⁿ results. It's worth knowing because Combination Sum and Subsets II use this shape, not the two-call one.

* **Corrections to the other LLM's review:**
    * **"`reserve(1 << n)` makes it mathematically perfect":** measured, it makes no difference. n = 10: 0.034 ms vs 0.037 ms. n = 20: 83 ms with `reserve` vs 70 ms without (noise, plus reserving 25 MB up front). When the outer vector grows, it **moves** its inner vectors (a 24-byte pointer swap each), which is cheap. The real cost is the **2ⁿ heap allocations** from copying `temp` into the result at each leaf, and `reserve` can't remove those. Keep it if you like (it's free when the size is known), but it isn't an optimization worth calling out.
    * **"Pass-by-reference ensures strict O(N) memory":** the **auxiliary** space is O(n) (recursion depth n + `temp`). The output is **O(n · 2ⁿ)** and dominates; say both.
    * **`1 << nums.size()`:** `1` is an `int`, so the shift happens in 32-bit signed arithmetic. That's fine for n ≤ 10 (the LeetCode limit), but it's UB from n = 31. `size_t{1} << nums.size()` says what you mean.

* **Code notes on `1231003` (+ working tree):**
    * `tempVector.reserve(nums.size())`: this one is right. `temp` never holds more than n elements, so it never reallocates during the search (§1 Hardware Reality).
    * `if (i >= nums.size())`: `==` is enough, since `i` only ever goes up by 1. `>=` is the defensive version. Either is fine.
    * `subsets` takes `const vector<int>&`, but LeetCode's signature is a non-const ref. It's fine to tighten it locally.

* **Time & Space Complexity:** **O(n · 2ⁿ)** time (2ⁿ leaves, each with an O(n) copy; the 2ⁿ − 1 internal nodes do O(1) each). **O(n)** auxiliary space (depth + `temp`), plus **O(n · 2ⁿ)** for the output.

### [3] Combination Sum

> Started with no idea at all. I could see "if the target is divisible by a number, repeat it x times", but not how that scales to x of one number plus y of another. With nudges only (no code), I wrote the first pass `98354ce`. **My process, worth reusing:** (1) decide the arguments the helper needs, (2) write the base cases in order: record on hitting the target, back out when it's exceeded, stop at the index boundary *before* reading `nums[i]`, (3) write the two recursive calls: stay on the same index, or move forward. Verified against an independent DP count of the combinations on 3,000 random inputs: 0 mismatches, no duplicates.

* **The Core Pattern:** Subsets' include/exclude tree (`[2]`) **with one argument changed**. At index `i`:
    * **take** `nums[i]` → **stay at `i`** (it may be used again)
    * **skip** `nums[i]` → **go to `i + 1`** (and never use it again on this path)
    * In Subsets both branches went to `i + 1`. Here, keeping the take branch at `i` is the whole difference.

* **Where I was stuck, and how each one resolved:**
    * **"x of this, y of that: how does it scale?"** You never decide counts up front. The counts **come out of the path**: "2 used three times" is just *take, take, take, skip* at index 0. One small decision per level, exactly as in Subsets, where nobody decided "this subset has 2 elements" either.
    * **"When it exceeds, backtrack and somehow not add that number again."** No extra logic is needed. Exceeding is just a `return`: the parent's `pop_back` undoes the last push, and the parent goes on to its **skip** branch, which is the "don't add it again" I was looking for.
    * **"I shouldn't add the same combination twice."** It's solved by the same forward-only rule as Subsets. `[2,3]` and `[3,2]` could both appear only if, after skipping past 2, you could come back to it, and skip only moves forward. Each combination is produced in exactly one order: non-decreasing index.
    * **"What if it's exceeded by 3 and the next number is −3?"** Then the prune would be wrong. It's safe **only because every candidate is positive** (constraints: `2 ≤ candidates[i] ≤ 40`). With positive numbers, the sum can only grow, so once it's over the target, the whole subtree has no answers. And with reuse plus a negative number, the problem itself breaks: `[3, −3]` with target 3 gives `[3]`, `[3,3,−3]`, `[3,3,3,−3,−3]`, … infinitely many answers. **Habit:** before writing any prune, name the constraint that makes it safe.

* **Code notes on `98354ce`:**
    * **Base-case order is right.** "Hit the target" is checked before "out of elements / exceeded", so a finished combination is recorded before anything can reject it, and `nums[i]` is never read with `i == n`. (Here the order happens not to change the output, since a skip can't arrive at `i == n` with the target already met, because the parent would have recorded and returned. But "record first, then reject" is the order that stays correct when the problem changes.)
    * **`tempSum` is passed by value, so `tempSum -= nums[i]` is only there because the local was mutated.** An alternative with nothing to undo: pass `tempSum + nums[i]` straight into the take call. **The rule this shows:** only state that's **shared by reference** (`tempVector`) needs an un-choose step. By-value arguments restore themselves when the call returns, because each frame has its own copy. Tracking **what's left** (`target − tempSum`) instead also drops one parameter: the base cases become `left == 0` and `left < 0`.
    * `combinationSum` takes `const vector<int>&` (LeetCode's signature is a non-const ref). Fine, and the same as Subsets.

* **The optimization (implemented in the working tree, after `9b43732` added the sort): sort, then prune both branches at once.** Right now, an over-target take returns, but the **skip** branch keeps walking `i + 1, i + 2, …`, and each step tries a take that also overshoots. If `nums` is sorted ascending, then `nums[i] > left` means **every later candidate is too big as well**, so *both* branches are dead and the helper can return before either call. Measured on candidates `2…31` with target 40 (6,131 answers):
    | Version | Helper calls | Time |
    | -- | -- | -- |
    | my first pass `98354ce` | 1,270,815 | 2.9 ms |
    | same two-call shape, sorted + the one-condition prune | 74,541 | 0.55 ms |
    | `start`-loop shape (§2 Pattern 1), sorted + `break` | 37,271 | 0.50 ms |

    | **my final version** (sorted, `tempSum + nums[i] > target` → return, before the push) | **74,541** | **0.42 ms** |

    That's 17× fewer calls from sorting once and changing one condition.
    * **Where the waste was (I didn't see it at first), as a trace:** `[2,3,6,7]`, target 7, path `[2,2,2]`, so 1 is left at `i = 0`. Nothing can fit any more, but the old code still made **8 calls**: take 2 → overshoot, skip → take 3 → overshoot, skip → take 6 → overshoot, skip → take 7 → overshoot, skip → `i = 4`. The skip branch walks down the right edge, and every take on the way fails. Frames like this one are all over the tree.
    * **Sorting alone did nothing** (`9b43732`). The old check `tempSum > target` fires *after* a take has overshot, one level down. The cut comes from asking **"does `nums[i]` fit in what's left?" before taking it**: sorted ascending means that if `nums[i]` doesn't fit, nothing after it does either, so the whole frame returns. **Reject before you take.**
    * **Why the skip call needs no check of its own:** it enters a frame that runs the same fit check against `nums[i + 1]` at the top. And because a take only happens when it fits, `tempSum` can never exceed `target` on entry, so the old `tempSum > target` became dead code. I dropped it and left a comment explaining why. The `start`-loop version is the shape Combination Sum II needs (skipping duplicate siblings only works in a loop), so it's worth writing once here.

* **Time & Space Complexity** (n = candidates, T = target, m = smallest candidate): a path makes at most T/m takes plus n skips, so the depth is ≤ **T/m + n**, and the tree has **O(2^(T/m + n))** nodes as a loose bound (pruning cuts most of them; in practice it's driven by the number of answers). Each recorded answer costs an O(T/m) copy. **Auxiliary space O(T/m + n)** (recursion depth + `tempVector`), plus the output.

### [4] Combination Sum II

> Took a while, with nudges only. First pass `790f0a9`, then `87baaa5` with the Combination Sum `[3]` prune put back after review (I'd forgotten it), and a working-tree refactor that counts up from `−target` to 0. I came in with the right index reasoning on my own; the wall was **duplicate values**. Four bugs on the way, each one a classic (below). Verified against a brute force (every index subset, deduplicated) on 3,000 random inputs with heavy duplicates: 0 mismatches.

* **The Core Pattern:** Subsets' include/exclude tree (`[2]`), where **both** branches move to `i + 1` (each element is used at most once), plus **sort and skip equal values on the exclude side**. That's the two-call version of §2 Pattern 3.
    * **Include `candidates[i]` → `i + 1`.** The next index may hold **another copy** of the same value. That's how `[1, 1, 6]` gets built from two different `1`s.
    * **Exclude `candidates[i]` → jump past *every* equal value, then `i + 1`.** Excluding means *"I'm done with the value 1"*, not *"I'm done with this index"*. If the exclude branch landed on the next `1`, it could take a 1 after all, and that's the duplicate.
    * **The decision is asymmetric:** include → "at least one more of this value". Exclude → "no more of this value, ever, on this path".

* **What I got right before any help:** both calls advance to `i + 1`, so **no index is ever reused**, and call 1 and call 2 never do the same work at the same step (they differ in whether `candidates[i]` is in the basket). Correct, and it's the reason the duplicates are a **value** problem, not an index problem.

* **Why duplicates still appeared (the `1a` / `1b` trace):** `[1, 1, 2]`, target 3. Label the ones `1a` (index 0) and `1b` (index 1).
    * Include `1a`, exclude `1b`, include `2` → `[1, 2]`.
    * Exclude `1a`, include `1b`, include `2` → `[1, 2]` **again**.
    * Two different index sets with the same values give the same combination. The fix: once the exclude branch rejects `1a`, it must not be offered `1b` either.

* **The wrong fix I rejected myself: a global `unordered_set` of "values already used".** With `[1, 1, 2]` and target 2, the answer is `[1, 1]`. A global set would see the first `1`, ban the value, and never build `[1, 1]`. Duplicates are fine **down** a path (include → include). They're only a problem **across sibling branches** (exclude one copy, then include its twin). The skip has to happen at that one decision point, not globally.

* **Sort first.** The skip only works if equal values are **adjacent**. In `[1, 2, 1]` the two 1s are separated by the 2, so "skip past equal neighbours" never sees the second one. The input is `const&`, so sort a copy (`candidatesCopy(candidates)`) rather than mutating the caller's data.

* **The four bugs, in order (each one worth remembering):**
    1. **Early `return` without undoing `tempSum`:** the next frames used a stale sum and recorded wrong combinations. *Fix:* pass `tempSum + candidates[i]` as the argument. By-value state undoes itself (the `[3]` lesson, applied again).
    2. **Early `return` before `pop_back`:** `tempVector` kept the last element. I first tried a second `pop_back` inside the `if`, then saw the simpler fix: **pop first, then decide**. *Rule:* undo the choice immediately after the include call, before any branching logic.
    3. **`return` instead of skip:** `if (candidates[i] == candidates[i+1]) return;` didn't just skip the twin, it **killed the whole exclude branch**. On `[10,1,2,7,6,1,5]` (sorted `[1,1,2,5,6,7,10]`), target 8, at the root this deleted every combination without a 1, which lost `[2, 6]`. **Every answer I found started with 1.** That pattern was the clue. *Fix:* advance `i` past the duplicates, then **still make the exclude call**.
    4. **`if` instead of `while`:** an `if` steps over one duplicate. `[1, 1, 1, 2]` needs to step over two. *Fix:* `while (i + 1 < n && candidates[i] == candidates[i + 1]) ++i;`, and the bounds check comes first so `candidates[i + 1]` is never read past the end.
    5. *(And a repeat:* I forgot the `[3]` prune entirely. The review put it back: `tempSum + candidates[i] > target` → return, which kills **both** branches, because sorted means every later value is too big as well.)

* **My condition reduction:** the full guard would be `tempSum > target || tempSum + candidates[i] > target`. Since every candidate is **positive** (`1 ≤ candidates[i] ≤ 50`), `tempSum > target` implies `tempSum + candidates[i] > target`, so the first half is **subsumed** and only the second half is needed. (It also can't happen at all, as in `[3]`: a take only happens when it fits.) Same habit as before: name the constraint that makes the simplification safe. Here it's positivity again.

* **The working-tree refactor: count up from `−target` to 0.** The `target` parameter is gone: start with `tempSum = −target`, the base case is `tempSum == 0`, and the prune is `tempSum + candidates[i] > 0`. Correct, and it's the same arithmetic as the usual **"remaining"** form (start at `target`, subtract, base case `remaining == 0`, prune `candidates[i] > remaining`), with the sign flipped. The remaining form reads more directly ("does it still fit?") and avoids the `-1 * target` setup. Either is fine; just make sure the name says what the value means.

* **Q&A: what about zeros? (Not allowed by the constraints, but worth knowing.)** Two separate cases:
    * **A zero *target* is harmless.** With positive candidates the root matches immediately and the answer is `[[]]`, in both `[3]` and `[4]`.
    * **A zero *candidate* in `[3]` causes infinite recursion:** take-0 stays at `i` without changing the sum, forever, until the stack overflows. The problem *must* forbid it.
    * **A zero candidate in `[4]` terminates,** because both calls move to `i + 1`, and the duplicate skip still works: `[0, 0, 1]` with target 1 gives `[0,0,1]`, `[0,1]`, `[1]`, each exactly once (tested). But `[0, 0]` with target 0 returns only `[]`, never `[0]` or `[0, 0]`. **The cause isn't the duplicate logic, it's record-and-*return*:** the base case stops at the first match, and zeros are the only values that could extend a finished combination. If zeros were allowed, the fix would be to record and *keep going*.
    * Positivity has now been the safety condition three times: the prune, the guard reduction, and record-and-return.
* **Base-case order matters here (unlike `[3]`):** include moves to `i + 1`, so a combination that finishes on the **last** element arrives with `i == n`. If the index check ran first, it would throw that answer away. Record first, then reject. In `[3]` the order happened not to matter, because take stayed at `i`. Here it does.

* **Corrections to the other LLM's review:**
    * **"7 arguments → `target` spills to the stack on every call; 6 keeps the recursion purely in registers":** the ABI part is right. System V x86-64 passes the first **6** integer/pointer arguments in registers, and a member function's hidden `this` takes one of them. But "purely in registers" is wrong: every call still pushes a return address and saves callee-saved registers to the stack. Measured (member function, 100 candidates with 10 copies each of 1–10, target 30, 37,155 calls): **7 args 0.27 ms vs 6 args 0.24 ms**, about **1 ns per call**. That's real but tiny, and it can't be separated from the 6-arg version also doing one less add per check. Dropping the parameter is worth it for **clarity** (one less thing to pass around). The register count is a side benefit, not the reason. (My file is a free function, so it's 5 args, or 6 counting `this` on LeetCode.)
    * **"Subsets II is the exact same code":** yes, minus the sum, with every node recorded as an answer. That's next.

* **Time & Space Complexity:** **O(n log n)** to sort + **O(2ⁿ · n)** worst case (every subset explored, each recorded answer copied in O(n)). The prune and the duplicate skip cut it far below that in practice. **O(n)** auxiliary (depth ≤ n + `tempVector`), plus the output.

### [5] Permutations

> First pass `3cf725a`, after nudges only. My first idea was wrong (walk circularly with `i % n`), then it clicked: **the helper is just a `for` loop.** Level 1 loops over all elements, level 2 over all except the one level 1 chose, level 3 over all except the ones chosen above it, and so on. Verified against `std::next_permutation` for n = 1…7: identical output sets.

* **The Core Pattern:** pick any unused element (§1 "Four ways", row 3; §2 Pattern 2). Each level asks **"which number goes in *this position*?"**, not "do I take element `i`?". Loop over **all** of `nums`, skip what's used, then mark, push, recurse, pop, unmark. Leaves are at depth n: 3 × 2 × 1 = 6 for `[1, 2, 3]`.
    * **Depth comes from `tempVector.size()`**, so no `i` parameter is needed. Every level scans all of `nums`, and the only thing that changes between levels is what's already used.

* **Why the forward-only index had to go.** In Subsets and both Combination Sums, moving only forward existed to **stop** `[1,2]` and `[2,1]` from both appearing. Here they're **different answers**, and both are needed. The rule that prevented duplicates there is exactly what blocks half the answers here. **Self-check (from `[2]`):** does order matter? Yes → loop over everything and keep a `used` record. No → a forward index.

* **My wrong idea: `i % n` (walk around the array circularly).** Starting at each index and wrapping around gives `[1,2,3]`, `[2,3,1]`, `[3,1,2]`. Those are **rotations**, n of them, not n!. It can never produce `[1,3,2]`, because a circular walk can't jump over an element to take a later one first. The "stop when the first number reaches the last" condition was a symptom of the same model.

* **The `seen` array is back, in the right problem.** My first attempt at Subsets `[2]` used one; there it was the wrong tool. Here it's required: "any unused element" means you have to remember what's used. And `seen` is state, so it gets un-chosen alongside `tempVector` (`erase` with `pop_back`).

* **Code notes on `3cf725a`:**
    * **`seen` is passed by value**, so every call **copies an `unordered_set`**: a fresh bucket array and one heap node per element. The code is still correct (the `erase` is still needed, because the loop reuses this frame's own copy for its next iteration), but the copy is the most expensive thing in the function. Measured:
        | Version | n = 6 (LeetCode max) | n = 9 |
        | -- | -- | -- |
        | mine, `unordered_set` **by value** | 0.26 ms | 223 ms |
        | `unordered_set` **by `&`** | 0.08 ms | 48 ms |
        | **bitmask** of used *indices*, passed by value | **0.03 ms** | **19 ms** |

        **By `&` is 3–5× faster than by value. The bitmask is another 2.5×.** Contrast with the `[3]`/`[4]` lesson ("by-value arguments undo themselves"): that's free for an `int`, but not for a container that allocates. By value is fine for a register-sized value; anything that allocates goes by `&` with an explicit undo.
    * **The bitmask (the embedded answer):** n ≤ 6, so "which indices are used" fits in a **byte**. Test bit `i` to skip, and pass `used | (1u << i)` to the child. It's register-sized, so passing by value is free and there's nothing to undo. No hashing, no allocation. A `std::array<bool, 6>` or `vector<bool>` by `&` works too.
    * **Key the `used` record by index, not by value.** `seen.contains(num)` only works because the constraints promise **distinct** values. With `[1, 1, 2]`, the second 1 would look "already used" and never get placed. Tracking indices makes the record independent of the values. Permutations with duplicates then also needs the sort-and-skip-twins trick from `[4]`.
    * `factorial` for `reserve` is fine at n ≤ 6 (and the `reserve` itself matters little, as measured in `[2]`). `tempVector.reserve(n)` is the one that pays.
    * `tempVector.size() >= nums.size()`: `==` is exact, since it grows by one per level.

* **Follow-up (working tree):** `seen` is now passed by `&` and holds **indices** (`unordered_set<size_t>`). Still verified against `next_permutation`.
* **Follow-up 2 (working tree): the bitmask, passed by value.** I went through three versions. (1) Set bit `i` before the call, and forgot to clear it after, so later iterations skipped index `i`. (2) Looked up how to clear a bit: `seen &= ~(1 << i)`. (3) Then asked myself why clear at all if it's passed by value, and passed `seen + (1 << i)` straight into the call, with nothing to undo. Correct, verified against `next_permutation`. `+` works because bit `i` is known to be clear there, but `|` says what's meant. The rule: **by value only undoes what happens in the argument expression.** Anything assigned to the local still needs an undo. Use `uint64_t{1} << i` for a 64-bit mask, since a plain `1` is a 32-bit `int` and shifting it by 32 or more is UB.
    * **The "it fails" scare was an uninitialized variable:** in the NeetCode editor I'd written `uint64_t seen;` without `= 0`. A local of a built-in type with no initializer holds **whatever was on the stack**. Reading it is undefined behaviour, so random bits mark indices as already used, and permutations vanish. It can look fine locally and fail on the judge. `-Wall` catches it (`warning: 'seen' is used uninitialized`), so compile with warnings on. Embedded reflex: same as an uninitialized `uint32_t` register shadow before a read-modify-write.
* **Q&A: could I drop `tempVector` and rebuild the permutation from `seen` at the leaf?** I started down that road, then stopped, because `unordered_set` doesn't preserve insertion order. Right call, and the deeper reason: **a permutation *is* an order**. Any set only answers "is it in?": `unordered_set` has no order, and `std::set` sorts by key, which throws away the order of choice. The order of choice is the answer, and `tempVector` is exactly that record (the path). `seen` is only an index for fast "already used?" lookups. Two jobs, two structures.
* **Q&A: doesn't a `uint64_t` bitmask cap n at 64?** Yes, and it doesn't matter, because the **output** runs out long before the bits do. Each permutation stored in a `vector<int>` costs about `24 + 4n` bytes:
    | n | permutations | output size |
    | -- | -- | -- |
    | 10 | 3.6 million | 0.23 GB |
    | 12 | 479 million | 35 GB |
    | 13 | 6.2 billion | 473 GB |
    | 20 | 2.4 × 10¹⁸ | 2.5 × 10¹¹ GB |

    Even a `uint16_t` mask outlives any n you could print. **The general rule:** a bitmask's width only becomes the real limit when the search is *pruned* or *counts* instead of listing everything (for example, counting N-Queens solutions without storing the boards). Then use `std::bitset<N>` or `vector<bool>`. Either way, an `assert(n <= 64)` documents the assumption in one line, the kind of guard an embedded reviewer looks for.
* **Another approach worth knowing: swap in place.** Fix position `k` by swapping each of `nums[k…n−1]` into slot `k`, recurse on `k + 1`, then swap back. No `seen` at all and no separate `tempVector`: the array itself is the path, and swap-back is the un-choose step. It needs a mutable copy of `nums`.

* **Time & Space Complexity:** **n!** leaves, each copied in O(n). The internal nodes add up to about **e · n!** (≈ 2.72 · n!), each doing an O(n) scan. So **O(n · n!)** time. **O(n)** auxiliary space (depth n, `tempVector`, the used record), plus **O(n · n!)** for the output.
