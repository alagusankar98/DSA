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
* *Use for:* Happy Number.
* *Discipline:* decide what "done" means (reaching the target vs reaching any repeat). A fixed point is a cycle of length 1, so with Floyd both pointers meet there too: the test after they meet is *where* they met, not *whether*.

### Pattern 5: Carry From the Least Significant End → stop when it dies
Walk from the last digit. Add, keep the digit, pass the carry on. As soon as the carry is 0, the rest of the number is unchanged, so return early.
* *Use for:* Plus One.
* *Discipline:* the only case that grows the number is a carry that ripples past the first digit (all 9s). Handle it once, at the end.

### Pattern 6: Fast Power → square the base, halve the exponent
While `n > 0`: if `n` is odd, multiply the result by the base; square the base; halve `n`. That's O(log n) multiplications. For negative `n`, compute with `|n|` and take the reciprocal.
* *Use for:* Pow(x, n).
* *Discipline:* widen `n` to `int64_t` **before** negating, because `-INT_MIN` overflows. Check `n = 0`, `x = 0`, `x = 1`, `x = −1`, and negative `x` with odd and even `n`.

### Pattern 7: Digit Positions → `i + j` and `i + j + 1`
For long multiplication, digit `i` of the first number times digit `j` of the second contributes to result positions `i + j` (the carry) and `i + j + 1` (the digit). Use an `int` array of size `m + n`, accumulate all products, then carry once from right to left.
* *Use for:* Multiply Strings.
* *Discipline:* strip leading zeros at the end, and short-circuit when either input is `"0"`.

### Pattern 8: Count Map of Points → fix one corner, look up the rest
Store every added point with its count. For a query point, loop over stored points that could be the **opposite** corner (axis-aligned: different x **and** `|dx| == |dy|`), then look up the two remaining corners and multiply all three counts.
* *Use for:* Detect Squares.
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

### [4] Happy Number
* **The Core Pattern:** repeated function → cycle detection (§2 Pattern 4). `n → sumSquares(n)` is a function, so the sequence either reaches the fixed point 1 or falls into a loop. Floyd: slow takes one step, fast takes two. They always meet, because a fixed point is a cycle of length 1. The answer is *where* they meet: at 1, or in the unhappy cycle `4 → 16 → 37 → 58 → 89 → 145 → 42 → 20 → 4`. Linked List Cycle used the same two pointers (`01-Linked-List/readme.md`, entry [3]); here the "next pointer" is a computed function instead of a stored one.
* **The "Gotcha":** **"7 is happy too. Can the pointers meet at 7 and miss it?"** No. **A meeting point is always on the cycle.** Each value has exactly one next value, so if a value appears twice in the sequence, everything after it repeats too, and it's on the cycle. When slow and fast meet, they hold the same value at different positions in the sequence, so it's a value that appeared twice. 7 sits on the **tail** (`7 → 49 → 97 → 130 → 10 → 1`), and a tail value appears only once. Traced for n = 7:

  | iter | slow | fast |
  |---|---|---|
  | start | 7 | 49 |
  | 1 | 49 | 130 |
  | 2 | 97 | 1 |
  | 3 | 130 | 1 |
  | 4 | 10 | 1 |
  | 5 | 1 | 1 → meet |

  Logged over every n from 1 to 10⁶: the pointers met only at 1 or at one of the eight values of the 4-cycle, never anywhere else.
* **The Struggle & Insights:**
    * **First idea: reduce to a single digit, then look it up.** I worked out 1–9 by hand: only **1 and 7** reach 1, the rest cycle. So: apply `sumSquares` while `n > 9`, then happy ⇔ the digit is 1 or 7. **It's correct** (verified on 1..10⁶ plus 200k random values up to `INT_MAX`). But it needs a second fact the hand check didn't cover: *does every number eventually reach a single digit?* If some number looped among 2- or 3-digit values forever, `while (n > 9)` would never end. The proof:
        1. **Big numbers shrink:** a 10-digit `int` has a sum of squares of at most 10 × 81 = 810, so anything ≥ 1000 drops below 1000, and stays there (3 × 81 = 243).
        2. **Below 1000, check them all:** all 999 reach a single digit within 15 steps, because the only unhappy cycle passes through **4**.
        3. **From a single digit d, the rest is d's own sequence,** so the lookup is exact.

      Sound, but it's a proof by computation about this particular function. In an interview "how do you know it terminates?" needs steps 1–2. Floyd needs no facts about the function at all, which is why it's the answer to give, with the number fact as a bonus observation.
    * **Floyd version (`824e55a`):** fast starts one step ahead (`sumSquares(n)`), so `while (slow != fast)` doesn't exit immediately, and `return slow == 1` reads the meeting point. n = 1 starts with both at 1 and returns `true` without entering the loop.
    * **Measured at `-O2`** (average over n = 1..10⁶): `unordered_set` 181–185 ns, **Floyd 42 ns**, "loop until 1 or 4" 32 ns. The set pays for heap nodes and hashing; Floyd pays three `sumSquares` calls per iteration and still wins by 4×. "Until 1 or 4" is faster only because it relies on the computed fact.
    * **A guess that measured wrong:** exiting as soon as `fast == 1` looked free (for n = 7, fast reaches 1 three iterations before slow). It measured **35% slower** (56 vs 42 ns): an extra comparison in a loop whose iterations are this cheap costs more than the iterations it saves. Don't add it.
* **Time & Space Complexity:**
    * **Time O(log n):** the first `sumSquares` costs O(digits). After that, every value is ≤ 810, so the tail (≤ 15 steps) and the cycle (8) are constant.
    * **Space O(1):** two `int`s. The `set` version is O(tail + cycle) on the heap.

### [5] Plus One
* **The Core Pattern:** carry from the least significant end, and stop when it dies (§2 Pattern 5). Adding 1 only changes the trailing 9s (each becomes 0) and the first non-9 to their left (it goes up by one). Everything further left stays the same. The only case where the number grows is all 9s, and its answer is always `1` followed by n zeros.
* **The "Gotcha":** **the question I asked was the wrong one.** I spent the effort on a trade-off: push the digits into a new vector and `std::reverse` at the end, or copy and add in place, then `insert` the carry at the front. On paper the in-place version wins: `reverse` always costs O(n), while `insert` costs O(n) only for all 9s (and also reallocates, because the copy's capacity is exactly n). **Measured, they tied** (139 vs 141 ns at n = 100, 13.3 vs 13.7 µs at n = 10⁴), because both spent almost all their time in the same loop: a `%` and a `/` on **every** digit, long after the carry had died. The real choice was neither: exit when the carry dies.
* **The Struggle & Insights:**
    * **First pass (`14a4444`):** copy, then a full right-to-left loop with `sum`, `% 10`, `/ 10` and a `uint8_t carry`, then `insert(begin, 1)` if the carry survived. Correct. On `[1, 2, 3]` it still visited 2 and 1 just to write back the same values.
    * **Early exit:** `return` as soon as the carry hits 0. Typical case: 141 → **9 ns** at n = 100, 13.7 → **0.3 µs** at n = 10⁴, which is just the copy. Once the loop returns, **the `if (carry)` after it is always true**, the same dead-check lesson as Merge Triplets: the check inside the loop already decides what's true after it.
    * **Optimal (`6408899`): the 9 test replaces the arithmetic.** The carry is only ever 0 or 1, so `sum` is at most 10, and the only real question per digit is *is it 9?* Below 9: increment and return. A 9: becomes 0 and the loop continues. **The loop itself is the carry**, with no `sum`, `%`, `/` or carry variable. That also removed a `-Wconversion` warning: `uint8_t` saved nothing, since it's promoted to `int` in `carry + digits[i]` anyway. All 9s: 148 → 40 ns at n = 100.
    * **Left as is:** `insert` at the front for all 9s still reallocates and shifts n zeros. Building `1` + n zeros directly measured 13–20% faster, and only on that one input. Not interview-relevant, but worth saying: inserting at the front of a full vector means a reallocation plus a shift.
* **Time & Space Complexity:**
    * **Time O(n)** for the copy (the parameter is `const&` and a new vector is returned), plus **O(k + 1)** for k trailing 9s. All 9s: O(n).
    * **Space O(n):** the result.

### [6] Pow(x, n)
* **The Core Pattern:** binary exponentiation (§2 Pattern 6). Write the exponent in binary: 13 = `1101` = 8 + 4 + 1, so x¹³ = x⁸ · x⁴ · x¹. Squaring a running `base` produces x, x², x⁴, x⁸, …, and the result multiplies in only the ones whose bit is set. One iteration **per bit**, not per unit: about 31 instead of 2³¹. Negative `n`: compute with |n|, then take the reciprocal.

  | `n` | lowest bit | `base` | action | `result` |
  |---|---|---|---|---|
  | 13 | 1 | x | result *= base | x |
  | 6 | 0 | x² | skip | x |
  | 3 | 1 | x⁴ | result *= base | x⁵ |
  | 1 | 1 | x⁸ | result *= base | x¹³ |

* **The "Gotcha":** **`|INT_MIN|` doesn't exist as an `int`.** `int` covers −2³¹ to 2³¹ − 1, one more negative value than positive ones. The first try was `static_cast<unsigned>(-1 * n)`: the cast is on the right on the page, but **too late in time**. `-1 * n` is evaluated in `int` first, overflows (UB), and the cast converts the already-broken value. UBSan: `negation of -2147483648 cannot be represented in type 'int'`. **Change the type first, then negate:** `-static_cast<long long>(n)`, or `0u - static_cast<unsigned>(n)` (unsigned arithmetic wraps by definition). The loop counter `int i < cycles` was a second UB on the same input: `i` reached `INT_MAX` and `++` overflowed, which `-Wsign-compare` had flagged.
* **The Struggle & Insights:**
    * **Dodging the negation with structure:** I asked whether to branch on the sign instead: multiply counting down for positive `n`, divide counting up for negative `n`, so nothing is ever negated. It's safe (`INT_MIN + 1` exists), but it means two loops, it's still O(n), and it avoids the `INT_MIN` question instead of answering it. What an interviewer wants to hear is one sign check before the loop: widen, flip, invert, plus the sentence "`-INT_MIN` overflows `int`, so I widen first." A single `if` outside the loop costs nothing; avoiding it isn't a goal. **A variant that's valid if you can defend it:** invert `x` and run fast power directly on the negative `n`. It works because C++ division truncates toward zero, so halving shrinks a negative `n` to 0 the same way. The trap: `-3 % 2 == -1`, so the odd test must be `!= 0` or `& 1`, never `== 1`. Tested on −60..60 and `INT_MIN`.
    * **Brute force with the cast fixed (`84c709c`):** widening to `long long` before `std::abs` was right. Then **TLE**: one multiply per unit of `n`, 4.36 s for a single `INT_MIN` call.
    * **Binary exponentiation (`8c829ee`):** `myPow(2.0, INT_MIN)` takes 0.2–0.3 µs. One stop on the way: the exponent was briefly a `long`. That's 64 bits on Linux, which is why it passed, but **32 bits on Windows/MSVC**, where `std::abs(INT_MIN)` overflows again. Use `long long` or `int64_t` when you need the guarantee.
    * **Floating point compounds:** at n = ±2³¹ with x = 1.0000001, the result is off from `std::pow` by a relative 1.2 × 10⁻⁸, from rounding in ~31 squarings. That's not a bug; LeetCode compares to about 10⁻⁵.
    * **Edges the constraints exclude, but worth naming:** `x = 0, n < 0` is ∞, and 0⁰ should be 1. The code returns 0 for both, because the `x == 0` check runs before the `n == 0` check; swapping them fixes 0⁰. Negative bases need no special case: an odd exponent multiplies the sign in an odd number of times.
    * **Systems details:** `cycles /= 2` on a signed type adds a sign fix-up before the shift, because the compiler can't know the value is non-negative (`>>= 1` or an unsigned type gives one shift). The last iteration squares `base` for nothing. The recursive version (halve, square the result) costs O(log n) stack frames; the iterative one costs none. All irrelevant at 32 iterations.
* **Time & Space Complexity:**
    * **Time O(log |n|):** at most 32 iterations for any `int`.
    * **Space O(1).**

### [7] Multiply Strings
* **The Core Pattern:** long multiplication **without carrying until the end** (§2 Pattern 7). One `int` cell per column, m + n cells (an m-digit times an n-digit number has at most m + n digits). Every digit pair drops its product **whole** into its column, then one right-to-left pass carries. On 23 × 45:

  | product | i (in "23") | j (in "45") | cell `i + j + 1` |
  |---|---|---|---|
  | 3 × 5 = 15 | 1 | 1 | 3 (units) |
  | 3 × 4 = 12 | 1 | 0 | 2 (tens) |
  | 2 × 5 = 10 | 0 | 1 | 2 (tens) |
  | 2 × 4 = 8 | 0 | 0 | 1 (hundreds) |

  The cells are `[0, 8, 22, 15]`. Carrying right to left gives 15 → 5 with 1 carried, 23 → 3 with 2 carried, 10 → 0 with 1 carried, so `[1, 0, 3, 5]` = **1035**. **Why `i + j + 1`:** index 0 is the *leftmost* digit, so units × units (the two last indices) goes to the last cell, and moving one digit left in either number moves the product one column left. The `+ 1` keeps cell 0 free for the final carry.
* **The "Gotcha":** **a zero is "leading" only if everything to its left is zero, so you can't decide it while walking from the right.** The first buffer version carried and converted in one right-to-left loop, and stopped at the first `sum == 0 && carry == 0`. On `2 × 5` the cells are `[1, 0]`: the loop hit the units 0 of **10** first and returned `""`. `101 × 1` lost its middle zero and returned `"1"`. That was 24,861 of the 90,601 pairs in 0..300 × 0..300. The fix separates the jobs: carry right to left **in the vector**, then convert left to right from the first non-zero cell. Because the carry pass walks right to left, the last non-zero cell it sees is the leftmost one, so `firstNonZeroIdx` falls out of the same loop without a second scan. The early `"0"` return guarantees there is one, so the string is never empty.
* **The Struggle & Insights:**
    * **First pass (`da94012`): Horner on strings.** For each digit of the shorter number: result = result × 10 (a `push_back('0')`) + num1 × digit, through `multiplyDigit` and `add` helpers. Correct, and already O(m·n). But: 2–3 heap allocations per digit, two `std::reverse` calls per iteration, and an `add` pass over a result that kept growing. `"0"` worked through a chain of accidents (`""` from `multiplyDigit`, then `"0" + ""`), not through a stated rule. **161 µs at 200 × 200 digits.**
    * **The buffer idea didn't land at first** ("I can't comprehend item 1"). What made it land: long multiplication on paper, **writing each product whole and not carrying yet**, then one carry pass over the column totals. The buffer is just those columns.
    * **"If I split 81 into 8 and 1, can't the cells overflow?"** Cells going above 9 isn't overflow: they're `int`s, not digits, until the carry pass. The tens column of 23 × 45 held 22. A cell collects at most min(m, n) products of ≤ 81: about 16,200 at 200 digits, against `int`'s 2.1 billion. Real `int` overflow would need numbers with about 24 million digits.
    * **Split vs whole:** splitting each product into `% 10` into `i + j + 1` and `/ 10` into `i + j` while accumulating is correct, because the carry pass fixes everything anyway. But it does work in the m·n inner loop that the carry pass already does once per cell. **Left as is:** the split costs a measured 2× (29–31 µs vs 13.8 µs at 200 × 200; 2.7–3.0 ms vs 1.21 ms at 2000 × 2000). Replacing it with `cell[i + j + 1] += product` was tested, with the same results. A constant factor, but the version to write next time.
    * **Optimal (`8db58d5`):** no recursion for the operand swap (both loops cover every pair either way), commented-out code gone, `static_cast<char>` for the digit conversion. 90,601 exhaustive pairs plus 100,000 random pairs up to 25 digits clean, with sanitizers.
* **Time & Space Complexity:**
    * **Time O(m·n):** one multiply-add per digit pair, plus O(m + n) for the carry pass and conversion.
    * **Space O(m + n):** the buffer and the output. One allocation each, against 2–3 per digit in the string version.

### [8] Detect Squares
* **The Core Pattern:** a count map of points (§2 Pattern 8), queried by **fixing one corner and looking up the rest**. Every axis-aligned square with a corner at the query Q = (x1, y1) has exactly one other corner **directly above or below Q**, at (x1, y2). That vertical side fixes the side length `side = |y2 − y1|`, and the square can only hang to the **right** (x1 + side) or the **left** (x1 − side) of it. **Only the x changes:** the two missing corners keep Q's and the column point's y values, (x ± side, y1) and (x ± side, y2). The number of squares is the product of the three corners' counts. Store the counts as x → (y → count), so the query's column is one lookup away.
* **The "Gotcha":** **"can Q be the corner of four squares at once?"** Yes, and the column walk finds all four. For a given size, the point **above** Q gives the two squares above it (slid right and left), and the point **below** gives the two below. Up or down, times right or left: 4. Picture Q as a nail. Any square with a corner on it has one vertical edge running through the nail, going up or down, and the square hangs off that edge to the right or the left. Traced on Q = (2,2) with (2,4), (2,0), (4,2), (4,4), (0,2) and (0,0) stored:

  | column point | side | slide to x = | look for | square? |
  |---|---|---|---|---|
  | (2,4) | 2 | 4 | (4,2), (4,4) | **yes** |
  | (2,4) | 2 | 0 | (0,2), (0,4) | no |
  | (2,0) | 2 | 4 | (4,2), (4,0) | no |
  | (2,0) | 2 | 0 | (0,2), (0,0) | **yes** |

* **The Struggle & Insights:**
    * **First pass (`596eed2`): find the opposite corner.** Walk every stored point; if it's on a diagonal from Q, look up the other two corners. Right idea, and three bugs on the way to it:
        1. **The test was inverted** (`!=` under a comment saying "diagonals match"), so the one point that *was* the opposite corner got skipped. LeetCode's example returned `0 0 0`.
        2. **Zero size counts too** once the test is `==`: Q itself has |dx| = |dy| = 0, and its "other corners" are itself. Require dx ≠ 0.
        3. **Duplicates multiply; they don't add.** First only the opposite corner's count was added; then the rewrite multiplied the two side corners but **dropped the opposite corner's count**. LeetCode's example duplicates a side corner, so it passed. Duplicating the *opposite* corner (two (0,0), one (0,1), one (1,0), query (1,1): 2 squares) broke it.
    * **"I don't know anything about the sides."** The column idea didn't land as a formula. What made it land: running the loop by hand on real points (the table above), then the nail picture. The rewrite's one remaining bug: **`if (side == 0) return 0;` instead of `continue`.** Reaching the query's own entry in its column exited `count` and threw away every square, found or not. The inner map is an `unordered_map`, so the query's own y can come before or after the real squares; either way they were lost. 281 of 20,000 random sequences failed. The editor not saving cost one extra round.
    * **Final (`91c04bb`):** a `const` helper, "count at (x, y1) and (x, y2), times the column point's count, or 0", with **one `find` per corner** instead of `contains` + `at`, so each side is one line. Inputs by `const&` instead of a by-value `vector` copy (a heap allocation on every call). `const` restored on `count`, the helper `private`, and the unused alias and empty constructor removed. A rename to `CountSquares` along the way would have failed to compile on LeetCode.
    * **Overflow bound to say out loud:** at most 3000 calls, so three counts sum to ≤ 3000 and the largest product is 1000³ = 10⁹, which fits in `int`. A product of three counts is the classic place for an overflow to hide.
    * **Measured at `-O2`** (identical answers): full scan 7.5 ms → column walk **0.13 ms** at LeetCode size; 11.7 s → **53 ms** at 100k adds + 10k counts. A flat 1001 × 1001 count grid, possible because the coordinates are bounded to 0..1000, would be another 2–7× (0.05 ms / 5.6 ms) by removing the hashing. That's the systems point to *mention*, not to write.
* **Time & Space Complexity:**
    * **`add`: O(1) average.** **`count`: O(k)** for the k distinct y values in Q's column, with up to 6 hash lookups each. The full scan was O(N log N).
    * **Space O(N)** for N distinct points.
