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
* **Make a target from pieces (unbounded):** `dp[a]` = best way to build amount/prefix `a`, from `dp[a - piece]` over every piece. Coin Change (`dp[a] = 1 + min dp[a - c]`, with "impossible" as a sentinel), Word Break (`dp[i]` = "the first `i` characters split into words", true if some word ends at `i` and `dp[i - len]` is true).
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
* *Use for:* House Robber, House Robber II (circle = two linear runs: drop the first house, or drop the last).
* *Discipline:* House Robber II with one house has to return that house, because both runs would be empty.

### Pattern 3: Make a Target from Pieces → `dp[target]` over every piece
* *Use for:* Coin Change, Word Break.
* *Discipline:* `dp[0]` is the base case (0 coins / empty prefix = true). The "impossible" sentinel must survive `+ 1`.

### Pattern 4: Each Item Once → 0/1 knapsack, sums iterated downward
* *Use for:* Partition Equal Subset Sum.
* *Discipline:* odd total → false immediately. Iterate sums high → low, or each item gets used repeatedly.

### Pattern 5: Best Run Ending Here → answer is the max over all `i`
* *Use for:* Maximum Product Subarray (track max **and** min), Longest Increasing Subsequence.
* *Discipline:* the answer is not `dp[n-1]` when a run can end anywhere.

### Pattern 6: Palindromes → expand around 2n − 1 centres
* *Use for:* Longest Palindromic Substring, Palindromic Substrings.
* *Discipline:* odd and even centres both. Store the best `(start, length)`, not substrings, and build the string once at the end.

### The Non-Negotiable Discipline
* **Say what `dp[i]` means in one sentence before writing code.**
* **Look at the last decision** to get the recurrence.
* **Base cases first, then loop order:** every state you read must already be filled.
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
