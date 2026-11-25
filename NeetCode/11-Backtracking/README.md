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

### [6] Subsets II

> Passed on the first try (`09163f7`) after rereading Subsets `[2]` and Combination Sum II `[4]`. It's literally `[2]`'s two-call tree plus `[4]`'s sort and skip-the-twins. **But it felt like magic, not understanding.** This entry is the "why", so a reread can rebuild it from first principles instead of from memory. Verified against a brute force (every index subset, deduplicated) on 3,000 random inputs: 0 mismatches. The output count also equals the formula below every time.

* **The Core Pattern:** `[2]` (take → `i + 1`, skip → `i + 1`, record at `i == n`) + sort + **on the skip side, jump past every copy of `nums[i]`** (the `while` from `[4]`). There's no target, so the only base case is `i == n`: every leaf is a valid subset.

* **My first idea (and why it duplicates):** "push the empty set, loop `0…n`, keep index 0 fixed and repeat for the others". That's the plain `[2]` tree, which treats `2a` and `2b` as different elements. On `[1, 2, 2]` it gives 8 subsets, not 6. `[1, 2]` appears twice (take `2a` / skip `2b`, and skip `2a` / take `2b`), and so does `[2]`.

* **Why it's not magic: count the copies.** This is the argument to rebuild from.
    * Take a value that appears **k** times, e.g. `2, 2, 2`. As far as the answer is concerned, a subset doesn't care **which** 2s it holds, only **how many**: 0, 1, 2 or 3. That's **k + 1** distinct choices.
    * The plain tree chooses *which* copies (each one in or out): 2ᵏ paths for only k + 1 distinct outcomes. That's where the duplicates come from: "one 2" can be made 3 ways (`2a`, `2b` or `2c`).
    * **What the skip does:** at the first copy of the value, the code can only:
        * **skip** → jump past the whole run → **0 copies**
        * **take**, then at the next copy: skip → past the run → **1 copy**
        * take, take, skip → **2 copies**
        * take, take, take → **3 copies**
    * That's **exactly k + 1 paths, one per count.** Every path uses the **leftmost** copies (`2a`, then `2a 2b`, …), never `2b` alone. **"If you use c copies, use the first c"** is the rule, and the `while` loop enforces it. Each multiset has exactly one path, so it appears exactly once.
    * **Why only on the skip side:** take stays *inside* the run (the next index may be another copy, and you may want more). Skip means "I'm done with this value", so it leaves the run entirely. If skip could land on `2b`, it could "change its mind" and take a 2 after all, which is the duplicate.
    * **Why sort:** "the first c copies" only works if the copies are **adjacent**, so the run is one contiguous block the `while` can jump over.
    * **The check that proves it:** the number of answers = **∏ (copies of each value + 1)**. `[1, 2, 2]`: (1+1) · (2+1) = **6** ✔. All distinct: 2ⁿ, the Subsets `[2]` formula as the special case where every k = 1. The verification checked this formula on all 3,000 inputs.

* **The same proof covers Combination Sum II `[4]`.** It's the same "first c copies" argument with a target added. Duplicate handling in this whole topic comes down to one idea: **choose a count per value, not a set of indices.**

* **The `start`-loop shape does the same thing** (§2 Pattern 3): record at every node, loop `j = start…n−1`, and skip `j` when `j > start && nums[j] == nums[j−1]`. "At this level, only the first copy of each value may be *the next* element" is the same leftmost-copies rule, expressed per level.

* **Code notes on `09163f7`:** the bounds check comes before `nums[i + 1]` in the `while` (the `[4]` lesson held). `tempVector.reserve(n)` is the one that pays. The sorted copy keeps the input `const`. Nothing to fix.

* **Time & Space Complexity:** **O(n log n)** sort + **O(n · 2ⁿ)** worst case (all distinct). With duplicates, the leaves are **∏ (kᵥ + 1)** and each is copied in O(n). **O(n)** auxiliary, plus the output.

### [7] Generate Parentheses

> A real wall. I couldn't generate ideas, and even with nudges my brain "wasn't braining". I looked at the solution, corrected my code (`255e14a`), and only *then* understood it by tracing n = 3 by hand. My trace was right; I just didn't trust it. The fix that made it pass was one comparison: `close < n` → `close < open`. Verified for n = 1…10: every string valid, no duplicates, and the counts are exactly the Catalan numbers (1, 2, 5, 14, 42, …, 16,796).

* **The Core Pattern:** constraint building (§2 Pattern 4). It's a two-way tree like take/skip, but **both** branches add a character: `(` or `)`. Instead of building strings and checking them afterwards, put a **gate in front of each branch**, so an invalid string never gets built:
    * **Branch A, add `(`:** allowed only if `open < n` (there are only n opens to spend).
    * **Branch B, add `)`:** allowed only if `close < open` (there must be an unclosed `(` *right now*).
    * **Base case:** length `2n` → record. Because every move was legal, every full-length string is valid. Nothing is checked at the end.

* **Where I was stuck, and how each one resolved:**
    * **"Record when open == close."** That accepts `""` at the start, `()` before it's long enough, and `)(` / `))((`, which have equal counts but are invalid. Equal counts **at the end** isn't validity. Validity is a property of **every prefix**.
    * **"When do I clear `tempString`?"** Never. Each branch is its own matched pair: push `(` → recurse → pop, then push `)` → recurse → pop. After A's pop, the string is exactly what this call received, so B starts clean. It's the Subsets `[2]` invariant ("every call hands the string back unchanged"). The difference is that in Subsets only take pushed; here both branches do.
    * **"A single `i ≥ n` base case."** One counter can't express this. You need **two** counts: `open` gates A, and `close` *compared with `open`* gates B. They aren't base cases that `return`; they're conditions on whether a branch runs at all.
    * **`close < n` vs `close < open` (the bug).** `close < n` only limits the **total** number of `)`. It never asks whether this `)` has a `(` to match. With n = 1 it builds `)(`. With n = 3 it gives **20** strings instead of 5, and its first three answers are the same as the correct ones, which is why it looked right at first. The first difference is at `(())` (open 2, close 2): `close < open` blocks it (nothing left to close), but `close < n` allows it and builds `(()))(`.
    * **"Why does it unwind 'three levels'?"** Each `)` in `((()))` was pushed by a **different** call, so each call pops its own on the way out. Then the call at `((` pops its branch-A `(` and runs its own branch B → `(()` → … → `(()())`. That's the second answer, exactly as I traced it.

* **The link to Valid Parentheses (Stack topic):** `open − close` is **the depth of the stack** at that moment. `close < open` is "the stack isn't empty, so a `)` has something to pop". `open < n` caps the pushes. The final length check plus the gates means it ends with depth 0. This problem is Valid Parentheses run in reverse: instead of checking a string with a stack, generate only strings whose stack never underflows.

* **Code notes on `255e14a`:**
    * **`open == close &&` in the base case is redundant.** At length `2n`, the gates guarantee `open ≤ n` and `close ≤ open`, and `open + close = 2n`, so `open = close = n`. Harmless, but it suggests the end check is doing work when the gates already did it.
    * **`tempString.size() == 2 * n` compares `size_t` with `int`.** `-Wall -Wextra` warns `-Wsign-compare`. Equivalent check with no signedness mix: `open + close == 2 * n` (both `int`), or `static_cast<size_t>(2 * n)`.
    * `tempString.reserve(2 * n)` once: the string never grows past it.
    * `close < n` can be dropped entirely, not just replaced: `close < open ≤ n` already implies it.

* **Time & Space Complexity:** the number of answers is the **n-th Catalan number** `Cₙ = (2n choose n) / (n + 1)`, which grows like **4ⁿ / (n^1.5 · √π)**. Each answer is copied in O(n), so the time is **O(4ⁿ / √n)**. Thanks to the gates, there are no dead branches: every path leads to a valid answer. **O(n)** auxiliary (depth 2n + the string), plus the output.

### [8] Palindrome Partitioning

> **Three days.** The longest wall of the topic. First pass `ca670dd`. I started from the wrong model (include/exclude, which builds *subsequences*), went through two broken versions, and needed three explanations of the loop shape before it landed. Then I spent more time on one C++ detail: getting a `std::string` out of a `string_view` substring. Verified against a brute force (every set of cut positions) on 3,000 random strings: 0 mismatches.

* **The Core Pattern:** loop from `start` (§2 Pattern 1's shape) where each call asks **"how long is the next piece?"** The tape left is `s[start …]`. Loop `end` from `start` to the last index. The piece is `s[start … end]`. If it's a palindrome: push, recurse with `end + 1`, pop. If not: skip it (no subtree). Base case `start == s.size()`: the tape is empty, and every piece was checked when it was cut, so record.

* **Mental model: snapping pieces off a strip of tape.** Each level snaps one piece off the **front** and hands the rest down. The full walk for `"aab"`:
    | Level | Tape left | Tries | Kept? | Then |
    | -- | -- | -- | -- | -- |
    | 1 | `aab` | `a` | ✔ | level 2 with `ab` |
    | 2 | `ab` | `a` | ✔ | level 3 with `b` |
    | 3 | `b` | `b` | ✔ | empty → **record `[a,a,b]`** |
    | 2 | `ab` | `ab` | ✗ | skip |
    | 1 | `aab` | `aa` | ✔ | level 2 with `b` |
    | 2 | `b` | `b` | ✔ | empty → **record `[aa,b]`** |
    | 1 | `aab` | `aab` | ✗ | skip, done |

* **A partition is a choice of cuts.** `"aab"` has 2 gaps, `a ▢ a ▢ b`, and each gap is cut or not: 2ⁿ⁻¹ = 4 partitions in total, of which 2 are all-palindrome. So my binary instinct wasn't wrong, but the yes/no question is **"cut after this character?"**, never **"include this character?"**. Every character always stays.

* **The three wrong versions, and what each taught:**
    1. **Include / exclude (the Subsets tree).** Excluding a character **throws it away**: `"abcd"` minus `b` and `d` is `"ac"`. That's a subsequence, not a partition. Lesson: in a partition, nothing is ever skipped. The only decision is where the boundaries go.
    2. **Two calls with a shared `tempString` + `isPalindrome` at the top + `tempVector.clear()` in the base case.** Three bugs: (a) `ab` was rejected before it could grow into `aba`, so a growing piece may be non-palindromic *until* it's sealed; (b) the second call popped `s[i]` and moved on, which threw the character away again; (c) `clear()` deleted pieces that the calls above still owned. Lesson: never clear shared state. Pop exactly what you pushed.
    3. **The loop, but driven by a shared `tempString` and the parameter `i` itself.** Output: `"aab" → [b]`, `"aba" → [a,aa] [a]`. The piece never **grew** inside the loop (push `s[i]`, then pop it at the end of the iteration, so it tried `a`, `a`, `b` from three different positions), and the child **inherited** the parent's piece (shared by `&`, so level 2's first piece was `a` + `a`). Root cause: **`i` was doing two jobs**, "where this call's tape starts" and "where the piece ends". Incrementing it lost the start. *Fix:* a fixed `start` parameter, a separate `end` loop variable, and the piece computed from the two indices, with no `tempString` at all.

* **The C++ detail that cost time: `string_view` → `string`.**
    * **`string_view::substr` returns another `string_view`**: a pointer + length into the same buffer, so no copy and no allocation. (`std::string::substr` returns a new `std::string`. Same name, different cost.)
    * **Converting needs an explicit construction:** `std::string piece(view);`, or `tempVector.emplace_back(view)`, which builds the string in place inside the vector. There is **no implicit conversion** from `string_view` to `string`. That's deliberate: a hidden allocation would be easy to miss, so the language makes you write it.
    * The reverse direction is implicit and free: a `std::string` passed to `isPalindrome(std::string_view)` just becomes a view.

* **Code notes on `ca670dd`:**
    * **A `std::string` is built for every candidate piece, before the palindrome check**, and `push_back(tempString)` copies it a second time. Check the **view** first, and construct only on success (`emplace_back(piece)`, or `push_back(std::move(tempString))`). Measured: on `"abaabbabaababbab"`, **0.036 → 0.028 ms**. On 16 `a`s (32,768 partitions, every piece valid), there's no difference beyond noise: the run order alone swung timings between 4 and 7 ms. **Why the copy is cheap here:** libstdc++'s **small-string optimization** stores up to **15 characters inside the `string` object itself** (checked: an empty `std::string` reports capacity 15). LeetCode caps `s` at 16 characters, so almost every piece is a stack copy, not a heap allocation. With long pieces, building before checking would allocate for every rejected piece.
    * **`std::string tempString;` in `partition` is unused.** It's left over from version 3. Delete it.
    * `isPalindrome(std::string_view)` with two indices moving inward: right, and no allocation. `int` + `std::ssize` keeps `right = −1` safe for an empty view.

* **Follow-up (working tree):** the palindrome check now runs on the **view**, and the `std::string` is built only for pieces that pass. The unused `tempString` is gone. Still 0 mismatches, with clean sanitizers. Two polish points: the same `s.substr(startIdx, i − startIdx + 1)` is written twice, so name it once (`auto piece = s.substr(…);`, which is a free view). And `push_back(std::string(piece))` builds a temporary and then moves it in, while `emplace_back(piece)` builds it directly in the vector's slot. Both are tiny, but the named view is the readability win.
* **The known follow-up: precompute every palindrome check.** The same `s[i … j]` gets checked again in different branches. A table `isPal[i][j] = s[i] == s[j] && (j − i < 2 || isPal[i+1][j−1])`, filled once in O(n²) (rows from the bottom up, so `isPal[i+1]` is ready), turns each check into a lookup. Measured at n ≤ 16: **no gain** (0.026 ms with or without it), because the output dominates. It matters when n is larger, and it's the first taste of 2-D DP (topic 15), so say it in an interview, but it doesn't change anything here.

* **Time & Space Complexity:** up to **2ⁿ⁻¹** partitions (all of them, for `"aaa…a"`). Each one costs O(n) to check its pieces and O(n) to copy, so **O(n · 2ⁿ)**. **O(n)** auxiliary (depth ≤ n + `tempVector`), plus the output.

### [9] Letter Combinations of a Phone Number

> First pass `47166bc` (committed as "brute force", but it's already the right complexity: there are 3–4ⁿ answers and nothing can beat the output size). I wrote the **iterative cascade** on my own, the version I blanked on for Subsets `[2]`. Three questions came out of it: how to `emplace_back` a string "plus one more character", whether a fixed array can replace the map, and where backtracking even comes in. All three are answered below. Verified against two independent versions on 2,000 random inputs: 0 mismatches. Empty input returns `[]`.

* **The Core Pattern:** a product of choices. Each digit offers 3–4 letters, and an answer picks one letter per digit, **in order**. That's a tree with one level per digit and one branch per letter. There are two ways to walk it:
    * **Cascade, level by level (what I wrote):** start with `[""]`. For each digit, extend **every** string so far with **each** of its letters, building a **new** vector, then swap it in. `"23"`: `[a,b,c]` → `[ad,ae,af,bd,…,cf]`.
    * **Backtracking, path by path:** one `string` path. Level `i` loops over the letters of `digits[i]`: push, recurse to `i + 1`, pop. At `i == n`, record. It's the Permutations `[5]` loop shape, but without a `used` record, since the choices at each level come from the digit.

* **"Where does backtracking come in?"** It's the **same tree, walked in a different order**. The cascade is **breadth-first**: it finishes a whole level before starting the next, so it holds every partial string of that level at once (3–4ⁱ strings at level `i`, plus the next level while it's being built). Backtracking is **depth-first**: it walks one complete path at a time, so it holds **one** string of length ≤ n. Here both end up storing all 4ⁿ answers, so the memory difference is hidden by the output. It matters when you only need to *stream* or *count* the answers. **This BFS-vs-DFS distinction is the core of the next topic (Graphs):** a queue walks level by level, recursion or a stack walks path by path.

* **My edge-case handling was right:** the `if (resultVector.empty()) push_back("")` sits **inside** the loop, so it only seeds on the first digit, and `""` input never enters the loop, so it returns `[]`. Seeding `{""}` *before* the loop would have returned `[""]`, which is the trap LeetCode tests. (An explicit `if (digits.empty()) return {};` at the top says the same thing more obviously.)

* **Code notes on `47166bc`:**
    * **`for (const auto str : resultVector)` and `for (const auto val : …)` copy every string, on every iteration.** `-Wall -Wextra` flags both (`-Wrange-loop-construct: loop variable 'str' creates a copy`). Use `const auto&`. **Habit:** in a range-for over non-trivial elements, write `const auto&` unless you mean to copy. This was the biggest cost in the function.
    * **"There's no string constructor that appends."** Right, but `emplace_back` **returns a reference to the new element** (since C++17), so `tempVector.emplace_back(str).push_back(ch);` copy-constructs the prefix **in place** and then appends one character to it. That's exactly the "construct plus append" I was after, with no temporary.
    * **`vector<string>` values to avoid converting `char` → `string`:** not needed. `std::string + char` is a built-in overload (`str + 'a'` works), and `push_back(ch)` appends a `char` directly. So the value can be one string per digit (`"abc"`), iterated character by character. That also replaces 26 tiny heap-free strings, plus a vector for each, with 8 strings.
    * **`digitMap[c]` runs inside the loops.** Look it up once per digit (`const auto& letters = …`). And the whole `unordered_map` is rebuilt on every call: 8 hash nodes plus their vectors.
    * **`tempVector.reserve(...)`:** right. The size of the next level is known exactly.

* **The fixed-array question ("is this the packing you meant?"):** my `array<uint8_t, 26>` idea maps **letter → digit**, the **reverse** direction. To use it you'd have to search for where a digit's letters begin, which is the traversal I worried about. Flip it: map **digit → letters**. The keys `'2'…'9'` are 8 consecutive characters, so `c − '2'` is an index from 0 to 7, and the table is just `std::array<std::string_view, 8> = {"abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"}`. Then `table[c − '2']` gives `"abc"` directly: **one subtraction and one load, no hashing, no search, no allocation**. Make it `static constexpr` and it lives in read-only data, built at compile time. It's the same trick as Task Scheduler's `count[task − 'A']`, and the same as an embedded lookup table in flash indexed by an ADC channel number.

* **Follow-up (working tree): the rewrite.** `static constexpr std::array<std::string_view, 8>`, `const auto&` over the strings, `emplace_back(str).push_back(val)`, and `reserve`. Still 0 mismatches. Two notes from it:
    * **The index bug:** I first wrote `c − '0' − 1`. `'2'` must map to slot 0, so it's `c − '0' − 2`, which is just **`c − '2'`**: one subtraction, and it says what it means. *Self-check for any offset table:* plug in the first key (`'2'` → 0) and the last (`'9'` → 7).
    * **Keeping `const auto val` by value for the `char`s: right call.** A `char` is exactly **1 byte** (`sizeof(char) == 1` by definition). A reference, if it isn't optimized away, is an address (**8 bytes** on x86-64). The rule is **by value for small, trivially copyable types** (`char`, `int`, pointers, `string_view`), and **`const&` for anything that owns memory** (`string`, `vector`). At `-O2` the compiler generated the **same loop** for both versions here (checked: identical instructions, only label names differ), because it reads the element straight from memory either way. So the choice is about intent, and by value states it better.
    * Still open: `digitMap[c − '0' − 2]` is computed twice (in `reserve` and in the loop). Name it once: `const std::string_view letters = digitMap[c − '2'];` (16 bytes, by value).
* **The recursive version I wrote (kept commented out in the file): two calls instead of a loop.** Call 1 `(i + 1, 0)` = "this letter is chosen, go to the next digit". Call 2 `(i, j + 1)` = "try the next letter of this digit". That's the `for` loop over letters, written as recursion. Two bugs on the way: (1) recording after the first call returned, at every level, which recorded the partial strings `a`, `b`, `c`; (2) passing `j` instead of `0` to the next digit, so after choosing `b` (j = 1), digit 3 started at `e` and `bd`, `cd`, `ce` vanished. Final form: record right after the push when `tempString.size() == s.size()`. A side effect is that `""` input never records anything and returns `[]` with no extra guard. Verified against the iterative version on 2,000 inputs: 0 mismatches.
    * **Is it optimal?** Same big-O, **about 2× the calls** of the loop version. Every letter costs two calls, plus a dead call past the last letter (`j >= size`) and another past the last digit. Measured per run: **681 vs 341 calls** for `7979`, **174,761 vs 87,381** for `79797979`, and ~1.5× the time. It also carries an extra `j` parameter and recursion depth up to about 5n instead of n. Correct, just more machinery than the loop.
* **The loop-based version, written after the two-call one:** one call per digit (`i`), one `for` over that digit's letters, push → recurse `i + 1` → pop, recording right after the push once the string is full length (so `""` returns `[]` with no guard). 0 mismatches on 2,000 inputs, and **341 / 87,381 calls**: half of the two-call version, exactly one call per tree node. **The wrong turn on the way:** I first wrapped it in an outer `for` over the digits. Its increment was `i++` while the condition tested `currentIdx`, so it never terminated. Even fixed, it's the wrong shape. An outer "which digit next?" loop is Subsets' `start` loop, but here every answer uses **every digit in order**, so there's no digit choice to make. **The depth *is* the digit.** (Paths that skipped a digit could never reach full length, so the output would have been right, but every one of them was wasted work.)
    * **Final textbook form:** record in the base case (`i >= s.size()` → push, return), with `if (!digits.empty())` in the caller so `""` returns `[]`. Same 341 / 87,381 calls, 0 mismatches. **This is the one to present.**
* **Which to use in an interview:** the **loop-based backtracking** (level `i` = digit, `for (char ch : letters) { push; recurse(i + 1); pop; }`, record at `i == n`, plus an empty-input guard). It's the topic's standard template, so the interviewer recognizes it instantly. It's one decision per level, and "each digit starts at its first letter" can't go wrong. Offer the **iterative cascade** as the follow-up ("the same tree, breadth-first; no recursion"). Keep the two-call version as understanding, not as the one to present.
* **Measured** (`79797979`, 65,536 answers; this machine is noisy, so compare ratios): **mine ~0.9–1.6 ms**, the cascade with `const&` + array + `emplace_back(...).push_back(ch)` **~0.4–0.7 ms** (about 2.3×). Backtracking 2.4 ms **without** reserving its result vector, and ~0.9 ms with it. Here both walks cost about the same once the output is pre-sized. At LeetCode's limit (n ≤ 4, at most 256 answers), every version runs in a few microseconds.

* **Time & Space Complexity:** **O(n · 4ⁿ)** time (up to 4ⁿ answers, each of length n, built once). Space: the output is **O(n · 4ⁿ)**. The cascade's peak extra memory is the previous level, **O(n · 4ⁿ⁻¹)**. Backtracking's extra memory is **O(n)** (depth + one path).

### [10] N-Queens

> The last Backtracking problem, and the first Hard of the topic. **I designed it myself** after one round of nudges, and committed the first pass to the working tree. My first validity checker assumed a full board. Once the design was "check each queen as it's placed", I trimmed it down to what can actually conflict. A chain of small bugs followed (below), each one found and fixed by me. Verified for n = 1…9: exactly 1, 0, 0, 2, 10, 4, 40, 92, 352 boards, every one valid (no shared row, column or diagonal), and identical to an independent implementation.

* **The Core Pattern:** constraint building (§2 Pattern 4). **One queen per row, and the depth is the row.** Level `r` tries each column: if the square is safe, place, recurse to `r + 1`, remove. A full board (`r == n`) is a solution, because every queen was checked when it was placed.

* **Trimming the checker (my reasoning, and it's right):**
    * **No row check:** each level places exactly one queen in its own row, so two queens can never share a row.
    * **No downward checks:** rows below `r` are empty when row `r` is being placed. Only the column **above**, the up-left diagonal and the up-right diagonal can hold a queen.
    * **The row isn't on the board yet:** I append the row only *after* the check passes, so during the check `board.size() == rowIdx`, and the checker must never index `board[rowIdx]`. All three scans start at `rowIdx − 1`.

* **The diagonal loops:** the comma operator lets one `for` carry two variables: `for (int i = r − 1, j = c − 1; i >= 0 && j >= 0; i--, j--)`. Getting the start and stop right for both diagonals only resolved **on paper**. Up-left stops when the row **or** the column hits 0. Up-right stops when the row hits 0 or the column hits the **width**.

* **The bugs, in order:**
    1. **Up-right bound `j < board.size()`:** my board grows **row by row**, so `board.size()` is the number of rows placed so far, not the width. The two only agree on a full square board. *Fix:* `board[0].size()`. Is that safe? Yes: `&&` evaluates left to right and stops early, so `i >= 0` (i.e. `rowIdx ≥ 1`) is checked first, which guarantees at least one row exists. **Cleaner:** pass `n` and test `j < n`. That needs no reasoning about short-circuiting, and it also removes the `-Wsign-compare` warning (`int` vs `size_t`) the current line triggers.
    2. **Result type `vector<vector<vector<string>>>`:** one board is a `vector<string>` (one string per row), so the result, a list of boards, is `vector<vector<string>>`. *Self-check:* write down the type of **one** answer first, then wrap it in one `vector`.
    3. **Next row started at `colIdx` instead of 0:** a queen in row `r + 1` could then never sit in a column to the **left** of row `r`'s queen. **This is the same bug as Letter Combinations `[9]`** (passing `j` instead of `0` to the next digit). The rule, twice now: **a new level starts its own choices from scratch.** Only state that genuinely carries over (the board, the path) gets passed down.
    4. *(C++ detail)* `std::string(n, '.')` is the "n copies of a char" constructor. `currentBoard.emplace_back(n, '.')` builds the row **in place** inside the vector from those same arguments, with no temporary, then `back()[colIdx] = 'Q'`. With n ≤ 9 the row fits in the small-string buffer (≤ 15 characters, `[8]`), so each row costs no heap allocation.

* **Version 2 (working tree): a loop over columns.** The `colIdx + 1` second call became `for (c = 0; c < n; ++c)`: if safe, push the row with `'Q'` at `c`, recurse `r + 1`, pop. One parameter fewer, and each row's loop starts at column 0 by construction (bug 3 can't happen any more). Correct for n = 1…9. Two leftovers: `size_t i < n` compares unsigned with `int` (`-Wsign-compare`; make `i` an `int`), and `isQueenPositionValid(board, rowIdx, i)` now passes `size_t` into an `int` parameter.

* **The search, traced on n = 4** (what "backtrack" actually does here):
    * Row 0 → col 0. Row 1: col 0 ✗ (column), col 1 ✗ (diagonal), **col 2** ✔.
    * Row 2: every column is attacked → **dead end**. Return to row 1, remove its queen, and its loop moves on to **col 3** ✔.
    * Row 2 → col 1 ✔. Row 3: all ✗ → back to row 2, nothing else fits → back to row 1, out of columns → back to **row 0**, whose loop moves to **col 1**.
    * Row 1 → col 3, row 2 → col 0, row 3 → col 2 → **first solution** `.Q.. / ...Q / Q... / ..Q.`
    * **Mental model:** a row that runs out of columns just returns. The row above then slides its queen one column right and tries again. Nothing else needs undoing beyond popping that one row.

* **Measured, all three versions:**
    | Version | n = 9 calls | n = 9 time | n = 12 calls | n = 12 time |
    | -- | -- | -- | -- | -- |
    | v1: two calls (`r + 1` / `c + 1`), O(n) scan per check | 80,772 | 0.86 ms | 10,960,057 | 112 ms |
    | **v2: loop over columns, O(n) scan per check** | **8,394** | **~1 ms** | **856,189** | **~117 ms** |
    | loop over columns + **line flags**, O(1) per check | 8,394 | 0.28 ms | 856,189 | 41 ms |
    * **The surprise:** the loop cut calls **10×**, but the time **didn't move**. So the calls were never the bottleneck. **The scanning is:** every square tried walks up to three lines of the board. With the call count now minimal, the remaining cost is the O(n) check, and only a faster check removes it. *Lesson:* measure before assuming which part is slow. Fewer calls felt like the fix, and it wasn't.

* **Three ways to answer "is this square attacked?", and where each pays:**
    | Approach | Check a square | Place / remove a queen |
    | -- | -- | -- |
    | Scan the board (v1, v2) | O(n): walk up to 3 lines | O(1) |
    | **Count per cell** (how many queens attack it) | O(1): one lookup | O(n): +1 / −1 on every cell the queen attacks, ~3n cells |
    | **Flag per line** | **O(1): 3 lookups** | **O(1): 3 writes** |

    My "temp board of invalid cells" idea fails with plain true/false marks, because **cells overlap**: one cell can be attacked by two queens, so un-marking it for one queen wrongly frees it from the other. **Counts** fix that (it's reference counting), but they move the cost to placing. Checks are far more frequent than placements (up to n checks per row), so counts beat scanning only somewhat. **Flags per line** make both operations O(1).

* **Line flags from scratch (the approach to implement):** a queen doesn't really attack *cells*. It attacks **three lines**: its column, its `\` diagonal and its `/` diagonal. Give each line a number and one flag.
    * **Columns:** column `c` is line `c`, giving `n` flags.
    * **`\` diagonals:** write `r − c` in every cell (n = 4):
      ```text
       0 -1 -2 -3
       1  0 -1 -2
       2  1  0 -1
       3  2  1  0
      ```
      Each `\` diagonal carries **one number all the way along**. The range is −(n−1) … n−1, so add `n − 1` to index it, giving **2n − 1** flags.
    * **`/` diagonals:** write `r + c`:
      ```text
       0 1 2 3
       1 2 3 4
       2 3 4 5
       3 4 5 6
      ```
      Again one number per diagonal, already 0 … 2n − 2, giving **2n − 1** flags.
    * **Safe check:** `!col[c] && !diagDown[r − c + n − 1] && !diagUp[r + c]`. **Place:** set those three (and `'Q'`). **Remove:** clear the same three (and `'.'`).
    * **Check it against the trace:** the queen at (1, 2) has `r + c = 3`. Square (2, 1) also has `r + c = 3`, so it's attacked, which is exactly why row 2, col 1 failed above.
    * **Why clearing is safe for lines but not for cells:** on a valid board, no two queens share a column or a diagonal. So each set flag belongs to **exactly one** queen, and clearing it on the way back can't remove another queen's protection.
    * **What changes in the code:** three `vector<bool>` (or `std::array<bool, …>`) shared by `&` alongside the board. They're set and cleared in the same choose/un-choose pair as `'Q'`/`'.'`, and `isQueenPositionValid` disappears.
    * **The embedded version, after that works:** n ≤ 9, so each set of flags fits in one integer. Use three bitmasks passed **by value** with `|`, so there's nothing to undo (Permutations `[5]`).

* **Version 3 (working tree): line flags. This is the optimal approach.** Three `vector<bool>`s of sizes `n`, `2n − 1`, `2n − 1`, set and cleared in the same choose/un-choose pair as the row, and the checker became one expression of 3 lookups. Correct for n = 1…9. **n = 12: 117 ms → 71 ms**, the same 856,189 calls. The O(n) scan is gone. It took two passes on the explanation: the click was that these are **1-D lists with one bool per *line***, not three 2-D boards (that would be marking every square, i.e. the counting approach again), and that the 2n − 1 comes from counting where the diagonals *start* (n on the top row + n down the left column − 1 shared corner).
    * **What's left between 71 ms and the fastest version (measured, n = 12):**
        | Change on top of v3 | Time |
        | -- | -- |
        | v3 as written | 71 ms |
        | + **prebuilt board** (`n` rows of `'.'` made once; set `'Q'`, restore `'.'`, instead of `emplace_back` + `pop_back` of a fresh row per placement) | 63 ms |
        | + **`vector<char>` instead of `vector<bool>`** | **43 ms** |
        | three `unsigned` **bitmasks** passed by value | **24–27 ms** |
    * **`vector<bool>` is the surprise:** it's a **specialization** that packs 8 flags per byte. `operator[]` returns a **proxy object**, not a `bool&`, and every write is a read-modify-write: load the word, mask the bit, store it back. That's the same cost as setting one bit in a shared GPIO or status register. It saves memory nobody needs here (at most 17 flags), at about 30% of the runtime. It's also famous for not being a real container (`bool* p = &v[0]` doesn't compile). Use `vector<char>`, `std::array<bool, N>` or a bitmask when speed matters.
    * **Naming:** `diagonalLeftUsed` is indexed by `r + c` (the `/` lines) and `diagonalRightUsed` by `r − c + n − 1` (the `\` lines). "Left" and "right" don't say which. Name them by the formula (`sumDiagUsed`, `diffDiagUsed`) so the index and the name check each other. Pass `n` instead of recomputing `std::ssize(colUsed)`.
    * **Final (working tree):** prebuilt board (`'Q'` / `'.'` in place), stack `std::array<bool, 9>` and two `std::array<bool, 17>` sized for the **maximum** n (`n` is a runtime value, so `std::array<bool, n>` can't compile; the constraint `n ≤ 9` gives the bound), and the indices computed once per square. n = 9: **0.29 ms** (v3 was 0.71 ms), the same as the reference line-flag version, and clean under ASan/UBSan for n = 1…9.
        * **The bound is a real contract, and the sanitizer proved it:** my benchmark ran this at n = 12, and ASan reported a **stack-buffer-overflow** on `colUsed`. Nothing in the code stops `n > 9`, so add `assert(n <= 9);` at the top of `solveNQueens`. A fixed-size buffer needs its limit checked at the entry point, just like a fixed DMA buffer in embedded code.
        * **`rowIdx − i + std::ssize(colUsed) − 1` uses an offset of 8, not `n − 1`.** It's still correct (each `\` diagonal still gets a unique slot, and the maximum index is 16, inside 17), but only because the arrays are sized for n = 9. With arrays sized `2n − 1`, the same line would overflow. Write `n − 1` so the formula matches the derivation.
        * `currentBoard.reserve(n)` is now redundant, since the board is constructed with `n` rows. The `diagonalLeftUsed` / `diagonalRightUsed` names still don't say which is `r + c` and which is `r − c`.
    * **Bitmask, when you want it:** three `unsigned` masks passed **by value**. Check with `(cols >> c) & 1` etc., and recurse with `cols | (1u << c)`, `diff | (1u << (r − c + n − 1))`, `sum | (1u << (r + c))`. Nothing to undo. With 2n − 1 ≤ 17 bits for n ≤ 9, a 32-bit `unsigned` is plenty.

* **Time & Space Complexity:** row `r` has at most `n − r` free columns, so the search is bounded by **O(n!)** placements. Each check is **O(n)** with scanning (so O(n · n!), measured as the dominant cost) or **O(1)** with flags. Each solution is copied as n strings of n characters, O(n²) per solution. Space: **O(n²)** for the board (n rows of n), O(n) for the flags and recursion depth, plus the output.

---

**Topic complete (10 / 10).** The patterns all came from one tree: take/skip (Subsets, Combination Sum I/II, Subsets II), pick any unused (Permutations), loop from `start` (Palindrome Partitioning), one level per input position (Letter Combinations, N-Queens), and gates/pruning before the recursive call (Generate Parentheses, Combination Sum, N-Queens). Two rules came back again and again: **every choose has an exact un-choose**, and **a new level starts its own choices from scratch.**
