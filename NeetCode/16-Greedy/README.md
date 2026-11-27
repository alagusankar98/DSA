# Greedy - Topic Guide

## 1. Core Fundamentals

DP (topics 14–15) answers a question by **trying every choice** at each step and keeping the best: take or skip, which balloon goes last, which unit eats the next character. **Greedy makes one choice per step, the one that looks best right now, and never revisits it.** No table, no recursion tree, usually one pass and O(1) extra state.

That makes greedy the cheapest algorithm there is, and the most dangerous. **It is correct only when you can show that the locally best choice never blocks a globally best answer.** Without that argument, a greedy is a guess that passes the examples. You've already seen both sides:

* **Greedy that fails:** Min Cost Climbing Stairs `[2]` in `14-1D-Dynamic-Programming/README.md` (stepping to the cheaper neighbour paid 3 where 2 was possible), and Coin Change `[8]` there: with coins `{1, 3, 4}` and amount 6, "largest coin first" gives 4 + 1 + 1 = **3 coins**, but 3 + 3 = **2**. The big coin looked best and blocked the better answer.
* **Greedy that works, with a proof you already used:** Dijkstra (the closest unsettled node can't be improved later, because every edge is non-negative), Prim and Kruskal (the cheapest edge across a cut is always in some minimum spanning tree), and the activity-selection greedy in Intervals `[3]` (keep the interval that **ends earliest**: an exchange argument, written up in §1 of `17-Intervals/README.md`).

**The topic is mostly about the proof, not the code.** The code is usually 10 lines. The interview question is "why is that safe?", and "it worked on the examples" is the answer that fails.

### The Vocabulary
* **Greedy choice:** the one decision made at each step from local information only (the current element, a running total, the farthest point seen so far).
* **Greedy-choice property:** some optimal answer *starts with* the greedy choice. If that's true at every step, committing to it is safe.
* **Optimal substructure:** after the greedy choice, what's left is a smaller instance of the same problem. (DP needs this too. Greedy needs it **plus** the greedy-choice property, which is the rare part.)
* **Exchange argument:** take any optimal answer that *doesn't* make the greedy choice, swap one of its choices for the greedy one, and show the result is no worse. Then an optimal answer with the greedy choice exists. This is the standard proof, and the one to say out loud in an interview.
* **"Greedy stays ahead":** show by induction that after every step, the greedy's partial answer is at least as good as any other strategy's partial answer at the same step (it has reached at least as far, used at most as many). So it's also at least as good at the end.
* **Invariant:** a statement about your running variables that's true after every iteration ("`best` is the max over every prefix seen so far", "`reach` is the farthest index reachable from anything before `i`"). Greedy code is correct exactly when its invariant is, so write the invariant as a comment before the loop.
* **Counterexample hunting:** before trusting a greedy, spend two minutes trying to break it with an input of 3–5 elements. If a small case breaks it, it's a DP problem.
* **Sort key:** many greedies start with a sort, and **the sort key is the whole decision** (by start or by end, by value or by deadline). Picking the wrong key is the commonest greedy bug.

### The Four Questions, Greedy Edition (ask them before writing any code)
1. **What's the one decision at each step, and what does "best right now" mean?** Farthest, cheapest, earliest-ending, smallest-first. If you can't name it in a few words, it isn't greedy yet.
2. **Can I break it with 3–5 elements?** Try the cases that broke earlier greedies: a big item that blocks two medium ones, a cheap step that leads to an expensive one, a choice that looks good early and costs later.
3. **Why is it safe?** One sentence of exchange argument or stays-ahead. "Any answer that doesn't do this can be changed to do it, without getting worse, because…"
4. **What's the invariant, and what's in O(1) state?** A running sum, a farthest reach, a count, a lower and upper bound. Greedy almost always ends as a single pass with a few variables.

### Greedy vs DP: How to Tell Which One You're In
* **The DP view of a greedy:** a greedy is a DP where you can prove only one choice ever wins, so the table collapses to the winner. Maximum Product Subarray `[9]` (1-D) already showed the halfway point: keep the best and the worst running product, two variables instead of a table, because the extremes are the only states that can matter.
* **The test:** write the DP recurrence, then ask whether the `max`/`min` over choices always picks the same option. If yes, that option is the greedy, and the recurrence is its proof. If the winner depends on the future, it's DP.
* **The danger sign:** "take the biggest / cheapest / closest" with **items of different sizes competing for a fixed budget** (coins, knapsack, subsets) is usually *not* greedy. "Reach as far as possible", "end as early as possible", "drop the prefix that hurts" usually is.
* **Cost of guessing wrong:** a wrong greedy doesn't crash. It returns a plausible number that's slightly off on some inputs. **Fuzz it against a brute force** (all orders, all subsets) on small inputs. That's how most bugs in this run were found.

### The Problem Families by Shape
* **Running value with a reset:** walk once, keep a running total, and throw away the prefix the moment it can only hurt what comes next. The answer is the best value the running total ever reached.
* **Farthest reach / frontier:** each position lets you move some distance. Track the farthest index reachable so far. Feasibility is "did the frontier ever stop before the end?", and the fewest moves comes from counting frontier expansions, which is BFS levels without a queue.
* **Running balance on a circle:** gains and costs around a loop. If the total is negative, nothing works. Otherwise, a start that runs dry at `k` proves every start between it and `k` also runs dry, so the next candidate is `k + 1`. One pass.
* **Sort, then form groups from the smallest:** when items must be grouped into consecutive runs, the smallest remaining item has only one possible role (it must start a group), so it forces the decision. Needs a count map or a min-heap.
* **Extend to the last occurrence:** cut a sequence into the most pieces so each value lives in one piece. Precompute each value's last index; the current piece can't end before the last occurrence of anything inside it.
* **Track a range instead of branching:** when a symbol can mean several things, don't branch (that's backtracking or DP). Keep the lowest and highest value the running state could have, and fail only when even the best case fails.
* **Filter, then combine:** discard every candidate that would overshoot the target in any coordinate. Whatever's left can't hurt, so take all of it and check whether the target is covered.

### Complexity Cheat Sheet
| Shape | Time | Space |
| -- | -- | -- |
| Single pass with running variables | O(n) | O(1) |
| Sort, then a single pass | O(n log n) | O(1)–O(n) (the sort) |
| Sort + count map / min-heap grouping | O(n log n) | O(n) |
| Precompute last occurrence, then one pass | O(n) | O(alphabet) |
| Frontier expansion (BFS levels, no queue) | O(n) | O(1) |

### The Hardware Reality
* **One pass over a contiguous array is the fastest thing a CPU does.** The prefetcher streams it, the branch predictor learns the loop, and with no table there's no extra memory traffic. A greedy is often 100–1000× faster than the DP for the same problem, and O(1) space.
* **Branches on data are the cost that's left.** `if (sum < 0) sum = 0;` on random data mispredicts often. `sum = std::max(sum, 0)` or a conditional move has no branch. The Prim lesson from Advanced Graphs applies: an always-write `std::min` beat an `if`-guarded update by 15–30%. Measure before rewriting.
* **The sort dominates when there is one.** O(n log n) with a cache-friendly `std::sort` on a `vector` of small structs is fast. Sorting indices or pointers into another array costs a cache miss per comparison. Sort the data itself when you can.
* **Heap vs count map for grouping:** a `std::priority_queue` is a contiguous array (good), but each pop is O(log n) of unpredictable branches. When the values are small integers, a sorted list of distinct values plus a count map is often faster.
* **Embedded fit:** greedy is what runs on a microcontroller. No allocation, no recursion, constant stack, a deterministic number of cycles per element. That's why real-time schedulers and allocators are greedy.

### Where This Shows Up in the Real World
**Schedulers:** earliest-deadline-first in real-time operating systems (an exchange argument proves it optimal on one core), shortest-job-first in batch systems. **Compression:** Huffman coding (merge the two rarest symbols first) in DEFLATE, gzip, PNG and JPEG. **Networks:** Dijkstra in link-state routing (OSPF), spanning trees in Ethernet switches. **Memory allocators:** first-fit and best-fit free lists. **Caches:** Belady's optimal eviction (evict what's needed farthest in the future) is a greedy, and LRU is its practical approximation. **TCP congestion control:** additive increase, multiplicative decrease, a running-state greedy. **Money:** making change with real currency denominations, which are designed so that greedy works (unlike `{1, 3, 4}`).

---

## 2. General Summary / Quick Reference

### Pattern 1: Running Value with a Reset → drop the prefix that can only hurt
Keep a running total and a best-so-far. When the running total becomes something no future element can benefit from, reset it.
* *Discipline:* say exactly when a prefix "can only hurt" (a negative sum makes every later sum smaller). Update the best **before or after** the reset deliberately, and check an all-negative input.
* *Use for:* Maximum Subarray `[1]`.

### Pattern 2: Farthest Reach → a frontier instead of a search
Track the farthest index reachable from anything seen so far. Feasibility: stop if the current index ever passes the frontier. Fewest moves: count how many times you must extend the frontier (each extension is one BFS level).
* *Discipline:* the invariant is "everything up to `reach` is reachable". Check the off-by-one at the end (reaching the last index vs passing it) and the single-element input.
* *Use for:* Jump Game `[2]`.

### Pattern 3: Running Balance on a Circle → total first, then one candidate start
If the total gain is negative, the answer is impossible. Otherwise, run a balance from a candidate start. When it goes negative at `k`, no start in `[candidate, k]` can work, so the next candidate is `k + 1`.
* *Discipline:* prove the skip in one sentence (every start in between arrives at `k` with less fuel, not more). The total check is what makes a single candidate enough.

### Pattern 4: Sort, Then the Smallest Forces the Decision
After sorting (or with a min-heap / count map), the smallest remaining item can only play one role, so it decides the next group.
* *Discipline:* decrement counts and stop as soon as a needed item is missing. Cheap rejections first (the total size must divide evenly).

### Pattern 5: Last Occurrence → extend the current window's end
Precompute where each value appears last. The current piece's end is the max last-occurrence of everything inside it. Cut when the index reaches that end.
* *Discipline:* an array of 26 (or 256) beats a hash map for character keys.

### Pattern 6: Range of Possibilities → `[lo, hi]` instead of branching
When a symbol can stand for several things, track the lowest and the highest value the running state could have. Fail when even `hi` is invalid; clamp `lo` so it never goes below what's legal; succeed if `lo` can reach the target at the end.
* *Discipline:* this replaces an exponential branch with two numbers. Check it against the brute force, because the clamp is where the bugs live.

### Pattern 7: Filter, Then Combine
Throw away every candidate that would break the target in any coordinate. Everything left is harmless, so combine all of it and check whether each coordinate of the target is reached.
* *Discipline:* "harmless" must be argued (a candidate that never exceeds the target can only help, since combining takes the max).

### The Non-Negotiable Discipline
* **Write the greedy choice and the invariant as comments before the loop.**
* **Try to break it with 3–5 elements before coding.** If you can, it's DP.
* **Say the proof in one sentence:** exchange argument or stays-ahead.
* **Fuzz against a brute force** on small inputs (all orders, all subsets). A wrong greedy is plausible, not obviously wrong.
* **The sort key is a decision, not a detail.** Say why that key and not another.

---

## 3. Problem Strategies & Patterns

### [1] Maximum Subarray
**Pattern:** Running Value with a Reset (Pattern 1), i.e. Kadane's algorithm. Keep `sum` = the best sum of a subarray **ending at this index**, and `maxSum` = the best seen anywhere. Before adding the next element, drop the carried sum if it's negative. The whole problem in one line: `best[i] = nums[i] + max(0, best[i-1])`.

**Initial approach: two pointers.** `left` at 0, `right` consuming elements while the sum kept growing. When `right` hit a negative, I tried to decide by comparing `nums[left]` with `nums[right]` and couldn't find a rule. **Why it stalled:** the decision isn't about any single element. It's about the **whole stretch being carried**: does the sum built so far help or hurt the next element? Two endpoint values can't answer that; the running sum can.

**Second approach: one pass, restart when the carry hurts.** Keep a running sum. When it's negative, start fresh at the current element. That's the right idea, but two things came up:
1. **The best answer got lost.** By the end, `sum` had moved on past the best stretch. The fix was a separate `maxSum` updated on every step. These are two different quantities, "best ending here" and "best overall", the same split as 1-D DP's state vs answer.
2. **An extra condition that was harmless but blurred the invariant.** The first passing version reset only when `sum < 0 && num > sum`. With `sum = -4`, `num = -6` it did **not** skip the −6. It added it: `sum = -10`, while the true best ending there is −6 alone. It then reset on the first number greater than that inflated sum, which can be another negative (`-7 > -10`), not necessarily a positive:

   | num | reset? | `sum` (first version) | true best ending here | `maxSum` |
   | --- | ------ | --------------------- | --------------------- | -------- |
   | −4 | (start) | −4 | −4 | −4 |
   | −6 | `-6 > -4`? no | **−10** | −6 | −4 |
   | −7 | `-7 > -10`? yes | −7 | −7 | −4 |

   The answer stayed correct because `sum` is only wrong while the true best-ending-here is a negative number already ≤ `maxSum`, so nothing it misses could ever win. But "correct because of a side argument" means an interviewer asking *"what does `sum` hold?"* doesn't get a one-sentence answer. Deleting `&& num > sum` restored the invariant. As a side effect it was ~25% faster at `-O2` (10M random ints: 7.1 → 5.3 ms), because the two-part data-dependent test compiled to worse code than a single `sum < 0`. That's a constant factor no interview cares about.

**Insight: why greedy works here (and why it's still DP).** I couldn't justify the greedy at first. The justification is that Kadane's **is** the DP, with a choice so simple it collapses to one variable:
* **Only two options, and they cover every start.** Every subarray ending at `i` is `[j..i]`. Either `j = i` (just `nums[i]`), or `j < i`, which is `nums[i]` plus some subarray ending at `i-1`, and the best of those is `nums[i] + best[i-1]`. So the `max(0, …)` compares **all** possible starts at once. For `[2, -3, 4]`, the candidates ending at 4 are `[4]` = 4, `[-3, 4]` = 1, `[2, -3, 4]` = 3: the best is `4 + max(0, -1)` = 4, with no enumeration.
* **Why one number per index is enough (the safety argument).** Two candidates ending at `i` with sums 5 and 2: whatever comes next is added to **both equally**, so 5 beats 2 under every possible future. The loser can be thrown away permanently. **Greedy is safe exactly when a discarded choice can never become the better one later.** Coin Change is the contrast: two ways to reach a state differ in what's left, so the future decides and you need the table.
* **Backpack picture:** the carried sum is a backpack whose weight is added to everything that follows. A negative one makes every future subarray worse by exactly that much, so put it down.
* This is the same clamp as `max(0, child)` in Binary Tree Maximum Path Sum (`08-Trees/README.md` [14], "Kadane's on a tree"), and the one-variable version of Maximum Product Subarray (`14-1D-Dynamic-Programming/README.md` [9]), which needs two variables because a negative can flip the worst into the best.

**Edge cases:** `maxSum` starts at `INT_MIN`, not 0, so an all-negative array returns its largest element (`[-3]` → −3), the classic trap. Overflow: |sum| ≤ 1e5 × 1e4 = 1e9 fits in `int` under the given constraints. Empty input returns `INT_MIN`; LeetCode guarantees n ≥ 1.

**Follow-ups to have ready (prose):**
* *Return the indices:* record the start index on each reset and `(start, i)` whenever `maxSum` improves. That's the original two-pointer idea, made workable.
* *Divide and conquer:* the best lies entirely in the left half, entirely in the right half, or crosses the middle (best suffix of the left + best prefix of the right). O(n log n), worse than Kadane's.
* *Circular array* (LC 918): `max(Kadane, total − minimum subarray)`, except when everything is negative.

**Complexity:** O(n) time, single sequential pass (prefetch-friendly), O(1) space, no allocations.

### [2] Jump Game
**Pattern:** Farthest Reach (Pattern 2). Keep `farthest` = the farthest index reachable from anything seen so far. Walk left to right: if the index you're about to stand on is past `farthest`, you're stuck; otherwise extend `farthest` with `i + nums[i]`. Invariant: **every index ≤ `farthest` is reachable** (you can always stop short).

**Initial approach: a fuel tank, traced on paper.** The "work backwards from the end" explanation I'd read the night before didn't click, so I traced forward instead. `stepsLeft` = fuel: arriving at each index burns one unit (`if (--stepsLeft < 0) return false`), and the start is a "free pass" (`stepsLeft = 1`, because arriving at index 0 also burns one).

**Stumble 1: add or max?** My first instinct was `stepsLeft += nums[i]`. Tracing `[3, 2, 1, 0, 1]` showed it was off: after index 0 I have 3, and after moving to index 1 I should have 2, not 2 + 2 = 4, because **the two tanks cover the same indices**. Standing on index `i` doesn't add `nums[i]` steps; it offers a **fresh tank** of `nums[i]` from here. You keep whichever tank is fuller, so it's `max(nums[i], stepsLeft)`. Adding counts the same stretch of road twice (fake reach). That's also why the greedy is safe, the same argument as Maximum Subarray `[1]`: two tanks starting at the same index differ only in size, so the bigger one is at least as good under every future, and the smaller can be thrown away for good.

**Stumble 2: the early exit with a sign error.** The review asked for a success exit (best case O(n) → O(1): stop once the fuel covers what's left). I wrote `stepsLeft >= i + n - 1`. The distance left is `(n - 1) - i`, which **shrinks** as `i` moves; `i + n - 1` **grows**. On `[3, 1, 5, 0, 0, 0, 0]` the fuel at i = 2 is 5 with 4 indices left (should exit), but the threshold was already 8, so it scanned to the end. **Why fuzzing didn't catch it:** the two expressions are equal at i = 0 and the wrong one is larger after that, so the exit could only fire *late*, never wrongly. The answers stayed right; only the speed-up was missing. **Lesson: a too-strict early exit is invisible to correctness fuzzing.** Test it with an input where the exit should fire in the *middle*, and check that the loop actually stops (here: big value at N/2 → half the time of a full scan). The fix, `stepsLeft + i >= n - 1`, is already in absolute terms: `stepsLeft + i` *is* the farthest reachable index, which led to the final version.

**Stumble 3: the reversed stuck test.** Switching to `farthest`, I first wrote `if (i < farthest) return false`, the opposite of "I'm past the frontier". It returned false exactly when things were going **well** (spare reach ahead) and kept going when I was stuck **exactly at** the frontier: `[2, 3, 1, 1, 4]` → false, `[1, 0, 1]` → true, while `[1, 1, 1, 1]` still passed (`farthest == i` at every step). Found it by debugging. The guard is "the index I'm standing on is beyond what I can reach": `farthest < i`. Say the invariant out loud, then read the comparison back against it.

**Why farthest beats fuel (measured, constant factor only).** Same algorithm, same O(n), but in the fuel version every iteration has to wait for the previous `stepsLeft`, decrement it, then compare and pick the max (decrement → compare → `cmov`, three steps in a row: a *loop-carried dependency chain*). In the farthest version `i + nums[i]` doesn't depend on the previous iteration, so the CPU computes it ahead of time and the chain is just compare → `cmov`. At `-O2` on 10M elements: without early exits, 7.0 vs 3.5 ms (2×); with the early exit in both, 6.95 vs 5.27 ms (1.3×, because the extra exit test per element is common to both). Interviews don't care about this; the clearer invariant matters more, and it's the form Jump Game II builds on (count how many times the frontier has to be extended).

**The backward version (what I read first) is the mirror image:** keep a `goal` starting at the last index, walk right to left, and move `goal` to `i` whenever `i + nums[i] >= goal`. The answer is `goal == 0`. Same greedy, same O(n), with the frontier moving the other way.

**Edge cases:** `[0]` → true (already at the end), `[0, 1]` → false, `[2, 0, 0]` → true (reach carries over zeros). With n ≥ 1, the `return true` after the loop is unreachable: reaching index n−1 at all means `farthest ≥ n − 1`, so the early exit fires inside the loop first. No overflow: `i + nums[i]` ≤ 1e4 + 1e5.

**Complexity:** O(n) time worst case, O(1) best case (early exit), O(1) space, one sequential read.
