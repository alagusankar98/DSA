# Math & Geometry - Topic Guide

## 1. Core Fundamentals

Every topic so far had a data structure at its centre: a list, a tree, a heap, a graph, a DP table. **This one doesn't.** The input is a grid, a number, a string of digits, or a set of points, and the work is **arithmetic on indices and digits**: where does cell `(r, c)` go after a rotation, how does a carry move through a number, how do you raise something to a huge power without doing a huge number of multiplications.

There's no clever search to find. **The difficulty is precision:** off-by-one on a boundary, a sign that C++ handles differently from maths, an `int` that overflows silently, a `double` that can't hold the exact answer. The code is short and the bugs are small. You've met most of the parts already:

* **Digit extraction and overflow:** Reverse Integer `[6]` in `02-Bit-Manipulation/readme.md`: `% 10` takes off the last digit, `/ 10` drops it, `res * 10 + d` rebuilds the number, the overflow bound is checked *before* multiplying, and `abs(INT_MIN)` is undefined behaviour.
* **2-D ↔ 1-D index maths:** Search a 2D Matrix `[2]` in `07-Binary-Search/README.md` (`row = i / cols`, `col = i % cols`, and why the row count doesn't appear), and the box index `(i / 3) * 3 + j / 3` from Valid Sudoku `[8]` in `03-Arrays-Hashing/readme.md`.
* **Cycle detection without a set:** Floyd's tortoise and hare from Linked List Cycle `[3]` and Find the Duplicate Number `[9]` in `01-Linked-List/readme.md`. It works on **any** function that maps a value to a next value, not just on `next` pointers.
* **Grids as data:** the four-direction walks from Graphs (Number of Islands `[1]`, Surrounded Regions `[7]`) and 2-D DP. Here the grid is something you transform in place, not something you search.

### The Vocabulary
* **In place:** the answer overwrites the input, with O(1) extra memory. For matrices this is the whole difficulty: once a cell is overwritten, its old value is gone unless you saved it first.
* **Transpose:** swap across the main diagonal, `(r, c) ↔ (c, r)`. **Reflect (mirror):** reverse each row (left ↔ right) or the row order (top ↔ bottom). Every 90° rotation is a transpose plus one reflection.
* **Layer (ring):** the outer border of a square matrix, then the border inside it, and so on. An `n × n` matrix has `n / 2` full layers (plus a centre cell when `n` is odd).
* **Boundary pointers:** `top`, `bottom`, `left`, `right`, the current unvisited rectangle. Walking a border moves one of them inwards.
* **In-band marker:** storing a flag *inside* the data you're processing (the first row and column of a matrix, a sign bit, an otherwise impossible value) instead of in a separate array, to get O(1) extra space.
* **Carry:** what spills into the next digit when a digit sum exceeds 9. **Ripple:** a carry that keeps going (`999 + 1`), which is the worst case.
* **Exponentiation by squaring (fast power):** compute `x^n` from the binary digits of `n`. `x^13 = x^8 · x^4 · x^1` because 13 = 1101₂. That's about `log₂ n` squarings instead of `n − 1` multiplications.
* **Truncation toward zero:** C++ integer division rounds toward 0, so `-7 / 3 == -2` and `-7 % 3 == -1`. The remainder takes the **dividend's** sign. Maths "mod" is never negative; C++ `%` can be.
* **Two's complement asymmetry:** `INT_MIN = −2³¹` has no positive counterpart in `int`. `-INT_MIN`, `abs(INT_MIN)` and `INT_MIN / -1` are all undefined behaviour.
* **Floating-point precision:** a `double` has a 53-bit mantissa, so integers above 2⁵³ aren't all representable (2⁵³ + 1 rounds to 2⁵³). Multiply many doubles and the rounding errors add up. Compare with a tolerance, never with `==`.
* **Axis-aligned:** sides parallel to the x and y axes. An axis-aligned square is fully determined by **one diagonal**: two opposite corners give you the other two.

### The Four Questions, Math Edition (ask them before writing any code)
1. **What's the index mapping?** Write where `(r, c)` goes, or which digit position feeds which, as a formula. Check it on a 3×3 (or a 2-digit) case **by hand** before coding.
2. **What gets overwritten before it's read?** In place means order matters. Save the value you're about to destroy, or choose an order that reads everything first.
3. **What are the extreme values?** Zero, one, `INT_MIN`, a negative exponent, all 9s, a 1×1 or 1×n matrix, an empty string. Most of the bugs here live at these points.
4. **Does it fit the type?** Work out the largest intermediate value (a product of two `int`s, a running sum, `n` squarings) and pick `int64_t`, a digit array or a `double` deliberately.

### Brute Force vs the Math Trick
* **Simulate first, then look for the structure.** Walking the spiral one cell at a time, multiplying `x` by itself `n` times, or doing long multiplication one digit at a time are all correct brute forces. The improvement is usually a structural observation (squaring, layers, digit positions), not a different algorithm.
* **The extra-space version is the reference.** Copy the matrix and rotate into the copy; store which rows and columns to clear in two arrays; keep visited values in a `set`. Get that right first, then remove the extra space. **Fuzz the in-place version against it.** In-place bugs come out as wrong cells, not crashes.
* **Don't reach for `std::pow` or a `double` on integer problems.** `std::pow` works in floating point. Some results come out right, but you'd be relying on rounding. For exact integer answers, use integer maths.

### The Problem Families by Shape
* **In-place matrix transform:** turn a square grid in place. Either as two simple passes (transpose, then reflect) or one pass over each layer that moves four cells at once through one temporary.
* **Boundary walk:** visit a grid's cells in spiral order. Keep the four boundary pointers and walk one side at a time, moving that boundary in after each side; stop when the rectangle is empty. Non-square inputs and single rows or columns are where it breaks.
* **Mark using the grid itself:** a cell's value tells you to change its whole row and column. Changing them straight away corrupts cells you haven't read yet, so record the decisions first. O(1) space comes from using the first row and column as markers, with one extra flag for their own overlap.
* **A number-to-number function with a cycle:** apply a digit-based function (the sum of squared digits) repeatedly; it either reaches a fixed point or loops forever. Detect the loop with a `set`, or with Floyd's two speeds and O(1) space.
* **Carry through a digit array:** add to a number stored as its digits. Walk from the least significant digit, stop as soon as the carry dies, and handle the case where it ripples off the front.
* **Fast power:** `x^n` with `n` up to ±2³¹. Square the base and halve the exponent; multiply into the result when the low bit is set. Negative `n` means `1 / x^|n|`, and `|INT_MIN|` doesn't fit in an `int`.
* **Grade-school arithmetic on strings:** multiply two numbers too large for any built-in type. The digit at position `i` of one and `j` of the other lands at positions `i + j` and `i + j + 1` of the result. Accumulate into an `int` array, carry at the end, strip leading zeros.
* **Geometry by counting points:** count shapes formed by points added over time. Store points in a count map; for each query, fix one corner, loop over candidates for the opposite corner, and look up the other two. Multiply the counts, because duplicate points make several distinct shapes.

### Complexity Cheat Sheet
| Shape | Time | Space |
| -- | -- | -- |
| In-place matrix transform | O(n²) | O(1) |
| Boundary walk | O(m·n) | O(1) besides the output |
| Mark rows/cols in the grid itself | O(m·n) | O(1) (O(m + n) with flag arrays) |
| Digit function + cycle detection | O(log n) per step, few steps | O(1) Floyd / O(steps) set |
| Carry through digits | O(n) worst case, O(1) typical | O(1), or O(n) if it ripples off the front |
| Fast power | O(log n) multiplications | O(1) iterative / O(log n) recursive |
| String multiplication | O(m·n) | O(m + n) |
| Shape counting from a point map | O(candidates) per query | O(points) |

### The Hardware Reality
* **Division is the slow arithmetic operation, unless the divisor is a constant.** Verified on this machine at `-O2`: `x / 10` compiles to a multiply by a magic constant plus shifts (no divide instruction), while `x / y` with a runtime `y` emits `idiv`, which takes tens of cycles where a multiply takes a few. Digit loops by 10 are cheap. Dividing by a runtime value inside a hot loop isn't.
* **Matrix walks: rows are contiguous, columns are strided.** Reading `m[r][c]` along a row streams one cache line after another. Reading down a column jumps a whole row each step, and with `vector<vector<int>>` every row is a separate heap allocation, so each step is a pointer chase as well. Transpose and layer rotation both read columns. For small `n` it doesn't matter; for big matrices, blocked (tiled) transposes exist for this reason.
* **One temporary vs a copy:** the four-way layer swap touches each cell once with one register. Transpose + reflect touches each cell twice but in simpler, more predictable loops. Measure before believing either is faster.
* **Fast power is the same loop as `[4] Reverse Bits` in Bit Manipulation:** test the low bit, shift right. A branch on `n & 1` is data-dependent but there are only ~31 iterations, so it doesn't matter.
* **Digit arrays: use `int`, not `char`.** Accumulating products into an `int` array and carrying once at the end avoids a carry pass after every single product. Converting `'7' → 7` is one subtraction (`c - '0'`).
* **Points as hash keys:** a `pair<int,int>` needs a custom hash. Packing `(x, y)` into one 64-bit integer (`uint64_t(uint32_t(x)) << 32 | uint32_t(y)`, unsigned so the shift is defined for negative coordinates) gives a plain integer key. With small coordinates, a fixed 2-D count array beats any hash map (the Hand of Straights lesson).
* **Embedded fit:** fixed-point maths, fast power for modular exponentiation (crypto on a microcontroller), and rotating a frame buffer for a display are the real-world versions of this topic. No allocation, a known worst case, integer-only.

### Where This Shows Up in the Real World
**Graphics and displays:** rotating images and frame buffers (screen orientation, camera sensors read out sideways), and spiral or zig-zag scan orders (JPEG walks the 8×8 DCT block in zig-zag order). **Cryptography:** RSA and Diffie-Hellman are fast modular exponentiation on numbers thousands of bits long. **Big-number libraries:** GMP and Python's `int` do grade-school multiplication for small sizes and Karatsuba/FFT beyond. **Checksums and hashing:** digit and modular arithmetic (Luhn for card numbers, CRCs). **Pseudo-random number generators:** each is a number-to-number function, and its cycle length is its period (Floyd's and Brent's algorithms measure it). **Computational geometry:** collision detection, GIS and CAD all start from point lookups and axis-aligned bounding boxes.

---

## 2. General Summary / Quick Reference

### Pattern 1: Rotate = Transpose + Reflect → or four-way swaps per layer
A 90° clockwise rotation sends `(r, c)` to `(c, n − 1 − r)`. Two passes do it: transpose (swap across the diagonal), then reverse every row. Counter-clockwise is transpose, then reverse the row order. In one pass, rotate each layer by moving four cells at a time through one temporary.
* *Use for:* Rotate Image.
* *Discipline:* write the mapping and check it on a 3×3 by hand. Transpose only the upper triangle (`c > r`), or every pair is swapped twice and nothing changes. For layers, get the loop bounds for `n` odd and even.

### Pattern 2: Boundary Pointers → walk a side, then move it in
Keep `top`, `bottom`, `left`, `right`. Walk the top row and move `top` down; the right column and move `right` left; the bottom row (reversed) and move `bottom` up; the left column (upwards) and move `left` right. Repeat while the rectangle isn't empty.
* *Use for:* Spiral Matrix.
* *Discipline:* re-check `top <= bottom` and `left <= right` **between** sides, not only at the top of the loop, or a single remaining row or column gets visited twice. Test 1×n, n×1, and non-square inputs.

### Pattern 3: Record First, Change Later → markers inside the grid
When a cell's value decides a change to its whole row and column, decide everything before changing anything. O(m + n) space: two flag arrays. O(1): use row 0 and column 0 as the flags, plus one variable for whether row 0 (or column 0) itself needs clearing, because cell `(0, 0)` can only hold one of the two flags.
* *Use for:* Set Matrix Zeroes.
* *Discipline:* apply the changes to the inner cells first and the marker row/column **last**, or you erase your own markers.

### Pattern 4: Repeated Function → detect the cycle
When a value is replaced by `f(value)` over and over, it ends at a fixed point or loops. A `set` of seen values detects the loop simply. Floyd's slow (`f(x)`) and fast (`f(f(x))`) pointers detect it in O(1) space.
* *Discipline:* decide what "done" means (reaching the target vs reaching any repeat). A fixed point is a cycle of length 1, so with Floyd both pointers meet there too: the test after they meet is *where* they met, not *whether*.

### Pattern 5: Carry From the Least Significant End → stop when it dies
Walk from the last digit. Add, keep the digit, pass the carry on. As soon as the carry is 0, the rest of the number is unchanged, so return early.
* *Discipline:* the only case that grows the number is a carry that ripples past the first digit (all 9s). Handle it once, at the end.

### Pattern 6: Fast Power → square the base, halve the exponent
While `n > 0`: if `n` is odd, multiply the result by the base; square the base; halve `n`. That's O(log n) multiplications. For negative `n`, compute with `|n|` and take the reciprocal.
* *Discipline:* widen `n` to `int64_t` **before** negating, because `-INT_MIN` overflows. Check `n = 0`, `x = 0`, `x = 1`, `x = −1`, and negative `x` with odd and even `n`.

### Pattern 7: Digit Positions → `i + j` and `i + j + 1`
For long multiplication, digit `i` of the first number times digit `j` of the second contributes to result positions `i + j` (the carry) and `i + j + 1` (the digit). Use an `int` array of size `m + n`, accumulate all products, then carry once from right to left.
* *Discipline:* strip leading zeros at the end, and short-circuit when either input is `"0"`.

### Pattern 8: Count Map of Points → fix one corner, look up the rest
Store every added point with its count. For a query point, loop over stored points that could be the **opposite** corner (axis-aligned: different x **and** `|dx| == |dy|`), then look up the two remaining corners and multiply all three counts.
* *Discipline:* reject the zero-area case (`dx == 0`). Multiply counts instead of adding 1, since duplicate points make distinct shapes.

### The Non-Negotiable Discipline
* **Write the index or digit mapping as a formula, and check it by hand on the smallest non-trivial case** (3×3, two digits, `n = 5`).
* **Work out the largest intermediate value before choosing a type.** `int` × `int` needs `int64_t`; `|INT_MIN|` needs `int64_t`; integers above 2⁵³ don't fit exactly in a `double`.
* **For in-place work, name what gets overwritten and when.** Then fuzz against the extra-space reference version.
* **Test the edges deliberately:** 1×1, 1×n, n×1, empty, zero, one, negative, `INT_MIN`, all 9s.
* **No `std::pow` and no `==` on doubles** when the answer has to be an exact integer.

---

## 3. Problem Strategies & Patterns

### [1] Rotate Image
* **The Core Pattern:** two reflections make a rotation (§2 Pattern 1). Flip the row order (top ↔ bottom), then transpose. `(r, c)` → flip → `(n − 1 − r, c)` → transpose → `(c, n − 1 − r)`, which is exactly the 90° clockwise mapping. The order matters: transpose first, then the flip has to be **within** each row (reverse every row) instead. Both give the same result; pick one and check it on a 3×3.
* **The "Gotcha":** **a transpose is its own inverse.** Run it over the whole grid and every off-diagonal pair is swapped twice, which undoes it. Trace the pair `(0,1)` / `(1,0)` on the flipped 3×3, where they hold `8` and `4`:

  | cell visited | after the swap: `m[0][1]` | `m[1][0]` |
  |---|---|---|
  | start | 8 | 4 |
  | `(0, 1)`: upper triangle | 4 | 8 ✓ |
  | `(1, 0)`: lower triangle | 8 | 4 ✗ undone |

  The output was just the row-flipped matrix (`7 8 9 / 4 5 6 / 1 2 3`), as if the transpose never ran. The rule: **each pair is swapped exactly once**, so walk only the strict upper triangle (`j` from `i + 1`). The row flip already obeyed it, which is why it stops at `n / 2`. The transpose needs the same "half the space" bound, as a triangle instead of the top half.
* **The Struggle & Insights:**
    * **This was a repeat.** The same double-swap had bitten before, and §2's Discipline already warned about it, written before this solve. Knowing the rule wasn't enough; the check that catches it is mechanical: for any in-place pairwise swap, **count how many times each pair gets visited. It must be once.**
    * **First fix overshot by the diagonal.** Changing the loop to start at `j = i` made it correct, but every `(i, i)` was then swapped with itself: n useless swaps. The diagonal is the mirror line, so it doesn't belong to either triangle. `j = i + 1` states the intent.
    * **Swap the rows, not their elements.** `vector<vector<int>>` is an array of `vector` headers (pointer, size, capacity), each pointing at its own heap block. The first version swapped two rows element by element, which copies n ints each way. `std::swap(matrix[i], matrix[n − 1 − i])` swaps two 24-byte headers instead, so the flip drops from O(n²) data movement to O(n). Measured at `-O2`, together with the `j = i + 1` change: 0.11 → 0.06 µs at n = 20, 1431 → 1118 µs at n = 2000. A constant factor (the transpose is still O(n²)), but it shows you know what the outer vector actually holds.
    * **Cache, said in one sentence:** `matrix[j][i]` walks down a column, so each access lands in a different heap block. At n ≤ 20 the whole matrix (1.6 KB) sits in L1 and it doesn't matter. At large n, this access pattern is why real transposes are done in tiles.
    * **The follow-up to know:** "Can you do it without the transpose?" Rotate layer by layer. For each ring and each offset along it, cycle four cells (top → right → bottom → left) through one temporary. Every cell is written once, in a single pass: same O(n²) / O(1), so not an algorithmic win, but the index arithmetic is where people slip. Dry-run it on a 4×4 before an interview.
    * **Still open (left as is):** the code keeps separate `rows` and `columns`. `matrix[j][i]` is only in bounds because the matrix is square, so a single `n` would make that invariant visible.
* **Time & Space Complexity:**
    * **Time O(n²):** the flip is n/2 header swaps; the transpose is n(n − 1)/2 element swaps. Every cell must move, so this is optimal.
    * **Space O(1):** in place.

### [2] Spiral Matrix
* **The Core Pattern:** two ways to walk the spiral. **Simulation:** carry a position and a direction from a 4-entry `{dr, dc}` table, and turn clockwise when blocked by a wall or by a visited cell. **Boundary pointers (§2 Pattern 2):** the unvisited cells always form a rectangle, so four integers (`top`, `bottom`, `left`, `right`) replace any visited marks. Walk its four sides, shrinking it after each one. Both are O(m·n) time; only the second is O(1) extra space.
* **The "Gotcha":** **a side's `for` loop guards only the index it walks along, not the row or column it sits on.** On `[[1, 2, 3]]`, the top row outputs `1 2 3` and `top++` makes the rectangle empty. The bottom row's column range is still valid, so it runs on row `bottom = 0` (the row just consumed) and outputs `2 1` again: `1 2 3 2 1`.

  | side | walks along (guarded by its own loop) | sits on | needs | broken by |
  |---|---|---|---|---|
  | top row | columns `left..right` | row `top` | `top <= bottom` | nothing: true at the start of every round |
  | right column | rows `top..bottom` | column `right` | `left <= right` | nothing: only `top` has moved |
  | **bottom row** | columns `right..left` | row `bottom` | **`top <= bottom`** | **the `top++` after the top row** |
  | **left column** | rows `bottom..top` | column `left` | **`left <= right`** | **the `right--` after the right column** |

  Without those two checks, **66 of the 144 shapes from 1×1 to 12×12 output cells twice**: every shape whose last layer is a single row or column of length ≥ 2 (`1×2`, `3×1`, `3×5`, …). A single leftover *cell* survives by luck, since every range is already empty. The `while` check can't help, because it only runs at the top of a round. **The rule:** before a side, check the dimension it sits on, if an earlier side in the same round moved that boundary. Test 1×n and n×1 first.
* **The Struggle & Insights:**
    * **First pass: simulation with an in-matrix sentinel.** Take `(0,0)` before the loop, then each iteration: step, check, and either take the cell or **undo** the step and turn. Correct, but it felt like three rules with no reason behind them: why take a cell before the loop, why undo, when to check. **All three come from one unstated decision: what the position means at the top of the loop.** Here it meant "the cell I just took". Nothing ever steps onto `(0,0)`, so it has to be taken by hand. Moving before checking can leave you on an invalid cell, so the undo restores the meaning. And turn iterations output nothing, so the loop has to run on `size < n` instead of exactly n times. The sentinel (`INT_MAX` written into taken cells) also had two hidden costs: it only worked because the values are bounded to [−100, 100], and it destroyed the caller's matrix.
    * **Look-ahead variant: "the position is the next cell to take".** No setup before the loop, exactly n iterations: take, look ahead, maybe turn, move. The worry was "turn until valid loops forever on the last cell", and it **does**. On a 3×3, every neighbour of the final `5` is taken, and four turns lead back to the start. Two facts remove the loop entirely:
        * At the n-th cell there's nowhere left to go, so the count ends the loop before any look-ahead is needed.
        * While cells remain, **one turn is always enough**: you're blocked exactly at a corner of the unvisited rectangle, and the clockwise turn points down its next edge. Checked on all 900 shapes up to 30×30, and two turns were never needed.

      So it's `if`, not `while`.
    * **The fold.** The blocked and clear branches both *moved*; they differed only in which direction. So the `if` body shrank to "turn", and the move is written once after it. **The validity check asks about a direction, not a position.** This also removed a `-Wshadow` warning from a second structured binding.
    * **The review's gap: `seen` was O(m·n) extra space the geometry doesn't need**, plus a five-part branch and a load from a separate allocation on every cell. The boundary version needs checks only between sides. The first try at it left out the checks between sides (the Gotcha above). The fix put a guard into the condition of all four loops. Only two of the guards can ever fire; the other two just keep the pattern symmetric, and "which ones matter?" is the follow-up question to expect. The boundaries keep moving after a skipped side, which is harmless only because a skipped side means everything has been output and `size < n` exits.
    * **Measured at `-O2`** (10×10 / 1000×1000): sentinel simulation 0.11 µs / 2038 µs, `seen` simulation 0.13 µs / 2570 µs, boundary pointers 0.05 µs / 1472 µs. Guards in the loop condition vs `if` around the loop: identical, because the compiler lifts the guard out of the loop.
* **Time & Space Complexity:**
    * **Time O(m·n):** every cell is output once. The boundary version spends O(1) per side on checks, not per cell.
    * **Space O(1) extra** for the boundary version, not counting the O(m·n) output. The simulations were O(m·n) extra (`seen`), or O(1) by writing over the input.

### [3] Set Matrix Zeroes
* **The Core Pattern:** record first, change later (§2 Pattern 3). A zero you write must never be mistaken for an original zero, or it cascades and wipes out the whole matrix. What needs recording isn't *which cells* are zero but *which rows and columns die*: m + n facts. Store them in two flag arrays (O(m + n) space), or inside the matrix itself, using row 0 for column flags and column 0 for row flags (O(1) space).
* **The "Gotcha":** **in the O(1) version, row 0 and column 0 have two jobs: they store the markers, and they're also real cells that may need zeroing.** The first attempt marked and applied only the inner cells (`i, j ≥ 1`), so nothing ever zeroed row 0 or column 0 themselves. Smallest failure: `[[2, 0]]` should become `[[0, 0]]` but came back unchanged. With one row, the loops from 1 never run, so the 0 that says "row 0 dies" is never acted on. That broke 120,627 of 200,000 random cases. The fix is about **timing**:
    1. **Before marking:** record whether row 0 and column 0 originally contain a zero, in two bools. After marking they hold markers, and the original information is gone.
    2. **Mark and apply to the inner cells.**
    3. **Last:** zero row 0 and/or column 0 from the two bools. They had to stay intact until step 2 finished reading them.
* **The Struggle & Insights:**
    * **Brute force: a list of `(r, c)` for every zero, then zero each one's row and column.** Correct from the start, and the record-first rule was there without being told. But a row with k zeros was zeroed k times. All-zeros 200×200 meant 40,000 zeros × 400 writes = **16 million writes to set 40,000 cells**. `reserve(rows * cols)` also allocated 320 KB up front even with no zeros.
    * **Second try: split the pairs into a row list and a column list.** That still pushed one entry *per zero*, so `[[0, 0, 0], [1, 1, 1]]` gave the row list `[0, 0, 0]` and row 0 was zeroed three times. Measured: 1838 µs all-zeros, no better than the brute force. **An index you set can't repeat; a list you append to can.** Flags indexed by row and column number make duplicates disappear by themselves.
    * **Flag arrays (`2782854`):** the expected interview answer. Every cell is checked once and written at most once: all-zeros **1.8 ms → about 25 µs**. These were `std::vector<bool>` flags: bit-packed, written through a proxy with a read-modify-write on a word, a C++ special case worth knowing. Swapping to `vector<char>` was inside the noise at m + n flags.
    * **O(1) space (`1344588`):** coded after the walkthrough above, on a tired brain. I used **two bools instead of the usual trick** of one variable plus cell `(0,0)`. Cell `(0,0)` sits in both row 0 and column 0, so it can hold only one flag, and the one-variable version is the confusing part of the textbook answer. Two bools are the same O(1) and remove the trap entirely.
    * **When "better" is slower:** with no zeros, the brute force wins (about 14 vs 21–25 µs) because it makes one pass and the others make two. The worst case decides the interview.
* **Time & Space Complexity:**
    * **Time O(m·n):** a scan of row 0 and column 0, one marking pass, one apply pass, two final sweeps. The brute force was O(m·n + z·(m + n)), so O(m·n·(m + n)) worst case.
    * **Space O(1) extra:** two bools. Flag arrays: O(m + n). Brute force: O(z), allocated as O(m·n).
