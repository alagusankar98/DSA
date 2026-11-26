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

### Memo → Table: The Translation (do it, don't intuit it)
**Writing the table by translating your memo is the standard method, not a crutch.** People who seem to write tables directly are running this translation in their heads. Write the memo first, then fill in these five rows for the problem, then write the loops.

| In the memo | In the table |
|---|---|
| **1. Parameters** `(i, j)` | Table indices `dp[i][j]`, one loop per parameter. Size = every value a parameter can take, **including the base-case values** (`i == n1` exists, so `n1 + 1` rows). |
| **2. Base case** (`if (i == n1 \|\| j == n2) return 0`) | The table's edge cells. If the base value is 0, a zero-filled table already has it; otherwise seed those cells **before** the loops. |
| **3. Recursive calls** (`f(i+1, j)`, `f(i, j+1)`, `f(i+1, j+1)`) | **Reads** of cells that must already be filled when `(i, j)` is computed. The body's `if`s, `max`es and gates carry over word for word. |
| **4. Top-level call** `f(0, 0)` | Where the answer is: `return dp[0][0]`. |
| **5. Loop direction** | Follows from row 3, and it's the **only** decision. Calls go to `i + 1` → `i` loops **downward**. Calls go to `i − 1` → upward. Do this per index. |

**The picture: a table is the memo filled in before anyone asks.** The recursion starts at `f(0, 0)`, dives to the base cases, and fills the memo on the way back **up**. The table skips the dive. It starts at the base cases and fills cells in the order the recursion would have **returned** them. Same cells, same formula, same completion order; it just never makes a call.

**Worked through on LCS `[2]`** (memo `1b48e68` → table `9e62f2e`):
1. `f(i, j)`, `i ∈ [0, n1]`, `j ∈ [0, n2]` → `dp` is `(n1 + 1) × (n2 + 1)`.
2. Either suffix empty → 0 → row `n1` and column `n2` are 0. Zero-filled table, nothing to seed.
3. Calls: `(i+1, j+1)`, `(i+1, j)`, `(i, j+1)`: below, diagonal and right.
4. `f(0, 0)` → `dp[0][0]`.
5. Every call increases `i` or `j` → both loops run **downward** (`n1 − 1 → 0`, `n2 − 1 → 0`).

**What changes on the way over (every one of these has bitten me):**
* **The `-1` sentinel and the `if (dp[i][j] != -1)` check disappear.** The loop order guarantees every cell read is already written (Fibonacci's "loop order replaces the check").
* **The early `return` for the base case disappears too.** It becomes the edge cells, so the loops start just inside them (`n1 − 1`, not `n1`).
* **A table fills every state.** No variable carries "the current path" across iterations (LIS's single `prevIdx`), and a skip branch can't be dropped by starting a loop late (Partition's `left` loop starting at `nums[i]`).
* **Keep the memo's indexing.** Suffix memo → loops run right to left; prefix memo → left to right. Don't flip the indices to make the loops "look normal": that's how a seed ends up in the wrong cell (Unique Paths Stumbles 2 and 3).
* **The answer is the top-level call's cell**, not the last cell written and not the base (Partition's `return dp[0][0]` asked "can I make 0?").

Only once the table passes, roll it (next section).

### Rolling a 2-D Table Down to One Row
If row `i` only reads row `i − 1`, keep two rows, or one row updated in place. In place, a cell holds the **old** row's value until you overwrite it:
* **Up** `(i−1, j)` is the cell itself before you write it. Free.
* **Left** `(i, j−1)` was already overwritten this pass, so it's the **new** row's value. That's what you want for "left".
* **Diagonal** `(i−1, j−1)` is the catch: it was overwritten one step ago. **Save it in a variable before overwriting each cell** (`diag = row[j]` before the write, used by the next `j`). Forgetting this is the classic one-row bug.
* **Diagonal without left:** if a cell reads only up and the diagonal (no same-row neighbour), you don't need the saved variable: loop so the diagonal's index is visited **after** the cell, keeping it old (Distinct Subsequences `[8]`: suffix framing, `row[j + 1]` is the diagonal, so `j` runs **upward**).
* **The direction rule in one line:** for each cell you read in place, ask whether it should be the **old** row (visit it later) or the **new** row (visit it first). The same index can need either: `row[j + 1]` was *right* (new) in Interleaving `[6]` and *diagonal* (old) in Distinct Subsequences `[8]`.
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
* *Use for:* Unique Paths (one row: left-to-right, because "left" must already be this row's value).

### Pattern 2: Two Sequences → `dp[i][j]` over prefixes, with an empty row and column
Decide on the **last characters** `a[i − 1]`, `b[j − 1]`: match (diagonal), or drop one from either side (up / left).
* *Discipline:* table is `(m + 1) × (n + 1)`, and cell `(i, j)` reads characters `i − 1` and `j − 1`. Fill the empty row and column first and say what each means ("turning `a[0..i)` into an empty string takes `i` deletions"). One row needs a saved diagonal.
* *Use for:* Longest Common Subsequence (suffix framing: the extra row and column are "empty suffix → 0", so no seeding; match = `1 + diagonal`, never needs the skips).
* *Use for:* Interleaving String (`s3`'s index is `i + j`, not a third axis; only the corner is a base case, so the last row and column must be computed; no skip move, `||` of two guarded takes).
* *Use for:* Distinct Subsequences (counting with `+`: a match keeps the skip; base `j == n2 → 1` before `i == n1 → 0`; one row loops `j` **upward** because the take reads the diagonal).
* *Use for:* Edit Distance (base = characters left, `(n1 − i) + (n2 − j)`, so the edges are seeded but not constant; mismatch is `1 + min` of replace / delete / insert; one row is the case that needs the saved diagonal, since `row[j + 1]` is both right and diagonal).

### Pattern 3: Counting Knapsack → count ways to reach a target
Partition's table, but cells hold counts (`+`) instead of `||`.
* *Discipline:* items outer, amounts inner counts **combinations**; amounts outer counts **orderings**. Unlimited use → amounts upward; once each → downward. Reduce first if the problem allows (an assignment of `+`/`−` signs is a subset choice). Watch for negative indices: shift by an offset, or reduce to a non-negative target.
* *Use for:* Coin Change II (unlimited coins → one row, coins outer, amounts upward from the coin; `unsigned` cells because unreached amounts overflow while the answer fits).
* *Use for:* Target Sum (reduce signs to a subset: P = (S + target) / 2, odd → 0; then count subsets with one row looping **downward**, since each number is used once).

### Pattern 4: Position + Mode → a small state machine per step
`dp[i][s]` = best value at step `i` in mode `s`. Write the allowed transitions first, as a table or a diagram, then the code.
* *Discipline:* every mode gets a base value at step 0, including "impossible" ones (a sentinel that survives `+` and `−`). Usually rolls to one variable per mode, updated from the **previous** step's values (copy them first, or the order of updates leaks today's values into today).
* *Use for:* Best Time to Buy and Sell Stock with Cooldown (modes = holding / not holding; the cooldown is a `+2` jump, so the base is `i >= n` and the roll keeps a day-after-tomorrow variable).

### Pattern 5: DFS + Memo on a Grid DAG
When moves are forced to be strictly increasing (or otherwise can't cycle), `memo[r][c]` = the best path starting at `(r, c)`, computed once per cell.
* *Discipline:* no visited set: the ordering already rules out cycles. Run it from **every** cell, and the answer is the max over all. Mind the recursion depth on large grids.
* *Use for:* Longest Increasing Path in a Matrix (count cells: `count = 1`, dead end returns 1; the memo is the final form, since a table needs value order: Kahn's peeling from the peaks, layers = answer).

### Pattern 6: Interval DP → `dp[l][r]` from smaller ranges inside
Pick the element (or split point) handled **last** inside `[l, r]`. Its cost then depends only on the range boundaries, and the two sides become independent subproblems.
* *Discipline:* "last", not "first": choosing the first element leaves neighbours that change as the range shrinks. Pad the boundaries with neutral values if the problem's edges need them. Outer loop = range length.
* *Use for:* Burst Balloons (the balloon that bursts **last** in `[l, r]` sees only the walls `l − 1` and `r + 1`, so it is the one that splits the range; `max` across candidates, `+` inside one; `i` downward / `j` upward is as valid as range length; the table can't be rolled).

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

### [1] Unique Paths

> BFS brute force `ec35efe` (correct, exponential), memo `3d3ccb6`, 2-D table `d92a0b7`, then one row updated in place (with a right-to-left mirror kept as a comment). The walls: thinking "no cycles" meant "no duplicate work", a padded table seeded on both sides (every answer exactly doubled), an unpadded table that kept the padding's seed, and which way the one-row loop is allowed to run.

* **The Core Pattern:** grid paths (§2 Pattern 1). Moves are only right or down, so `r + c` goes up by 1 every step: the grid is a **DAG**, every path has exactly `(m − 1) + (n − 1)` steps, and every walk ends.
    * **`dp[r][c]` = the number of ways to get from the start to `(r, c)`.** You can only arrive from above or from the left, so `dp[r][c] = dp[r−1][c] + dp[r][c−1]`.
    * **Base cases are the whole top row and the whole left column, not just the start.** A cell on the top edge can only be reached by going straight right; on the left edge, only straight down. One way each: all 1s.
    * The mirror meaning works too: `dp[r][c]` = ways from `(r, c)` **to the target**, reading below and right, filled bottom-up. That's what the memo computed: `f(m, n)` = paths through an `m × n` grid = `f(m−1, n) + f(m, n−1)`, base `m == 1 || n == 1 → 1`. Both are correct because a path reversed is still a path. **Pick the meaning first; the fill direction follows from it.**

* **Stumble 1: "there are no cycles, so I don't need a seen set" is true, and it's not why BFS blew up.** In a normal BFS, a `seen` set does two jobs: (1) it stops cycles, and (2) it stops the same cell being queued again by a *different* path. Here job 1 is unnecessary, but job 2 is where all the cost went. With no merging, a queue entry is a **path prefix**, not a cell. On 3×3:

  | level (r+c) | entries pushed | distinct cells |
  |---|---|---|
  | 0 | (0,0) | 1 |
  | 1 | (0,1), (1,0) | 2 |
  | 2 | (0,2), **(1,1), (1,1)**, (2,0) | 3 |
  | 3 | (1,2) ×3, (2,1) ×3 | 2 |
  | 4 | (2,2) ×6 | 1 |

    * (1,1) comes in twice, from (0,1) going down and from (1,0) going right: two paths, not a cycle. Each copy then explores the identical subtree below it. The final `count++` at the target is correct only because every path is walked out in full, one unit of work per path: about C(m + n, m) pushes in total. **Exponential.**
    * The vector-as-queue never popped (a `read` index instead), so every entry ever pushed stayed in memory: **~430 MB at 14 × 14**, at 0.27 s. And `reserve(m * n)` assumed one push per cell, which is exactly the wrong model. It would run out of memory before running out of time.
    * **A plain `seen` set would make it *wrong*:** skip the second (1,1) and you lose a path; 3×3 would return 1, not 6. **The fix is a seen set that carries a count:** when a cell arrives again, don't push it, add to `ways[cell]`. That *is* the DP. The duplicate rows in the table above are exactly what memoization deletes.

* **Stumble 2: the padded table returned exactly 2× the answer, on every size.** The table was `(m+1) × (n+1)`, the seeding loops ran from index 0, and the main loop started at `(1, 1)`.
    * The seeding wrote 1s into the **padding**: `dp[1][0]` (from the row loop at `i = 0`) and `dp[0][1]` (from the column loop at `i = 0`). Then the main loop recomputed the start cell instead of keeping it: `dp[1][1] = dp[0][1] + dp[1][0] = 1 + 1 = 2`. Everything downstream is built from that 2:

      ```
           j=0 j=1 j=2 j=3
      i=0   0   1   0   0
      i=1   1   2   2   2
      i=2   0   2   4   6
      i=3   0   2   6  12   ← want 6
      ```

    * Fuzzed: owner = 2 × reference on all 225 sizes up to 15 × 15. (A hidden third bug: the 1-indexed seeding stopped at `n − 1`, so `dp[1][n]` was never seeded; the overwrite hid it.)
    * **Why exactly ONE padding seed.** The start cell gets its value from the same formula as every other cell: above + left. Both of those are fake padding cells. The right value at the start is 1 (one way to stand where you begin). So exactly one fake neighbour must feed it 1: seed `dp[0][1]` **or** `dp[1][0]`. Seed both and it's as if two walkers enter the grid, one from above and one from the left. Every other padding cell stays 0, since no path enters the grid from outside.

* **Stumble 3: removing the padding but keeping the padding's seed.** Shrinking the table to `m × n` with `dp[0][1] = 1` and the loop still at `(1, 1)` returned **2 on 3 × 3**:

  ```
   0 1 0        want   1 1 1
   0 1 1               1 2 3
   0 1 2                1 3 6
  ```

    * Without padding, `dp[0][1]` isn't fake any more. **It's a real cell** (right of the start), and row 0 / column 0 are real cells the loop never fills, so they stay 0 except for the one stray 1. The early `if (m == 1 || n == 1) return 1` hid it for thin grids but didn't fix anything.
    * **The two consistent setups, never mixed:** (A) `(m+1) × (n+1)`, one seed in the padding, loop over all real cells including the start; or (B) `m × n`, row 0 and column 0 all 1s, loop from `(1, 1)`. The committed table is B, and the early return went away because the seeding already handles 1 × n and m × 1.

* **The final version: one row, in place.** Row `r` only reads row `r − 1`, so keep a row of `n` ones (the top edge) and for each of the `m − 1` remaining rows do `row[c] += row[c − 1]` for `c = 1 .. n−1`. `row[0]` stays 1 by itself, which is the left edge for free.
    * **Why left-to-right.** At the moment `row[c] += row[c−1]` runs, the array is half old row, half new row. `row[c]` must still be the **old** row's value (that's "up"), and `row[c − 1]` must already be the **new** row's value (that's "left"). **Visit the neighbour you read before the cell that reads it.** Left to right visits `c − 1` first. ✓
    * **Flipping only the loop direction is wrong.** Right to left with the same `row[c] += row[c−1]`: when `c` is updated, `c − 1` still holds the old row, so you compute up + **up-left**. 3 × 3 returns **4**:

      | version | after row 1 | after row 2 | result |
      |---|---|---|---|
      | L→R, `row[c] += row[c−1]` | 1 2 3 | 1 3 6 | 6 ✓ |
      | R→L, `row[c] += row[c−1]` | 1 2 2 | 1 3 4 | 4 ✗ |
      | R→L, `row[c] += row[c+1]`, answer `row[0]` | 3 2 1 | 6 3 1 | 6 ✓ |

    * **Right-to-left works when the meaning flips with it.** The commented version (`row[c−1] += row[c]` for `c` from `n − 1` down to 1, answer `row[0]`) is the third row of the table with the index shifted by one: a cell = paths from here **to the target**, reading the right neighbour, which right-to-left has already updated. Same rule. The outer loop is just a counter of m − 1 passes; with one row, no row index is ever used, so "top-down" vs "bottom-up" lives entirely in the inner loop.
    * **Same rule as Partition `[12]` in the 1-D guide, seen from the other side:** there, downward was chosen so that `row[left − x]` would still be the *old* row (use each item once). Here, left-to-right is chosen so that `row[c − 1]` is already the *new* row (left is in the same row). In both cases the question is "old row or new row for the cell I'm reading?", and the loop direction answers it.

* **Overflow argument (say it in the interview).** Counts only grow moving right or down, so every cell is ≤ the final answer. LeetCode guarantees the answer ≤ 2·10⁹ < `INT_MAX`, so no intermediate cell can overflow `int`. Without that guarantee, 100 × 100 is ~10⁵⁸ and no integer type holds it.

* **Measured** at `-O2`, averaged over all valid inputs: 2-D `vector<vector>` 474 ns/call (m + 1 heap allocations), **one row 76 ns (6×)**, closed form 6 ns.

* **Not taken (talking points):**
    * **The closed form C(m+n−2, m−1).** A path is m − 1 downs and n − 1 rights in some order; choose where the downs go. O(min(m, n)) time, O(1) space. Use 64-bit and multiply before dividing at each step: after step `i` the running value is itself a binomial, so every division is exact. In a DP round the DP is the expected answer.
    * **Keep the row along the shorter side** (swap m and n first, the answer is symmetric): O(min(m, n)) space. Constant factor here.

* **Time & Space Complexity (final, one row):**
    * **Time:** **O(m · n)**, one addition per cell.
    * **Space:** **O(n)**, one heap allocation; 400 bytes at the limits, lives in L1.

### [2] Longest Common Subsequence

> Brute force `30d70b9` (TLE), memo `1b48e68`, 2-D table `9e62f2e`, then `1eee9fd`: two rows with `swap`, and the longer string forced onto the outer loop. The walls were all in the model before any code: an "anchor and restart" greedy walk, and a recursion that could only skip in one string and added two alternative futures together.

* **The Core Pattern:** two sequences (§2 Pattern 2), in **suffix** framing. **`f(i, j)` = the LCS of `text1[i..]` and `text2[j..]`.** Answer: `f(0, 0)`. Base: either suffix empty → 0.
    * **Match** (`text1[i] == text2[j]`): `1 + f(i+1, j+1)`, pair them and move both.
    * **Mismatch:** at least one of the two characters is not in the answer *paired with the other*, and you don't know which, so try both: `max(f(i+1, j), f(i, j+1))`.
    * **The subsequence rule is "pairs don't cross"**, not anything about `i` vs `j`. They index different strings and move independently (`"xxxxa"` / `"a"` pairs index 4 with index 0). Moving both forward after a match is what keeps the pairs from crossing; nothing else needs checking.

* **Stumble 1: the first plan was a greedy walk, not a recursion.** "Anchor `i`, walk both strings taking every match, restart from `i + 1` when one runs out." Same mistake as LIS's "take whenever you can": from each anchor it follows **one** path, and the restart only skips characters of `text1` *before* the walk, never in the middle. `"abcd"` / `"acbd"` (want 3, `"abd"`):

  | step | `text1[i]` | `text2[j]` | action | count |
  |---|---|---|---|---|
  | 1 | a | a | match | 1 |
  | 2 | b | c | mismatch, j++ | 1 |
  | 3 | b | b | match | 2 |
  | 4 | c | d | mismatch, j++ | 2 |
  | 5 | | end | stop | **2** |

    Matching `b` used up `text2` past its `c`, and then `c` in `text1` ate the rest of `text2`, so `d` never got a turn. Every restart scored ≤ 2. The "`i >= j` to enforce the subsequence" idea was a second false constraint on top.

* **Stumble 2: the first recursion had two bugs, 126,078 wrong out of 200,000.**
    * **Only one skip.** The only call besides the match was `(i, j + 1)`, so a character of `text1` could never be dropped. `"ab"` / `"b"` → 0: `a` finds nothing, `j` runs off the end, and `b` is never reached. **LIS had one sequence and one skip. Two sequences means two skips.**
    * **`count += max(take, count)`.** The `max` already picks the winner between take and skip; adding it to `count` stacks the skip result on top. `"a"` / `"aa"` → **2**: `f(0, 1)` = 1, then at `f(0, 0)` the match adds `max(1, 1)` to that 1. Take and skip are **alternative futures**, so they combine with `max`, never `+`. (LIS's first draft had the same shape with `=` overwriting the best.)

* **Insight: on a match you never need the skips.** It's tempting to write `max(1 + diag, skip i, skip j)` "to be safe", and that's correct, but the plain `1 + diag` is too. Take any best answer that doesn't pair `(i, j)`. Its first pair uses `text1[i]`, or `text2[j]`, or neither. Replace that first pair with `(i, j)`: nothing crosses (everything else is to the right in both strings), and the length doesn't drop. So pairing equal front characters is never worse, and a matched cell costs one read.

* **The table (`9e62f2e`) was a straight transliteration.** `(n1+1) × (n2+1)`, all zeros, both loops downward because a cell reads **below, right and diagonal-down-right**, all already filled. The extra row `n1` and column `n2` are the "empty suffix → 0" base, so **no seeding at all**: the Unique Paths seeding bugs had nowhere to happen. 0 wrong on 200k fuzz. Measured on two random 1000-char strings: **memo 4.5 ms → table 0.90 ms** (5×, a million recursive calls with `-1` checks vs a tight loop).

* **Two rows (`1eee9fd`): O(n) space, 2.4× faster.** Row `i` reads only row `i + 1`, so `current` and `next` of `n2 + 1` ints, `swap` after each row, answer `next[0]` (the last row written, after the final swap).
    * **Why `swap` is safe here (Partition's Stumble 4 rule):** every `j` from `n2 − 1` down to 0 is written each row, by the `max` or by `1 + diagonal`. So whatever was left on the board from two rows ago is overwritten before it's read. The only unwritten cell is `[n2]`, the base column, and it starts at 0 in **both** buffers.
    * **0.89 → 0.37 ms**, and 4 MB → 8 KB. The whole table didn't fit in L2; two rows sit in L1. Plus 2 allocations instead of n1 + 2.
    * **The shorter string on the row is space only, not speed.** Measured: 1000 × 50 (short row) 0.034 ms vs 50 × 1000 (long row) 0.018 ms. Same n1 · n2 cells, but a 50-long inner loop pays the per-row overhead 1000 times. Both layouts are in L1 at these sizes, so the smaller row buys nothing in time. The committed swap (recurse once with the arguments flipped) is an O(min(n1, n2)) space remark, worth one sentence in an interview. (The review had told me to swap "for speed"; the numbers retracted that.)
    * Parameters are `std::string_view`, fixing the table's by-value `std::string` copies.

* **Not taken:**
    * **One row with a saved diagonal.** Same speed as two rows (0.38 vs 0.36 ms). In place, `row[j + 1]` is already the new row ("right") and `row[j]` is still the old row ("below"), but the diagonal, the **old** `row[j + 1]`, was overwritten one step earlier. Save each cell's old value before writing it, hand it to the next `j` as the diagonal, reset it to 0 each row. Practice this before Edit Distance, which needs it.
    * **Faster than O(n1 · n2):** none known in general. Bit-parallel LCS (64 columns per machine word, like Partition's bitset) is a systems talking point.

* **Time & Space Complexity (final, `1eee9fd`):**
    * **Time:** **O(n1 · n2)**, one compare and one read (match) or two reads (mismatch) per cell.
    * **Space:** **O(min(n1, n2))**: two rows of the shorter length + 1, ~8 KB at the limits.

### [3] Best Time to Buy and Sell Stock with Cooldown

> Brute force `f27a473` (TLE), memo `901f2ed`, 2-D table `379542c`, two vectors `82827c2`, then three rolled variables (committed with this entry). The wall was reading the editorial's recursion: buy and sell made sense, but what "skip" meant didn't. After that, every version was right first time apart from returning the wrong mode's cell once.

* **The Core Pattern:** position + mode (§2 Pattern 4). **`f(i, holding)` = the most profit still makeable from day `i` onward, given whether I'm holding a stock.** Answer: `f(0, not holding)`. Base: `i >= n` → 0.
    * **Rename the flag from "is buying allowed?" to "am I holding?"** It's a **mode**, not a permission. Then each day has one take and one skip per mode:

      | mode on day `i` | take | skip (do nothing) |
      |---|---|---|
      | **not holding** | buy: `−prices[i] + f(i+1, holding)` | rest: `f(i+1, not holding)` |
      | **holding** | sell: `+prices[i] + f(i+2, not holding)` | keep holding: `f(i+1, holding)` |

    * **Skip = move to `i + 1` with the mode unchanged.** Only buying or selling flips the flag. Same shape as LIS and Partition: one take, one skip, `max` of the two.
    * **The money goes into the return value**, not into a parameter: buy subtracts the price from what the future returns, sell adds it (Coin Change's "return the answer, don't carry it down").
    * **The cooldown is the `i + 2` on sell.** So the base case must be `i >= n`, not `i == n`: a sale on the last day jumps to `n + 1`.
    * **Still holding at the end is fine at 0.** The buy branch already paid `−price`, so `max` never keeps a buy it can't sell at a profit. No "impossible" sentinel needed in suffix framing.

* **Stumble 1: what is the skip?** I had both takes right (buy → can't buy again, sell → `i + 2`) and called them both "taking", then couldn't see where skipping fit. **Without the skip, the recursion must act the moment it's allowed, which is greedy.** `[1, 2, 4]`: always-act sells at 2 (profit 1); holding one more day and selling at 4 makes 3. `[1, 2, 3, 0, 2]` (want 3): holding to sell at 3 gives 2, because the cooldown lands on the day of the 0; resting on day 0 gives 1. Only exploring every rest/hold finds that sell-at-2, cooldown, buy-at-0, sell-at-2 is the best.

* **The table (`379542c`) was the checklist run straight through** (§1 "Memo → Table"):
    * **`n + 2` rows, not `n + 1`.** The sell reads `i + 2`, so a sale on the last day reads row `n + 1`. Two zero rows are the `i >= n` base; nothing to seed.
    * `i` downward (all calls go to `i + 1` / `i + 2`). The inner mode loop order doesn't matter: `j = 0` reads `dp[i+1][1]`, the row below.
    * **The cost was all in the layout.** `vector<vector<int>>` with **2-wide rows** = n + 3 heap allocations, each ~16 B of malloc header + a 24 B `vector` header to hold 8 B of data. Measured at n = 5000: memo 112 µs, **table 83 µs** (only 1.35× over the memo), flat `2 × (n+2)` array 5.4 µs. **~94% of the table's time was `malloc`.** On LCS the rows were 1000 wide, so the allocations hid behind real work; here each row is two `max`es.

* **Two vectors (`82827c2`): 83 → 4.8 µs.** `buyDp` and `sellDp` of `n + 2` each.
    * **"Does the order of the two lines matter?" Not here.** `buyDp[i]` reads `[i+1]`, `sellDp[i]` reads `[i+1]` and `buyDp[i+2]`: neither reads index `i`, so neither depends on the other. Swapped and fuzzed: 0 wrong. **Order only starts to matter once the rows are rolled into variables**, where new and old share a name.
    * **Stumble 2: returned `sellDp[0]`.** That's `f(0, holding)`: "start day 0 already holding a stock I **got for free**". On `[1, 2, 3, 0, 2]` it returns **4**: sell the free stock at 2, cool down, buy at 0, sell at 2. **The answer is the top-level call's cell** (checklist row 4; Partition's `dp[0][0]` was the same mistake). The names caused it: `buyDp` / `sellDp` name the action you *may* take, not the state you're *in*. `notHolding` / `holding` would have made the right return obvious.

* **The final version: three variables, O(1).** The loop only reads `i + 1` (both modes) and `i + 2` (not-holding only), so keep `buyTomorrow`, `sellTomorrow`, `buyDayAfterTomorrow`.
    * **Compute both of today's values into temporaries first, then shift.** The sell reads the day-after's not-holding value; if `buyTomorrow` were overwritten before that read, it would read today's.
    * **The shift order:** `buyDayAfterTomorrow = buyTomorrow` **before** `buyTomorrow = buyToday`. Reverse them and the day-after slot gets today's value.
    * **Return `buyTomorrow`:** after the last iteration (`i = 0`) the shift moved `buyDp[0]` into it. The inline comments mapping each variable to its `dp[...]` cell are what kept this straight.
    * **4.8 → 2.8 µs at n = 5000**, no allocations. 30× over the 2-D table.

* **Time & Space Complexity (final):**
    * **Time:** **O(n)**, two `max`es per day. Optimal: every price has to be seen.
    * **Space:** **O(1)**, three `int`s.

### [4] Coin Change II

> Brute force `0bb5917` (TLE), memo `273881b`, 2-D table `059bd39`, two rows `a0b87f7`, then one row (`0fbfcf9`, with the loop start moved to the coin and committed with this entry). The walls: a first recursion that counted **orderings** (9 instead of 4), a table loop that started at the coin and dropped the skip (Partition's Stumble 2 again), and intermediate cells overflowing `int` even when the answer fits.

* **The Core Pattern:** counting knapsack, unlimited supply (§2 Pattern 3). **`f(i, remaining)` = the number of ways to make `remaining` using only `coins[i..]`.** Answer: `f(0, amount)`.
    * **Take** `coins[i]`: `f(i, remaining − coins[i])`. `i` **stays**, because the coin can be used again.
    * **Skip** `coins[i]`: `f(i + 1, remaining)`. Done with this coin **forever**, and `i` moving is what makes it terminate.
    * **The two branches add, not `max`.** They're two disjoint sets of piles (at least one more of this coin / none), and the question is *how many*, not *which is best*. Same take/skip as LCS and Stock; the combine operator comes from the question.
    * Base: `remaining == 0` → 1 (the pile is complete), `remaining < 0` or no coins left → 0.

* **Stumble 1: the first recursion counted orderings.** Three issues at once:
    * **Base case on `remaining == 1`** instead of 0. Reaching exactly 0 is the one success.
    * **A loop over coins *plus* take/skip inside it, with a skip that didn't move.** A call with identical arguments is the same call again, so it never bottoms out (LIS's first draft: loop and take/skip mixed).
    * **Take-only over all coins answers a different question.** At each step it asks "which coin comes next?", so `1 2 2`, `2 1 2` and `2 2 1` are three branches. `[1, 2, 5]`, amount 5 (my hand trace said 8; run, it's 9):

      | combination | orderings the loop counted |
      |---|---|
      | {1,1,1,1,1} | 1 |
      | {1,1,1,2} | 4 |
      | {1,2,2} | 3 |
      | {5} | 1 |
      | **4** | **9** |

      That's Combination Sum IV, not this problem. **The fix: count each pile once, in one fixed order**: coins in list order, and once you move past a coin you never go back. That's exactly what the `i` parameter enforces: `2 1` is impossible because after skipping the 1 you can't return to it.

* **Stumble 2: the table started its loop at `coins[i]` (31,045 wrong / 100k).** I did it to avoid a negative index on the take, and forgot the skip is legal for every amount. Smallest: **amount 1, coins `[6, 1]`** → 0, want 1. Row 1 (coin 1) gets `dp[1][1] = 1`; row 0 (coin 6) starts at 6 > 1, so `dp[0][1]` is never written and stays 0 instead of the skip value `dp[1][1]`. **Partition's Stumble 2 word for word: skip is always legal, take only when it fits.** The fix keeps the loop at 0 and guards only the take with `if (fillAmount >= coins[i])`. (Also fixed: `n + 1` rows, and seeding `dp[i][0] = 1` for every row and the `i == n` row to 0 explicitly.)

* **Stumble 3: cells nobody asked about overflow `int`.** amount = 4999, coins `[2, 4, 6, 8, 10]`: answer **0** (even coins can't make an odd amount), which fits, so LeetCode allows it. But the table also fills every even amount below it, and the ways to make 4998 are over 10¹²: UBSan `signed integer overflow: 2144759510 + 5447085`, and LeetCode's sanitizers report it as a runtime error. The memo didn't have it: from 4999, subtracting even coins only reaches odd amounts, all 0. **A table fills every state, including ones the real question never reaches.**
    * **The fix is `unsigned`, and it's principled, not a band-aid:** (1) unsigned arithmetic wraps mod 2³², defined behaviour; (2) the table **only adds**, so every cell is its true count mod 2³²; (3) the answer is guaranteed < 2³¹, so "right mod 2³²" means exactly right, and the final `static_cast<int>` is exact. Checked against Python big integers: `[2,4,6,8,10,4001]`, amount 4999 → 22,849,600 exactly, with cells reaching ~2³⁴.
    * **`long long` would be the band-aid:** it only moves the wall. Ways to make 4998 from all even coins is ~2¹⁷¹.
    * **The trick only works because nothing but `+` touches the counts.** A `max`, a comparison or a division on wrapped values is garbage, so it's for counting DP only, never for LCS- or Coin-Change-I-style "best value" tables.
    * Interview line: "intermediate cells for amounts we don't need can exceed 32 bits; unsigned makes the wraparound defined, and since it's all additions and the answer is < 2³¹, the final cell is exact."

* **From the table to one row: five deletions, each one still correct.** I said I could never have written the final version from the recursion. Nobody does; this is the derivation:
    1. **Recursion:** take `(i, rem − coin)`, skip `(i + 1, rem)`.
    2. **Table:** take reads **the same row** `i`, skip reads **the row below** `i + 1`.
    3. **Two rows** (`a0b87f7`): only the row below is ever needed. `swap` is safe because the loop from 0 writes every cell (Partition's Stumble 4 rule). 1,273 → 445 µs, 6 MB → 40 KB.
    4. **One row:** in two rows, `current[f] = next[f]` copies the row below into the same slot. With one row, that slot *already holds* the row below's value, so it's a self-copy. Delete it, and `next` disappears. 445 → 343 µs.
    5. **Loop starts at the coin:** cells below the coin are now never touched, so they keep the row below's value, which *is* the skip. The `if` becomes the loop bound. 343 → **164 µs** (large coins skip most of the row). **This is exactly Stumble 2's loop start, and with one row it's correct.**

* **Why upward (and why Partition went downward).** The take reads `row[f − coin]`. Here that's the **same row** (the coin can be reused), so in place it must already hold **this** row's value: visit smaller amounts first, **upward**. In Partition the take read row `i + 1` (each element once), so it had to still hold the **old** row's value: **downward**. Same question as Unique Paths and LCS: *for the cell I read, do I want the old row or the new one?* **Unlimited → upward, once → downward.**
    * The final line in words: **"for each coin, every amount `coin` below is reachable here too, with one more of this coin."**
    * **Keep coins as the outer loop.** Amounts outside and coins inside counts orderings: Stumble 1's 9 in loop form.

* **Time & Space Complexity (final):**
    * **Time:** **O(n · amount)**, one add per (coin, amount ≥ coin).
    * **Space:** **O(amount)**: one row of `amount + 1` unsigned ints, ~20 KB at the limits.
    * Measured at `-O2`, 300 random coins, amount 5000: 2-D table 1,273 µs → two rows 445 → one row 343 → loop from the coin **164 µs** (7.8×).

### [5] Target Sum

> Brute force `bf5ae4a` (exponential, but n ≤ 20 so it passes), offset memo `d3400b2` → `3b359ed`, subset-sum memo `5c90423`, 2-D table `284c375`, one row `32b2973`. The walls: three base cases copied from Coin Change that don't hold when every number must be used, sizing a table for a range that goes negative, the sign → subset reduction itself, an early "success" that undercounted zeros, and the one-row loop direction.

* **The Core Pattern, first form:** `f(i, remaining)` = the number of ways to sign `nums[i..]` so they add up to `remaining`. `+` branch: `f(i+1, remaining − nums[i])`; `−` branch: `f(i+1, remaining + nums[i])`; the two **add** (counting, Coin Change II). Answer `f(0, target)`.

* **Stumble 1: Coin Change's base cases don't transfer (my first recursion, built from the description).** In Coin Change, coins were **optional** and only **subtracted**, so `remaining` only went down. Here **every number gets a sign** and a `−` *adds*. All three base cases broke:

  | nums | target | got | want | why |
  |---|---|---|---|---|
  | `[2, 1]` | 1 | 0 | 1 | `remaining < 0 → 0` killed `+2` (rem −1), but `−1` brings it back to 0 |
  | `[1, 1]` | 1 | **1** | 0 | `remaining == 0 → 1` fired after `+1` with the second 1 still unsigned |
  | `[1, 0]` | 1 | 1 | 2 | same early success: the 0 can be `+0` or `−0`, two ways |
  | `[1]` | 1 | 0 | 1 | `i == end → 0` checked *before* the success check |

    **The fix is one base case: `if (i == n) return remaining == 0`.** A result exists only once every number has a sign. Checking both at the same moment also removes the ordering question. n ≤ 20 means ≤ 2²⁰ leaves (9.9 ms all-ones), so this brute force passes on LeetCode.

* **Stumble 2: sizing the memo when `remaining` can be negative.** First version (`d3400b2`): non-negative values at `[0, S + |t|]`, negatives folded to `|r| + S + |t|`. Correct (no two values share a slot) but `2(S + |t|) + 1` wide.
    * **The true range:** `remaining` starts at `target` and moves by `±nums[i]` per step, so it always lies in **`[target − S, target + S]`**, width **2S + 1**, centred on the target, not on 0 (fuzz-checked: never outside). One offset maps it: **index = `remaining − (target − S)`**. Plus `|target| > S → return 0` before allocating. 4001 → 2001 columns at the limits.
    * **The accident in `3b359ed`:** with `absSum` declared `unsigned`, `target − absSum` converted `target` to unsigned (`1 − 3u` = 4,294,967,294), and the index came out right only because the next subtraction wrapped back. Correct mod 2³², like Coin Change II, but there the wrap was chosen and argued; here it was luck (`-Wsign-conversion` flags it). `S ≤ 1000` → keep it `int`, and pass one precomputed `offset = S − target`.

* **The reduction: why and how signs become a subset** (the part I said I didn't understand a single thing of).
    * **Two buckets.** Every number goes in the **plus** bucket (sum P) or the **minus** bucket (sum N). The total is **P − N**. Every number is in exactly one bucket, so **P + N = S** always. Knowing P fixes N = S − P, and so fixes the total: **total = 2P − S**. The minus signs aren't a separate decision; they're *everyone not picked for plus*.
    * **On `[1, 2, 3]`** (S = 6), all 8 sign choices:

      | signs | total | plus bucket | P |
      |---|---|---|---|
      | + + + | 6 | {1,2,3} | 6 |
      | + + − | **0** | {1,2} | **3** |
      | + − + | 2 | {1,3} | 4 |
      | + − − | −4 | {1} | 1 |
      | − + + | 4 | {2,3} | 5 |
      | − + − | −2 | {2} | 2 |
      | − − + | **0** | {3} | **3** |
      | − − − | −6 | {} | 0 |

      Total = 2P − 6 on every row. Target 0 ⟺ P = 3: two rows, two subsets ({1,2}, {3}). **Each sign choice is exactly one plus-bucket, and each plus-bucket exactly one sign choice**, so counting one counts the other.
    * **Solve for P:** `2P − S = target` → **`P = (S + target) / 2`**. The question becomes **"how many subsets sum to P?"**: Partition's take/skip, counting instead of `||`.
    * **Edge cases fall out of the algebra:** `S + target` odd → P isn't a whole number → **0** (target 1 on `[1,2,3]` needs P = 3.5, and no row totals 1). P < 0 can't happen after the `|target| > S` check.
    * **How the two recursions are literally the same one.** At step `i` in the sign version, let R = the sum of `nums[i..]` (not yet signed). Define **`g = (remaining + R) / 2`**. Watch what each sign does to `g`:
        * `+x`: remaining → remaining − x, R → R − x, so **g → g − x**. That's **take**.
        * `−x`: remaining → remaining + x, R → R − x, so **g is unchanged**. That's **skip**.
        * Start: `g = (target + S) / 2 = P`. End: R = 0, so `g = remaining / 2`, which is 0 exactly when remaining is 0.

      Trace on `[1, 2, 3]`, target 0, path `+1 +2 −3`:

      | after | remaining | R | g | subset move |
      |---|---|---|---|---|
      | start | 0 | 6 | **3** | |
      | +1 | −1 | 5 | 2 | take 1 |
      | +2 | −3 | 3 | 0 | take 2 |
      | −3 | 0 | 0 | 0 | skip 3 ✓ |

      **So the subset recursion is the sign recursion with `remaining` renamed to `g`: `+` became take, `−` became skip.** What the rename buys: `g` is never negative and never above P, so the range shrinks from `[t − S, t + S]` (2S + 1, needs an offset) to `[0, P]` (≤ S + 1, no offset). And because `g` only goes **down**, `remaining < 0 → 0` becomes a valid exit again.

* **Stumble 3: early success undercounted zeros (22,704 wrong / 200k, every one with a 0 in `nums`).** The subset memo started with `if (remaining == 0) return 1`. Smallest: **`[0]`, target 0 → 1, want 2** (`+0` and `−0`; subsets `{}` and `{0}`). Taking a 0 doesn't change `remaining`, so every zero after the sum hits 0 still doubles the count. Partition could stop early because "yes" stays "yes"; **when counting, every element still has to make its choice.** Same bug as Stumble 1's `[1, 0]`, in the subset form. Fix: success only at `i == n`; keep `remaining < 0 → 0` first (valid now, and it guards the index).

* **The 2-D table (`284c375`)** was the checklist: `(n + 1) × (P + 1)`, base `dp[n][0] = 1`, loop from 0 so the skip is written for every sum, take guarded by `remSum >= nums[i]` (Coin Change II's Stumble 2, not repeated).

* **Stumble 4: the one-row loop ran upward (51,823 wrong / 200k).** Smallest: **`[1, 1]`, target 2** (P = 2, want 1: only `{1, 1}`). Row starts `1 0 0`:

  | number | step | reads | row after |
  |---|---|---|---|
  | second 1 | r = 1 | `cur[0]` = 1 | `1 1 0` |
  | second 1 | r = 2 | `cur[1]` = 1, **written one step ago by this same 1** | `1 1 1` ✗ |
  | first 1 | r = 1, 2 | | `1 2 3` → **3** |

  Downward, the same row gives `1 1 0` → `1 2 1` → **1** ✓.

* **Why the direction only matters in the one-row version** (the other part I was murky on):
    * **2-D table and two rows:** row `i` is written into one array and row `i + 1` is read from a **different** array. A write can never change something you're about to read, so **any loop order is correct**. That's why `284c375` loops upward and is right.
    * **One row:** reads and writes share **the same memory**. Each cell holds the **old** row's value until you overwrite it, and the **new** row's value after. So for every cell you read, the question is: *have I already overwritten it this pass?* The loop direction decides the answer.
    * The take reads `cur[r − x]`, a **smaller** index than `r`.
        * **Upward** visits smaller indices first, so `cur[r − x]` is already **new**: it may already include this number. Using it again = **reuse**.
        * **Downward** visits smaller indices last, so `cur[r − x]` is still **old**: subsets of the *other* numbers. Adding this number once = **use once**.
    * **The direction isn't a property of the problem; it's how one array pretends to be two rows.** In the 2-D table, "old" and "new" are separate rows, so the choice is in the indices (`dp[i + 1][…]` vs `dp[i][…]`). In one row, it can only be expressed through visit order:

      | problem | 2-D take reads | in place must be | one-row loop |
      |---|---|---|---|
      | Coin Change II | `dp[i][r − x]` (same row: reuse) | new | upward |
      | Partition, Target Sum | `dp[i + 1][r − x]` (row below: once) | **old** | **downward** |
      | Unique Paths | `row[c − 1]` (left, same row) | new | left → right |
      | LCS | diagonal, old | old (saved in a variable) | — |

    * **Zeros work in both directions:** `cur[r] += cur[r − 0]` reads the cell before writing it, so each zero doubles every count, which is right.

* **Overflow: `int` is safe here, unlike Coin Change II.** Each cell counts subsets of a suffix, at most 2²⁰ ≈ 10⁶. "Use once" caps the counts; Coin Change II's unlimited reuse didn't.

* **Measured** at `-O2`, n = 20:

  | version | random values (S = 1007) | all 50s (S = 1000) |
  |---|---|---|
  | brute force `bf5ae4a` | 1,292 µs | 1,261 µs |
  | offset memo `3b359ed` | 25.0 µs | 8.5 µs |
  | subset memo `5c90423` | 12.5 µs | **0.88 µs** |
  | 2-D table `284c375` | 4.0 µs | 3.9 µs |
  | **one row `32b2973`** | **2.2 µs** | 2.2 µs |

    The reduction halved the memo by halving the state range. On all-50s only multiples of 50 are reachable (~21 sums per row out of 501), so the memo beats every table: Partition's "table vs memo isn't one-sided" again.

* **Time & Space Complexity (final, `32b2973`):**
    * **Time:** **O(n · P)**, P = (S + target) / 2 ≤ 1000.
    * **Space:** **O(P)**: one row of ≤ 1001 ints (~4 KB, L1).

### [6] Interleaving String

> Brute force `dfbbb52` (TLE), memo `ecf6332`, 2-D table `862ceeb`, two rows `5effae2`, one row `f2d564b`. The walls: a greedy "take from `s1` if it matches, else `s2`" plan with an off-by-one success check, not seeing why "neither matches → false" was right, a table whose loops started one cell in, and a rolled version that only wrote cells when a move was legal.

* **The Core Pattern:** two sequences (§2 Pattern 2) where the third string's index is **derived**: **`f(i, j)` = "can `s1[i..]` and `s2[j..]` interleave into `s3[i+j..]`?"** `i + j` characters of `s3` are already placed, so `s3`'s position needs no index of its own.
    * **Length check once, before anything:** `n1 + n2 != n3` → false. After that, "both strings used up" and "`s3` used up" are the same event, so there's **one** base case: `i == n1 && j == n2` → true.
    * **Two moves, each with its own guard:** take from `s1` if `i < n1` and `s1[i] == s3[i+j]` → `f(i+1, j)`; take from `s2` if `j < n2` and `s2[j] == s3[i+j]` → `f(i, j+1)`. Combine with **`||`** (yes/no question). No legal move → false.

* **Stumble 1: the first plan was greedy, and its success check was one character early.** Coded from my description and checked against a reference:

  | s1 | s2 | s3 | mine | want | why |
  |---|---|---|---|---|---|
  | `a` | `ab` | `aba` | false | **true** | took `s1`'s `a` because it matched; the real interleaving starts with `s2` |
  | `a` | (empty) | `b` | **true** | false | `i + j + 1 == n3` → true with `s3[0]` never compared |
  | `ab` | (empty) | `a` | **true** | false | no length check: the leftover `b` was ignored |

    * **When both tops match the next character, you can't know which string it came from: try both, `||`.** "Take from `s1` whenever it matches" is LIS's "take whenever you can" again; LCS's "you don't know which, so try both" is the fix, with `||` for `max`.
    * At state `(i, j)`, **`i + j` characters are consumed**, so `i + j + 1 == n3` still has one to check. And "both used up → false" was backwards: that's the success state.

* **Insight: why "neither matches → false" is immediate (the part I couldn't fully see).** Picture `s1` and `s2` as two face-up decks and `s3` as a riffle shuffle of them: every card is taken **from the top** of one deck (order kept), and **none is thrown away**. Reading `s3` back, each card must be the current top of deck 1 or deck 2, i.e. `s1[i]` or `s2[j]`. `s1 = "ab"`, `s2 = "cd"`, `s3 = "acxb"`: at `(1, 1)` the card is `x` and the tops are `b` and `d`. The `x` has to be placed **now**; it can't come from deeper in a deck and it can't be skipped. Nothing later in `s3` can rescue it.
    * **Unlike LCS, there is no skip move.** LCS asks for the longest *common part*, so a mismatched character can be dropped. An interleaving accounts for **every** character in order. So each state has 0 legal moves (false), 1 (no choice), or 2 (try both).

* **Stumble 2: starting the table loops at `n1 − 1`, `n2 − 1` to skip the bounds checks.** It returned false on true inputs: the corner's 1 never reached `(0, 0)`. `s1 = "a"`, `s2 = "b"`, `s3 = "ab"`:

  | cell | meaning | value |
  |---|---|---|
  | `dp[1][1]` | both used up | 1 (base) |
  | `dp[1][0]` | `s1` used up; does `"b"` spell `s3[1..] = "b"`? | **1**, but skipped by the loop, so 0 |
  | `dp[0][1]` | `s2` used up; does `"a"` spell `"b"`? | 0 |
  | `dp[0][0]` | take `a` from `s1` → `dp[1][0]` | should be 1 |

    * **Starting one cell in only works when the whole last row and column are constants.** LCS: row `n1` / column `n2` mean "one string empty → LCS 0", every cell, so skipping them was free. Here **only the corner** is a constant; the rest of the last row is "can the leftover of the other string spell the rest of `s3`?", which depends on the characters, and it's the **only path** the corner's 1 can travel.
    * **Read it off the memo's base case** (checklist row 2): LCS's was `i == n1 || j == n2` (whole edges are base); this one is `i == n1 && j == n2` (corner only), and the edges were ordinary guarded states in the memo. **`||` vs `&&` in the memo's base case tells you whether the loops can skip the edges.** The fix: loops from `n1` and `n2`, with the memo's `i < n1` / `j < n2` guards.

* **Stumble 3: rolling to two rows leaked stale values (54,579 wrong / 200k).** The first rolled attempt kept the 2-D table's two `if`s, which wrote **into the cell** only when a move was legal. Smallest: `s1 = ""`, `s2 = "b"`, `s3 = "a"` → **true** (swapped to `s1 = "b"`, `s2 = ""`; one-cell rows, both buffers initialised to the corner's `[1]`):

  | row | cell | `s1` move legal? | `s2` move legal? | written | value |
  |---|---|---|---|---|---|
  | i = 1 | j = 0 | no (`i == n1`) | no | nothing (the corner) | 1 ✓ |
  | *swap* | | | | | |
  | i = 0 | j = 0 | `b` vs `a`: no | no | **nothing** | **stale 1** ✗ |

    * **Why the 2-D table got away with it:** every cell started at 0 and was visited once, so "not written" meant "still 0", the correct false. Once buffers are reused (two rows + `swap`, or one row), **"not written" means "still holding an old row's value"**. Partition's Stumble 4 rule: every cell of the new row must be written *by this row*, **including the false ones**. The second `if`'s `current[j] || current[j + 1]` had the same leak through `current[j]`.
    * **The fix was already in my memo:** `bool resultFlag = false` fresh for every cell, the two `if`s update `resultFlag` (not the cell), and **`current[j] = resultFlag` always runs**. The memo did exactly that (`return dp[i][j] = resultFlag;`). The corner is the one cell with no legal move that is still true, so it gets its own `i == n1 && j == n2` case inside the loop.

* **One row (`f2d564b`): drop `next`.** With `j` looping **downward**, `row[j]` before it's written is still the row below ("below"), and `row[j + 1]` was written this pass ("right"). **No diagonal**, so unlike LCS no saved variable. The strings are swapped so the row is the shorter one: O(min(n1, n2)) space (LCS: that's space only, not speed).

* **Measured** at `-O2`, 100 + 100 chars:

  | input | memo | 2-D table | one row |
  |---|---|---|---|
  | all `a`s, `s3` ends in `b` (false, every state explored) | 48.0 µs | 9.9 µs | 12.0 µs |
  | random 2-letter, true | **2.7 µs** | 10.6 µs | 6.1 µs |

    The memo's `||` stops at the first path that reaches the corner, so on true inputs it beats every table; the table always fills all cells and wins when every state must be explored. Same trade as Partition and Target Sum. The one row's win is space: ~10 KB → ~100 bytes, which is LeetCode's follow-up ("O(s2.length) extra memory").

* **Time & Space Complexity (final, `f2d564b`):**
    * **Time:** **O(n1 · n2)**, two compares and two reads per cell.
    * **Space:** **O(min(n1, n2))**: one row of the shorter length + 1 bytes.

### [7] Longest Increasing Path in a Matrix

> Brute force `fec67e6` (TLE), memo `ec86b24` (counting steps), `3326ac6` (counting cells inside the helper), then the grid dimensions hoisted in the helper. No real wall in the recursion: the model came together in one brainstorm. The open questions were which counting convention is standard, and the editorial's "peeling the onion" table, which went over my head.

* **The Core Pattern:** DFS + memo on a grid DAG (§2 Pattern 5). **`f(r, c)` = the number of cells in the longest strictly increasing path *starting* at `(r, c)`.** `f = 1 + max f(neighbour)` over in-bounds neighbours with a **strictly larger** value; no such neighbour → 1. Answer: **max of `f` over every cell** (a path can start anywhere).
    * **No `seen`, and why:** every step goes to a strictly larger value, so returning to a cell would need a value larger than itself. No cycles → the grid is a DAG. Compare Word Search (Backtracking): moves in any direction, so it needed `seen` and an undo, and could not be memoized.
    * **Why caching is valid:** whether a step is legal depends only on the current cell's value, never on how you got there. So the longest path *from* `(r, c)` is the same for every caller. Each cell computed once × 4 neighbours = **O(m · n)**, even though the top level starts a search from every cell.

* **Counting steps vs counting cells (my question: which is standard?).** Both are the same recursion with the starting cell's `+ 1` in a different place:

  | | `ec86b24` (steps) | `3326ac6` (cells, standard) |
  |---|---|---|
  | `f(r, c)` means | most **moves** from here | most **cells** from here |
  | dead end | 0 | 1 |
  | `count` starts at | 0 | **1** |
  | top level | `max(…, 1 + f)` | `max(…, f)` |

    **Cells is better because each cached value *is* the answer for that start**, with no adjustment the caller has to know about, and the base case falls out: `count = 1` ("at least this cell"), and a dead end just never updates it. The two edits (start at 1, drop the top-level `+ 1`) have to be made **together**, or every answer is off by one. Check: `[[1, 2]]` → 2.

* **Insight: the memo is the right final form here, and the plain table is not.** The checklist (§1 "Memo → Table") picks the loop order from where the calls point: `i + 1` → loop downward. Here a cell's calls point at **larger-valued neighbours**, which can be in any direction. So the fill order comes from the **values**, not the positions. The naive table sorts all m·n cells by value (largest first) and fills them in that order: O(mn log mn), measured **5× slower** than the memo. First problem in the topic where memo → table isn't an improvement.

* **"Peeling the onion" (the editorial's table, the part that went over my head).** It finds that value order **without sorting**, using Kahn's algorithm from Course Schedule II (`12-Graphs/README.md`).
    * **Peaks first.** A **peak** is a cell with no strictly larger neighbour. Its longest path is just itself: `f = 1`. Peel all peaks off as **layer 1**.
    * **Then whatever became a peak.** Once a cell's larger neighbours have *all* been peeled, nothing above it is left, so it's a peak of what remains: **layer 2**. Keep peeling. **The number of layers is the answer.**
    * **Trace on LeetCode's example** `[[9, 9, 4], [6, 6, 8], [2, 1, 1]]` (answer 4: `1 → 2 → 6 → 9`):

      | layer | cells peeled (value at row, col) | why now |
      |---|---|---|
      | 1 | 9 (0,0), 9 (0,1), 8 (1,2) | no larger neighbour at all |
      | 2 | 4 (0,2), 6 (1,0), 6 (1,1) | their larger neighbours (the 9s, the 8) are gone |
      | 3 | 2 (2,0), 1 (2,2) | 2's larger neighbour 6 (1,0) is gone; 1 (2,2)'s larger neighbour 8 is gone |
      | 4 | 1 (2,1) | its larger neighbours 6 (1,1) and 2 (2,0) are finally gone |

    * **Why layer number = `f`:** a cell is peeled only after **all** its larger neighbours, and the last of them to go was in the previous layer. So a cell in layer k has a larger neighbour in layer k − 1, which has one in layer k − 2, … down to a peak: a path of exactly k cells, and none longer. **The layer number *is* `dp[r][c]` in the cells convention**, and peeling order *is* the table's fill order: every larger neighbour done before the cell that reads it.
    * **Where "topological sort" comes in:** a topological order lists every node after all the nodes it depends on. Here a cell depends on its larger neighbours (its `f` reads theirs). Kahn's algorithm produces that order by repeatedly taking nodes with **zero unfinished dependencies**: count each cell's larger neighbours, queue the zeros (the peaks), and when a cell is peeled, decrement each smaller neighbour's count, queueing it when it hits 0. Course Schedule II did exactly this with prerequisites: a course becomes takeable once all its prerequisites are done.
    * **Why it matters:** it's **iterative**, so no recursion depth. The memo's depth equals the longest path: 40,000 frames on a 200 × 200 snake (values increasing along a boustrophedon). That fit in 8 MB, even under ASan, but a 2000 × 2000 snake would overflow the stack. **"What if the grid were huge?" → Kahn's peeling.** Measured 0.35 ms on the snake (same as the memo) and 1.30 ms on random values (1.8× slower). A talking point, not something to code.

* **Measured** at `-O2`, 200 × 200:

  | version | snake (path 40,000) | random values |
  |---|---|---|
  | **memo (final)** | **0.32 ms** | **0.71 ms** |
  | flat memo, dimensions read once | 0.20 ms | 0.50 ms |
  | sorted-by-value table | 1.63 ms | 3.25 ms |
  | Kahn's peeling | 0.35 ms | 1.30 ms |

    The final version hoists `rows` / `columns` in the helper (the neighbour check no longer re-reads `matrix.size()`); the top-level loops still compare `int` against `matrix.size()` (`-Wsign-compare`, harmless). The flat memo's 1.4–1.7× is a constant factor and not interview material.

* **Time & Space Complexity (final):**
    * **Time:** **O(m · n)**: each cell computed once, 4 neighbours each. Optimal: every cell must be seen.
    * **Space:** **O(m · n)** memo **plus O(m · n) worst-case recursion depth** (one frame per cell on the longest path).

### [8] Distinct Subsequences

> Brute force `95b9bd4` (TLE, right first try), memo `822b901`, 2-D table `a46f570`, two rows `0c9447d`, one row `f1a7b35`. Once the base cases were settled the rest was mechanical. The bumps: a memo helper missing its `dp` argument, the unreached-cell overflow from Coin Change II, and the one-row loop that had to run the **opposite** way from Interleaving's.

* **The Core Pattern:** two sequences, counting (§2 Pattern 2 with Pattern 3's `+`). **`f(i, j)` = the number of ways `s[i..]` contains `t[j..]` as a subsequence.** Answer `f(0, 0)`.
    * **Skip `s[i]`:** `f(i + 1, j)`, always legal. **Take** (only if `s[i] == t[j]`): `f(i + 1, j + 1)`, use this character for `t[j]`. The two **add**: different choices of which `s` characters spell `t` are different subsequences.
    * **Unlike LCS, a match does *not* make the skip redundant.** LCS asked for the *best* length, and taking a match was never worse. Here every way counts: `s = "aa"`, `t = "a"` has **2** (either `a`), and dropping the skip on a match would count 1.
    * **Base-case order:** `j == n2 → 1` (all of `t` placed: one way) **before** `i == n1 → 0`. Finishing `t` exactly as `s` runs out is a success, so success wins (Partition's rule).
    * **`n2 > n1 → 0`** before anything: a longer `t` can't fit.

* **The table (`a46f570`):** `(n1 + 1) × (n2 + 1)`, column `n2` all 1s (**including** row `n1`: empty `t` is matched once, by picking nothing), row `n1` otherwise 0. Cell = below, plus diagonal on a match. Loops downward in both.

* **Overflow again, and why the memo never had it.** `s` = 1000 × `a`, `t` = `"x"` + 500 × `a`: answer **0** (no `x`), but the table fills "ways to match the `a`s" for every `i`, up to C(1000, 500). Fixed with `unsigned` (Coin Change II's argument: only additions, answer < 2³¹, so the final cell is exact).
    * **The memo uses plain `int` and is safe in principle, not by luck.** It only visits states **reachable** from `(0, 0)`. Reaching `(i, j)` means `t[0..j)` is already matched inside `s[0..i)`, so every way to finish from `(i, j)` extends that fixed prefix into a **different** full match: **`f(i, j)` ≤ the final answer** for every visited state. On the case above the memo never gets past `'x'`.
    * **General rule for counting DP:** the memo only computes counts that are part of the answer, so it's safe whenever the answer fits; the table also computes counts the answer never uses, so it needs the wraparound argument. (Coin Change II's memo was safe for the same reason.)

* **Two rows (`0c9447d`):** every `j` in `[0, n2)` is written each row (skip copies `next[j]`, take adds `next[j + 1]`), so `swap` is safe. The never-written base cell `[n2]` starts at 1 in **both** buffers because `next` is copied after `current[n2] = 1`: Partition's Stumble 4 rule plus Interleaving's corner, both handled.

* **One row (`f1a7b35`): why the inner loop had to flip from downward to upward.**
    * **The copy disappears.** In two rows, `current[j] = next[j]` (the skip) copies the row below into the same slot. In one row that slot already holds the row below's value, so the line goes (Coin Change II's deletion). Only the take is left: on a match, `row[j] += row[j + 1]`.
    * **That take reads the *diagonal*, which must still be the OLD row.** In the table it's `dp[i + 1][j + 1]`: row `i + 1`, the row below. In place, `row[j + 1]` holds the old row's value only until this pass overwrites it. **So `j` must be visited before `j + 1`: loop `j` UPWARD.** Downward visits `j + 1` first, so `row[j + 1]` already holds **this** row's value, and the same `s[i]` gets used for both `t[j + 1]` and `t[j]`.
    * **Smallest failure of the downward loop: `s = "aa"`, `t = "aa"` → 3, want 1** (46,690 wrong / 200k). Row starts `[t0, t1, base] = 0 0 1`:

      | `s[i]` | downward, step by step | upward, step by step |
      |---|---|---|
      | `a` (i = 1) | j=1: `row[1] += row[2]` → 1 · j=0: `row[0] += row[1]` = **1 (just written)** → 1 ✗ | j=0: `row[0] += row[1]` = 0 → 0 · j=1: `row[1] += row[2]` → 1 |
      | row after | `1 1 1` (claims one `a` spells `"aa"`) | `0 1 1` ✓ |
      | `a` (i = 0) | j=1 → 2 · j=0: `row[0] += 2` → **3** | j=0: `row[0] += row[1]` = 1 → 1 · j=1 → 2 |
      | answer | **3** ✗ | **1** ✓ |

    * **Same index, opposite direction from Interleaving `[6]`.** Both read `row[j + 1]`. In Interleaving that cell was **right**, `dp[i][j + 1]`, the **current** row, so it had to be already written: **downward**. Here it's the **diagonal**, `dp[i + 1][j + 1]`, the **old** row, so it must not be written yet: **upward**. **The direction comes from which row the read belongs to, not from which index it is.**
    * **Why no saved variable, unlike LCS's one-row diagonal:** LCS's cell read the diagonal **and** the right neighbour (`j + 1` in both rows), so one direction couldn't serve both and the old value had to be stashed. Here the cell reads only below (itself, `row[j]`, untouched until written) and the diagonal, so choosing the direction is enough.

* **Measured** at `-O2`, `s` and `t` 1000 random chars, each version in its own process (in-process back-to-back runs skewed the numbers by order):

  | version | 2 letters | 26 letters |
  |---|---|---|
  | memo `822b901` | 1,900 µs | 554 µs |
  | 2-D `a46f570` | 897 µs | 1,005 µs |
  | two rows `0c9447d` | 309 µs | 531 µs |
  | **one row `f1a7b35`** | **228 µs** | **361 µs** |

    (The 2-letter input's true answer is far over 2³¹, so it's a timing input, not a valid LeetCode case.) Two rows: 4 MB → 8 KB, out of L2 and into L1, 1001 → 2 allocations. One row: no copy on the 96% of cells that don't match (26 letters). On sparse matches the memo beats the 2-D table: a table fills every state.

* **Time & Space Complexity (final, `f1a7b35`):**
    * **Time:** **O(n1 · n2)**, one compare per cell, one add per match.
    * **Space:** **O(n2)**: one row of `n2 + 1` unsigned ints.

### [9] Edit Distance

> Brute force `d931372` (TLE), memo `a274ed4` → `339c666`, 2-D table `4c68b0b`, two rows `0eb3b30`, then `79164cd` (strings swapped so the row is the shorter one). The walls: a first idea that pulled the length difference out as a fixed cost, a base case that kept that idea alive, and a two-row initialisation that put the wrong values in both base edges. Stopped at two rows on purpose (see the end).

* **The Core Pattern:** two sequences, minimise (§2 Pattern 2). **`f(i, j)` = the fewest operations to turn `word1[i..]` into `word2[j..]`.** Answer `f(0, 0)`.
    * **Match** (`word1[i] == word2[j]`): `f(i+1, j+1)`, free.
    * **Mismatch:** `1 + min` of three: **replace** → `(i+1, j+1)` (both characters dealt with), **delete** `word1[i]` → `(i+1, j)`, **insert** `word2[j]` in front → `(i, j+1)` (`word2[j]` is now satisfied, `word1[i]` still waiting). The `1 +` is the operation just spent, once, outside the `min`.
    * **Base:** one word used up → **the number of characters left in the other**, one insert or delete each: `(n1 − i) + (n2 − j)`, which covers both cases and gives 0 when both are used up.

* **Stumble 1: "the length difference is a constant, so count it up front".** True part: `|n1 − n2|` is a **lower bound** (nothing else changes the length). False part: it isn't a cost you can pull out before the recursion, and "count the non-common characters" ignores order:
    * **`"abcd"` → `"bcda"`** (equal lengths, so the up-front cost is 0): position by position is 4 replaces, but **delete the front `a`, insert an `a` at the end = 2**. A delete + insert can beat replaces because it **shifts** everything after it into line. So the number of deletes isn't fixed until you know which characters line up.
    * **`"ab"` → `"ba"`**: same characters, so "non-common" says 0; the answer is **2**. Order matters (LCS's "pairs don't cross").
    * **`"abc"` → `"bc"`**: *which* character you delete is the whole problem (delete `a`: done; delete `c`: 2 more replaces).
    * **Where the idea really belongs:** once **one word runs out** there's no choice left, and "the leftover length" *is* the cost. That's the base case, not a pre-computation.

* **Stumble 2: the base case returned the whole-word difference, `|n2 − n1|`** (57,007 wrong / 200k, wrong in both directions: `"horse"` → `"ros"` 4 instead of 3, `"abcd"` → `"bcda"` 1 instead of 2). Smallest: **`"c"` → `"bc"` → 2, want 1** (insert `b`). The insert branch reaches `f(1, 2)`, both words used up, which should be **0**, but `|2 − 1|` charged 1 for comparing nothing with nothing; the delete branch `f(1, 0)` (still need `"bc"`) should be 2 and got 1. Stumble 1's constant leaking back in. **What's left is measured from `(i, j)`, not from the start.**

* **Memo detail (`a274ed4` → `339c666`).** The first memo returned on a match *before* the lookup, so matched cells were never cached and each visit walked its diagonal of matches. Measured harmless (≤ 2.9 calls per cell on random and adversarial inputs), but the bound was measured, not guaranteed. `339c666` moved the lookup above the match line and stores match results too: O(n1 · n2) by construction.

* **The 2-D table (`4c68b0b`):** checklist row 2 read off the memo's `i == n1 || j == n2` base: **the whole last row and column are base cells**, like LCS, but **not constant**: `dp[i][n2] = n1 − i` (delete the rest), `dp[n1][j] = n2 − j` (insert the rest), corner 0. Both edges seeded, both loops downward (reads below, right, diagonal-below-right).

* **Stumble 3: the two-row initialisation (122,919 wrong / 200k).** Both buffers started as `[0, …, 0, n1]`. Smallest: **`""` → `"c"` → 0, want 1.** Two separate mistakes, both about which base value lives where:
    * **The starting `next` must *be* the bottom row:** `[n2, n2 − 1, …, 1, 0]`. Every cell was wrong: the 0s should be "characters left to insert", and the last cell should be 0 (both used up), not `n1`. With `n1 = 0` the loop never runs and the bad starting row is returned as is.
    * **The right column is one reused slot whose value changes every row.** In 2-D it was seeded once per row up front. With two rows, `current[n2]` must be set to **`n1 − i` at the start of each row**, because the `j` loop (from `n2 − 1`) never writes it; otherwise `current[j + 1]` at `j = n2 − 1` reads whatever the swap left behind. Interleaving's rule again: **every cell of the new row is written by this row**, including the one the loop skips.

* **Final (`79164cd`):** two rows, strings swapped so the row is the shorter one (the distance is symmetric: insert and delete mirror each other; space only, as LCS showed).

* **Measured** at `-O2`, 500 + 500 random chars, each version in its own process:

  | version | 4 letters | 26 letters |
  |---|---|---|
  | memo `339c666` | 1,485 µs | 1,707 µs |
  | 2-D `4c68b0b` | 258 µs | 307 µs |
  | **two rows `0eb3b30`** | **120 µs** | **170 µs** |
  | one row + saved diagonal (not committed) | 133 µs | 180 µs |

    The memo is 6–12× slower than every table: no early exit, and three branches per mismatch make nearly every cell reachable, so recursion overhead buys nothing (unlike Interleaving, where `||` let the memo win on true inputs). Two rows: 1 MB → 4 KB, into L1.

* **Why I stopped at two rows, and what one row would take.**
    * **Why stop:** two rows is already **O(min(n1, n2)) space**, the optimal bound, and it measured **fastest**. One row with a saved diagonal measured **5–10% slower**: the extra variable shuffling per cell costs more than the one `swap` per row it replaces. Same space class, more code, slower. Nothing to gain except being able to explain it.
    * **Why this is the one problem where a single row needs the saved variable.** The cell reads three neighbours, all through index `j` or `j + 1`:

      | neighbour | 2-D cell | in one row | must be |
      |---|---|---|---|
      | below | `dp[i+1][j]` | `row[j]` before writing it | old ✓ (free) |
      | right | `dp[i][j+1]` | `row[j+1]` | **new** (written this pass) |
      | diagonal | `dp[i+1][j+1]` | `row[j+1]` | **old** |

      **`row[j + 1]` would have to be new and old at once.** Downward `j` makes it new (right ✓, diagonal ✗); upward makes it old (diagonal ✓, right ✗). Interleaving needed only *right* (downward, done); Distinct Subsequences needed only *diagonal* (upward, done). **Edit Distance needs both, so no direction works**: the old value has to be saved before it's overwritten. (LCS's one-row version was the same case, parked there.)
    * **What it would take:**
        1. One row, initialised to the bottom row `n2 − j` (as `next` is now).
        2. At the start of each row `i`: save the old `row[n2]` into a `diag` variable (it's the diagonal for `j = n2 − 1`), then set `row[n2] = n1 − i`.
        3. For `j` from `n2 − 1` **down**: save `below = row[j]` (old) **before** writing; compute the cell from `diag` (diagonal), `below`, and `row[j + 1]` (right, new); write it; then **`diag = below`**: this cell's old value is the next `j`'s diagonal.
        4. The answer is `row[0]`.

      The traps: forgetting to reset `diag` from the old base cell at each row's start, and updating `diag` with the *new* value instead of the saved old one.

* **Time & Space Complexity (final, `79164cd`):**
    * **Time:** **O(n1 · n2)**, one compare and up to three reads per cell.
    * **Space:** **O(min(n1, n2))**: two rows of the shorter length + 1 ints.

### [10] Burst Balloons

> Brute force `bf85c46` (TLE), memo `c9597b1`, 2-D table `66c7ff2`. The wall was entirely the model: four rounds to get from "burst `i` first, its neighbours are `i ± 1`" to "`i` bursts **last**, its neighbours are the walls `start − 1` and `end + 1`". Then three transliteration bugs, all from the memo → table checklist. Recursion to table in under an hour once the model was right.

* **The Core Pattern:** interval DP (§2 Pattern 6), the Matrix Chain Multiplication shape. **`f(start, end)` = the most coins from bursting every balloon in `[start, end]`, given that the balloons at `start − 1` and `end + 1` (the walls) are still standing the whole time.** A wall off the end of the array is the problem's imaginary 1. Answer `f(0, n − 1)`.
    * **Candidate `i`** = "balloon `i` bursts **last** in this range". Its coins are `wall_left · nums[i] · wall_right`, and before it bursts, `[start, i − 1]` and `[i + 1, end]` burst on their own, with `i` as a wall for both.
    * **`max` across candidates, `+` inside one:** `best = max(best, coins(i) + f(start, i − 1) + f(i + 1, end))`.
    * **Base:** empty range (`end < start`) → 0.

* **Stumble 1: "pick the balloon that bursts first" (the famous trap).** My first plan used `i`'s **original** neighbours `i − 1` and `i + 1`, then recursed on the two halves. On `[3, 1, 5]` it answers **23**; the truth is **35**. The coins for the first burst are right. What breaks is everything after: **when a balloon bursts, the gap closes**, and the left half's last balloon slides next to the right half's first. Best real order: burst 1 (3·1·5 = 15), then 3, whose right neighbour is now **5** (1·3·5 = 15), then 5 (1·5·1 = 5). My left call on `{3}` saw "the edge of my range" and assumed "nothing there, use 1": 1·3·1 = 3. And peeking past the range doesn't fix it: whether 3 sees 5 depends on when 5 bursts, which the *other* call decides. **Bursting first couples the two halves.**

* **Why "last" fixes exactly that: `i` is a wall.** If `i` stays standing until everything else in `[start, end]` is gone, the gap at `i` never closes while the halves burst: the left half bursts against `i` and never sees past it, and the same for the right. The halves are genuinely independent. And when `i` finally bursts, everything inside the range is gone, so its neighbours are **whatever stands just outside the range**: `start − 1` and `end + 1`.
    * **Each call is a range plus two walls that outlive it.** The left child `[start, i − 1]` inherits the parent's left wall and gets `i` as its right wall; the right child gets `i` as its left wall and inherits the parent's right wall. By induction, `start − 1` and `end + 1` are alive for every call. That's why the walls come from `start`/`end` alone and the memo key is just `(start, end)`.
    * **Winning path on `[3, 1, 5]`**, read bottom-up = the real order:

      | call | walls | winning `i` (bursts last) | coins | + left | + right | best |
      |---|---|---|---|---|---|---|
      | `[0, 2]` {3,1,5} | 1, 1 | 5 | 1·5·1 = 5 | `[0,1]` = 30 | 0 | **35** |
      | `[0, 1]` {3,1} | 1, **5** | 3 | 1·3·5 = 15 | 0 | `[1,1]` = 15 | 30 |
      | `[1, 1]` {1} | **3**, **5** | 1 | 3·1·5 = 15 | 0 | 0 | 15 |

* **Stumble 2: "how do I pick `i`?" and "how can `i` be last if I compute it first?"**
    * You don't pick one `i`: the loop **tries every `i`** as a separate plan and keeps the best. The split pair I thought I'd miss (`[start, 1]` + `[2, end]`) is just the `i = 1` iteration, and each half runs its own loop.
    * **Code order isn't time order.** Nothing is ever removed from the array; each iteration *scores* a plan by adding three numbers, and addition doesn't care which you compute first. Matrix Chain does the same: the cost of the **final** multiplication is written in the same line as the two sub-chains it waits for.

* **Stumble 3: four rounds of code, each fixing one thing.** Smallest failing inputs, from fuzzing against a plain recursive reference:
    1. **`count += …` and `max(left, right)`: `+` and `max` swapped.** `[1, 5]` → 16, want 10 (it *added* plan A's 10 and plan B's 6). Only one plan happens, so plans compete with `max`; within a plan both halves burst, so their coins add.
    2. **Neighbours still `i ± 1`, bounded by `start`/`end`** → `[3, 1, 5]` still 23: the range edge treated as the world's edge.
    3. **Bounds moved to the array (`>= 0`, `< n`), indices still `i ± 1`** → still 23. "I haven't burst `i` yet, so `i − 1` is there" is true when you *choose* `i`, false when it *bursts*: `i − 1` is inside the range and gone by then. In the sub-call `{3, 1}` with right wall 5, "3 last" scored 1·3·**1** (the already-burst 1) instead of 1·3·**5**, so the winning "5 last" plan totalled 13 instead of 35. `i ± 1` equals the walls only when `i` sits at the range's edge, which is why `[1, 5]` passed.
    4. **`start − 1` and `end + 1`** with array-bound checks → correct. My worry that `start` now had "two meanings" dissolved: `start − 1` **always** means the wall; the only special case is a wall off the array (→ 1), checked against `0` and `n`, never against `start`/`end`. Padding the array with a 1 at each end would remove even that branch.

* **Stumble 4: the table (`66c7ff2`): three bugs, one per checklist row.**
    * **No cell for `end = −1` (the crash).** The memo's `if(end < start) return 0` ran *before* any array access; the table has no `if`, so `dp[i][k − 1]` at `k = i = 0` read `dp[0][-1]`. UBSan said *"addition of unsigned offset"*: `operator[]` takes `size_t`, so `−1` became 2⁶⁴ − 1. **That's the index, not the values**, so switching the cells to `unsigned` couldn't fix it. Checklist row 1: size the indices to include the base values (guard or shift).
    * **Empty-range cells started at 1.** Row `n` is never written by the loops, so `dp[n][n − 1]` (the empty right half when `k = n − 1`) kept the 1: `[5]` → 6. Base `return 0` means empty cells hold 0. Checklist row 2.
    * **`j` ran downward.** `dp[i][j]` reads `dp[i][k − 1]`: **same row, smaller column**, so `j` must go **upward**. `dp[k + 1][j]` is a lower row, so `i` downward was right. `[1, 5]` → 6: `dp[0][1]` read `dp[0][0]` = 0 a step before it was computed as 5. Checklist row 4: the direction comes from where the reads point. Physically: every range needs all its shorter sub-ranges first. The textbook loops over range length; `i` down / `j` up guarantees the same thing.
    * Cells stay `int`: worst case 300 bursts × 100³ = 3 × 10⁸ < 2³¹, and every `i ≤ j` cell is a real sub-range, so no unreached cell can blow up the way the counting tables did.

* **Why this table can't be rolled.** `dp[k + 1][j]` reads **every** lower row in column `j`, not just the previous one. No row can ever be discarded, so O(n²) space is the floor. First problem in the topic where the full table is the final form.

* **Review leftovers (declined, noted for a reread):** the comment in the loop still says "currently being blast", the *first* wording that cost four rounds; the left-half guard tests `k > 0` (works only because `dp[i][i − 1]` stays 0), where the meaning is `k == i`; `n == 0` reads `dp[0][-1]` (LeetCode guarantees `n ≥ 1`). Padding `nums` fixes the last two at once.

* **Measured** at `-O2`, random values 0–100, best of 15:

  | version | n = 300 | n = 1000 |
  |---|---|---|
  | memo `c9597b1` | 11.5 ms | 415 ms |
  | **2-D table `66c7ff2`** | **2.7 ms** | **107 ms** |
  | flattened + padded (not committed) | 2.5 ms | 97 ms |

    The table is ~4× the memo (no recursion, no sentinel check). Flattening buys 8–10%; the column walk `dp[k + 1][j]` jumps a row per step, but at n = 300 the table is ~360 KB and sits in L2, so it barely shows. Not worth chasing for an interview.

* **Time & Space Complexity (final, `66c7ff2`):**
    * **Time:** **O(n³)**: ~n²/2 ranges, up to n candidates each (~n³/6 inner steps). Standard for interval DP.
    * **Space:** **O(n²)**, and it stays there (see above).
