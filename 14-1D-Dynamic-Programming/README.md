# 1-D Dynamic Programming - Topic Guide

## 1. Core Fundamentals

Backtracking (topic 11) explored a **tree of choices**: at each step, try every option, recurse, undo. It was exponential, and it had to be, because every leaf was a different answer (every subset, every permutation). **Dynamic programming (DP) is what you do when the tree keeps asking the same question.** If a problem's recursion reaches the *same subproblem* through many different paths, solve each subproblem **once**, store the answer, and look it up every other time. That one change turns exponential into linear (or quadratic).

"1-D" means the stored answers fit in **one array indexed by one number**: a position in the input, an amount, a length. (2-D DP, topic 15, needs two indices: two strings, a grid.)

### The Vocabulary
* **Subproblem / state:** a smaller version of the question, named by a few numbers. "The number of ways to reach step `i`", "the most money from houses `0..i`", "the fewest coins to make amount `a`". **`dp[i]` is the answer to the subproblem named `i`.**
* **Recurrence (transition):** the formula for `dp[i]` in terms of **smaller** states. `dp[i] = dp[i-1] + dp[i-2]` is the whole of Climbing Stairs.
* **Base case:** the smallest states, answered directly, with no recurrence (`dp[0]`, `dp[1]`). Wrong base cases are the most common DP bug.
* **Overlapping subproblems:** the same state is needed by many larger states. This is what separates DP from plain recursion. Merge Sort's halves never overlap, so there's nothing to cache.
* **Optimal substructure:** the best answer to a big state is built from the **best** answers to smaller states. (Same idea as Dijkstra: the cheapest path to `v` ends with the cheapest path to some `u`, plus one edge.)
* **Memoization (top-down):** write the natural recursion, then add a cache. Before computing state `i`, check the cache; after, store it.
* **Tabulation (bottom-up):** no recursion. Fill `dp[]` with a loop from the base cases upward, so every state a formula needs is already filled.

### The Four Questions (ask them before writing any code)
1. **What does `dp[i]` mean, in one English sentence?** If you can't say it, you can't write the recurrence. Most of the difficulty of a DP problem is choosing this sentence.
2. **How does `dp[i]` come from smaller states?** Look at the **last decision**: the last step taken, the last house robbed or skipped, the last coin used, the last word in the split.
3. **What are the base cases?** The states the recurrence can't reach below.
4. **In what order, and where's the answer?** Usually `i` increasing, and the answer is `dp[n]`, or `dp[n-1]`, or the **max over all** `dp[i]` (when the answer can end anywhere, as in LIS).

### Why It's Exponential Without the Cache
Naive `fib(n) = fib(n-1) + fib(n-2)` calls itself **2·F(n+1) − 1** times: **331,160,281 calls for `fib(40)`**, for an answer that only has 41 distinct subproblems. Each state is recomputed once for every path down the call tree that reaches it. With a cache, each of the 41 is computed once: **41 calls**. That gap (exponential → linear) is the entire topic.

### Top-Down vs Bottom-Up
| | Memoization (top-down) | Tabulation (bottom-up) |
| -- | -- | -- |
| How you write it | the recursion you'd write anyway, plus a cache check | a loop over states in dependency order |
| States computed | only the ones actually reached | all of them |
| Cost per state | a function call (stack frame, return address, spills) | a few loads and an add |
| Danger | **stack depth**: n = 10⁴ means 10⁴ frames | getting the loop order wrong |
| Unlocks | easiest way to *find* the recurrence | **rolling variables** (below) |

Write it top-down to discover the recurrence, then convert to bottom-up. In interviews, bottom-up is the expected final form.

### Rolling Variables: O(n) Space → O(1)
If `dp[i]` only reads `dp[i-1]` and `dp[i-2]`, you never need the whole array: keep **two variables** and shift them each step (`prev2 = prev1; prev1 = cur;`). The array becomes two registers. Most of this topic's problems (stairs, robber, decode ways, max product) end up O(1) space.
* **You've already done this.** Cheapest Flights' Bellman-Ford (Advanced Graphs `[6]`) is a DP: `dp[r][v]` = cheapest price to `v` with at most `r` flights. The `prev`/`current` pair was a 2-D table rolled down to two rows, and `prev = current` was the shift.

### The Families in This Topic
* **Step recurrences:** `dp[i]` from `dp[i-1]` and `dp[i-2]`. Climbing Stairs (count ways = Fibonacci), Min Cost Climbing Stairs (min instead of +, and the top is one *past* the last step), Decode Ways (each step is only allowed if the 1 or 2 digits it consumes are a valid letter: `'0'` alone is not).
* **Take or skip:** at item `i`, either take it (and lose the right to take its neighbour) or skip it. House Robber: `dp[i] = max(dp[i-1], dp[i-2] + nums[i])`. House Robber II puts the houses in a **circle**, so the first and last can't both be taken: solve the line twice, once without the first house and once without the last.
* **Make a target from pieces (unbounded):** `dp[a]` = best way to build amount/prefix `a`, from `dp[a - piece]` over every piece. Coin Change (`dp[a] = 1 + min dp[a - c]`, with "impossible" as a sentinel), Word Break (suffix form: `dp[i]` = "`s[i..n)` splits into words", base `dp[n]` = true, true if some word starts at `i` and `dp[i + len]` is true. The prefix form, with base `dp[0]` and words ending at `i`, is the mirror image. Pick one and keep the base case, loop direction and matching consistent with it).
* **Each item at most once (0/1 knapsack):** Partition Equal Subset Sum. "Can some subset reach sum `t`?" for every `t` up to half the total. Iterate the sums **from high to low** for each item, so an item can't be counted twice in the same pass. It's Bellman-Ford's copy rule again ("don't read your own writes"), achieved by loop direction instead of a second array.
* **Best ending exactly here:** when the answer is a contiguous run, `dp[i]` = best run **ending at** `i`, and the answer is the max over all `i`. Maximum Product Subarray: a negative number turns the smallest product into the largest, so track **both** the max and the min ending at `i`.
* **Best ending here, from any earlier point:** Longest Increasing Subsequence. `dp[i]` = longest increasing subsequence ending at `i` = 1 + the max `dp[j]` over earlier `j` with a smaller value. That's O(n²). The O(n log n) version keeps the smallest possible tail for each length and uses binary search (`lower_bound`, the Binary Search topic).
* **Palindromes (expand around the centre):** Longest Palindromic Substring, Palindromic Substrings. Every palindrome has a centre, and there are **2n − 1** of them (each character, and each gap between two). Expand outward while the ends match. O(n²) time, O(1) space, with no table at all. It's on the DP list because the table version (`isPal[i][j]` from `isPal[i+1][j-1]`) is the textbook answer, but expanding is simpler and faster.

### Complexity Cheat Sheet
| Problem shape | Time | Space (after rolling) |
| -- | -- | -- |
| Step / take-or-skip (stairs, robber, decode) | O(n) | O(1) |
| Make a target, k pieces (Coin Change) | O(amount · k) | O(amount) |
| Word Break | O(n · L) checks, L = longest word | O(n) |
| Subset sum (Partition) | O(n · sum) | O(sum), or a bitset |
| Max/min ending here (Max Product) | O(n) | O(1) |
| LIS | O(n²) table, or O(n log n) tails | O(n) |
| Palindromes by expansion | O(n²) | O(1) |

### The Hardware Reality
* **Tabulation is a sequential walk over one array**, which is the prefetcher's best case. Rolling variables are better still: the state lives in **registers**, and the loop touches memory only to read the input.
* **The memo is a `vector`, not an `unordered_map`**, whenever the states are 0..n. Indexing is one load. A hash map is a hash, a bucket walk, and a node allocation per state.
* **Recursion depth is a real limit.** Top-down at n = 10⁴ is 10⁴ stack frames (the linker-script stack-limit instinct applies, and on an 8 KB embedded stack it would already be dead). Bottom-up has no depth.
* **Sentinels and overflow:** Coin Change's "impossible" must not be `INT_MAX`, because `1 + INT_MAX` overflows (the Advanced Graphs `INT_MAX + w` lesson again). Use `amount + 1`: no real answer can need more coins than that.
* **Subset sum as a bitset:** with `std::bitset`, "every sum reachable so far, plus this item" is `bits |= bits << x`. One shift and one OR handle 64 sums per machine word, instead of one `bool` per loop iteration.

### Where This Shows Up in the Real World
**Viterbi decoding** (convolutional codes in modems, Wi-Fi, GSM; speech recognition) is DP over time steps, and probably the most important DP in EEE. Sequence alignment in bioinformatics, diff tools, spell-checkers' edit distance (2-D DP). Knuth–Plass line breaking in TeX. Shortest paths in a DAG, and Bellman-Ford itself. Compilers' instruction selection (tree-pattern DP) and register allocation heuristics. Resource allocation and knapsack-style scheduling.

---

## 2. General Summary / Quick Reference

### Pattern 1: Step Recurrence → `dp[i]` from `dp[i-1]`, `dp[i-2]`, rolled to two variables
* *Use for:* Climbing Stairs, Min Cost Climbing Stairs, Decode Ways.
* *Discipline:* write the base cases first. Decode Ways: a `'0'` can only be the second digit of `10` or `20`.

### Pattern 2: Take or Skip → `max(skip, take + value)`
* *Use for:* House Robber, House Robber II (circle = two linear runs: drop the first house, or drop the last), Longest Increasing Subsequence with state `(i, prev)`.
* *Discipline:* House Robber II with one house has to return that house, because both runs would be empty. Skip is always legal, so compute it unconditionally; take only when legal, then `max`. Taking whenever you *can* is greedy (LIS `[1,5,2,3]`).

### Pattern 3: Make a Target from Pieces → `dp[target]` over every piece
* *Use for:* Coin Change, Word Break.
* *Discipline:* the base case is the empty remainder (`dp[0]` for 0 coins, `dp[n]` for Word Break's empty suffix). The "impossible" sentinel must survive `+ 1`. A piece only counts if the remainder it leaves is solvable: **read** that entry, don't just match the piece.

### Pattern 4: Each Item Once → 0/1 knapsack, sums iterated downward
* *Use for:* Partition Equal Subset Sum.
* *Discipline:* odd total → false immediately. State `(i, remaining)`; the answer is `dp[0][target]`, and an early return on the `target` column is valid. One row in place: iterate sums **high → low** so `row[left − x]` is still the previous item's value (low → high reuses the item = unbounded, Coin Change). With **two** rows, every cell must be written each row: don't start at `x` after a `swap`.

### Pattern 5: Best Run Ending Here → answer is the max over all `i`
* *Use for:* Maximum Product Subarray (track max **and** min), Longest Increasing Subsequence (the O(n²) `dp[i]` = LIS ending at `i` form).
* *Discipline:* the answer is not `dp[n-1]` when a run can end anywhere.

### Pattern 6: Palindromes → expand around 2n − 1 centres
* *Use for:* Longest Palindromic Substring, Palindromic Substrings.
* *Discipline:* odd and even centres both. Store the best `(start, length)`, not substrings, and build the string once at the end.

### Pattern 7: Smallest Ending per Length → sorted record + `lower_bound`
* *Use for:* Longest Increasing Subsequence in O(n log n).
* *Discipline:* `tails[k]` = the smallest ending of any increasing subsequence of length k+1. Overwrite the **first element ≥ x** (not the largest), or append. First ≥ for strictly increasing, first > for non-decreasing. `tails` holds the right length, not the actual subsequence.

### The Non-Negotiable Discipline
* **Say what `dp[i]` means in one sentence before writing code.**
* **Look at the last decision** to get the recurrence.
* **Base cases first, then loop order:** every state you read must already be filled.
* **A table fills every state.** Each recursion parameter becomes a loop; a variable carried across iterations means you're walking one path.
* **Top-down to discover, bottom-up to submit,** then roll the array if only the last one or two states are read.
* **Check the answer's location:** `dp[n]`, `dp[n-1]`, or the max over all.

---

## 3. Problem Strategies & Patterns

<!-- NeetCode 150 1-D Dynamic Programming (12): Climbing Stairs · Min Cost Climbing Stairs · House Robber · House Robber II · Longest Palindromic Substring · Palindromic Substrings · Decode Ways · Coin Change · Maximum Product Subarray · Word Break · Longest Increasing Subsequence · Partition Equal Subset Sum -->

### [0] Warm-up: Fibonacci Number (not on the NeetCode 150, not counted)

> Done to learn the four forms before the real problems: naive recursion → memoization `ef2c7e8` → tabulation `7d71aed` → rolled variables `028729e` (the final, three named ints). I'd read §1 and "understood somewhat". Writing all four made the vocabulary concrete.

* **The Core Pattern:** `dp[i] = dp[i-1] + dp[i-2]`, base cases `dp[0] = 0`, `dp[1] = 1`, answer `dp[n]`. It's the smallest possible DP, so all the effort goes into the *forms*, not the recurrence.

* **The "Gotcha":**
    * **My first memo wrote to the cache but never read it.** `return dp[n] = fib(n-1) + fib(n-2);` stores every answer and still makes all 2·F(n+1) − 1 calls. **A memo is a check *before* the recursion** (`if (dp[n] != -1) return dp[n];`), not just a store after it.
    * **`0` can't mean "not computed" when 0 is a real answer** (`fib(0) = 0`). The memo needs a sentinel no real answer can take (`-1`, since Fibonacci is never negative).
    * **Pre-filling `dp[0]` and `dp[1]` replaces the base-case `if` inside the recursion.** The filled slots stop it. The `n < 2` early return then lives in the entry function, where it also keeps `dp[1] = 1` in bounds when n = 0 (the vector has only one slot).
    * **Swapping `fibHelper`'s parameter order (on an LLM's style advice) without updating the call site** left the committed top-down version not compiling. Change a signature, grep its callers.
    * **"Bottom-up or memoization" is a contradiction.** Bottom-up = **tabulation** (a loop, from the base cases up). Top-down = **memoization** (recursion + cache, from n down). Interviewers notice when the two get mixed up.

* **The Struggle & Insights:**
    * **Tabulation doesn't need the sentinel.** The `-1` existed only because top-down has to ask "is this slot computed yet?". In a loop from 2 up, `dp[i-1]` and `dp[i-2]` are always written before `dp[i]` reads them. **Loop order replaces the check.**
    * **Rolling: the loop only ever reads the two slots just behind `i`,** so the other n − 1 are dead. Keep two values and shift them each step.
        * **My first roll was a `std::vector<int>(3)`.** That's still `operator new` + `operator delete` for 12 bytes. GCC `-O2` doesn't remove it. Measured (10M calls, n = 0..30): **vector 15–16 ns, `std::array<int, 3>` 6–7 ns, three named ints 6–7 ns** (14 instructions, all registers). The whole point of O(1) space is that the state lives in registers, and the vector put it back behind a pointer.
        * **LeetCode said the named ints were *slower*.** It times a microsecond of work in milliseconds, so it's measuring process start-up and server load. Below ~10 ms, LeetCode's runtime says nothing. Measure locally at `-O2`.
        * **Names in the right order:** `prev1` = one step back, `prev2` = two back, so the shift reads `prev2 = prev1; prev1 = current;`, "everything moves one step older". I had them reversed at first. It still worked (the sum doesn't care), but the shift lines read backwards, which is a trap when writing it from memory.
        * **`current` is only safe uninitialised because of the `n < 2` return,** which guarantees the loop runs at least once. Initialising it to 0 removes that dependency.
    * **When top-down actually wins:** when only a *few* states are reachable *and* they overlap. Coins `[50, 100]` → 1000 only ever reaches the 21 multiples of 50, while a table fills all 1001. (The LLM's other example, f(n) → f(n/2) or f(n−1), was misleading: each state calls just *one* smaller state, so nothing is ever reached twice and plain recursion already visits only ~15 states. No overlap = nothing for a memo to save.)
    * **Recursion depth, measured** (this machine, 8 MB default stack, the memo shape): **n = 10⁵ runs, n = 10⁶ segfaults**, at both `-O0` and `-O2`. ASan's bigger frames lower the limit. On an 8 KB embedded stack it would die at a few hundred. Bottom-up has no depth at all.
    * **`int` overflows at `fib(47)`** (2,971,215,073 > 2³¹ − 1). LeetCode caps n at 30. Ask about the constraint in an interview.

* **Time & Space Complexity:**

    | Form | Time | Space |
    | -- | -- | -- |
    | naive recursion | O(φⁿ), 2·F(n+1) − 1 calls | O(n) stack |
    | memoization | O(n) | O(n) memo + O(n) stack |
    | tabulation | O(n) | O(n) table |
    | **rolled (final)** | **O(n)** | **O(1)**, registers only |

### [1] Climbing Stairs

> First pass, the rolled form straight away: Fibonacci's loop with one base value changed (committed with the next problem, in `87f0798`). I listed every climb to 4 by hand and couldn't see how they came from the base cases. The wall was **which end to look at**: I was building climbs from the first step. The answer comes from the last one.

* **The Core Pattern:** step recurrence (§1 "The Families", §2 Pattern 1). **`dp[i]` = the number of distinct ways to reach step i.** `ways(n) = ways(n − 1) + ways(n − 2)`, with ways(1) = 1, ways(0) = 1. It's Fibonacci shifted by one: stairs gives 1, 2, 3, 5, 8, … and Fibonacci gives 0, 1, 1, 2, 3, 5, 8, …, so ways(n) = fib(n + 1).

* **The "Gotcha":**
    * **Look at the *final* move, not the first.** Standing on step n, the last move was a 1-step (from n − 1) or a 2-step (from n − 2), and nothing else. **Erase that final move** and what's left is a complete climb to the smaller step. The climbs to 3:

        | climb to 3 | final move | erase it → |
        | -- | -- | -- |
        | 1+1+**1** | 1-step | 1+1, a climb to 2 |
        | 2+**1** | 1-step | 2, a climb to 2 |
        | 1+**2** | 2-step | 1, a climb to 1 |

        Every climb to 2 appears exactly once in the top group, and every climb to 1 in the bottom one, so ways(3) = ways(2) + ways(1). Different final moves means no climb is counted twice, so it's **+**, not "and". My first example ("how do the base cases make up the 5 climbs to 4?") wasn't wrong, it was just looking from the wrong end. This is the §1 Four Questions' "look at the last decision" in its simplest form.
    * **ways(0) = 1, not 0.** I first said "n = 0, no steps, so 0". But standing still at the bottom *is* one way: the empty climb. The recurrence proves it, because ways(2) = ways(1) + ways(0) is only 2 if ways(0) = 1. **Counting DPs almost always have "one way to do nothing" as the base.** (LeetCode's n ≥ 1, so ways(0) only exists to start the recurrence. Starting the loop at 3 with ways(2) = 2 and ways(1) = 1 avoids it entirely.)

* **The Struggle & Insights:**
    * **The one-line change from Fibonacci** was mine: `prev2` (two steps back, ways(0)) starts at 1 instead of 0.
    * **Return `prev1`, and the early return disappears.** After every iteration, `prev1` holds ways(i) for the step just finished, so when the loop ends it holds ways(n), even when the loop never ran. The starting values (ways(1) = 1, ways(0) = 1) already *are* the answers for n = 1 and n = 0. Then `current` can live inside the loop body, so it never exists uninitialised. My version keeps the `n < 2` return and returns `current`. That's correct for LeetCode, but it returns 0 for n = 0, which disagrees with its own `prev2 = 1`. **The trick only works when `prev1`'s starting value is the right answer for every n the loop skips:** Fibonacci's `prev1 = fib(1)` covers n = 1 but not n = 0 (fib(0) is in `prev2`), so Fibonacci still needs its guard.
    * **Overflow checked:** ways(45) = 1,836,311,903, so `int` holds LeetCode's whole range with ~15% to spare.
    * Verified for every n = 1..45 against a brute force that enumerates every climb, under ASan/UBSan, 0 wrong.

* **Time & Space Complexity:**
    * **Time:** **O(n)**: one addition per step.
    * **Space:** **O(1)**: two or three ints, all in registers.

### [2] Min Cost Climbing Stairs

> First pass `87f0798`: `dp[i]` = cheapest total to stand on step i, **including** `cost[i]`, answer `min(dp[n-1], dp[n-2])`. Then `3cf0072`: no early return, rolled into two variables. Final `572737d`: `dp[i]` = cheapest cost to **arrive at** step i, before paying, so the top is an ordinary state and the answer is `dp[n]`. "It just isn't clicking": my first plan was a greedy walk backwards from the top, and I could already feel it breaking before I knew why.

* **The Core Pattern:** step recurrence with **min** instead of + (§1 "The Families", §2 Pattern 1). Same question as Climbing Stairs `[1]`: standing at the top, what was the final move? From n − 1 or from n − 2. Stairs **added** the two counts; here you **compare two totals** and keep the smaller.

* **The "Gotcha" — why my greedy walk fails:** my plan was to start at the top and step back to whichever of the two steps below was cheaper, until reaching index 0 or 1. I couldn't decide what to do on ties, and, more importantly, I worried that the cheaper choice might make a later step worse. That second worry is exactly the bug. Smallest tie-free case: **cost = [3, 2, 1]**.

    | from | candidates | greedy picks | paid |
    | -- | -- | -- | -- |
    | the top (3) | cost[2] = 1, cost[1] = 2 | index 2 | 1 |
    | index 2 | cost[1] = 2, cost[0] = 3 | index 1 | 3 |
    | index 1 is a start | | | **3** |

    The real answer is **2**: start on index 1, then jump straight to the top. Greedy compared **single prices**, but whether a step is a good last stop depends on **the cheapest total to get onto it in the first place**. Index 2 was cheap, but reaching it still meant paying for index 1. Compare the two **finished totals** (`dp[n-1]`, `dp[n-2]`) and nothing earlier can surprise you. **Ties stop mattering too:** `min` of two equal totals is the same number either way, and "later" is already inside each total. (With ties allowed, `[1, 1, 1]` breaks greedy as well: it pays 2, and the answer is 1.)

* **The Struggle & Insights:**
    * **Two correct `dp[i]` sentences, and the sentence decides the base cases, the loop range and where the answer is** (the §1 Four Questions, 3 and 4):

        | | "cheapest to stand on i, **including** `cost[i]`" (`3cf0072`) | "cheapest to **arrive at** i, before paying" (`572737d`) |
        | -- | -- | -- |
        | base cases | dp[0] = cost[0], dp[1] = cost[1] | dp[0] = dp[1] = 0 (both starts are free) |
        | rule | `cost[i] + min(dp[i-1], dp[i-2])` | `min(dp[i-1] + cost[i-1], dp[i-2] + cost[i-2])` |
        | loop | `i < n` (the top isn't a state, `cost[n]` doesn't exist) | **`i <= n`** (the top is an ordinary state) |
        | answer | `min(dp[n-1], dp[n-2])` after the loop | **`dp[n]`** |
        | n = 1 | reads `cost[1]` out of bounds | returns 0 (index 1 *is* the top) |

        In the first, I hit the out-of-bounds `cost[n]` and worked around it with a final `min`, and my first `dp` had n + 1 slots for a top I couldn't compute. That spare slot was the hint toward the second definition. **In the second, you pay for a step when you *leave* it**, so the rule only ever reads `cost[i-1]` and `cost[i-2]`, and `i = n` is in bounds. At i = 2 it looks like "compare the raw costs" (because dp[0] = dp[1] = 0), which confused me for a moment. From i = 3 on it's comparing real totals.
    * **Early returns that weren't needed:** my first pass returned `min_element` for n < 3, but with n ≥ 2 guaranteed that only caught n = 2, which the general path already handled. Of the two later versions, only the "including" one still relies on n ≥ 2.
    * **Measured** (fuzzed against exhaustive search on 200,000 random arrays, n = 2..15, under ASan/UBSan, 0 wrong for every version; `-O2`):

        | Version | n = 10 | n = 1,000 |
        | -- | -- | -- |
        | first pass `87f0798` (`dp(n+1)`) | 16–17 ns | 842–964 ns |
        | `dp(n)` table | 16–17 ns | 846–995 ns |
        | rolled, "including" `3cf0072` | 4.5–5.3 ns | 779–858 ns |
        | **rolled, "arrive" `572737d`** | 5.4–5.5 ns | 794–819 ns |

        Rolling is ~3.3× at small n, where the `vector` allocation dominates (the Fibonacci `[0]` lesson). At n = 1,000 it's only 7–20%, because **every step waits for the one before it**: `dp[i]` can't start until `dp[i-1]` exists, so there's nothing to overlap or vectorise. It's one chain of add-then-min, ~0.8 ns per step, and the table version only adds a store and a load per step on top of it. The two definitions tie within noise. Choose by clarity: "arrive" has no special cases.
    * **Naming:** I called the new value `temp` here. It's the same role as `current` in `[0]` and `[1]`, and one name for one role makes the shape easier to recall.

* **Time & Space Complexity:**
    * **Time:** **O(n)**: one add-and-min per step.
    * **Space:** **O(1)**: two ints.


### [3] House Robber

> First pass `466420e`: `dp[i]` = most money **if I rob house i**, looking back to i − 2 **and** i − 3, answer `max(dp[n-1], dp[n-2])`. Then `22fc0e3`: `dp[i]` = best money **if the street ended at house i**, a 2-cell lookback, answer `dp[n-1]`. Final `48e2004`: the same, rolled into two variables. Before any of that, a decision tree gave me an odd/even-sum idea that passed both examples. I didn't follow the backwards-table explanation at all ("did not understand anything"). What got me unstuck was a video on a Zonal Computing Olympiad 2014 problem (30 Nov 2013) with the same shape.

* **The Core Pattern:** take or skip (§1 "The Families", §2 Pattern 2). **`dp[i]` = the most money from houses 0..i, whether or not house i itself is robbed.** At house i you rob it (`nums[i] + dp[i-2]`, so house i − 1 is off-limits) or skip it (`dp[i-1]`), and you keep the larger. Rolled: two ints, both starting at 0, and no base cases.

* **The "Gotcha" — why odd/even sums fail:** I drew the decision tree for `[1,1,3,3]` and `[2,9,8,3,6]`. Both best picks looked like every other house, so I coded "max of the even-index sum and the odd-index sum". It passed both examples and failed **`[2,1,1,2]`**:

    | index | 0 | 1 | 2 | 3 | total |
    | -- | -- | -- | -- | -- | -- |
    | even sum | ✓ | · | ✓ | · | 3 |
    | odd sum | · | ✓ | · | ✓ | 3 |
    | **best** | ✓ | · | · | ✓ | **4** |

    The best pick skips **two** houses in a row. Odd/even forces the gap between robbed houses to be exactly one. **The gap can be one or two, never three:** with three skipped houses, you can always rob the middle one too, since the values are never negative. So **not robbing is a real choice**, and my tree didn't have a branch for it. I had branched on "which house next" and should have branched on "this house: rob or skip".

* **The Struggle & Insights:**
    * **The ZCO bridge.** In the video's problem, `dp[i]` was the minimum time **if I work on day i**, built as `cost[i] + min` of three earlier values (every place the previous working day could be), with a final min over the last few cells because the last day didn't have to be worked. I carried it over as "`dp[i]` = most money if I rob house i", and first wrote `dp[i] = nums[i] + dp[i-2]`. **That's the odd/even sum again:** it says the previous robbed house is *always* i − 2, so on `[2,1,1,2]` it gives `dp[3] = 2 + dp[1] = 3`. The fix was the ZCO question, "if I rob house i, where could the previous robbed house be?": not i − 1 (next door), i − 2 or i − 3 (one or two skipped), and never i − 4 (see above). So `nums[i] + max(dp[i-2], dp[i-3])`, and the answer is `max(dp[n-1], dp[n-2])` because the last house might be skipped. Correct first time (200,000 fuzzed arrays against brute force, ASan/UBSan clean).
    * **Why "if I rob house i" needs a 3-cell lookback here but only 2 in Min Cost Climbing Stairs `[2]`:** there, you *have* to land on i − 1 or i − 2, because a jump is at most two steps. Here, skipping is free, so the gap varies, and a "must rob i" state has to list every possible gap. That costs three hand-written base cases (`dp[2] = nums[0] + nums[2]`), an `n < 3` path, and the final `max`.
    * **What "whether or not house i is robbed" means:** I didn't get it at first. The phrasing that worked: **"if the street ended at house i, what's the most I could steal?"** House i may or may not be in that best pick. The two definitions, side by side on `[2,1,1,2]`:

        | i | street if it ended here | "must rob i" (`466420e`) | "street ends at i" (`22fc0e3`) |
        | -- | -- | -- | -- |
        | 0 | `[2]` | 2, rob {0} | 2, rob {0} |
        | 1 | `[2,1]` | **1**, forced to rob {1} | **2**, rob {0}, house 1 left alone |
        | 2 | `[2,1,1]` | 3, rob {0,2} | 3, rob {0,2} |
        | 3 | `[2,1,1,2]` | 4, rob {0,3} | 4, rob {0,3} |

        **The i − 3 case is already inside `dp[i-2]`:** "the best for the street ending at i − 2" covers both "robbed i − 2" and "robbed i − 3, skipped i − 2". So the lookback drops to two, the last cell is the answer, and the `n < 3` path disappears. `dp[1] = max(nums[0], nums[1])` is exactly "the street `[a, b]`: take the better house".
    * **"Why from the last?"** The first explanation I got filled a table from the back, with `f(i)` = the best from house i **to the end**. That reads from the right, so it has to be filled right to left. **The direction comes from the definition:** fill so that every cell a formula reads is already there. `f` (suffix) and the "street ends at i" `dp` (prefix) are mirror images with the same answer. Front-to-back is the usual one, and it matches the stairs problems.
    * **Rolling (`48e2004`):** keep "best up to the previous house" and "best up to the one before it", both starting at **0**, which stands for the houses before index 0. Then every house goes through the same update: one house gives `max(nums[0] + 0, 0)`, and **an empty street returns 0**. Both earlier versions crashed on empty input (the first in `max_element` of an empty range, the second reading `nums[0]`). LeetCode's n ≥ 1 hides that.
    * **Measured** (`-O2`):

        | Version | n = 100 | n = 100,000 |
        | -- | -- | -- |
        | "must rob i", 3-lookback `466420e` | 136–139 ns | 133–135 µs |
        | "street ends at i", table `22fc0e3` | 65–68 ns | 58–62 µs |
        | **rolled `48e2004`** | **55 ns** | **52–56 µs** |

        **The 2× was the 3-cell lookback, not the `vector`.** The review first blamed the allocation, and the table version disproved that: it matched a two-int version at both sizes. At n = 100 one `malloc` is spread over 100 steps (Min Cost's 3.3× was at n = 10). Rolling gave the last 10–15%. In an interview it matters as the **O(1) space** answer to the follow-up, not as a speed-up.

* **Time & Space Complexity:**
    * **Time:** **O(n)**: one add-and-max per house.
    * **Space:** **O(1)**: two ints.

### [4] House Robber II

> First pass `170f447`: run House Robber `[3]` on two lines (houses 1..n−1 and houses 0..n−2) and keep the larger, with `n == 1` handled separately. The helper was a lambda that passed its result out through a captured `prev1`, so `prev1` had to be reset by hand between the two calls. Final version: a pure helper (array, start, end) → best money on that line. Before any code I got stuck trying to **rotate** the circle into a line.

* **The Core Pattern:** take or skip on a circle = **two linear runs** (§1 "The Families", §2 Pattern 2). Houses 0 and n − 1 are neighbours, so **at least one of them is not robbed** in any valid answer. Case A: house 0 isn't robbed, so delete it, and houses 1..n−1 are a plain line. Case B: house n − 1 isn't robbed, so houses 0..n−2 are a plain line. Every valid answer is in at least one case, and some (neither end robbed) are in both, which `max` doesn't mind. Answer = `max(line(1, n), line(0, n-1))`.

* **The "Gotcha" — why rotating the circle fails:** my idea was that the only new rule is "0 and n − 1 are connected", so process the houses in the order n − 1, 0, 1, …, n − 2 and run the same algorithm. But **a circle has no ends**: whatever order you pick, the first and last houses of the line are neighbours in the circle. Rotating only moves the broken pair. Smallest case, `[2,3,2]`, where all three houses touch and the answer is **3**:

    | line position | house | value | best so far |
    | -- | -- | -- | -- |
    | 0 | 2 | 2 | 2 |
    | 1 | 0 | 2 | 2 |
    | 2 | 1 | 3 | 3 + 2 = **5** |

    The 5 robs houses 2 and 1, which are next door. Before rotating, the unchecked pair was (0, n − 1). After, it's (n − 1, n − 2). **The fix isn't a better order. It's a case split that makes the conflict impossible:** decide up front which end stays unrobbed, and the circle becomes a line.

* **The Struggle & Insights:**
    * **n = 1 is the one special case.** Both runs delete the only house, so both lines are empty and both return 0, but the answer is `nums[0]` (the §2 Pattern 2 discipline). n = 2 needs nothing: each line is one house. n = 0 returns 0 with no crash, because the `n == 1` check never reads `nums[0]` and neither loop runs.
    * **Give the helper a range, not a copy.** `(start, end)` over the same `const&` vector, so no sub-vector is built. The helper is word for word the `[3]` rolled loop.
    * **Return values, not captured state.** In the first pass, `prev2` was a local (it reset itself) and `prev1` was captured by reference from outside (it didn't), so line 16's `prev1 = 0;` held the whole thing up. **Measured: with that one line deleted, 80,985 of 100,000 fuzzed circles are wrong** with no error, because the second run starts out believing the first run's answer is its "best so far". As a pure function, both variables are locals that start at 0 on every call, the hand reset and the `bestVal` copy disappear, and `rob` is one line: the max of two calls. **If a helper needs a reset between calls, it should be returning a value.**
    * **Measured** (`-O2`; all versions fuzzed against a brute force over every valid circle pick, 200,000 cases, ASan/UBSan clean):

        | Version | n = 100 | n = 100,000 |
        | -- | -- | -- |
        | lambda with captured `prev1` `170f447` | 112–121 ns | 105 µs |
        | **pure helper (final)** | 112–115 ns | 105–108 µs |
        | one fused pass, both lines at once | 115–140 ns | 107–123 µs |

        **The capture cost nothing:** GCC inlines the lambda and keeps `prev1` in a register, so the rewrite was about correctness, not speed. **Fusing both lines into one loop is slower:** the two `if (i < n-1)` / `if (i > 0)` tests inside the loop cost more than reading a 400-byte array twice from L1.

* **Time & Space Complexity:**
    * **Time:** **O(n)**: two passes of n − 1 houses.
    * **Space:** **O(1)**: two ints per pass.

### [5] Longest Palindromic Substring

> First pass `de7b458`: expand around every centre, with a helper that builds a `std::string` for each candidate. Then `ac6cd06`: the helper returns (start, end) indices and the string is built once at the end. Final `d585508`: the helper returns a **length**, and the start is recovered as `i − (L − 1) / 2`. Afterwards, for the pattern only: the `isPal` DP table `29d3e9a`, then the same with an early exit (the file now holds the DP version; the interview answer is `d585508`). "Can't even come up with recursion, let alone DP." I read some solutions, and the only one that made sense was expanding from a character. Then I had two worries: which answer to return when every palindrome has length 1, and whether one pass can handle odd and even. The off-by-one in the helper took an LLM conversation with two wrong attempts.

* **The Core Pattern:** expand around **2n − 1 centres** (§1 "The Families", §2 Pattern 6). Every palindrome has a centre: a **character** (odd length, `aba`) or the **gap between two characters** (even length, `abba`). There are n of the first and n − 1 of the second. At each `i`, expand from (i, i) and from (i, i+1) with the **same** helper (while in bounds and `s[left] == s[right]`, step both outward). Only the starting pair differs. Starting the odd case at (i, i) makes the first comparison a character against itself, so a separate "check i − 1 and i + 1" step isn't needed.

* **The "Gotcha":**
    * **Odd-only misses every even palindrome.** `"cbbd"` → `"bb"` only exists at the gap centre (1, 2). With character centres alone, the answer would be `"c"`.
    * **"abcd": which one-letter answer?** Any of them. The problem accepts any longest palindrome (LeetCode's `"babad"` takes `"bab"` or `"aba"`). Replacing only on a strictly longer length (`>`) keeps the first one found. A single character is always a palindrome, so for n ≥ 1 the answer is at least 1 long.
    * **The off-by-one.** The loop stops when the check **fails**, so `left` and `right` sit one step **past** the palindrome on each side. The palindrome is `left + 1 .. right − 1`, so **start = `left + 1`** and **length = (right − 1) − (left + 1) + 1 = `right − left − 1`**. My two wrong attempts:
        * **`std::abs(left)`:** treated `left = −1` as a bad value to clean up. It's a real position, "one before index 0", and `left + 1 = 0` already handles it.
        * **`(right − 1) − (left − 1) + 1`:** a sign slip. The first valid index is one step *right* of where `left` stopped. On `"babad"` at (0, 0), this gives 3 for a palindrome of length 1.
    * **The LLM's test comment was wrong too:** it said centre (1, 2) of `"babad"` gives `bab`. (1, 2) is the gap `a|b`, which fails immediately. `bab` comes from the character centre (1, 1).

* **The Struggle & Insights:**
    * **The recursion was hiding in the expansion:** `s[i..j]` is a palindrome **exactly when** its ends match **and** `s[i+1..j−1]` is a palindrome. That's the textbook `isPal[i][j]` table. Expanding checks the same rule from the inside out (the inside was confirmed one step earlier), so it's the DP's logic with nothing stored.
    * **"Isn't a table on i and j 2-D DP?"** Yes. By §1's own definition (the state needs one number), the `isPal` table is 2-D DP: n² states. NeetCode files the problem under 1-D because it groups by **input shape** (one string) rather than state shape. What matters is the cost (next bullet).
    * **The DP table, measured** (n = 1,000, `-O2`, scratch only): **920–950 µs on random text vs 3–4 µs expanding (~250×)**, and still 2.3× slower on expanding's worst case, all `'a'`. The table always fills all n²/2 cells, plus a megabyte of memory. Expanding stops after a step or two at most centres on real text. Same O(n²) on paper, very different in practice. **Its value is the pattern, not the speed:** it's the first *interval* DP, where the state is a range (i, j), and that comes back in Burst Balloons (2-D DP). **Fill order:** `isPal[i][j]` reads `isPal[i+1][j−1]`, one row *down* and one column *left*, so the table fills by increasing length, or with `i` from n − 1 *down*. It's the "why from last?" question again: the formula decides the direction.
    * **Don't build a string per candidate.** The first pass built 2n `std::string`s. libstdc++ stores strings of up to 15 characters inside the object (the small-string optimisation), so short ones don't `malloc`, but each is still built, compared and destroyed, and long palindromes allocate and copy their whole length. Tracking (start, length) and calling `substr` once: **2.5–4× on typical input** (7.7–12.7 µs → 2.5–5 µs at n = 1,000).
    * **`s.data() + left + 1` was technically UB.** It evaluates as `(s.data() + left) + 1`, and with `left = −1` the first step makes a pointer **one before the start** of the array. One-past-the-end is legal; one-before-the-start isn't. No sanitizer reports it (checked with `-fsanitize=undefined,pointer-overflow`), and every compiler does the obvious thing, but it's the line a systems interviewer circles. Returning indices or a length makes it impossible.
    * **(start, end) can't say "nothing found".** `ac6cd06`'s first draft started at `start = end = 0`, which is one character, so `""` returned a 1-byte string holding `\0` (`std::string`'s terminator read as data, and a read out of bounds for a `string_view` that isn't null-terminated). The committed version guards it. **(start, length) starting at (0, 0) means "nothing" by itself**, with no guard and no `+ 1`.
    * **Getting the start from the length:** expansion is symmetric, so a palindrome of length L at centre i reaches the same distance each way. **start = `i − (L − 1) / 2`** for both kinds, with integer division: odd L = 2k + 1 gives `i − k`, and even L = 2k gives `i − k + 1`, because the left half ends at `i`. Checked on `"babad"` (i = 2, L = 3 → 1, `aba`), `"cbbd"` (i = 1, L = 2 → 1, `bb`) and `"abba"` (i = 1, L = 4 → 0). With lengths, taking `max(odd, even)` first means one comparison per centre.
    * **A comparison against a stale best.** The first (start, end) draft computed the best length once, before both checks, so the second check could in principle replace a longer palindrome with a shorter one. Instrumented over 2,000,000 random strings it **never** did, and it can't: both centres at the same `i` beating the old best needs a run of one letter, where the even centre at `i − 1` was already at least as long. Correct, but only through a proof the reader has to do. Compare against the current best each time.
    * **Beyond O(n²):** Manacher's algorithm is O(n). It reuses the mirror image of palindromes already found inside a bigger one, so it never re-checks characters. Measured: 40× on all-`'a'`, but **2× slower on random text** at n = 1,000 (two arrays plus bookkeeping). Know the name and the mirror idea. Interviews don't expect the code. Skipping centres that can't beat the best (a centre at `i` reaches at most about 2·min(i, n−1−i)+2) halves the worst case and does nothing on typical input.
    * **Measured, all versions** (`-O2`, n = 1,000; every version fuzzed against brute force over every substring, 200,000 strings, ASan/UBSan clean):

        | input | first pass `de7b458` | (start, end) `ac6cd06` | **length `d585508`** |
        | -- | -- | -- | -- |
        | random, 26 letters | 7.7–11.1 µs | 3.0 µs | **2.5–2.8 µs** |
        | random, 2 letters | 12.6–12.7 µs | 4.9–5.3 µs | **4.6–4.9 µs** |
        | all `'a'` | 352–411 µs | 300–368 µs | 299–365 µs |

        The two index versions tie within noise. An apparent 15–20% worst-case gap disappeared on back-to-back reruns (the run-to-run spread was bigger).

* **The DP Version (coded afterwards, for the interval-DP pattern, not for submission):**
    * **Push vs pull, the doc line I didn't see the point of.** "If (i, j) is a palindrome and `s[i−1] == s[j+1]`, then (i−1, j+1) is one" and "if `s[i] == s[j]` and (i+1, j−1) is a palindrome, then (i, j) is one" are the **same fact in opposite directions**. The first goes *outward* from an answer you already have: that's expand-around-centre. The second goes *inward* from the cell you're filling, which is what a table needs, because the loop visits **every** (i, j), including the "no" cells, and each one must read already-filled cells. Written as one line, the second is the recurrence I couldn't find: **`isPal[i][j] = s[i] == s[j] && isPal[i+1][j−1]`**.
    * **Fill by difference d = j − i:** 0, 1, 2, …, n − 1. Cell (i, j) reads (i+1, j−1), whose difference is **d − 2**, so that diagonal is already done. d = 0 is always true. At d = 1 only the ends count (the inside is empty). These two have nothing inside to read, so handle them first, or fold them in with `d < 2 || isPal[i+1][j−1]`. Short-circuiting means the out-of-range cell is never read. The other valid order is `i` from n − 1 down with `j` going up. On `"abba"`, d = 0 is all true, d = 1 has only `bb` (1, 2), d = 2 has nothing (`abb`, `bba`), and d = 3 gives `abba` because `a = a` and (1, 2) is true.
    * **The bug: the difference is not the length.** My first table recorded `j − i` as the length, so every answer came out one short: `"aba"` → `ab`, `"abba"` → `abb`, `"bb"` → `b`. Single letters vanished entirely (`"a"` → `""`), because difference 0 is never `> 0`. An inclusive range i..j has **j − i + 1** characters. It's the same off-by-one as the expand helper's `right − left − 1`, from the other side. Also: a `size_t i` against `int n` (two `-Wsign-compare` warnings), and an `if` in the main loop that was mostly dead. Its `(i + 1) < n` always holds because the loop condition `i + d < n` with d ≥ 2 already implies `i + 1 < i + 2 ≤ i + d < n`. That check was only needed in the first loop, where `dp[i][i+1]` really can run off the end.
    * **The early exit: "a palindrome contains a smaller one" works as a stopping rule too.** Strip both ends of a palindrome with difference d + 2 and you get one with difference d. So **an empty diagonal d means d + 2, d + 4, … are all empty.** Odd lengths live on even d and even lengths on odd d, so it takes **two consecutive empty diagonals** to stop: one empty diagonal kills only its own parity (`"abcba"` has d = 1 and d = 3 empty, but d = 2 `bcb` and d = 4 `abcba` are true). On `"abcd"`, d = 1 and d = 2 are both empty, so it stops there. My first question was "aren't the (i, i) cells all true?" Yes, but "diagonal" means every cell with the same d, and the exit only applies from d = 2. **It goes in the outer loop, not the lambda:** a `currentAny` reset per diagonal, a `previousAny` shifted after the check, and `previousAny` seeded from the d = 1 cells in the first loop. Seeding it `false` would make `"abba"` stop at d = 2 and miss `abba`.
    * **Why the exit gave 2.5–4.5×, not 150×:** it cut the *filling* to a few diagonals, but the n × n table is still *built* in full. **Constructing `vector<vector<uint8_t>>(1000, vector<uint8_t>(1000, 0))` alone takes 470–625 µs:** 1,001 allocations and 1 MB zeroed. That's nearly all of what's left. (The review first called the next item "memory, not speed". It's only memory without the exit. With the exit, it's the speed-up.)
    * **Not done, stopped here on purpose: roll the table into three arrays.** It's House Robber's rolling, with arrays instead of ints. `twoBack[]` (diagonal d − 2), `oneBack[]` (d − 1), `current[]` (d), each indexed by `i`, so `dp[i+1][j−1]` becomes `twoBack[i+1]`. `current` reads only `twoBack`. `oneBack` is kept because it becomes `twoBack` next, as `prev1` did. Shift with **`std::swap`** (swap `twoBack` and `oneBack`, then `oneBack` and `current`): it exchanges the vectors' internal pointers, O(1), with no copying. `current`'s leftover data is overwritten before it's read, like `temp`. Seed `twoBack` with all 1s (d = 0) and `oneBack[i] = s[i] == s[i+1]` (d = 1). Bounds: diagonal d has n − d cells, the largest read is `twoBack[n − d]`, and diagonal d − 2 has n − d + 2 cells, so arrays of n are enough. **1 MB → 3 KB, 1,001 allocations → 3.** In a scratch version: ~500 → 6 µs on random text.
    * **Measured** (`-O2`, n = 1,000; both of my DP versions fuzzed against brute force, 200,000 strings, ASan/UBSan clean):

        | input | DP table `29d3e9a` | + early exit (mine) | + exit + 3 rolled arrays (scratch) | expand `d585508` |
        | -- | -- | -- | -- | -- |
        | random, 26 letters | 830–1270 µs | 470–500 µs | 6 µs | 3 µs |
        | random, 2 letters | 1990–2570 µs | 540–610 µs | 57 µs | 5 µs |
        | all `'a'` | 940–1350 µs | 1400–1420 µs | 540–560 µs | 280–290 µs |

        Smaller levers, measured but not worth doing on a version I won't submit: one flat n × n buffer instead of a vector of vectors (~1.5×), and row order (`i` descending) instead of the diagonal walk, which jumps a row (a new cache line) on every cell (2–4× total). Row order rules out the length-based early exit. **Even the best DP version is 2× behind expand. Expand is the interview answer, and the table is the follow-up: "yes, and here's how to make it cheap."**

* **Time & Space Complexity:**
    * **Time:** **O(n²)** worst case (all one letter); close to O(n) on typical text. DP table: Θ(n²) always; with the early exit, O(n · longest palindrome) for the filling, plus the Θ(n²) table build.
    * **Space:** **O(1)** besides the returned string. DP table: O(n²), or O(n) with three rolled diagonals.

### [6] Palindromic Substrings

> First pass `b3e7170`: the `isPal` DP table from `[5]`, with d = 0 and d = 1 folded into one rule and the two-empty-diagonals early exit, counting true cells. Final `c15ffe4`: expand around the 2n − 1 centres and add up how many palindromes each centre holds. Coming straight off `[5]`'s DP, I reached for the table first, though `[5]`'s own lesson was that expanding is the better answer.

* **The Core Pattern:** expand around 2n − 1 centres (§1 "The Families", §2 Pattern 6), **counting instead of maximising**. Every successful expansion step at a centre is exactly one more palindrome: (i, i) itself, then (i − 1, i + 1), and so on. So a centre's count is **the number of steps it expanded**, and the answer is the sum over all centres. `[5]`'s `formPalindromeLength` helper is reused unchanged. Only the step from length to count is new.

* **The "Gotcha":**
    * **Turning a length into a count.** I reused `[5]`'s `(L − 1) / 2` twice (first with `i` subtracted, then without) before writing out what a length means at each kind of centre:

        | centre | lengths | palindromes held | count |
        | -- | -- | -- | -- |
        | odd (i, i) | 1, 3, 5, 7 | 1, 2, 3, 4 | **(L + 1) / 2** |
        | even (i, i+1) | 0, 2, 4, 6 | 0, 1, 2, 3 | **L / 2** |

        `(L − 1) / 2` is the **start offset** (how far a palindrome reaches left of its centre), not how many it holds. The version with `i` was summing positions. **When a formula won't come, tabulate the small cases and fit it.** That worked first time once I made myself do it.
    * **Merging the two into `(odd + even + 1) / 2` is only safe because of parity.** Integer division rounds down, so (a + b) / 2 ≠ a/2 + b/2 in general (1/2 + 1/2 = 0, but (1 + 1)/2 = 1). Here `odd` is always odd (so `odd + 1` is even) and `even` is always even, so each half divides exactly and nothing is rounded away. The committed line keeps the long form as a comment.
    * **`count += count += x`: a typo that doubles the total.** The inner `+=` runs first and its result is added again, so each step does `count = 2 · (count + x)`. `"a"` returns 2, and `"acbbc"` returns 78 instead of 7. **No warning, no sanitizer:** since C++17, the right side of `a += b` is fully evaluated before the left, so the line is well-defined and simply wrong. Before C++17 it was undefined behaviour. The fuzzer caught it.

* **The Struggle & Insights:**
    * **The DP first pass was good DP and the wrong choice.** One loop from d = 0 with `s[i] == s[j] && (d < 2 || dp[i+1][j−1])`, so there was no separate base loop and no seeding to get wrong, and `count += dp[i][j]` had no branch. Correct on the first try. But expanding needs no table, and counting needs even less than `[5]` did: no start, no best. **Measured at n = 1,000: table 478–567 µs, expand 2.4–3.6 µs on random text (~200×).** The table's cost is again mostly *building* it: 1,001 allocations and 1 MB zeroed. Even rolled into three arrays (the `[5]` follow-up, which I already knew) it's 5–47 µs, 2–10× behind expanding.
    * **`currentAny = currentAny || dp[i][j]` vs `|=`: no difference at `-O2`.** My idea was that `||` short-circuits, so it skips reading `dp[i][j]` once the flag is true. I compared the assembly. Both inner loops are branch-free and differ only in register names. Two reasons. **The value is already in a register** (`count += dp[i][j]` loaded it on the line before). And **`||` only promises the right side behaves as if it weren't evaluated:** when evaluating it has no visible effect, the compiler may compute it anyway (the "as-if" rule) and drop the branch, because one `or` is cheaper than a branch per cell. At `-O0` the `||` version really does branch, which is where the intuition comes from. **Short-circuiting matters when the right side has side effects, is expensive, or would be invalid to evaluate:** `d < 2 || dp[i+1][j−1]` is exactly that last case, where the short-circuit is what prevents the out-of-range read.
    * **Worst case fits in `int`:** 1,000 `'a'`s gives n(n+1)/2 = 500,500, since every substring is a palindrome.
    * **Manacher counts too:** the answer is the sum of the palindrome radius at every centre, O(n). Measured: 6–7 µs on all-`'a'` vs ~280 µs expanding, but 2× behind on random text. Name it, don't code it.
    * **Measured** (`-O2`, n = 1,000; all versions fuzzed against brute force over every substring, 200,000 strings, ASan/UBSan clean):

        | input | DP table `b3e7170` | 3 rolled arrays + exit (scratch) | **expand `c15ffe4`** | Manacher (scratch) |
        | -- | -- | -- | -- | -- |
        | random, 26 letters | 478–487 µs | 5 µs | **2.4–2.5 µs** | 5–8 µs |
        | random, 2 letters | 557–567 µs | 35–47 µs | **3.6 µs** | 6–14 µs |
        | all `'a'` | 1650–1710 µs | 420–520 µs | 270–300 µs | **6–7 µs** |

* **Time & Space Complexity:**
    * **Time:** **O(n²)** worst case (all one letter); close to O(n) on typical text.
    * **Space:** **O(1)**.

### [7] Decode Ways

> Brute-force recursion through three wrong versions, then memo `e7d619b`, table `0eee9b4` (its commit message says "memoization", but it holds the table), rolled `f1f816d`, and finally a branch-free rolled loop after the review. My first attempt counted valid **pieces** (one loop for 1-digit, one for 2-digit), and only after coding it did I see that a "way" has to decode the **whole** string. Then I hit the real wall: **"how can you say a way exists before you've read to the end?"** Two explanations bounced off ("I still don't understand"). The tree of walks got through. Even with a passing memo, I felt I didn't understand what I'd written.

* **The Core Pattern:** step recurrence with **gates** (§1 "The Families", §2 Pattern 1). It's Climbing Stairs, where each step is allowed only if the digits it consumes form a letter. **`finish(p)` = the number of ways to finish decoding from position `p` to the end.** From `p`, bite 1 digit (allowed if it isn't `'0'`) and continue from `p + 1`, or bite 2 digits (allowed if 10–26) and continue from `p + 2`. Add the two, where a blocked bite contributes 0. `finish(n) = 1`, and the answer is `finish(0)`. Filled from n down to 0, then rolled into two ints.

* **The mental model — a way is counted only when it reaches the end:** you never decide in advance that a way exists. You **walk**: bite, move, bite again, and count a walk only at the moment the string runs out exactly. `"226"`:

    ```
    start (226)
    ├── "2"  → (26)
    │     ├── "2"  → (6) ── "6" → END ✓    2|2|6
    │     └── "26" → END ✓                 2|26
    └── "22" → (6) ── "6" → END ✓          22|6
    ```

    A walk that gets stuck (`"206"`: `2` → `06`, where neither `0` nor `06` is a letter) contributes 0, and nothing was counted early. **"(6)" appears twice:** once after `2|2` and once after `22`, with the same remaining digits and the same answer. That repeat is the DP. The prefix framing ("if the message stopped here, how many readings?", the same as House Robber's "if the street ended here") is the mirror image, filled front to back, but it didn't click for me. The walk did.

* **The "Gotcha" — three wrong recursions, one bug each:**
    1. **The next bite started at `start + 1` after a 2-digit bite.** My helper took a chosen bite (start, end) and recursed with `start + 1` in both calls. After a 2-digit bite that reuses its second digit: on `"226"` it produced `22|2|6` and `22|26` (index 1 used twice), missed `22|6`, and returned **4**. `"2101"` and `"11106"` gave 0. **The next bite starts at `end + 1`**, the first unused digit. The same design is why main needed two calls: the helper meant "this bite is already chosen", so the caller had to choose the first one. Changing the helper to mean **"the position where the next bite begins"** gave one call from main, with both bites tried inside it.
    2. **The 2-digit check was backwards:** `s[start] == '2' && s[start+1] > '6'` is true exactly for **27–29**, the 2-digit bites that *aren't* letters. Valid means `'1'` with anything (10–19), or `'2'` with at most `'6'` (20–26). A leading `'0'` is already excluded by the outer gate, which is why the 2-digit test is nested inside it.
    3. **The base case `start + 1 == n` fired with one digit still left,** counting it unchecked. `"10"` counted the invalid `1|0`, and got the right total only because bug 2 had skipped the real answer `10` (two bugs cancelling). Worse, after a 2-digit bite ending exactly at the end, `start` became `n`, the base never fired, and the code read `s[n]`. **ASan: heap-buffer-overflow** when the buffer has no `'\0'` after it. `std::string`'s terminator hides it. **The base case is `start == n`: nothing left.**

* **The Struggle & Insights:**
    * **Correct but exponential:** with the bugs fixed and nothing reused, all-`'1'` strings (every split valid) grow like Fibonacci. Measured: **n = 30: 8 ms, 35: 72 ms, 40: 817 ms**, about 10× per 5 digits, and around 10²⁰ calls at LeetCode's n = 100. It's the Fibonacci `[0]` lesson again.
    * **Why each recursive call sits inside an `if`** (I'd only seen two calls side by side). In Climbing Stairs both moves are always legal, so it's `ways(n−1) + ways(n−2)` unconditionally. Here a move is legal only if its bite is a letter, so each call is gated and a blocked move adds 0. It's still "two calls added", with each one switched off when its move is illegal. Checking the gate **before** moving (rather than at the top of the next call, as in my first try) is what made the "where does the next bite start" bug disappear.
    * **Why `start == n` returns 1:** how many ways are there to finish decoding nothing? Exactly one, because you're done. Every complete walk arrives there exactly once, so that 1 is the walk's ✓. If it returned 0, every answer would be 0, because the base case is the only place a 1 enters. **Dead ends need no base case:** no bite allowed means no calls, so 0 + 0. The same "one way to do nothing" as Climbing Stairs' ways(0) = 1.
    * **Why only n + 1 states:** `s`, `n` and the memo are the same in every call, and only `start` changes. Finishing from `start` doesn't care how you got there (in `"226"`, position 2 is reached by `2|2` or `22`, and either way `"6"` is left). `start` takes the values 0..n, so there are **n + 1 distinct questions**, and every other call was a repeat.
    * **Why `start` never passes `n`:** the `start + 1` call happens only after `start == n` returned, so `start ≤ n − 1`. The `start + 2` call happens only when `start + 1 < n` (**"two digits are left"**), so `start + 2 ≤ n`. A bite that fits inside the string can't carry you past its end. Delete that guard and position n − 1 calls n + 1, which is out of bounds in the n + 1-slot memo.
    * **Memo → table is mechanical,** except for one decision: the memo becomes the table, the base case becomes `dp[n] = 1`, each call becomes a read, the gates stay as they are, and the answer is `dp[0]`. **The decision is the loop direction:** the recursion did the reads at `start + 1` and `start + 2` first, so the loop must too, which means running **from n − 1 down to 0**. **The `-1` sentinel disappears**, because the loop order guarantees every cell read is already written (the Fibonacci "loop order replaces the check" lesson). It matters here because 0 is a real answer (`"06"`), so 0 couldn't have been the sentinel anyway.
    * **Rolled:** `next1` (ways from one ahead) starts at 1 (`dp[n]`), and `next2` starts at 0 for the nonexistent `dp[n+1]`. That value is never read, because at `i = n − 1` the two-digits-left guard skips the 2-digit branch. Shift: `temp` first, then `next2 = next1`, `next1 = temp`.
    * **Overflow is a precondition, not a guarantee.** All-`'1'` counts are Fibonacci numbers and pass `INT_MAX` at **46 characters**. 100 `'1'`s is about 1.3 × 10¹⁸, which UBSan reports as signed overflow (it returned −1,869,596,475). LeetCode promises the answer fits in 32 bits, so **say it out loud** in an interview: "int because the problem guarantees 32 bits; otherwise `int64_t` (fits n = 100) or modulo what they ask." A sentence, not a code change, unless they ask for big inputs.
    * **Branch-free (after the review, constant factor only):** each digit decided two unpredictable branches (`'0'`? and 10–26?), so random digits mispredict often. Three ideas:
        * **a comparison is already 0 or 1**, so `(s[i] != '0') * next1` is "`next1` if allowed",
        * **test the number, not the characters**: build `two` and range-check it against 10–26, so `"06"` = 6 fails by itself and no nesting is needed,
        * **peel the last digit out of the loop**: `next1 = (s[n−1] != '0')`, `next2 = 1`, then loop from n − 2, so `s[i+1]` always exists and the bounds check is gone.

        **Measured: 244–261 → 104–110 ns (~2.3×).** It's not an interview item; mention it only if asked about hardware. **The peel brings back an empty-input hole:** with n = 0, `s[n−1]` is `s[−1]` (ASan: SEGV). The rolled version returned 1 for `""`. LeetCode's n ≥ 1 hides it, but the peeled form needs its own `n == 0` line.
    * **Measured** (`-O2`, n = 100, 10,000 random-digit strings whose answers fit in `int`; every version fuzzed against an independent DP on 200,000 strings of length 1–40, buffers without a terminator, ASan/UBSan clean):

        | version | ns per call |
        | -- | -- |
        | memo `e7d619b` | 664–670 |
        | table `0eee9b4` | 283–286 |
        | rolled `f1f816d` | 243–261 |
        | **branch-free rolled** | **104–110** |

* **Time & Space Complexity:**
    * **Time:** **O(n)**: one or two gated additions per digit (the brute-force recursion was exponential, Fibonacci in the worst case).
    * **Space:** **O(1)**: two ints (memo and table: O(n); memo also O(n) stack).

### [8] Coin Change

> Accumulator recursion (TLE, and I couldn't see how to memoize it), a greedy detour, the returning recursion, then memo `efdabcd` (the caller wrote the cache), standard memo `c9f011c`, a literal memo → table transliteration `a78773b`, and finally a coin-outer table after the review. The wall was the same one as Decode Ways, in a new costume: my recursion carried `count` **down** and wrote the answer at the leaf, so no call ever *returned* an answer worth caching. After the reframe I misread "min over c" as "pick a coin", then minimised the leftover **amount** instead of the returned **count**.

* **The Core Pattern:** make a target from pieces, unbounded (§1 "The Families", §2 Pattern 3). **f(a) = the fewest coins that pay exactly `a`.** To pay `a`, put down any coin `c ≤ a` and pay the rest as cheaply as possible: `f(a) = 1 + min over every coin c ≤ a of f(a − c)`. `f(0) = 0`. If no coin leads anywhere, `a` is impossible, and impossible must lose every `min` and survive `+ 1`. There are `amount + 1` states, filled from 0 upwards because every read is at a smaller amount.

* **The mental model — a cashier with assistants:** to pay 6 with `[1,3,4]`, hand out one question per coin: "fewest coins for 5?", "for 3?", "for 2?". The **amounts** (5, 3, 2) go out. The **counts** (2, 1, 2) come back. Compare only the counts, take the smallest (1, from the 3-coin), and add the coin you put down: **2**. Each assistant does the same with its own amount. Two comparisons live in the loop and they never mix units: `coin ≤ a` is money against money (a filter), and returned count against best count is coins against coins.

* **The "Gotcha" — four wrong turns:**
    1. **The accumulator recursion can't be memoized.** `helper(amount, count, &minCount)` was correct but exponential (`[1,2,5]`: 544 calls at amount 10, 1.6 million at 25, ~14× per +5). Paths `1,1` and `2` both reach leftover 9 with different `count`s, and the subtree below 9 is identical, but the function returned `void` and its effect depended on the `count` it arrived with. **Nothing to store under `memo[9]`.** The fix is the Decode Ways move: make the state only "what's left" and **return** the answer, so the `+1` moves from going down (`count + 1`) to coming back up (`1 + f(...)`), and `minCount` disappears.
    2. **"min over c" doesn't mean "pick a coin".** I read it as choosing the next coin (the smallest one above the last), traced 1 → 2 → 5 on amount 11 and got stuck on what to do when 5 overshoots. Nothing is chosen in advance: **every coin gets a turn, each turn returns a number, the smallest number wins.** The function doesn't need the last coin either: 1+2 and 2+1 cost the same count, so order is harmless for a *min*. Combination Sum's start index existed to avoid *listing* duplicates, which doesn't apply here.
    3. **Minimising the input, not the output.** My next version found the coin with the smallest `amount − coin` and made **one** recursive call after the loop. That's "always grab the biggest coin", even when it overshoots: amount 1 with `[1,2,5]` picks the −4 leftover and returns −1. And even without overshooting, greedy is wrong: `[1,3,4]` with 6 gives 4+1+1 (3 coins), but 3+3 is 2. **You only learn which first coin is best once each branch comes back with its count.** So the recursive call goes *inside* the loop, one per coin.
    4. **The flipped loop, three bugs at once** (after the review): `dp[amount − coin]` instead of `dp[fillAmount − coin]` read one fixed cell, so the table never moved off its initial value (every answer came out equal to `amount`). The inner loop started at 1, so `fillAmount − coin` went negative (**ASan: heap-buffer-overflow** on `[5], 3`). And the sentinel was `amount`, which is a **real answer** (`[1]` pays 10 with 10 coins), so `> amount` never fired and `[2], 3` returned 3 instead of −1.

* **The Struggle & Insights:**
    * **The impossible sentinel had to do two jobs:** lose every `min` and never overflow on `+ 1`. With `INT_MAX` that needed the `== INT_MAX ? INT_MAX : 1 + best` guard, which I got right from the first returning version. The cleaner bound: no real answer needs more than `amount` coins (every coin is ≥ 1), so **`amount + 1` is the first impossible value**, `+ 1` stays far from overflow, and the final test is `> amount`.
    * **"Not computed yet" ≠ "impossible".** Both are real memo states. If they share a value, impossible amounts never count as cached, and `[2]` with a large odd amount goes exponential again. −1 marked "not computed" and `INT_MAX` was a stored final answer.
    * **The guard comes before the index.** Once the memo is an array indexed by amount, `difference >= 0` has to run before any access. That made the `amount < 0` base case unreachable, so I removed it.
    * **Caller-writes vs callee-writes.** My first memo stored `dp[difference] = helper(...)` in the caller. It worked, because every callee checks its own slot on entry. The standard form is "compute, `return dp[amount] = ...`": the cache logic is in one place, and it's exactly how a table cell writes itself.
    * **Memo → table was a transliteration:** the outer loop over `fillAmount` replaces the recursion, each recursive call becomes a read of `dp[fillAmount − coin]` (already filled because it's smaller), the coin loop and the guard line carry over unchanged, and the −1 marker disappears because the loop order guarantees every read is already written.
    * **No O(1) per cell, and no better overall.** I worried that the table wasn't a "single pass" and the min wasn't O(1). It *is* one pass: each cell is written once. The coin scan per cell is the minimum work, since greedy proved a cell can't know its best coin without checking all of them. And O(amount × k) is **pseudo-polynomial**: polynomial in the *value* of `amount`, not in its bit length. The general problem is NP-hard. This is the expected answer.
    * **Loop order, flipped (the review's constant factor):** coins outer, amounts inner. **Think of it as sorting the coins in your hand:** any payment can be laid down as all the 1s, then all the 5s. After the pass for coin `c`, `dp[a]` = the fewest coins to pay `a` **using only the coins so far**. Each pass only asks "keep what I have, or put down one `c` on top of the best for `a − c`?" Going left to right inside a pass is what allows reuse: in `[1,5]` → 10, `dp[10]` reads `dp[5]`, which was already improved to 1 earlier in the *same* pass, so 5+5. **Starting the inner loop at `coin` is the whole gain:** it removes the bounds check. My first fixed version kept the check as an `if`, and it was no faster than the amount-outer table. **Remember the shape for Coin Change II (2-D DP):** there, loop order changes the *answer* (coins outer counts combinations, amounts outer counts permutations).
    * **My register accumulator beat the textbook line.** Writing `dp[a] = std::min(dp[a], dp[a − c] + 1)` on every coin inside an amount-outer loop is **2–2.6× slower** than my local `minCoinCount`. The compiler can't keep `dp[a]` in a register, because `dp[a − c]` might be the same memory (it can't assume `c ≠ 0`), so it reloads and stores on every coin. The flipped order can't use an accumulator (each cell is visited once per coin), and it gets ahead only by dropping the bounds check.
    * **Don't time under ASan.** An ASan run made the memo look 7× faster than the table on `[2], 9999` (only reachable states are visited: the odd ones). At `-O2` they were equal. Reachable-only is real, but here it saves almost nothing. (The `[0]` entry's `[50, 100]` example is where it actually wins.)
    * **BFS, the other view (named, not coded):** amounts are nodes and coins are edges, so "fewest coins" is a shortest path in an unweighted graph. BFS from `amount` stops the moment it reaches 0, which is an early exit when the answer is small. Same O(A·k) worst case.
    * **Measured** (`-O2`, µs per call, ranges over runs on a noisy machine; every version fuzzed against an independent DP on 200,000 cases, ASan/UBSan clean):

        | version | [1,2,5], 10⁴ | 12 random coins, 10⁴ | [186,419,83,408], 6249 |
        | -- | -- | -- | -- |
        | memo `c9f011c` | 165–177 | 512–828 | 81–83 |
        | table, amount-outer `a78773b` | 31–43 | 81–168 | 20–32 |
        | textbook `std::min` into `dp[a]`, amount-outer | 70–112 | 185–319 | 25–42 |
        | coin-outer, `if` check kept | 33–44 | 109–139 | 25–32 |
        | **coin-outer, inner loop starts at `coin` (final)** | **31–49** | **62–99** | **15–19** |

        The memo also recursed ~10⁴ frames deep on `[1]`, which would blow any embedded stack budget.

* **Time & Space Complexity:**
    * **Time:** **O(amount × k)**, where k = number of coins (the accumulator and plain recursions were exponential).
    * **Space:** **O(amount)**: one table (memo: also O(amount) stack depth).

### [9] Maximum Product Subarray

> First pass `0a1e77c`: a forward scan multiplying every element and resetting on 0. Optimal `cc02337`: track `currentMax` and `currentMin` at every index, swap on negatives, and keep a global `maxResult`. The wall was not the swap trick but grasping *why* a single forward pass misses valid subarrays — specifically, when a negative drags the running product down and a later element alone would have been larger.

* **The Core Pattern:** best run ending here (§1 "The Families", §2 Pattern 5). **`currentMax` = the largest product of any contiguous subarray ending at the current index. `currentMin` = the smallest (most negative).** At each element, there are three candidates for the best product ending here: the element alone (start fresh), extend the previous max, or extend the previous min — because a negative times a large negative is a large positive. The answer is the global max over all `currentMax` values.

* **The mental model — the optimist and the doomsday tracker:**
    * `currentMax` (the optimist) hoards the largest product, hoping to extend it.
    * `currentMin` (the doomsday tracker) hoards the most negative product, waiting for a negative number to flip it into something huge.
    * When `num < 0`, **swap them before multiplying**: a negative flips signs, so the old max becomes the new worst candidate, and the old min becomes the new best.
    * `std::max(num, currentMax * num)` lets the optimist either extend the previous streak or start fresh from `num` alone — which is how it recovers after a 0 or a bad streak.

* **The "Gotcha" — why a single forward pass fails:**
    * **My first intuition:** multiply all elements left to right into a `product` variable, track `maxResult`, and reset `product = 1` on encountering 0. This misses any subarray that *starts after index 0* or *ends before a dragging negative*.
    * **`[3, -1, 4]`:** forward pass gives 3, −3, −12 → `maxResult = 3`. But the answer is **4** (the element alone). The product went negative after `−1`, and the forward pass never "restarted" from index 2. Even comparing `nums[i]` individually doesn't fix it: `[2, -5, -2, -4, 3]` gives a forward max of 20, but `[-2, -4, 3] = 24` beats it, because dropping the first `−5` lets the two later negatives cancel.
    * **A missing global answer tracker was the actual bug in my first "correct" attempt.** `currentMax` only tracks the best product ending at the current index — it gets overwritten at every step. On `[2, 3, -2, 4]`, `currentMax` peaks at 6 (index 1), drops to −2 at index 2, and ends at 4. Without a separate `maxResult` updated on every iteration, the peak is lost. **`maxResult = std::max(maxResult, currentMax)` is the line that remembers.**

* **The Struggle & Insights:**
    * **Starting from `i = 0` instead of `i = 1`.** My first version initialised `currentMax = currentMin = maxResult = nums[0]` and looped from 1. The LLM showed that initialising `currentMax = currentMin = 1` (the multiplicative identity) and `maxResult = INT_MIN` lets the loop start from 0 with no special case. At `i = 0`, `1 * nums[0] = nums[0]`, and `std::max(nums[0], nums[0]) = nums[0]`, so the first element bootstraps itself. Swapping 1 and 1 is a no-op. `INT_MIN` guarantees the first iteration overwrites `maxResult` even if all elements are negative.
    * **The swap is the "aha!" moment, but the `max(num, ...)` is the safety net.** The swap puts the right scout in front of the right formula before multiplying by a negative. But `std::max(num, currentMax * num)` is what handles the 0 case and the "start fresh" case: after a 0, `currentMax * 0 = 0`, and if `num` is positive, it starts a new subarray. Without the `max`, there's no way to drop a poisoned prefix.
    * **The range-for loop is cleaner than indexing.** Since the state is just two rolling products and a global max, there's no need for `i` at all. `for (const int num : nums)` reads directly from the input.

* **Time & Space Complexity:**
    * **Time:** **O(n)**: one pass, constant work per element (a compare, a possible swap, two multiplies, two maxes/mins).
    * **Space:** **O(1)**: three ints (`currentMax`, `currentMin`, `maxResult`).

### [10] Word Break

> Brute force `4298185` (TLE), memo `2f5cfdf` → `84de7ef` (cache only failures), table `5ca9fa5` (suffix table filled by *pushing* from word ends), then `900206a`: a hash set of `string_view`s looped over lengths instead of words. The walls were: a `char` sentinel that only works on x86, a cache whose meaning is *not* the return value, and a line-by-line bottom-up translation that dropped the one line that mattered: the recursive call.

* **The Core Pattern:** make a target from pieces (§1 "The Families", §2 Pattern 3). **`dp[i]` = "the suffix `s[i..n)` can be split into dictionary words."** Base case: `dp[n]` = true (the empty suffix splits trivially). The answer is `dp[0]`. A word can be used at `i` only if it matches there **and** what's left after it is splittable. Either half on its own proves nothing.

* **The brute force, and why it's exponential:** at index `i`, try every word that fits and matches, recurse to `i + len`, and return true as soon as any branch does. Correct, but on `s = "aaa…ab"` with `{"a","aa","aaa"}` (answer false, so every branch is tried) the calls grow like tribonacci: 600 calls at n = 10, 266k at n = 20, **118 million at n = 30**. At n = 10, index 10 alone is entered **274 times**, and it fails the same way each time. **The result depends only on `i`** (`s` and the dictionary never change), so how you got to `i` doesn't matter. That's the 1-D state: `n + 1` distinct questions, asked over and over.

* **Stumble 1: the sentinel type (embedded trap).**
    * `vector<bool>` was dropped. It isn't inaccurate: it's a **bit-packed specialization**, so `dp[i]` returns a proxy object instead of a `bool&` (`auto&` won't bind, and you can't take an element's address). Every access is a shift-and-mask, and two threads writing *different* elements can race because they share a word. Single-threaded, it's just slower and harder to work with.
    * `vector<uint8_t>` with `-1` as "not computed": `-1` is stored as **255**, and in `dp[i] != -1` the byte is promoted to `int` 255 first, so the comparison is always true. Every uncomputed index returned 255, which converts to `true`.
    * `vector<char>` with `-1` passed on x86, but **whether plain `char` is signed is implementation-defined, and on ARM (AArch64 Linux, Cortex-M toolchains) it is unsigned.** Built with `-funsigned-char`, `catsandog` and the `aaa…ab` case both failed, and `-Wextra` warned *"comparison is always true due to limited range of data type."* Same bug as `uint8_t`, just hidden by the host. **A signed byte is `int8_t` / `signed char`. Plain `char` is for characters.**

* **Insight: only failures need caching. The cache is not the answer.**
    * Because a `true` returns immediately all the way up, a `true` result is **never looked up again**. The only fact worth remembering is "index `i` has already failed." Three states (unknown / true / false) collapse to two: `0` = not known to fail, `1` = known to fail. That's 1 bit of information, not 2.
    * **The bug this caused:** in every earlier DP problem the cache held the answer itself, so `return dp[i] = value` was natural. Here `dp[i] == 1` means *fail* while returning `true` means *can split*. Writing `return dp[i] = 1` on success mixed up the two meanings. **When the cache stores something other than the answer, write it and return on separate lines.**
    * The final memo also sizes `dp` at `n`, not `n + 1`: the `i == n` base case returns before any lookup, so `dp[n]` is never needed.

* **Stumble 2: the bottom-up translation lost the recursive call.**
    * The method was good: take the memo line by line and replace each line with its table counterpart (`return true` at `i == n` → `dp[n] = 1`, recurse right-to-left → loop `i` from `n - 1` down).
    * **But the key line, `if (helper(i + len)) return true;`, became a *write* (`dp[start] = 1` whenever a word matched) instead of a *read* of an already-filled entry.** So "a word matches here" was treated as "solved from here." Smallest failing case: `s = "ab"`, `{"a"}`. `"a"` sits at index 0, so `dp[0] = 1` → true, and nobody checked that `"b"` can be split. `catsandog` fails the same way via `"cats"`.
    * **It also mixed two framings.** `dp[n] = 1` with a right-to-left loop is the **suffix** table, but the matching looked *backwards* (words ending at `i`, start `i + 1 - len`), which belongs to the **prefix** table (`dp[j]` = "first `j` chars split", base `dp[0] = 1`, loop left-to-right, read `dp[j - len]`). Both are valid. Base case, loop direction and matching must all come from the same one.
    * **The second attempt still checked the wrong index:** `if (dp[i]) continue` (wrong index, wrong polarity). A word occupying `s[start..i]` needs the part **after** it, `s[i+1..n)`, to be splittable: that's **`dp[i + 1]`**, and the skip is *when it's false*. Failed the fuzz on `"abbbba"`.

* **The final form: push, not pull.**
    * **Pull** (the direct translation of the recursion): at `i`, look *forward* at words starting at `i`, and **read** `dp[i + len]` to decide `dp[i]`.
    * **Push** (my committed version): at end index `i`, if `dp[i + 1]` is true, **write** `dp[i + 1 - len] = 1` for every word ending at `i`. It's valid because `dp[i + 1]` is already final when you reach `i`: any word that could set it ends at an index ≥ `i + 1`, and those are processed first. Same work as pull. If an interviewer asks you to *derive* bottom-up from the memo, pull is the form they expect.
    * **Two cheap skips, with measured costs.** Measured at `-O2` with n = 300 and 1000 words. `!dp[i + 1]` skips whole indices: on `"aaa…ab"` almost everything is skipped (1.4 µs vs 421 µs for the memo). `!dp[start]` placed **before** the `substr == word` compare lets `&&` short-circuit past the `memcmp` when the entry is already set: 421 → **92 µs** (4.6×) on an all-splittable input. The bounds check has to come first, or `dp[start]` is read at a negative index. A `dp[0]` early return was also correct but saved only 11%, because `dp[0]` gets set near the end of the loop.
    * **Bottom-up also removes the O(n) stack.** On 2,000,000 chars with `{"a","aa"}`, the memo **segfaulted (stack overflow)** and the table returned instantly. LeetCode caps n at 300, but on a Cortex-M a few hundred frames is already too deep.

* **The final step: loop over lengths, not words (the one an interviewer grades).** The table above still scanned **all m words** at every index: O(n · m · L), when at LeetCode's limits only ~20 of the 1000 words could ever end at a given index. Flip the question from "which of the m words end here?" to "**which of the substrings ending here, of length 1..maxLen, are words?**":
    * **My first try kept the word loop** and swapped `substr == word` for `wordSet.contains(std::string(substr))`. That made things **slower** (92 → 120 µs). With the word already in hand, the set lookup is just a more expensive equality test: hash the substring, then compare anyway. **A hash set only pays off when it replaces the loop it sits in.** `wordDict` is now used only to build the set and find `maxLen`.
    * **The `std::string` temporary:** `unordered_set<std::string>` wouldn't take the `string_view` substring, so I built a string per lookup. In libstdc++ anything over 15 chars (the small-string buffer) is a **heap allocation**, and words go up to 20. The fix: **store `string_view`s in the set.** Each one points into a string that `wordDict` owns, and `wordDict` outlives the call, so `s.substr(...)` is looked up directly with no conversion. (The other route is C++20 heterogeneous lookup: a hash and equality that both declare `is_transparent`. It does the same job with more machinery.)
    * **The loop bound replaces a check:** a word ending at `i` covers `s[start..i]` with `start = i + 1 − len`, and **only `i + 1` characters exist from index 0 to `i`**. So the length is capped twice, `≤ maxLen` and `≤ i + 1`, and the loop starts at `min(maxLen, i + 1)`. On `"leetcode"` at `i = 1` (`maxLen` = 4) the old loop tried lengths 4 and 3, got `start` = −2 and −1, and relied on `(i + 1) >= wordLength` to reject them. A negative `start` would index `dp[-2]`, and `substr` would see a huge `size_t` and throw. Starting at the min means those lengths are **never generated**, so the check is dead and was removed. A bound that can't produce bad values beats a check that filters them out.
    * **Small constant: `wordSet.reserve(wordDict.size())`** before inserting stops the set from rehashing as it grows: 18.2 → 16.4 µs. `std::numeric_limits<size_t>::min()` became plain `0`, which it always is for an unsigned type.
    * **Measured at `-O2`** (n = 300, 1000 words up to 20 chars): all-splittable input, **92 → ~16 µs (~5.6×)**. On `"aaa…ab"`, where the `dp[i+1]` skip prunes nearly everything, it's 1.4 → 13.9 µs: what's left is the one-time cost of building a 1000-entry set, which the word-scan version never paid.
    * **Not taken: the trie** (folder 09), at O(n · L). It measured ~15 µs, within 10–20% of the set, so it isn't worth the extra code here. In push form it would need *reversed* words, so if it's ever needed, write it in pull form.

* **Time & Space Complexity (final, hash set):**
    * **Time:** **O(m · L + n · L²)**: building the set hashes every word once. Then n indices × up to L lengths × an O(L) hash per lookup. The m factor is out of the main loop.
    * **Space:** **O(n + m)**: the `n + 1` byte table plus the set of views (the views don't copy the characters).

### [11] Longest Increasing Subsequence

> Brute force `c91f220` (take/skip recursion, O(2ⁿ)), memo `c1c09e0` (flat n × n table), table `5895e63` (2-D) → `1d691b7` (only real states) → `7ea02e4` (flat), then `5731aa9`: the O(n log n) "smallest ending per length" record with my own binary search. The walls were: what `prev` means, "take whenever you can" being greedy, counting the states of a range that starts at −1, a table loop that walked one path instead of filling every cell, and a binary search whose branches pointed the wrong way.

* **The Core Pattern (the DP versions):** take or skip (§2 Pattern 2), but with a **two-part state**. **`f(i, prev)` = "the longest increasing subsequence I can still build from `nums[i..n)`, given that the last element I actually picked is `nums[prev]`"** (`prev = −1` means nothing picked yet). Base case: `i == n` → 0. Answer: `f(0, −1)`. One top-level call covers every starting index, because the skip branch lets the subsequence begin anywhere. My first plan had an outer loop over start indices, which just redid the same work.

* **Stumble 1: the first recursion mixed two styles and broke both.**
    * It looped `i` from `startIdx` ("which element comes next?") **and** had a take/skip `if/else`. In the loop style the loop itself is the skip, so the `else` has no job.
    * **`count = …` inside the loop overwrote instead of keeping the best**, so the function returned whatever the *last* iteration produced. The last iteration is always `i = n − 1`, whose call returns 0, so **every input returned 1**, `[1,2,3]` included. Each loop iteration is a different choice; you want the `max` of them.
    * **The `else` moved `prev` to an element it never counted.** On `[3,4,1,2]` (answer 2) it took 3, 4, then made 1 the new `prev` without counting it, then took 2 because 2 > 1, and claimed "3, 4, 2" = 3. **`prev` must always be the last element actually in the subsequence.** If `nums[i]` can't be taken, it's simply not part of this path, and `prev` stays.
    * "Should I erase the previous element and use the current one instead?" No. Another branch of the recursion already covers that: the one that never picked the old `prev`. Recursion explores every choice for you. A path never undoes its own choices.

* **Stumble 2: "take whenever you can" is greedy.** The second version took `nums[i]` every time it was legal and only skipped when it had to. `[1,5,2,3]` returned 2 (want 3, `1,2,3`): taking 5 was *legal* but bad, because it blocked both 2 and 3. At index 1 you can't know whether 5 pays off. That depends on elements you haven't seen. So when taking is legal there are **two** futures, take (`1 + f(i+1, i)`) and skip (`f(i+1, prev)`), and you keep the max. **Rule to remember: skipping is always allowed. Taking is allowed only if `nums[i] > nums[prev]`.** So compute skip unconditionally, then `max` in take if it's legal. No `if/else` needed.

* **Why the brute force is exponential:** every legal element splits into two calls. On a sorted array every element is always legal, so it's a full binary tree: measured 1.1 ms at n = 20, 19 ms at n = 24, **316 ms at n = 28**, ×16 per +4 elements, which is 2ⁿ. LeetCode goes to n = 2500.

* **Stumble 3: counting the memo states (an off-by-one in a range that starts at −1).**
    * The result depends only on `(i, prev)`. `i` is stored for `0 … n−1` (n values; `i == n` returns before touching the table). `prev` goes from `−1 … n−2`.
    * I counted `prev` as **n − 1** values and sized the flat table `n * (n − 1)`. Wrong: **an inclusive range `a … b` has `b − a + 1` values**, so `−1 … n−2` has `(n−2) − (−1) + 1 = n`. The top end moved down by one, but so did the bottom end (−1 instead of 0), so the count doesn't change. With n = 3, `prev` is `−1, 0, 1`: three values.
    * **The `+1` shift only relabels.** `prev + 1` maps `−1 … n−2` onto columns `0 … n−1`: same n states, now valid array indices. It never changes how many there are.
    * What the wrong size did: the largest index is `(n−1)·n + (n−1) = n² − 1`, but the table had `n² − n` slots, so the last row wrote out of bounds. At **n = 1 the table had size 0**, so even the first lookup was out of bounds. **The stride must equal the column count**: the table is n × n, the index is `i * n + (prev + 1)`, and `−1` works as "not computed" because every answer is ≥ 0.
    * The memo is O(n²) time: 18 ms at n = 2500, with a recursion depth of n + 1.

* **Stumble 4: the bottom-up table was a walk along one path, not a fill of every state.**
    * Right to left was correct: `f(i, …)` only reads `f(i+1, …)`, so row `i+1` must be done before row `i`.
    * **The mental-model bug:** my first table loop carried a single `prevIdx` variable and updated it ("I took this one, so it's the new prev"). That's walking one subsequence, the greedy version again in disguise. In the recursion, `prev` was a **parameter**: thousands of calls, each with its own `prev`. The table has to hold the answer **for every `(i, prev)` pair**. So: **outer loop over `i` (n−1 down to 0), inner loop over every `prev`, and the body is the recursive body with each call replaced by a read from row `i+1`.** No variable carries between iterations. **The table is the only state.**
    * Filled by hand for `[2, 5, 3]` (answer 2), rows = `i`, columns = `prev`:

      | i (value) | prev = −1 | prev = 0 (2) | prev = 1 (5) | prev = 2 (3) |
      |---|---|---|---|---|
      | **3** (base) | 0 | 0 | 0 | 0 |
      | **2** (3) | skip 0, take 1 → **1** | 3 > 2 → **1** | 3 > 5 ✗ → **0** | |
      | **1** (5) | skip 1, take 1 + 0 → **1** | 5 > 2 → **1** | | |
      | **0** (2) | skip 1, take 1 + row 1[prev 0] = 2 → **2** ✓ | | | |

      Each cell reads the cell straight below it (skip) and the cell in column `i` of the row below (take).
    * **The base case is a whole row, not one cell.** `f(n, prev) = 0` for *every* `prev`. The memo never stored row `n` (it returned early), but the table reads it. And at `i = n−1`, take reads `(n, prev = n−1)`, which is column `n`. So the table is **(n+1) × (n+1)**, all zeros, and the base row comes for free.
    * **The index bugs on the second try:** `i++` instead of `i--`; stride `n` on an `n+1`-wide table (so rows bled into each other); a `−1` init plus one `dp[n*n − 1] = 0`; and the loop condition `(prevIdx + 1) < i`. That last one meant `prevIdx < i − 1`, which skipped the last legal `prev` in every row, and **at `i = 0` the loop never ran at all**, so `dp[0]` stayed −1 and every input returned −1. **The `+1` shift belongs only inside the `[]`, never in the loop logic.**
    * **Why take reads column `i + 1`, not `i`:** after taking `nums[i]`, the new `prev` is `i`, and `prev = i` lives in **column `i + 1`**. `dp[i+1][i]` would answer "last picked was `nums[i−1]`", a different question.
    * **What unlocked it:** separating the logic from the index arithmetic. I switched to a real `vector<vector<int>>` (`dp[i][prev + 1]`, no stride to get wrong), got it passing, then flattened it back as a mechanical step.

* **Only real states: `prev < i` (`1d691b7`).** `prev` is an element already picked, and picking goes left to right, so **`prev < i` always**. A cell like `(i = 1, prev = 2)` means "I already picked index 2, now I'm deciding index 1", which never happens. No call `f(1, 2)` ever ran. And a real cell only ever reads real cells (skip: same `prev` one row down; take: `prev = i` in row `i+1`), so the fake cells are computed and never used. Ending the inner loop at `prev < i` cuts the work from n² to about n²/2: **5.20 → 2.35 ms** at n = 2500.

* **Flattening didn't help (`7ea02e4`): 2.35 → 2.72 ms, within noise.** The inner loop walks along one row, and inside a row a `vector<vector>` is already contiguous: the compiler loads the row pointer once and then strides through it exactly like the flat version. Flattening pays off when you jump between rows a lot or allocate many small rows. Neither happens here.

* **What the review found on the table:** correct, but **O(n²) memory: (n+1)² ints ≈ 25 MB at n = 2500** (peak RSS 27.1 MB vs 2.6 MB without it), all zero-initialized, and only row `i+1` is ever read. Two rolling rows would be ~20 KB and measured 2.56 → 1.59 ms. Also, `(n+1) * (n+1)` in `int` wraps negative at n = 46,341 (LeetCode caps n at 2500, but the size belongs in `size_t`). The O(n²) "`dp[i]` = the LIS **ending** at `i`" form (§2 Pattern 5) is the DP version most interviewers expect, also O(n) space. **The real gap was algorithmic: LeetCode's follow-up asks for O(n log n), and at Senior level an O(n²) answer gets that follow-up.**

* **The heap detour: a data structure with no meaning.** Before the n log n version I tried a max-heap: if `x ≤ top`, pop the top, then push `x`, and return the size. It "worked surprisingly well" until duplicates appeared, so I added a dedupe pass, and it got *worse*. Fuzzed: 29,368 mismatches out of 200k without the dedupe, 42,274 with it.
    * `[1,5,1,3]` → 3 (want 2): popping 5 for 1 gives `{1, 1}`, which claims "a length-2 subsequence ending at 1". None exists, since 1 isn't greater than the 1 below it. From then on the size counts a subsequence that isn't real. The duplicates were a **symptom** of that broken meaning, and deleting them threw away length I'd legitimately earned (`[1,4,1]` → 1).
    * **The rule this taught: before writing the loop, say in one sentence what the data structure holds after each step, and check it on a 4-element trace.** The memo had that sentence. The heap didn't.

* **The insight (the O(n log n) record): keep the smallest ending for each length.**
    * **`tails[k]` = the smallest value that any increasing subsequence of length k+1 seen so far ends with.** A smaller ending is better because a future `y` can extend a subsequence only if `y > ending`, so a lower ending accepts more futures (a later 3 extends `1, 2` but not `1, 3`).
    * My rule in my own words: *add `x` if it's greater than the largest element; otherwise replace.* That first half does look like a max-heap. **The second half is where the heap breaks: "replace" has to say *which* element, and it's the smallest element ≥ `x`, not the largest.**
    * **Why exactly that slot**, asked slot by slot with `tails = [1, 3, 5]` and `x = 2`:
        * Length 1 (record 1): 2 > 1, so 2 would be a worse ending. Leave it. Every slot whose value is < `x` is already better than `x`.
        * Length 2 (record 3): `x` needs a length-1 ending below 2, and 1 qualifies, so `1, 2` exists and ends lower than 3. **Improve it: overwrite.** This is the first slot ≥ `x`. The slot to its left is < `x`, so `x` can extend that subsequence by exactly one.
        * Length 3 (record 5): `x` would need a length-2 ending below 2, but the best is 3. Impossible, and the same for every slot beyond.
        * No slot ≥ `x` means `x` extends the longest subsequence: append. Same rule, applied past the end.
    * **The heap and the record side by side on `[1, 3, 5, 2, 4]`** (true LIS 3):

      | x | max-heap (replace the largest) | `tails` (replace the first ≥ x) |
      |---|---|---|
      | 1, 3, 5 | {1, 3, 5} | [1, 3, 5] |
      | 2 | pop 5 → {1, 2, 3} | 3 is the first ≥ 2 → [1, **2**, 5] |
      | 4 | 4 > 3, push → **size 4** ✗ | 5 is the first ≥ 4 → [1, 2, **4**], **size 3** ✓ |

      Replacing the largest is only right when `x` is greater than everything else in the record, and a heap can't check that, because it only shows the top. It also can't *search* by value. **A sorted vector plus binary search can**, and `tails` stays strictly increasing by itself: you only append a new maximum or overwrite a slot with a value that still fits between its neighbours.
    * **`tails` is not the actual LIS.** `[3, 4, 1]` ends with `tails = [1, 4]`: the 1 comes *after* the 4 in the array. The length is right; the contents mix different subsequences.
    * **First ≥ (`lower_bound`), not first > (`upper_bound`).** With first-greater, an equal value would land in the next slot and grow the length, which computes the longest *non-decreasing* subsequence.

* **Stumble 5: the record with a linear scan was still O(n²).** My first version found the slot by walking from index 0. Correct (200k fuzz clean), but O(n · L). It looked fast on random input because random arrays have a short LIS (~90 at n = 2500). The worst case is easy to build: first half `0, 2, 4, …` (a long record), then a value that keeps landing near the end. **n = 100,000: 409 ms for the scan, 0.52 ms for `lower_bound`.**

* **Stumble 6: my own binary search, and why `left` lands exactly on the first element ≥ target.** I adapted the Koko Eating Bananas search (folder 07) and got the branch backwards: `subset[mid] <= target → right = mid − 1`. On `[1, 3, 5]` every answer was wrong (target 0 → 3, target 6 → 0). `subset[mid] <= target` mostly means mid is **too small**, which is exactly when you should go *right*. Koko's shape is "if mid *works*, look left; otherwise go right". Here "works" means `subset[mid] >= target`, so the fixed branches are: **`>= target` → `right = mid − 1`, else `left = mid + 1`, return `left`.** Even with the fix I couldn't see *why* `left` converges to the answer, so here's the model:
    * **Turn the array into a row of NO / YES** by asking "is this element ≥ target?". Target 4 on `[1, 3, 5, 7, 9]` gives `NO NO YES YES YES`. Because the array is sorted, it's **always some NOs followed by some YESes**: a NO never comes after a YES. The answer is **the first YES**, the boundary between the two groups.
    * **What the two walls mean:** everything **before `left`** is known to be NO. Everything **after `right`** is known to be YES. `[left, right]` is still unchecked.
    * **Each step checks `mid` and grows one zone.** mid is NO → mid and everything before it are NO (sorted) → `left = mid + 1`. mid is YES → mid and everything after it are YES → `right = mid − 1`. Mid isn't lost: it's now inside the YES zone. The unchecked window shrinks every step, and the zones never overlap.
    * **The trace** (target 4, `?` = unchecked):

      | step | left | right | mid | ≥ 4? | after the move (indices 0 1 2 3 4) |
      |---|---|---|---|---|---|
      | start | 0 | 4 | | | `? ? ? ? ?` |
      | 1 | 0 | 4 | 2 | 5 → YES → right = 1 | `? ? Y Y Y` |
      | 2 | 0 | 1 | 0 | 1 → NO → left = 1 | `N ? Y Y Y` |
      | 3 | 1 | 1 | 1 | 3 → NO → left = 2 | `N N Y Y Y` |
      | end | **2** | 1 | | window empty | |

      When the loop stops, nothing is unchecked: the row is split into `N N | Y Y Y`. **`right` is the last NO and `left` is the first YES.** That's the whole argument: `left` always sits just after everything known to be NO, so when nothing is unknown anymore, `left` is the first YES. The loop never checks whether `left` "is the answer". It only sorts elements into NO and YES, and `left` ends up on the boundary.
    * **The edge cases fall out of the same picture.** Target larger than everything: the row is all NO, `left` walks to `n = size()`, so "just after all the NOs" is past the end → append. Empty record: `right = −1`, the loop never runs, `left = 0 = size()` → append. So the `push_back(nums[0])` special case wasn't needed, and the caller only has to compare the returned index against `size()`.
    * Tested against `std::lower_bound` on 200k random sorted arrays: 0 mismatches. `std::lower_bound` is the one-line version; writing it by hand is what interviews sometimes ask for.

* **Stumble 7: the compact rewrite searched the wrong array.** A tighter version (range-for, `if (init; cond)`, no `nums.empty()` / `nums[0]` special cases, which is all fine since an empty record already returns `0 = size()`) called **`binarySearch(nums, num)`, the input, instead of `binarySearch(subset, num)`**. I had renamed the helper's parameter to `nums`, so passing the caller's `nums` looked natural. Binary search is only valid on a **sorted** array: the NO/YES split relies on it. On the unsorted input the NOs and YESes are interleaved, so `left` stops at an arbitrary index, which is then compared against `subset.size()`, a length it has nothing to do with. `[10,9,2,5,3,7,101,18]` returned **5** (want 4). `[1,2,3]` and `[3,2,1]` passed by luck because they're already sorted, which is why a quick test looked fine. **Name a search helper's parameter after its precondition (`sorted`, `subset`), never after the caller's variable**, so the call site shows which array it expects. Side note: `targetIdx < subset.size()` is `int` vs `size_t`, which `-Wsign-compare` flags. It's harmless here (`targetIdx` is never negative), but cast `size()` to `int` as the earlier version did.

* **Time & Space Complexity (final, `5731aa9`):**
    * **Time:** **O(n log n)**: n elements, one O(log L) binary search each (L ≤ n is the LIS length). Measured at n = 100,000: 0.68 ms on the scan's worst case (409 ms with the linear scan), close to `std::lower_bound`'s 0.58 ms.
    * **Space:** **O(n)**: the record, reserved once. That's down from ~25 MB for the n × n table at n = 2500.

### [12] Partition Equal Subset Sum

> Brute force `28cb7a4` (TLE), memo `899f1a8`, 2-D table `809a7ab`, flat table `4275dbc`, two rows `32b8886`, then `1568e38`: one row updated in place, looping sums **downward**. The walls were: a memo table passed by value (so it never remembered anything), a table loop that dropped the skip branch, a flat stride copied from LIS, and two buffers combined with a loop that doesn't write every cell.

* **The Core Pattern:** each item once, 0/1 knapsack (§2 Pattern 4), which is take or skip (Pattern 2) with a **sum** as the second state instead of LIS's `prev`.
    * **Reduce first.** Two equal halves means `total = 2 × half`, so **an odd total is false immediately**. Otherwise you only need **one** subset summing to `target = total / 2`; everything else automatically makes the other half. It's a **subset**, not a subarray: `[1, 5, 11, 5]` splits as `{1, 5, 5}` + `{11}`, and those positions aren't contiguous.
    * **`f(i, remaining)` = "using only `nums[i..n)`, can I pick some elements that sum to exactly `remaining`?"** Answer: `f(0, target)`. Skip keeps `remaining`; take subtracts `nums[i]`. Both move to `i + 1`, because each element is used at most once (in Coin Change a coin could be reused, so the index didn't have to advance). The result is `bool`, so the two futures combine with `||` (which short-circuits on the first success) instead of `max`.
    * **Base-case order:** `remaining == 0` → true **before** `remaining < 0 || i == n` → false. Hitting exactly 0 with the last element means `i == n` at the same moment, and success has to win.
    * At LeetCode's limits (n ≤ 200, values ≤ 100), `target ≤ 10,000`, so the table is at most 201 × 10,001 ≈ 2 MB of bytes. It's pseudo-polynomial: the cost grows with the *values*, not only with n.

* **Stumble 1: the memo table passed by value.** The memo had the right state, a correct `uint8_t` 255 sentinel (safe, unlike Word Break's `-1`: both sides of `!=` promote to `int` 255), and the right `target + 1` width. And it was **still TLE**, because the parameter was `std::vector<std::vector<uint8_t>> dp`, with no `&`.
    * Every call got its **own copy** of the whole table. A callee wrote its answer into *its* copy, which was destroyed when it returned. The caller's table never got the write, so its other branch recomputed everything. The lookup could never hit across branches.
    * So it was the full **exponential brute force plus a whole-table copy per call**, slower than the committed brute force. Measured on all-even inputs with an odd half (answer false, so every state is explored): **n = 22: 848 ms by value vs 0.002 ms by reference**. By value grew ×4 per +2 elements, which is 2ⁿ; at n = 200 it never finished, by reference it took 0.11 ms.
    * **No warning, no crash, correct answers: it's just slow.** **If a memo is about as slow as the brute force, first check that every call shares the same table.** A real memo is a huge drop, not a small gain. Same rule as `const std::vector<int>& nums`: a by-value parameter is the callee's private copy.

* **Stumble 2: the bottom-up table.**
    * **(n + 1) rows, not n.** The loop reads row `i + 1`, so row `n` (the base case, "no elements left") must exist.
    * **The base is a column, not a row:** `dp[i][0] = 1` for every `i` ("pick nothing → sum 0"). Every other cell of row `n` is 0, since with no elements left nothing else is reachable.
    * **`return dp[0][0]` asked the wrong question:** "can I make 0 from everything?", which I had just set to true. So every even total returned true (smallest failing case: `[2]`). The real question is the top-level call, `f(0, target)` = **`dp[0][target]`**.
    * **The inner loop started at `nums[i]`, which dropped the skip branch.** When `left < nums[i]`, take is illegal (it overshoots), but **skip is still legal**: `dp[i][left] = dp[i+1][left]`. Those cells were never visited, stayed 0 ("impossible"), and the wrong 0 spread upward. On `[2, 3, 2, 1]` (target 4, want true via {3,1} or {2,2}), row 2 never learned "skip the 2, use the 1 → sum 1", so row 1 couldn't see that 3 + 1 = 4. 1,295 fuzz mismatches. **The LIS rule again: skip is always legal, take only when it fits.** In a 2-D table the inner loop covers every `left` from 1 to `target`.

* **Insight: where the answer lives, and which early return is valid.**
    * Each cell answers **its own** question: `dp[i][left]` = "can a subset of `nums[i..n)` make `left`?". Most true cells are about other sums. The very first cell filled in the trace above, `dp[3][1] = 1`, only says "the last element alone makes 1", and every `dp[i][0]` is true. **"Return on the first true cell" would return true for everything.**
    * **The `target` column is different.** If `dp[i][target]` is true for *any* `i`, some subset of `nums[i..n)` sums to `target`, and a subset of a suffix is still a subset of the whole array. In the table it shows as the skip branch passing that true straight up to row 0. So: **return true as soon as `dp[i][target]` is true, return false after the loops.** Row 0 is the last row checked, so finishing the loops means `dp[0][target]` was false.
    * **Check it after the row, not inside the inner loop.** `target` is the last column. Checking on every `left` reads a cell this row hasn't written yet, which is wasted work (and a stale read once there are two buffers). It only saves time when the answer is true; a false answer still fills every row.

* **Stumble 3: the flat table used LIS's stride.** I indexed `i * (n + 1) + left` on a table allocated `(n + 1) × (target + 1)`. **The 2-D → 1-D formula is `rowIdx * rowWidth + colIdx`, where `rowWidth` is the number of columns.** In LIS the columns were `prev` values, so the width was `n + 1`. **Here the columns are sums, so the width is `target + 1`.** ASan on `[1, 1]` (n = 2, target = 1, 6 cells): the base-case loop wrote `dp[6]`, a **heap-buffer-overflow** on the first loop. `[1, 2, 5]` (answer false) didn't crash but returned **true**: with stride 4 on a 5-wide table, row 1 starts at row 0's last cell, so the rows overlap and skip/take read the wrong rows. **Take the width from what the columns mean, and name it once (`const int W = target + 1`)**, so the stride can't drift from the allocation. This is the second time (after LIS), so it's a habit to fix, not a one-off.

* **Flattening helped here (~1.5×), unlike LIS.** Measured at `-O2` on a false input (200 random even values, target 5203): 2-D with the check inside the loop 0.66 ms, check moved outside 0.62 ms, **flat 0.40 ms**. In LIS it bought nothing. The likely reason (I didn't read the assembly): `uint8_t` is a character type, and C++ lets a character-type store alias *any* object, including the `vector<vector>`'s internal row pointers. So after every byte store the compiler has to reload `dp[i]` and `dp[i+1]`. LIS stored `int`, which can't alias a pointer, so the row pointers stayed in registers. A constant factor, and an interesting one.
    * **Table vs memo isn't one-sided.** The table fills **every** sum `1 … target` in every row, reachable or not. On an input of mostly 100s (true, target 9999) only a few sums are reachable, and the memo (0.09 ms) beat every table (flat 0.90 ms). On diverse values the table wins (memo 1.41 ms).

* **Two rows (`32b8886`): O(T) space, same speed.** Row `i` reads only row `i + 1`, so `current` (row `i`) and `next` (row `i + 1`) of `target + 1` bytes are enough: ~20 KB instead of 2 MB. Speed is unchanged (0.388 ms vs 0.385 flat), because it's the same n × T cells of work. Copying 5 KB per row is negligible next to the 5K-cell loop, and `std::swap` measured the same (0.380 ms).

* **Stumble 4: `swap` + starting the loop at `nums[i]` is wrong, even though each one alone is fine.** 771 fuzz mismatches, smallest `[2, 3, 2, 1]` → false. (The review had said swap was safe "because every cell gets rewritten each row". Starting at `nums[i]` removes exactly that condition.)
    * **The picture: two whiteboards.** `next` holds row `i + 1`. You write row `i` on `current`. **What's already on `current` before you start writing?** You only write cells `≥ nums[i]`, so the cells below keep **whatever was on that board**. They should hold the skip value, `next[left]` (row `i + 1`).
        * **After `next = current` (copy):** both boards hold row `i + 1`, so the leftover cells already equal the skip value. Correct, by accident.
        * **After `std::swap(next, current)`:** swap only exchanges the labels; nothing is erased or copied. `current` is now the board you wrote **two rows ago**: row `i + 2`. Row `i + 2` doesn't know about `nums[i + 1]`, so the leftover cells are missing every sum that needs that element.
    * **The trace** (target 4, cells 0..4, rows from i = 3 down):

      | i (value) | `current` after the row | what happened |
      |---|---|---|
      | 3 (1) | `1 1 0 0 0` | sum 1 reachable. Swap: `current` is now the old base row `1 0 0 0 0` |
      | 2 (2) | `1 0 1 1 0` | loop starts at 2, so **cell 1 isn't written** and keeps the base row's **0**. It should be 1 (skip this 2, use the 1) |
      | 1 (3) | cell 4 = `next[4] \|\| next[1]` = 0 \|\| **0** = 0 | {3, 1} lost through the bad cell 1 |
      | 0 (2) | target never true | **false** ✗ |

    * **The rule: with two buffers, every cell of the new row has to be written *by this row*** (by taking, or by copying the skip value). Skipping the low cells is safe only with **one** buffer, because then the cells you don't touch *are* the previous row's values.

* **The final version: one row, in place, sums looping DOWNWARD.** `row[left] = row[left] || row[left − x]` for `left` from `target` down to `x = nums[i]`. Base `row[0] = 1`.
    * **Why the untouched cells are correct:** with one row, a cell you don't write still holds row `i + 1`'s value, which is exactly the skip value. So the loop can stop at `x`, and the skip branch costs nothing.
    * **Why the direction matters:** in place, each cell holds row `i + 1`'s value until you overwrite it, and row `i`'s value afterwards. The take read `row[left − x]` must still be **row `i + 1`'s** value, i.e. a subset that does *not* already use `nums[i]`.
        * **Upward**, you reach the smaller index `left − x` *first*, so by the time you read it, it already holds row `i`'s value, which may include `nums[i]`. Then you use the same element **twice**.
        * **Downward**, you only overwrite *larger* indices first. `left − x < left` hasn't been touched yet in this pass, so it's still row `i + 1`'s value.
    * **Smallest trace: one element `x = 2`, target 4**, row before = `1 0 0 0 0` (only sum 0 reachable):

      | direction | step by step | result |
      |---|---|---|
      | **upward** | left 2: `row[0]` = 1 → row[2] = 1 · left 3: row[1] = 0 → 0 · left 4: **`row[2]` = 1 (just set by this same 2)** → row[4] = 1 | `1 0 1 0 1`: claims 2 + 2 = 4 from a **single** 2 ✗ |
      | **downward** | left 4: `row[2]` still 0 → 0 · left 3: row[1] = 0 → 0 · left 2: `row[0]` = 1 → row[2] = 1 | `1 0 1 0 0` ✓ |

      Fuzzed: upward gave 17,651 mismatches out of 200k, e.g. `[1, 2, 5]` → true (want false; upward lets the 1 count twice and so on). Downward: 0.
    * **Upward isn't "wrong" in general: it's the *unbounded* version.** Reusing the item you just placed is exactly what Coin Change wants (a coin can be used again), which is why its loop runs upward. **Direction = how many times each item may be used: downward for once (0/1), upward for unlimited.**
    * Measured: **0.385 → 0.183 ms** on the diverse false input (2.1×): fewer cells (the loop stops at `x`), one buffer, and no copy-then-take step. 0.18 ms on the true input thanks to the early return.

* **Not taken: the bitset.** The row is just a set of reachable sums, and "every reachable `s` makes `s + x` reachable" for all `s` at once is `reachable |= reachable << x`. A `std::bitset<10001>` (sized by the constraints) does 64 sums per machine word, **O(n · T / 64)**: measured **0.025 ms**, 7× over the row and 16× over the flat table. It's the systems follow-up worth mentioning *after* the DP. Also skipped: the early exits "an element > target → false, == target → true" (one cheap check, constant factor).

* **Time & Space Complexity (final, `1568e38`):**
    * **Time:** **O(n · T)** with T = total / 2 ≤ 10,000: every element sweeps the sums from `T` down to itself, with an early return once `T` is reachable.
    * **Space:** **O(T)**: one row of `T + 1` bytes (~10 KB at the limits), down from ~2 MB for the full table.
