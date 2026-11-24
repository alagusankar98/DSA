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

### Pattern 3: Counting Knapsack → count ways to reach a target
Partition's table, but cells hold counts (`+`) instead of `||`.
* *Discipline:* items outer, amounts inner counts **combinations**; amounts outer counts **orderings**. Unlimited use → amounts upward; once each → downward. Reduce first if the problem allows (an assignment of `+`/`−` signs is a subset choice). Watch for negative indices: shift by an offset, or reduce to a non-negative target.

### Pattern 4: Position + Mode → a small state machine per step
`dp[i][s]` = best value at step `i` in mode `s`. Write the allowed transitions first, as a table or a diagram, then the code.
* *Discipline:* every mode gets a base value at step 0, including "impossible" ones (a sentinel that survives `+` and `−`). Usually rolls to one variable per mode, updated from the **previous** step's values (copy them first, or the order of updates leaks today's values into today).
* *Use for:* Best Time to Buy and Sell Stock with Cooldown (modes = holding / not holding; the cooldown is a `+2` jump, so the base is `i >= n` and the roll keeps a day-after-tomorrow variable).

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
