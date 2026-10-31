# Intervals - Topic Guide

## 1. Core Fundamentals

An **interval** is a pair `[start, end]` that stands for a stretch of something continuous: time (a meeting from 9 to 10), addresses (a memory region from `0x2000_0000` to `0x2000_FFFF`), or positions on a number line. Interval problems are about how these stretches **relate to each other**: which ones overlap, how to combine them, how many are active at once, and how few you'd have to drop so the rest don't collide.

There's almost no new data structure here. The whole topic rests on **one move: sort, then do a single linear pass.** The hard part is choosing *what* to sort by and *what* to carry through the pass.

### The Overlap Test (the one condition everything uses)
Two intervals `a` and `b` overlap if and only if **each one starts before the other ends**:
```text
a.start <= b.end  &&  b.start <= a.end
```
It's easier to remember the opposite. They **don't** overlap when one ends before the other starts: `a.end < b.start || b.end < a.start`. Negate that and you get the test above.
* **After sorting by start, half of this test is free.** If `a` comes before `b` in sorted order, then `a.start <= b.start <= b.end` already holds. The only check left is **`b.start <= a.end`**: does the next interval start before the current one ends?
* **The six ways two intervals can sit** (with `a` drawn first):
```text
a: |-----|                      a: |----------|
b:          |-----|  disjoint   b:    |----|         b inside a (containment)

a: |-----|                      a:    |----|
b:    |-----|        partial    b: |----------|      a inside b
        overlap
a: |-----|                      a: |-----|
b:       |-----|     touching   b: |-----|           identical
```
* **Containment is the case that breaks naive code.** When you merge `a` and `b`, the new end is **`max(a.end, b.end)`**, not `b.end`. For `[1, 10]` then `[2, 3]`, taking `b.end` would shrink the block to `[1, 3]`.

### Touching Endpoints: `<` vs `<=` (the problem decides, not you)
Do `[1, 4]` and `[4, 5]` overlap? There's no universal answer. Each problem defines it, and that one character is the most common bug in this topic.
| Problem | `[1,4]` and `[4,5]` | Why |
| -- | -- | -- |
| Merge Intervals / Insert Interval | **overlap** → merge into `[1, 5]` | closed intervals: point 4 is in both |
| Non-overlapping Intervals | **don't overlap** | the problem says intervals that only touch are fine |
| Meeting Rooms I / II | **don't conflict** | a meeting ending at 4 frees the room for one starting at 4 (half-open `[start, end)`) |

**Read the examples before writing the comparison.** Then decide whether it's `<` or `<=` and write that choice down as a comment.

### Why Sorting Works (the invariant)
Sort by start and sweep from left to right, keeping a current **block** (the merged interval so far). Suppose the next interval starts **after** the block ends (`next.start > block.end`). Then **no later interval can touch the block either**, because every later interval starts even further right. So the block is final: emit it and start a new block.

That's the whole proof, and it's why one pass is enough. Without sorting, an interval at the far end of the array could overlap the first one, so you'd have to compare every pair: O(n²).

### Sort by Start or Sort by End?
Both orders show up, and they answer different questions.
* **Sort by start** → **build or merge blocks** ("what's the combined coverage?"). Merge Intervals, Insert Interval, Meeting Rooms.
* **Sort by end** → **keep as many intervals as possible with no overlaps** ("which ones do I keep?"). This is the classic **activity selection** greedy: always keep the interval that **finishes earliest**, because it leaves the most room for everything after it.
    * *Why earliest end is safe (exchange argument):* take any optimal set of intervals. Its first interval ends no earlier than the earliest-finishing interval overall, so swap that first one for the earliest-finishing one. Nothing after it can collide, because the replacement ends sooner. The set has the same size and is still valid, so being greedy never loses. (This is a preview of topic 16, Greedy.)
    * You can also sort by start and fix collisions as you go: when two overlap, **drop the one that ends later**. It's the same greedy rule.

### "How Many at Once?": the Sweep Line
Some problems don't ask about merging. They ask for the **maximum number of intervals active at the same moment** (meeting rooms needed, CPU cores needed, peak concurrent connections). Two equivalent tools:
* **Events (the sweep line):** turn each interval into two events, `+1` at `start` and `−1` at `end`. Sort all `2n` events by time and walk them with a running counter. The answer is the counter's maximum.
    * **Tie-breaking is the `<` vs `<=` decision in disguise.** If one meeting ends at 4 and another starts at 4, process the **end first** (half-open), or you'll count one room too many.
    * A neat version: sort all the starts into one array and all the ends into another, then walk them with two pointers. Whichever comes next is the next event.
* **Min-heap of end times:** sort by start. For each meeting, if the earliest-ending active meeting (the heap top) has finished by now, pop it, since that room is free again. Then push the current meeting's end. The heap's size is the number of rooms in use, and its maximum size is the answer. This is the Heap topic's "only the top matters" idea (topic 10): you only ever need *the room that frees up soonest*.

### Offline Queries (answer them in your own order)
When the problem gives you a batch of queries ("for each point, the smallest interval that contains it"), you don't have to answer them in the order asked. **Sort the queries too**, sweep the intervals and the queries together from left to right, and write each answer back to the query's **original index**.
* You must keep the original index alongside each query before sorting (e.g. sort a `vector<pair<int,int>>` of `(value, index)`, or sort an array of indices).
* Intervals that have already ended by the current query point get discarded from the heap **lazily**, when they reach the top. This is the lazy-deletion trick from Heap §2 Pattern 6.

### Complexity
* **Sorting dominates: O(n log n).** The sweep itself is O(n).
* **Insert Interval is the exception at O(n):** the input is already sorted and non-overlapping, so you skip the sort.
* **Space:** O(n) for the output (or the heap/events), plus the sort's own overhead. `std::sort` is introsort, which uses O(log n) stack. Say "O(n) for the output, O(log n) extra for the sort" in an interview.

### The Hardware Reality
* **`vector<vector<int>>` is a bad layout for a pair of ints.** Every inner vector is its own heap allocation of 24 bytes of header (pointer, size, capacity) pointing at 8 bytes of data somewhere else. Sorting it chases a pointer on every comparison. An interval as `std::pair<int,int>`, `std::array<int,2>` or a `struct { int start, end; }` is 8 contiguous bytes. A whole array of them fits in a few cache lines, and the sort just swaps 8-byte values. LeetCode forces the nested-vector signature, but say this in an interview.
* **Take comparator arguments by `const&`.** With `[](vector<int> a, vector<int> b) { … }`, every comparison **copies two vectors**, which means two heap allocations. Sorting does about n log n comparisons, so that's about 2 · n log n `malloc`/`free` pairs for nothing. `[](const auto& a, const auto& b) { return a[0] < b[0]; }` costs nothing.
* **The default `std::sort` on `vector<vector<int>>` already works.** It compares lexicographically (start first, then end), so you only need a custom comparator when sorting by end, or to make the intent explicit.
* **Merge in place into the output.** Merging into `result.back()` (extend its end with `max`) needs no temporary block variable and no extra copies.

### Where Intervals Show Up in the Real World
* **Memory allocators:** when a block is freed, the allocator **coalesces** it with free neighbours, which is Merge Intervals on address ranges. That's how `malloc` fights fragmentation.
* **Linker scripts:** `ld` checks that output sections don't overlap in memory ("section `.data` overlaps section `.bss`"). That's the overlap test on `[VMA, VMA + size)`.
* **MPU / MMU configuration:** on Cortex-M, MPU regions are address intervals, and overlapping regions resolve by priority.
* **Compilers:** **linear-scan register allocation** sorts variables' live ranges by start and keeps the active ones ordered by end. It's Meeting Rooms II, where rooms are registers. When there are more active ranges than registers, the compiler spills to the stack.
* **Networking:** TCP reassembles out-of-order segments by merging byte ranges (SACK blocks are intervals). Firewall and routing tables match IP ranges.
* **Calendars and schedulers:** free/busy lookups, booking conflicts, and how many cores a set of jobs needs at peak.

---

## 2. General Summary / Quick Reference

When tackling Interval problems, keep these core patterns in mind:

### Pattern 1: Sort by Start, Merge into the Last Block
Sort by start. For each interval: if it overlaps `result.back()`, extend the end with `max`. Otherwise push it as a new block.
* *Use for:* Merge Intervals.
* *Discipline:* `max` on the end (because of containment), and check whether touching counts as overlap.

### Pattern 2: Three Phases on Already-Sorted Input (no sort needed)
The input is already sorted and non-overlapping, so walk it in three stretches: (1) intervals that end **before** the new one starts go straight to the output, (2) intervals that **overlap** the new one get absorbed into it (min of the starts, max of the ends), then the merged interval goes to the output, (3) everything after goes straight to the output.
* *Use for:* Insert Interval.
* *Discipline:* O(n). Don't append and re-sort, which works but throws away the sortedness you were given. Remember the edge cases: empty input, new interval before everything, new interval after everything.

### Pattern 3: Keep the Earliest End (activity selection, count the removals)
Sort by end (or by start, dropping the later-ending one on each collision). Keep an interval whenever it starts at or after the last kept end, otherwise count it as removed.
* *Use for:* Non-overlapping Intervals (minimum removals = n − maximum kept).
* *Discipline:* touching intervals don't count as overlapping here.

### Pattern 4: Any Overlap at All? (sort, check neighbours)
Sort by start, then check each interval against the one right before it. Any overlap means a conflict.
* *Use for:* Meeting Rooms (can one person attend every meeting?).
* *Discipline:* after sorting, only **adjacent** pairs need checking. Half-open: ending at 4 and starting at 4 is fine.

### Pattern 5: Maximum Concurrency (min-heap of ends, or a sweep line)
Sort by start and keep a min-heap of the end times of active intervals. Pop the ones that have finished, push the current one, and track the heap's size. Or: sort starts and ends separately and sweep with two pointers.
* *Use for:* Meeting Rooms II (minimum number of rooms).
* *Discipline:* at a tie, process the end before the start.

### Pattern 6: Offline Queries + Heap
Sort the intervals by start and the queries by value (keeping their original indices). For each query in order: push every interval that starts at or before it into a min-heap keyed by **(size, end)**, lazily pop the ones that end before the query, and the top is the answer (or −1 if the heap is empty).
* *Use for:* Minimum Interval to Include Each Query.
* *Discipline:* write answers to the **original** index, interval size is `end − start + 1` (inclusive), and an expired interval can be discarded forever because queries only move right.

### The Non-Negotiable Discipline
* **Is touching an overlap?** Check the examples, then pick `<` or `<=` deliberately.
* **Sort by what?** Start for merging and conflicts. End for "keep the most". Queries too, if they're offline.
* **Merged end = `max`**, never just the latest interval's end.
* **Comparators take `const&`.**
* **State the complexity as O(n log n)** (sort) + O(n) (sweep). Insert Interval is O(n).

---

## 3. Problem Strategies & Patterns

<!-- NeetCode 150 Intervals (6): Insert Interval · Merge Intervals · Non-overlapping Intervals · Meeting Rooms · Meeting Rooms II · Minimum Interval to Include Each Query -->

### [1] Insert Interval

> First problem of the topic, picked to get momentum back after 9 days off. It took **about 6 hours**. Getting the first passing version took about 1–2 hours, and the rest went into the review loop: two-pass → flat buffer → write-back → one pass. Commits: `ea17bae` (first pass), `286a5ea` (flat `array` buffer), `cae8a55` (struct + write-back), `a89c943` (one pass). Most of the walls were C++ (references, `emplace_back`), not the algorithm.

* **The Core Pattern:** the input is **already sorted and disjoint**, so there's no sort: one pass, O(n) (§2 Pattern 2). Every interval falls into one of three groups relative to the new one:
    * **Final:** it can't touch the new interval, so copy it unchanged.
    * **Absorbed:** it overlaps, so it widens the block.
    * **After:** everything once the block has closed, copied unchanged.
  **The condition for each group must say "can this still change?", not "does this come earlier?"** That one distinction cost hours (see the Struggle below).
* **My committed design (the hold-back version):**
    * Phase 1 walks the intervals that **start** before the new one. Each that also **ends** before it is final, so copy it.
    * The first one that reaches into the new interval is **held back** as the block: its start, and the `max` of the two ends.
    * Then a write-back merge loop runs over the rest, with a flush at the end.
    * The block is seeded with the earliest start in its group, so the start never needs correcting, and there's **no `min`**.
    * The textbook version is equivalent: phase 1 copies the intervals that **end** before the new one starts, phase 2 absorbs with `min` on the start and `max` on the end, phase 3 copies. It's more common in interviews, and either is fine to defend.

* **The "Gotcha":**
    * **Touching counts as overlap:** `[1,4]` + `[4,5]` → `[1,5]`, so the test is `<=`.
    * **Containment:** the merged end is `max(a.end, b.end)`, never just the latest end. `[1,10]` absorbing `[2,3]` must stay `[1,10]`.
    * **The final flush:** after the loop, the last block is still sitting in the locals. Forget to push it and the last interval silently disappears.
    * **If the block is seeded with the new interval, the start needs `min`.** See "Why `min`?" below.

* **The Struggle & Insights:**
    * **Stage 1: insert, then merge (two passes).** I inserted the new interval at its sorted position in a scratch vector, then merged. The idea was right, but the code had two bugs:
        * **The new interval was never appended when it belonged at the end.** The insert loop only inserted when some interval started *after* it. With empty input, this left the vector empty, and `.front()` on an empty vector is undefined behaviour. ASan stayed quiet only because `reserve` had already allocated the memory.
        * **The merge loop lost track of where the block lived.** It was sometimes already pushed to the result and sometimes still pending, and each of the three branches assumed something different. On LC example 2 this pushed `[3,8]` twice, and on `[3,5],[7,9] + [1,2]` it dropped `[3,5]`.
        * **The fix:** make **`result.back()` the block**. Seed the result with the first interval. Then each next interval either overlaps `back()` (extend it with `max`) or doesn't (push it). That's two branches and no pending state, and it's the Merge Intervals loop (§2 Pattern 1).
    * **A reference can't be re-pointed.** `auto& ref = mergeVector.front();` binds to element 0 *forever*. A later `ref = resultVector.back();` doesn't make `ref` refer to `resultVector.back()`. It **copies the value into element 0**. So my "reference" was really a value variable hiding inside the input buffer, which is why some cases passed by luck. The same misconception came back at the end (see the last bullet).
    * **The review measured the data layout, not the algorithm** (n = 100k, per call):

        | Version | Heap allocations | Bytes | Time |
        | -- | -- | -- | -- |
        | two-pass, `vector<vector<int>>` scratch buffer | 100,003 | 5.6 MB | ~2,970 µs |
        | one pass straight into the result | 100,001 | 3.2 MB | ~2,570 µs |
        | one pass, flat `vector<array<int,2>>` | **1** | 0.8 MB | **~72 µs** |

      Fixing the algorithm gained about 15%. Fixing the **layout** gained about 40×. Every `vector<int>` holding two ints is a separate `malloc`, behind a 24-byte header (pointer, size, capacity). LeetCode forces the nested signature, so say this out loud in the interview instead. Also from the review:
        * **A redundant check:** `cur.start >= block.start` is always true after sorting. Writing it showed I didn't trust my own invariant.
        * **`.front()` / `.back()` read as "first/last element of a container", not "start/end of an interval".** In `result.back().back()`, `back()` meant "last row" on the left and "interval end" on the right. Use `[0]`/`[1]`, or name the fields: `struct Interval { int start, end; }`, or a structured binding `auto [start, end] = arr;` (works for `std::array`, not `std::vector`).
    * **Q&A: why `emplace_back(a, b)` gave `[b]`.** `resultVector.emplace_back(1, 3)` on a `vector<vector<int>>` built **`vector<int>(1, 3)`, a vector of size 1 filled with 3**, so the start was thrown away. The rule: **parentheses never select the `initializer_list` constructor; braces prefer it.** `emplace_back` forwards its arguments into a parenthesised constructor call, so `(1, 3)` matches `vector(count, value)`. `-Wall -Wextra` gives **no warning**. What I verified, beyond what the other LLM said:
        * **`emplace_back({1, 3})` fails to compile on every conforming compiler.** A braced list has no type, so the template parameter can't be deduced. It isn't compiler-dependent.
        * **`emplace_back(std::vector<int>{1, 3})` gains nothing.** It builds a temporary and moves it in, which is what `push_back({1, 3})` already does.
        * **`vector<array<int,2>>::emplace_back(a, b)` doesn't compile, even in C++20.** `std::array` wraps a nested C array, and parenthesised aggregate init can't reach inside it. `vector<Interval>::emplace_back(a, b)` with a plain struct does work in C++20.
        * **Rule of thumb:** use `push_back({…})` for aggregates and for "these are the values". Use `emplace_back(args…)` only for real constructor arguments, after checking which constructor they pick. Here both cost one allocation per interval, so `emplace_back` saved nothing anyway.
    * **Write-through vs write-back (where the block lives).** My first "local" was a **shadow copy**. On every merge I still wrote to `result.back()`, then read the result back into the local. That gave two sources of truth to keep in sync. The fix was to keep the block **only** in locals: merges touch only the locals, and the block goes to the result **once**, when it closes, plus a **flush after the loop**. Trace on `[1,3] [2,5] [4,6] [8,9]`: write-through makes 4 writes and keeps a half-finished interval in the result, while write-back makes exactly 2 writes, both final. I forgot the final flush and found it by debugging, which is the same bug that dropped `[3,5]` in stage 1. It's the ADC-averaging pattern: accumulate in a register, store once per window, flush the partial window at the end. Expect it again in Merge Intervals, run-length encoding and grouping problems. The speed gain here is tiny, and the real value is the clean contract.
    * **"Ends before", not "starts before".** I wanted phase 1 to copy every interval that starts before the new one. The counterexample: `[[1,5]] + [3,8]` copies `[1,5]` as if it were final and outputs `[1,5] [3,8]`. The right answer is `[1,8]`. An interval can start earlier and still reach into the new one, so "final" has to be tested on the **end**. The same mix-up came back later as a dead `break` (last bullet).
    * **Why `min`? (the question I asked three times).** In two-pass, the merge loop sees intervals in **start order**. On LC2, `[3,5]` arrives *before* `[4,8]` and **becomes** the block, so the start is 3 from the moment it's loaded, and later intervals only push the end. In one pass, seeding the block with the new interval `[4,8]` lets it **jump ahead of `[3,5]`**. Then `[3,5]` gets *absorbed into* it, and without `min` the 3 is lost: `[4,10]` instead of `[3,10]` (24,367 of 200,000 fuzz cases wrong). Only the first absorbed interval can start earlier, so `min` changes the start at most once, but it's required. My hold-back design avoids the problem instead: it seeds the block from the earliest interval of its group, as two-pass did, so no `min` is needed.
    * **Pointers, flags and sentinels (the last mile).** To mark "maybe found the held-back interval" I first wrote `std::vector<int>& ref = nullptr;`, which doesn't compile. A reference must bind to an object, can't be null, and can't be re-pointed. "Maybe refers to something, maybe nothing" is a pointer:
        * declared as `const std::vector<int>* ref = nullptr;` (`const`, because the input is `const&`)
        * assigned with `ref = &intervals[i];`
        * read with `(*ref)[0]`, with the parentheses, because `[]` binds tighter than `*`

      The same draft also had a **dead `break`**: it tested `start >= new.start` inside a loop that only runs while `start < new.start`. The test I meant was on the **end**, the same mix-up again. I dropped the pointer and used a copied `vector` checked with `.empty()` (a heap allocation used as a flag), then an `INT_MIN` sentinel (only safe because the input values can't be negative). Finally it clicked: **make the default value of the block the "not found" answer.** Seed the block with the new interval, and overwrite it only if a held-back interval exists. Then no flag, no sentinel and no post-loop `if` are needed.
    * **Process lesson:** a correct solution existed by hour two. The other four hours went to constant factors I'd measured at about 15%. Learn from the review, but time-box it, and turn leftover polish into notes like this one.
    * **Known leftover, not taken:** after the block closes, my loop still runs every remaining interval through the block: one comparison and a copy each, then a push. It's correct, because the remaining intervals are disjoint so nothing merges. A separate phase-3 copy would say "nothing after this can change" more directly.

* **Time & Space Complexity:**
    * **Time O(n):** a single pass, with no sort (the input is already sorted).
    * **Space O(1) extra** beyond the output. The result is `reserve`d to n + 1 once.
    * **The output still costs n + 1 heap allocations** because of the `vector<vector<int>>` return type. With a flat `vector<Interval>`, it would be one allocation (measured: ~40× faster).

### [2] Merge Intervals

> Same day as Insert Interval, and the payoff for it. I reused Insert Interval's write-back loop unchanged, final flush included, and it passed once I remembered to sort: I forgot the sort on the first run. First pass `fbf7504`. The review then moved the cost from the loop to the **copy and sort before it**, and the flat-copy version followed one round later.

* **The Core Pattern:** sort by start, then one write-back sweep (§2 Pattern 1).
    * Seed the block with the first interval. For each next interval: if it starts at or before the block's end, widen the end with `max`. Otherwise push the block and start a new one from this interval.
    * After the loop, flush the last block.
    * Sorting by start is what makes one pass enough (§1, "Why Sorting Works"). Once an interval starts past the block's end, nothing later can touch the block, so it's final.
    * Unlike Insert Interval, the input isn't sorted, so the sort is the whole cost: **O(n log n)**.

* **The "Gotcha":**
    * **Don't forget the sort.** Without it, the sweep's invariant is false, and it silently merges only adjacent overlaps.
    * **Touching counts as overlap** (`<=`), and **containment needs `max`** on the end. Same as Insert Interval.
    * **If the input is `const&`, you need a copy before you can sort.** The only decision is what layout that copy uses (below).

* **The Struggle & Insights:**
    * **What `std::sort` does to `vector<vector<int>>` (my wrong assumption).** I assumed it compares only the first number, and that "even if it sorts the inner vectors too, that's what we want". Both are wrong:
        * **It's lexicographic.** It compares element 0, and only on a tie element 1. Verified: `[1,4] [1,3] [0,9] [5,2]` → `[0,9] [1,3] [1,4] [5,2]`, so `[1,3]` comes before `[1,4]`.
        * **It never touches the inside of an element.** `[5,2]` stayed `[5,2]`. `sort` rearranges whole elements, and swapping two inner vectors just swaps their internal pointers. Sorting *inside* an interval would corrupt it (`[5,2]` would become a different interval). It's not a harmless extra.
    * **Equal starts don't need a tie-break (the second wrong assumption).** I worried that a start-only comparator would break on `[1,3], [1,1]`. A comparator that says neither is smaller tells `sort` they're **equivalent**, so they end up next to each other in some order (`std::sort` isn't stable). Both orders merge to `[1,3]`. **Proof:** two intervals with the same start always overlap, because the second one's start equals the first one's start, which is ≤ the first one's end. So they always merge, and `max` takes the bigger end regardless of order.
    * **Never "fix" ties with `<=`.** `std::sort` requires a *strict weak ordering*, so `comp(a, a)` must be `false`. A `<=` comparator is **undefined behaviour**: libstdc++'s insertion sort then runs past the end of the array. Verified with 100 equal intervals plus one smaller: `AddressSanitizer: heap-buffer-overflow`. **A comparator always uses `<`.**
    * **A struct has no `operator<`.** My flat version failed to compile with `no match for 'operator<' (operand types are 'Interval' and 'Interval')`. C++ doesn't invent comparisons for your types. There are two ways to fix it:
        * **Pass a comparator.** Right when the order belongs to this one use, like "by start" here. Its parameters must be `const&`, or every comparison copies two elements.
        * **Give the type a natural order.** In C++20, `auto operator<=>(const Interval&) const = default;` generates all six comparisons, lexicographic in member declaration order (verified).
    * **The real cost was before the loop.** Measured at n = 100k (random starts, 60,752 intervals survive the merge):

        | Version | Heap allocations | Time |
        | -- | -- | -- |
        | First pass: deep copy into `vector<vector<int>>`, default sort | 160,754 | ~18.4 ms |
        | Deep copy, merge into the copy (compaction) | 100,001 | ~12.7 ms |
        | Sort the caller's input in place | 60,753 | ~11.6 ms |
        | **Flat `vector<Interval>` copy, sort by start** (final) | 60,754 | **~5.9 ms** |

      The 60k allocations left in every version are the output's inner vectors, which the `vector<vector<int>>` return type forces.
    * **Why the flat layout sorts faster (it took me two asks).** Real addresses from a run: each nested element is a 24-byte header in the outer buffer, **pointing to** an 8-byte payload in its own 32-byte heap chunk. That's 56 bytes to store 8. A flat `Interval` is 8 bytes, stored back to back. In a sort:
        1. **Every comparison is two dependent loads instead of one.** The CPU must load the pointer before it can load `start`. In the flat layout, the address is base + 8·i, so one load.
        2. **Fewer bytes per cache line.** A 64-byte line holds 8 `Interval`s, but only 2⅔ headers, and their ints live on other lines.
        3. **Cheaper swaps.** Three 64-bit words per swap against one.
        4. **The sort scatters the nested layout.** It moves the headers around, so neighbouring headers end up pointing far apart. The flat data moves *with* its element, so neighbours stay neighbours.

      Sort-only timing: 7.3 vs 4.65 ms at 100k (1.6×), and **330 vs 112 ms at 2M (2.9×)**. The gap grows as the nested version outgrows the cache. **Correction recorded:** I was told a fragmented heap would make the nested sort even slower. Shuffling which chunk holds which interval measured no difference (7.4 vs 7.3 ms), because point 4 scrambles the access order anyway.
    * **"If I'm allowed to mutate, isn't the nested sort still the bottleneck?" Yes.** Copying into a flat array first and sorting the copy beats sorting the input in place: 0.3 ms to copy + 4.7 ms to sort = **5.0 ms vs 7.3 ms**. The O(n) copy is one sequential pass, which the prefetcher handles well, and it pays for itself across about 1.7 million comparisons. **The rule:** when an O(n log n) or worse loop is limited by memory access, spend one O(n) pass converting the data into a contiguous layout first. It's the same idea as DMA-ing scattered data into one buffer before processing it. In an interview, say "I'll copy, unless the API says the function owns the input". Quietly reordering the caller's data is a contract question, not a free optimisation.
    * **The compaction variant and its trap.** Merging into the copy itself, with a write index and a read index and then a truncate (like `std::unique`), reuses the copy's inner vectors for the output. But while nothing has merged yet, the two indices are equal, and **moving an element onto itself empties it**. The review's first version of this segfaulted for exactly that reason. Guard the move whenever the indices can be equal.
    * **`std::pair<int,int>` vs `struct Interval`.** The speed is identical: same 8 bytes, same layout, 4.78 vs 4.65 ms. `pair` comes with a lexicographic `<`, so the default sort works. But `first`/`second` say nothing (the `front()`/`back()` problem again), and in libstdc++ `pair` is **not trivially copyable** (verified: its assignment operator is user-written). That doesn't matter in a sort, where the copy gets inlined. But it formally rules out raw-byte copying: `memcpy`, `std::bit_cast`, DMA or shared-memory buffers. Use the struct in a systems interview. `pair` is fine for a quick solution.
    * **A carry-over from Insert Interval:** `reference = intervalsCopy[i]` is a single struct copy. Assigning field by field can fall out of sync if the struct gains a member.

* **Time & Space Complexity:**
    * **Time O(n log n)** for the sort, plus O(n) for the copy and the sweep.
    * **Space O(n)** for the flat copy, which is **one allocation**, plus O(log n) stack for introsort, plus the output.
    * **The output still costs one heap allocation per merged interval,** because of the `vector<vector<int>>` return type.
