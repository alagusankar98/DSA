# 2-D Dynamic Programming - Topic Guide

## 1. Core Fundamentals

1-D DP (topic 14) stored one answer per **one number**: a position, an amount, a prefix length. **2-D DP stores one answer per *pair* of numbers.** You've already written two of these without calling them 2-D: LIS's memo was `f(i, prev)` and Partition's table was `dp[i][remaining]`. Both had a row per element and a column per "what I'm carrying", and both were filled row by row from a base row. That's the whole topic. What changes is **what the two axes mean**, and from that, **which neighbouring cells a cell reads** and therefore **the order you fill them in**.

The second index usually comes from one of four places:
* **Two inputs walked together:** `(i, j)` = a position in string `a` and a position in string `b`.
* **A grid:** `(r, c)` = a cell.
* **A position plus a small mode:** `(i, state)`, where `state` is something like "holding / not holding". A tiny second axis, often 2 or 3 values.
* **An interval of one input:** `(l, r)` = the sub-array from `l` to `r`. The answer for a range comes from smaller ranges inside it.

### The Vocabulary
* **State `(i, j)`:** the subproblem named by two numbers. **`dp[i][j]` is the answer to the subproblem named `(i, j)`**, and you still have to say what it means in one English sentence before writing anything.
* **Prefix framing:** `dp[i][j]` is about `a[0..i)` and `b[0..j)`, the first `i` and first `j` characters. Base cases sit at `i = 0` / `j = 0` (an **empty** prefix), and the answer is at `dp[m][n]`.
* **Suffix framing:** `dp[i][j]` is about `a[i..m)` and `b[j..n)`. Base cases at `i = m` / `j = n`, answer at `dp[0][0]`. This is what your top-down recursions have produced so far (Word Break, LIS, Partition: "from `i` onward"). Both framings work; **base case, loop direction and answer location must all come from the same one** (the Word Break lesson).
* **The empty row and column:** with two strings of lengths `m` and `n`, the table is **`(m + 1) × (n + 1)`**. The extra row and column are "one of the strings is empty", and they hold the base cases. So cell `(i, j)` compares characters **`a[i − 1]` and `b[j − 1]`**: the table index is one ahead of the string index. This is LIS's `prev + 1` shift again, on both axes.
* **Dependency (which cells a cell reads):** for two strings, a cell typically reads **up** `(i−1, j)`, **left** `(i, j−1)` and **up-left (diagonal)** `(i−1, j−1)`. The fill order is whatever makes every one of those already filled.
* **Interval DP:** `dp[l][r]` = the answer for the range `l..r`, built from strictly smaller ranges inside it. Filled **by length** (all ranges of length 1, then 2, …), not by row.
* **State machine DP:** `dp[i][s]` = the best value at step `i` while in mode `s`. The recurrence is a table of allowed transitions between modes, exactly like a hardware FSM's next-state logic.
* **Counting vs optimising vs deciding:** the same table shape answers "how many ways" (`+`), "best value" (`max` / `min`) or "is it possible" (`||`). Choose the combining operator from the question, as in Partition (`||` instead of LIS's `max`).

### The Four Questions, 2-D Edition (ask them before writing any code)
1. **What do `i` and `j` each mean, and what does `dp[i][j]` mean, in one sentence?** "The fewest edits to turn the first `i` chars of `a` into the first `j` chars of `b`." If one axis has no clear meaning, the state is wrong.
2. **What's the last decision?** For two strings it's almost always about the **last characters**: do they match? Use both, drop one from `a`, drop one from `b`. For a grid: which neighbour did I arrive from? For an interval: which element is handled **last** inside the range?
3. **What are the base cases?** The empty row and column (one string empty), the first row and column of a grid, ranges of length 0 or 1.
4. **In what order, and where's the answer?** Draw the arrows from a cell to the cells it reads. Fill so that every arrow points at a filled cell. The answer is the top-level call's arguments: `dp[m][n]` (prefix), `dp[0][0]` (suffix), `dp[0][n−1]` (interval), or the max over all cells.

### Reading the Dependency Picture
For a two-string table in prefix framing, a cell reads three neighbours:

```
            j−1        j
   i−1  [ diagonal ] [  up  ]
   i    [  left    ] [ (i,j) ]
```

All three are **above or to the left**, so a plain row-major fill (`i` increasing, `j` increasing inside it) is always valid. Suffix framing mirrors it: the cell reads below and to the right, so fill `i` and `j` decreasing. **If you can't draw this picture for a problem, you don't yet know the loop order.** Interval DP is the exception: `dp[l][r]` reads ranges *inside* `[l, r]`, which are shorter, so the outer loop is the range length.

### Built from 1-D (fold these in first)
* **LIS `[11]` and Partition `[12]` in `14-1D-Dynamic-Programming/README.md`** are the template: recursion → memo → table → fewer rows, with a two-part state. The tabulation lessons carry over unchanged: **a table fills every state**, each recursion parameter becomes a loop, and a variable carried across iterations means you're walking one path.
* **The recurring bookkeeping bugs** (the checklist under §1 "The Problems" in the 1-D README): skip is always legal, width = what the columns mean, shifts only inside `[]`, answer location, memo by reference, rolling rows. In 2-D they all come back, now on **two** axes at once.
* **Coin Change `[8]` and Partition `[12]`: loop direction = how many times an item may be used.** Downward (or two rows) for once, upward for unlimited. Counting problems add a second rule: whether the **items** or the **amounts** are the outer loop decides whether you count combinations or orderings.
* **Grid recursion from Graphs (topic 12):** a DFS over cells with a memo is DP on a grid. When the moves can never return to a cell (for example, only to strictly larger values), the grid is a **DAG**, so no visited set is needed and each cell's answer is computed once.
* **Bellman-Ford in Advanced Graphs `[6]`:** `dp[r][v]` with `prev`/`current` rows was a 2-D table rolled to two rows, the same move as Partition's two rows.

### Rolling a 2-D Table Down to One Row
If row `i` only reads row `i − 1`, keep two rows, or one row updated in place. In place, a cell holds the **old** row's value until you overwrite it:
* **Up** `(i−1, j)` is the cell itself before you write it. Free.
* **Left** `(i, j−1)` was already overwritten this pass, so it's the **new** row's value. That's what you want for "left".
* **Diagonal** `(i−1, j−1)` is the catch: it was overwritten one step ago. **Save it in a variable before overwriting each cell** (`diag = row[j]` before the write, used by the next `j`). Forgetting this is the classic one-row bug.
* **Two rows with `swap`:** every cell of the new row must be written by this row (Partition Stumble 4).
* **Which axis to keep:** roll along the longer one and keep a row the length of the **shorter** input. O(min(m, n)) space.

### Complexity Cheat Sheet
| Problem shape | Time | Space (after rolling) |
| -- | -- | -- |
| Grid paths, m × n | O(m · n) | O(n), one row |
| Two strings, m × n (compare / align / match) | O(m · n) | O(min(m, n)), one row + a saved diagonal |
| Counting knapsack, k items × target T | O(k · T) | O(T) |
| Position + small mode (state machine) | O(n · S) | O(S), often O(1) |
| DFS + memo on a grid DAG | O(m · n) | O(m · n) memo + recursion depth |
| Interval DP, n elements | O(n³) (n² ranges × n split points) | O(n²) |

### The Hardware Reality
* **Row-major fill is a sequential walk.** `dp[i][j]` with `j` innermost walks memory in order, which is the prefetcher's best case. Swapping the loops (`i` innermost) strides by a whole row each step. On a 1000-wide `int` table that's a 4 KB jump per access, so every access is a new cache line and often a new page.
* **Table size is real memory.** 1000 × 1000 `int` = **4 MB**: past L2, into L3 or DRAM. One rolled row of 1000 `int` = 4 KB fits in L1. Rolling is a complexity win in space *and* a constant-factor win in time (Partition: 2 MB table → 10 KB row, 2.1×).
* **`vector<vector>` vs flat:** a single `vector` with `r * W + c` is one allocation and contiguous. Whether it's faster depends on the element type: no gain in LIS (`int`), ~1.5× in Partition (`uint8_t`, likely because a character-type store may alias the row pointers). Measure, don't assume.
* **Interval DP is cache-hostile.** `dp[l][k]` walks a row but `dp[k][r]` walks a **column**. At n = 300 the table is 360 KB of `int`, so the column walk misses constantly. It's O(n³) either way, but this is where the constant gets big.
* **Grid DFS + memo recursion depth:** a strictly increasing path can visit every cell, so a 200 × 200 grid can recurse **40,000 frames** deep. That's fine under Linux's 8 MB stack, and dead on an embedded stack of a few KB. A bottom-up order (sort cells by value, or a topological order) has no depth.
* **Counting overflows.** "How many ways" answers grow combinatorially. An answer that fits in 32 bits can still have **intermediate cells** that don't. Check the bound, and use `uint64_t` (or a modulus when the problem gives one) before you need it.
* **Anti-diagonals are independent.** Every cell on the same anti-diagonal (`i + j` constant) depends only on earlier anti-diagonals, so they can all be computed in parallel. That's how alignment DP runs on GPUs, FPGAs and **systolic arrays** (a grid of identical processing elements, each passing values to its neighbour per clock).

### Where This Shows Up in the Real World
`diff` and `git diff` (longest common subsequence, Myers' algorithm). Spell-checkers and fuzzy search (edit distance). DNA and protein alignment (Needleman–Wunsch, Smith–Waterman), often on FPGA or GPU accelerators. Regex and glob matchers built as DP or automata instead of exponential backtracking. **Viterbi decoding** is a 2-D table, time × encoder state, rolled to one column per time step. Dynamic time warping in speech and gesture matching. CYK parsing and matrix-chain ordering (interval DP). Trading and protocol logic modelled as state machines with a value per state.

---

## 2. General Summary / Quick Reference

### Pattern 1: Grid Paths → `dp[r][c]` from the cells you can arrive from
Each cell combines the neighbours that can step into it (usually up and left).
* *Discipline:* the first row and first column are base cases (only one way to reach them). Rolls to one row: `row[c]` is "up" before you write it, `row[c − 1]` is "left" after.

### Pattern 2: Two Sequences → `dp[i][j]` over prefixes, with an empty row and column
Decide on the **last characters** `a[i − 1]`, `b[j − 1]`: match (diagonal), or drop one from either side (up / left).
* *Discipline:* table is `(m + 1) × (n + 1)`, and cell `(i, j)` reads characters `i − 1` and `j − 1`. Fill the empty row and column first and say what each means ("turning `a[0..i)` into an empty string takes `i` deletions"). One row needs a saved diagonal.

### Pattern 3: Counting Knapsack → count ways to reach a target
Partition's table, but cells hold counts (`+`) instead of `||`.
* *Discipline:* items outer, amounts inner counts **combinations**; amounts outer counts **orderings**. Unlimited use → amounts upward; once each → downward. Reduce first if the problem allows (an assignment of `+`/`−` signs is a subset choice). Watch for negative indices: shift by an offset, or reduce to a non-negative target.

### Pattern 4: Position + Mode → a small state machine per step
`dp[i][s]` = best value at step `i` in mode `s`. Write the allowed transitions first, as a table or a diagram, then the code.
* *Discipline:* every mode gets a base value at step 0, including "impossible" ones (a sentinel that survives `+` and `−`). Usually rolls to one variable per mode, updated from the **previous** step's values (copy them first, or the order of updates leaks today's values into today).

### Pattern 5: DFS + Memo on a Grid DAG
When moves are forced to be strictly increasing (or otherwise can't cycle), `memo[r][c]` = the best path starting at `(r, c)`, computed once per cell.
* *Discipline:* no visited set: the ordering already rules out cycles. Run it from **every** cell, and the answer is the max over all. Mind the recursion depth on large grids.

### Pattern 6: Interval DP → `dp[l][r]` from smaller ranges inside
Pick the element (or split point) handled **last** inside `[l, r]`. Its cost then depends only on the range boundaries, and the two sides become independent subproblems.
* *Discipline:* "last", not "first": choosing the first element leaves neighbours that change as the range shrinks. Pad the boundaries with neutral values if the problem's edges need them. Outer loop = range length.

### The Non-Negotiable Discipline
* **Say what each axis means, then what `dp[i][j]` means, in one sentence each.**
* **Draw the dependency arrows** (up / left / diagonal, or inside the range) before choosing the loop order.
* **Size = `(m + 1) × (n + 1)` for prefixes,** fill the empty row and column first, and index the strings at `i − 1` / `j − 1`.
* **A table fills every state.** Recursion parameters become loops; no variable carries a single path.
* **Answer location** = the top-level call's arguments.
* **Roll last, and only once it passes:** two rows first, then one row with a saved diagonal. With two rows, every cell is written each row.
* **Counting? Check overflow** of the intermediate cells, not just the final answer.

---

## 3. Problem Strategies & Patterns
