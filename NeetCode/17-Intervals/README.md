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

### [3] Non-overlapping Intervals

> Third problem of the day. I started by building intuition rather than code, and two wrong models came first. First pass `23871ae` (sort by start + `min`), then the sort-by-end version after the review.

* **The Core Pattern:** the activity-selection greedy (§1, "Sort by Start or Sort by End?"; §2 Pattern 3). Minimum removals = n − maximum number you can keep without overlaps.
    * **Sort by end.** Keep a single `int`: the **end of the last kept interval**.
    * For each next interval: if it starts before that boundary, it collides, so **drop it and count it**. Otherwise keep it, and the boundary becomes its end.
    * Because of the sort, the kept interval is always the one that ends first. So the only decision on a collision is "drop the newcomer". There's no `min`.
    * **The equivalent sort-by-start version:** on a collision, drop whichever **ends later**, so the boundary becomes the `min` of the two ends. It gives the same count and is O(n log n) either way.

* **The "Gotcha":**
    * **Touching isn't overlapping in this problem.** `[1,2]` and `[2,3]` both stay, so the test is a strict `<`. That's the opposite of Merge and Insert Intervals, which used `<=` (§1, "Touching Endpoints").
    * **Compare against the last kept interval, not the previous element.** See the first stumble below.
    * **The boundary moves only when you keep something.** That's the `else` branch, and it's required.

* **The Struggle & Insights:**
    * **Wrong model 1: check each interval against the previous element.** On LeetCode example 1 (sorted: `[1,2] [1,3] [2,3] [3,4]`), it counts `[1,3]` vs `[2,3]` as a second collision and returns 2 instead of 1. But `[1,3]` had already been removed, and a removed interval can't conflict with anything. **Every comparison is against the interval still standing.**
    * **Wrong model 2: count the collision, but never choose a winner.** I always dropped the newcomer and kept the original. On `[1,100] [2,3] [4,5] [6,7] [8,200]` that gives 4 instead of 1, because `[1,100]` swallows everything, and it was wrong on 8,142 of 100,000 fuzz cases. My own question pointed at the fix: *"If I start with `[1,100]` and then see smaller intervals, won't I miss the others?"* Yes, unless the kept interval can be **replaced**. On a collision, drop the one that **ends later**, so the boundary can **shrink**: `[2,3]` replaces `[1,100]`, the boundary drops from 100 to 3, and the next three intervals fit.
    * **Why "drop the later end" is always safe (the exchange argument).** Every later interval starts at or after both colliding intervals, because of the sort. Any future interval that fits after the later-ending one also fits after the earlier-ending one. So the earlier end **never blocks more of the future**, and keeping it can't cost anything.
    * **Deleting invariants the sort already guarantees (three times in one problem):**
        1. **`cur.start >= kept.start` is always true** after sorting by start. It's the same dead check as Insert Interval's first pass.
        2. **The kept interval's start is never used,** so the "reference interval" was really just an `int` boundary. After `end = min(...)` it could even be an interval that isn't in the input: the dropped interval's start glued to the kept one's end, e.g. `{1, 3}` from `[1,100]` + `[2,3]`.
        3. **After switching to sort by end, the `min` is dead.** Every later interval ends at or after the kept one, so `min` always returns the boundary unchanged.
    * **I over-deleted once.** Along with the `min`, I commented out the `else` assignment, so the boundary was frozen at the first interval's end. On `[1,2] [2,4] [3,5]`, `[3,5]` was then compared against `[1,2]` instead of `[2,4]`, giving 0 instead of 1. **Two branches, two jobs:** a collision means drop and count, with the boundary unchanged. A fit means keep, and **the boundary becomes the kept interval's end**.
    * **The review (n = 100k at LeetCode's limits, many equal starts):**
        * **`reserve` the copy.** Without it, the copy reallocates 18 times while growing, for 2.1 MB of heap traffic instead of 0.8 MB in one allocation. The time hardly changes for 8-byte pairs, but leaving out `reserve` on a buffer whose size you know reads as careless.
        * **Compare one field, not the default `pair` order.** The default `pair` `<` breaks ties on the second field, which is extra comparisons for an order the algorithm never uses. A one-field `const&` comparator measured ~10% faster on tie-heavy input.
        * **Sort by end vs sort by start measured within ~5% of each other.** The value of sort-by-end is the simpler loop and the equivalence argument, not the speed.
    * **`pair` is acceptable here,** since `.first` and `.second` are each read in only one place. In longer code, use the struct from Merge Intervals `[2]` for named fields.

* **Time & Space Complexity:**
    * **Time O(n log n)** for the sort, plus O(n) for the sweep.
    * **Space O(n)** for the flat copy (one allocation), plus O(log n) for the sort.
    * **No output buffer.** The answer is a single `int`, so the copy is the only allocation.

### [4] Meeting Rooms

> NeetCode calls it "Meeting Schedule". File `meeting_rooms_i.cpp` (renamed from `meeting_schedule.cpp` in `94a9f27`). First pass `e9410d7`, **optimal as written**: the review found nothing to change. Every habit from `[1]`–`[3]` came through without prompting: a flat copy, a `const&` one-field comparator, the boundary as an `int`, and strict `<`.

* **The Core Pattern:** sort, then check each meeting only against the one just before it (§2 Pattern 4). The first conflict returns `false`.
    * I sorted by **end**. Sorting by **start** works just as well. It's a yes/no question with no choice of what to keep, so unlike `[3]` the sort key doesn't matter.

* **The "Gotcha":**
    * **Back-to-back meetings are fine:** a meeting ending at 5 and the next starting at 5 don't conflict (half-open intervals). So the test is strict `<`.
    * **NeetCode's `Interval` is a class with a constructor.** It's 8 bytes and trivially copyable (verified), so copying it is a plain memory copy. It's **not** an aggregate, though, so brace-initialising it calls the constructor rather than filling the fields directly.

* **The Struggle & Insights:**
    * **Why neighbours are enough (the proof behind the early exit).** Suppose every neighbour check passes. Then each meeting starts at or after the previous one ends, which chains into `s₁ < e₁ ≤ s₂ < e₂ ≤ s₃ < …`. The meetings are laid out end to end in time, so **no pair**, near or far, can overlap. Turned around: if any two meetings overlap, some neighbour check must fail.
    * **Can you avoid the sort? In general, no.** In the comparison model, deciding whether any two of n intervals overlap is at least as hard as checking n numbers for duplicates (turn each number into a tiny interval), which takes Ω(n log n) comparisons. The early exit skips the rest of the sweep, never the sort.
    * **With bounded integer times, yes: a difference array.** My first worry was that a meeting `(1, 140)` would mean marking 140 slots. It doesn't:
        * Write **two** entries per meeting: `+1` at its start and `−1` at its end.
        * Then make one pass with a running sum. At each time t, the sum is the number of meetings in progress, and a value of 2 means a conflict.
        * Cost: O(n + T), where T is the size of the time range.
        * **Touching endpoints cancel automatically:** `[0,5]` and `[5,10]` put −1 and +1 in the same slot. This is the sweep line from §1, with array indices playing the role of the sorted events.
    * **When the difference array wins, and its sizing traps.**
        * **It's a space-for-time trade that pays only when T is small** compared with n log n. Minutes in a day is 1,440 slots, 5.6 KB as `int32_t`, which fits in L1. Times up to 10⁶ is 4 MB and a 10⁶-step pass even for 3 meetings, so sorting wins there.
        * **The slot type must hold ±n.** 200 meetings starting at 9:00 put 200 in one slot, which overflows `int8_t`. Signed overflow is UB, and in practice the count wraps to a wrong value.
        * **The embedded angle:** a timeline or slot table, like a scheduler's slot map or a histogram over a fixed ADC range. With a small fixed domain, index into it directly. With a large or unbounded one, sort.
    * **`front()` is fine here:** it means "the first meeting", which is what it says. The earlier complaint was about `front()`/`back()` standing in for an interval's start/end.

* **Time & Space Complexity:**
    * **Time O(n log n)** for the sort, plus an O(n) sweep that exits early on a conflict.
    * **Space O(n)** for the copy, which is required because the input is `const&`. It's one allocation, plus O(log n) for the sort.
    * **The difference-array variant:** O(n + T) time and O(T) space.

### [5] Meeting Rooms II

> First pass `9f5eb40`, then the cleanup. I diagnosed my own wall before asking: "I'm keeping track of only one room, but a room taken by a very early meeting could go free". Once I knew what state was needed, **I designed the loop myself** and it was correct as described. Then two slips in the code, both caught by me.

* **The Core Pattern:** sort by **start**, then keep a **min-heap of room free times** (§2 Pattern 5).
    * Seed the heap with the first meeting's end.
    * For each next meeting: if the room that frees soonest (the heap's top) is free by this meeting's start, **pop it**. Then **always push** this meeting's end.
    * The answer is the heap's size at the end.
    * **Mental model:** each heap entry is a **room**, labelled with when it frees up. A pop-then-push isn't "remove a meeting, add a meeting". It's **relabelling the same room**. So the size is the number of rooms ever opened.

* **The "Gotcha":**
    * **Sort by start, not end. I slipped into end order out of habit from `[3]`/`[4]`.**
        * Rooms are handed out as time passes, so meetings must be processed **in the order they begin**.
        * Smallest failing case, in end order: `[2,3] [2,6] [6,9] [5,10]`. Here `[6,9]` is processed first and takes room A (free since 3). But `[5,10]` starts earlier and needed it, so a third room opens (true answer: 2).
        * End order was wrong on 4,059 of 200,000 fuzz cases. Start order: 0.
        * **Why it differs from `[3]`:** there you **choose which intervals to keep**, and end order is what makes that choice greedy. Here every meeting must get a room, so you're **simulating time**.
    * **`std::priority_queue` is a max-heap by default.** A min-heap needs the full template: `std::priority_queue<int, std::vector<int>, std::greater<int>>`.
    * **Back-to-back meetings share a room,** so the reuse test is `start >= top`.
    * **`if`, not `while`, is what makes "return the size" correct.** Each meeting does at most one pop and exactly one push, so the size **never shrinks**, and the final size is the peak. A `while` that pops every finished room gives the **current** occupancy instead. That's also a correct algorithm, but then you must track the maximum separately.

* **The Struggle & Insights:**
    * **Why my first two attempts failed.**
        * **Counting overlaps** counts colliding *pairs*, not rooms. Three meetings that all overlap each other form 3 pairs, and chains of meetings break the count in other ways.
        * **Updating one `lastMeetingEnd`** tracks a single room, so it can't see an earlier room becoming free.
        * **What was missing:** the state is **every busy room's free time**. The only question per meeting is "is the room that frees soonest free yet?" That's a min-heap's top.
    * **`top()` on an empty heap can't happen, and I should be able to say why.** The heap is seeded with one entry before the loop, and its size never shrinks. An interviewer pointing at `top()` will ask about this.
    * **The review (n = 100k, at most 135 meetings at once):**
        * **An unused leftover variable** from the "count overlaps" model, flagged by `-Wunused-variable`. Compile with warnings on, and dead code shows up before a reviewer sees it.
        * **`const&` instead of LeetCode's non-const stub,** because the input is never modified.
        * **Don't `reserve` the heap.** That cuts across "always reserve" from `[3]`: its size is bounded by **the answer k**, which you don't know up front. Reserving n would allocate 400 KB to hold 135 entries. It cut allocations from 10 to 2 and changed nothing in time. Reserve buffers whose final size you **know** (the copy), not ones bounded by the result. (`priority_queue` has no `reserve`, but it can be constructed from an already-reserved `vector` moved into it, and it keeps the capacity.)
        * **The two-sorted-arrays sweep is ~25% *slower* here** (~9.7 vs ~7.7 ms). That version sorts all starts and all ends separately, then walks them with two pointers: a start opens a room, an end frees one, and the peak is the answer. But it does **two** full sorts, while the heap version does **one** sort plus n operations on a heap of ≤ 135 entries, about 7 levels deep and sitting in L1. When nearly every meeting overlaps, the heap grows toward n and the gap closes. **The talking point:** "heap is O(n log k), two arrays is two full sorts, so it depends on how many rooms there are".
        * **The same answer from the difference array in `[4]`:** the **maximum** running sum is the room count, at O(n + T) for bounded times.

* **Time & Space Complexity:**
    * **Time O(n log n)** for the sort, plus **O(n log k)** for the heap operations, where k = the number of rooms.
    * **Space O(n)** for the copy (one allocation), plus **O(k)** for the heap.

### [6] Minimum Interval to Include Each Query

> The topic's only Hard. Built up in layers: a correct but infeasible brute force, a heap I had to *see* working before I trusted it, then a loop driven by the wrong sequence. First pass `9ce9e43`, then the cleanup. The review found nothing measurable to optimize.

* **The Core Pattern:** offline queries + sweep + min-heap with lazy deletion (§1, "Offline Queries"; §2 Pattern 6).
    * Sort the intervals by start. Sort the queries by value, **keeping each query's original index**.
    * For each query q in sorted order:
        * Push every not-yet-pushed interval with `start ≤ q` into a min-heap of **(size, end)**. A forward-only cursor tracks how far you've got.
        * While the top has `end < q`, pop it.
        * If the heap isn't empty, its top's size is the answer, written to the query's **original** slot.
    * **Why the start isn't stored:** an interval enters the heap only once `start ≤ q`, and every later query is larger. So that condition can never fail again, and the only remaining question is "has it ended?".

* **The "Gotcha":**
    * **The queries drive the loop, and the intervals are a cursor.** The outer loop is over the sequence that **must produce output** (every query needs an answer). Intervals are consumed as needed, by an inner `while (i < n && start <= q)`. When the intervals run out, that `while` just stops pushing, and the heap keeps answering. It's the same guarded forward-only index as Insert Interval's phases.
    * **Size is inclusive:** `end − start + 1`.
    * **The result is sized by the *queries*, filled with −1.** `reserve` won't do: it gives capacity with `size() == 0`, and the answers are written in **scattered order** by original index, so the elements must exist up front. Each query is visited once, so its slot is written at most once. An unwritten slot keeps −1, which means "not found", so there's no `else` branch.

* **The Struggle & Insights:**
    * **The brute force: correct, but 10¹² operations.** My first idea was to map every point covered by an interval to the smallest size covering it, making each query an O(1) lookup. The cost is the **total length** of the intervals: 10⁵ intervals with endpoints up to 10⁷ is up to 10¹² insertions, and the map would take hundreds of MB. (If the range *were* small, the keys are dense integers, so an array would beat `unordered_map`.)
    * **Why the difference array from `[4]` can't save it:** +1/−1 works for **counts** because a sum can be undone when an interval ends. **A minimum can't be undone.** When the smallest covering interval ends, nothing tells you the next smallest. You have to keep all the candidates, and that's what the heap does.
    * **"Go back to the first interval containing the next query" was the slow part.** Rescanning earlier intervals is O(n·q) in the worst case: if every interval is `[1, 10⁷]`, that's 10¹⁰ steps. Also, in start order, intervals that have ended are mixed in with ones that haven't, so there's no single position to scan up to. The fix was to **never go back**: every interval already passed sits in the heap.
    * **Seeing the heap work (my example, worth re-running on a reread).** Intervals `(1,3) (1,4) (1,10) (1,100) (2,3) (2,4) (3,5)`, queries `2, 4, 7, 50, 101` → answers `2, 3, 10, 100, −1`. My three worries, and their answers:
        * **"Is `(1,100)` guaranteed to come after `(1,10)`?"** It doesn't matter. Equal starts all enter before the query is answered, and the heap orders them by size.
        * **"I can't lose `(1,100)`."** Nothing is removed unless it's at the top *and* expired. `(1,100)` sits at the bottom, becomes the answer at query 50, and is popped only at query 101.
        * **"Won't I lose the start?"** It's never needed again (see the Core Pattern).

      **Lazy deletion, made concrete:** stale entries *below* the top are allowed. They're checked only once they surface, so you never search the heap.
    * **The coding walls:**
        1. **The push condition was inverted.** I found and fixed it.
        2. **The loop was driven by intervals, so queries after the last interval start were never answered.** LC1 gave `3 3 −1 −1`.
        3. **One query was answered per interval, then the current interval was pushed without checking the *next* query.** On `[1,10] [5,5] [6,20]` with queries `2, 3`, `[5,5]` went into the heap before query 3 was answered and answered it (1 instead of 10). The heap only checks ends, so **the "start isn't needed" invariant holds only if nothing is pushed before its start**.
        4. **More intervals than queries read past the end of `queriesCopy`** (ASan: heap-buffer-overflow). Restructuring around the queries fixed 2–4 at once.
        5. **`resultVector` was sized by intervals, not queries.** That was the last 12 failing tests: extra trailing −1s when n > q, and an out-of-bounds write when n < q.
    * **The review (10⁵ intervals and queries, values up to 10⁷):** no change made a measurable difference.
        * **Reserving the heap** removed 14 reallocations, but they're cheap next to the sorts.
        * **Packing each query into one `uint64_t`** (value in the top 32 bits, index in the bottom), so the sort is a plain integer sort with no comparator, also measured as noise. It's a neat trick worth knowing.
        * **`pair<int, size_t>` is 16 bytes, with 4 of them padding** (verified). A `uint32_t` index would make it 8.
        * **What was left was hygiene:** `const auto&` bindings (no runtime difference, but a stray write becomes a compile error, and the reader can see the loop only reads), a local reference for the current interval, and fixing a comment that described the opposite of the code.
    * **An alternative to know, not write:** sort the intervals by **size** instead. Each one answers every still-unanswered sorted query inside `[l, r]`: binary-search the first, then skip already-answered queries with a "next unanswered" pointer (union-find, covered in Graphs). Each query is answered once: O(n log n + q log q + n log q).

* **Time & Space Complexity:**
    * **Time O(n log n + q log q).** The two sorts, plus heap work that is O(n log n) **in total**: each interval is pushed exactly once and popped at most once, however the queries fall. This amortised argument is the interview point, because the nested `while` loops look like O(n·q) and aren't.
    * **Space O(n + q):** the copies, the heap (up to n entries, stale ones included) and the result.
