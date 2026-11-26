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

### Pattern 2: Farthest Reach → a frontier instead of a search
Track the farthest index reachable from anything seen so far. Feasibility: stop if the current index ever passes the frontier. Fewest moves: count how many times you must extend the frontier (each extension is one BFS level).
* *Discipline:* the invariant is "everything up to `reach` is reachable". Check the off-by-one at the end (reaching the last index vs passing it) and the single-element input.

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
