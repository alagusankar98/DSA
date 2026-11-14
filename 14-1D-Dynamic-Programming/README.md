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

> Done to learn the four forms before the real problems: naive recursion → memoization `ef2c7e8` → tabulation `7d71aed` → rolled variables (the final, three named ints). I'd read §1 and "understood somewhat". Writing all four made the vocabulary concrete.

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
