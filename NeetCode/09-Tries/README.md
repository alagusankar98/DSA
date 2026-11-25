# Tries - Topic Guide

## 1. Core Fundamentals

A **trie** (pronounced "try", from re*trie*val; also called a **prefix tree**) is a tree built for one job: storing a set of **strings** so that everything sharing a prefix also shares the path that spells it. It's the first data structure in the run that builds directly on Trees: same nodes and pointers, same `nullptr` base cases, same DFS. The difference is what the shape *means*.

### The Mental Model: the letters live on the edges
Insert `car`, `cat`, `cap`, `do`, `dog`:
```text
                 (root)            ← represents the empty string ""
                /      \
             'c'        'd'
              |          |
             'a'        'o'  ★     ← ★ = "a word ends here" (do)
           /  |  \       |
         'p' 'r' 't'    'g'  ★     (dog)
          ★   ★   ★
       (cap)(car)(cat)
```
* **A node doesn't store "its" letter. The letter is *which child slot* you took to get there.** The root is the empty string, and every node stands for **the prefix spelled by the path from the root to it**. The node reached by `c → a` *is* the prefix `"ca"`.
* **Shared prefixes are stored once.** `car`/`cat`/`cap` share the single `c → a` path. That's the whole point. A hash set stores `"car"`, `"cat"`, `"cap"` as three unrelated strings, with nothing in common.
* **The end-of-word flag is not optional.** In the picture, `"do"` is a word *and* a prefix of `"dog"`. Reaching a node only proves the *prefix* exists. Whether a word *ends* there is a separate fact that must be stored on the node (a `bool`). Without it, inserting `"dog"` would make `search("do")` true. This is the #1 trie bug.

### Standard Node Structure (C++)
The LeetCode alphabet is usually lowercase `a–z`, so the classic node is a fixed array of 26 child pointers:
```cpp
struct TrieNode {
    TrieNode* children[26] = {};   // value-initialized: all nullptr
    bool isEnd = false;            // does a word END at this prefix?
};
```
* **Index = character:** `children[c - 'a']`. `'a'..'z'` are contiguous in ASCII, so subtracting `'a'` maps them to `0..25`. It's the same offset trick as the `[-3000, 3000]` lookup array in Trees `[13]`, and the `count[26]` arrays from Arrays & Hashing.
* **Compared with `TreeNode`:** `left`/`right` become 26 numbered slots. A binary tree is a trie over a 2-letter alphabet.
* **`= {}` matters.** Without it, a `new TrieNode` member array is uninitialized garbage, and `if (children[i])` follows a wild pointer. Default member initializers fix it for every constructor.

### The One Loop Every Trie Operation Is Built From
Almost every trie operation is the same **walk**: start a cursor at the root, and for each character of the input, move to `children[c - 'a']`. The operations differ only in **what you do when the slot is empty** and **what you check when the string runs out**:

| Operation | Slot is `nullptr` mid-walk | String ends — return |
| -- | -- | -- |
| `insert(word)` | **create** the child, keep walking | set `isEnd = true` |
| `search(word)` | stop → `false` | `isEnd` (a prefix alone isn't a word) |
| `startsWith(prefix)` | stop → `false` | `true` (reaching the node is enough) |

* This is **single-path descent**, the Trees §2 Pattern 4 idea again: one cursor, no branching, so it's a plain loop with **O(1) extra space**, no recursion and no stack. Like BST search, it never needs to remember an alternative path.
* **Recursion comes back only when the input branches.** A wildcard like `.` ("any letter") means trying *every* non-null child, and that's a DFS with backtracking (the shape of the `Design Add and Search Words` problem). Same rule as Trees: **one path → loop; many paths → DFS**.

### Complexity: why "O(L)" beats "O(1)" hashing here
Let **L** = length of the word.
* `insert` / `search` / `startsWith`: **O(L) time**, independent of how many words are stored. A hash set lookup is "O(1)" too, but it **hashes all L characters** first, then compares the whole string on a hit, so it's also O(L) in practice.
* **What a hash set *can't* do is prefixes.** "Is there any word starting with `ca`?" costs a hash set a scan over every word. For a trie it's a two-step walk. **Reach for a trie when the question is about prefixes, not whole-word membership.**
* Sorted array + binary search can answer prefix queries too (`lower_bound("ca")`), at O(L log N) per query. A trie trades memory to remove the `log N`, and it shines when a search must **extend letter by letter** (Word Search II), because each step is one array index from where you already are.

### The Hardware Reality: tries are memory-hungry
* **Per-node cost of the array node:** 26 × 8-byte pointers = **208 bytes**, + 1 `bool` → padded to **216 bytes** on a 64-bit machine, mostly `nullptr`s. Deep in the tree a node typically has 1 used child out of 26, so roughly 96% of each node is wasted space. Inserting 10⁴ words of length 10 with little sharing is ~10⁵ nodes ≈ **21 MB**. On a Cortex-M with 128 KB of SRAM that's not happening.
* **Alternatives (trade-offs worth naming in an interview):**
    * **`std::unordered_map<char, TrieNode*>` children:** stores only the children that exist, so it's better for large or sparse alphabets (Unicode). But each entry is its own heap allocation, plus the map's bucket array, and every step pays for hashing. Slower and less cache-friendly than direct indexing for `a–z`.
    * **Pool / arena allocation (the embedded answer):** keep all nodes in one `std::vector<TrieNode>` and store children as `int` indices instead of pointers. One contiguous block, half-size links (4 bytes instead of 8), no per-node `malloc`, and the entire trie is freed in a single deallocation. Same idea as a static memory pool on firmware.
    * **Compressed trie (radix tree):** merges single-child chains into one edge labelled with a string. That's what real routers (IP longest-prefix match) and the Linux kernel's radix trees use. Know it exists; it isn't needed for NeetCode.
* **Ownership:** every `new TrieNode` must be freed. LeetCode won't complain if it leaks; production code should. Either write a recursive destructor (post-order: free the children, then yourself), use `std::unique_ptr<TrieNode> children[26]`, or use the arena above, which avoids the problem entirely.

### Where Tries Show Up in the Real World
Autocomplete and search suggestions, spell checkers, IP routing tables (longest-prefix match on address bits, so a binary trie), T9 predictive text, and word games like Boggle (which is Word Search II).

---

## 2. General Summary / Quick Reference

When tackling Trie problems, keep these core patterns in mind:

### Pattern 1: Build + Walk (Basic Trie Operations)
The table in §1: `insert`, exact `search`, `startsWith`, all as a single iterative walk with one cursor pointer.
* *Use for:* Implement Trie (Prefix Tree), autocomplete, "does any word start with X", word counting (swap `bool isEnd` for `int count`).
* *Discipline:* decide up front what a node *means* (a prefix) and what the flag *means* (a word ends here). Every bug traces back to mixing the two up.

### Pattern 2: Wildcard Search = DFS over Children
When a query character can match **any** letter (`.`), the walk forks. At a `.`, try every non-null child recursively, and return `true` as soon as one branch succeeds. Regular characters still follow just one slot.
* *Use for:* Design Add and Search Words Data Structure, regex-lite matching against a dictionary.
* *Shape:* recursive `(node, index into word)`. Base case: the word is used up → return `node->isEnd`. It's Trees-style DFS where the "children" are 26 slots instead of 2.

### Pattern 3: Trie as a Pruning Index for Another Search
Load the dictionary into a trie, then run a *different* search (grid DFS / backtracking) and walk the trie **in lockstep**, one character per step. The moment the current path isn't a prefix in the trie, abandon that branch. One trie walk replaces running a separate search for every word.
* *Use for:* Word Search II (find all dictionary words in a letter grid).
* *Note:* this leans on grid DFS + backtracking (topic 11). Expect it to be the hardest of the three. Classic optimizations: store the whole word at its end node instead of rebuilding the string, and remove (or mark) a word once found so it isn't reported twice.

### The Non-Negotiable Discipline
* **Initialize children to `nullptr`** (`= {}`), every time.
* **Prefix ≠ word.** "I reached the node" and "a word ends here" are different facts. `search` checks the flag; `startsWith` doesn't.
* **One path → loop; branching (wildcards, grids) → DFS.** Same rule as Trees.
* **Know the memory bill.** Be ready to say "216 bytes per node, mostly null, here's the map / arena / radix alternative."

---

## 3. Problem Strategies & Patterns

*(Entries added one problem at a time as they're solved — same format as the other topic guides: **The Core Pattern**, **The "Gotcha"**, **Time & Space Complexity**, **The Struggle & Insights**.)*

<!-- NeetCode 150 Tries (3): Implement Trie (Prefix Tree) · Design Add and Search Words Data Structure · Word Search II -->

### [1] Implement Trie (Prefix Tree)

> Relatively easy, once the node shape was settled. Designing `TrieNode` was the tipping point; after that, the three operations were the §1 walk, and the rest of the time went into review-level polish. Two versions committed: the first pass (with a `char val` field and three copies of the walk), and the optimized one (no `val`, a shared walk helper, an iterative destructor, `string_view` parameters).

* **The Core Pattern:** The §1 walk, three times over. Start a cursor at the root and, for each character, step to `next[c - 'a']`. The three operations differ only in the empty-slot action and the end-of-string check:
    * `insert`: an empty slot means **create** the child. After the loop, mark `isDone = true` on the node where the word ends.
    * `search`: an empty slot means `false`. At the end, return `isDone`.
    * `startsWith`: an empty slot means `false`. At the end, return `true`.
    * The optimized version moves the shared walk into a private helper, `returnNode(word)`, that returns the node where the string ends, or `nullptr`. Then `search` is `node && node->isDone` and `startsWith` is `node != nullptr`. `insert` keeps its own loop because it *creates* nodes instead of failing.

* **The design question that unlocked it — where does the array of children live?** You weighed keeping a plain `val + next` node and putting the 26-slot array in the outer class. It belongs **in the node**, because *every* node can branch 26 ways, not just the top. The outer class only holds the entry point (the root) and the operations, the same split as `TreeNode` vs the functions that walk it.

* **Your self-review, resolved point by point:**
    * **`char val` is redundant — correct, and it's the key trie idea.** The letter is **which slot you came through**, not something the node stores (§1, "the letters live on the edges"). `val` was 1 byte, padded out to 8 bytes in a 216-byte node. That's small, but it's also a second copy of information the index already holds, and that copy could disagree with it.
    * **"Dummy" node → call it `root`.** It isn't a placeholder; it's the node for the **empty string `""`**, the prefix every word shares. That's why it always exists, even with zero words: `startsWith("")` is `true` on an empty trie because you're already standing at the `""` node. The "dummy" in Linked List was different: it was a *fake* node added to remove the head special case, and it held no meaning. The trie root holds the empty-prefix node and its `isDone` flag (set if `""` was inserted).
    * **"Checking all 26 slots to tell if it's empty seems wasteful."** It's only wasteful if you need that query often. 26 null checks on one contiguous 208-byte block is a few cache lines and essentially free. If `empty()` or `size()` were part of the API, keep a `size_t wordCount_` member and bump it in `insert` when `isDone` goes from `false` to `true`. That's O(1), and the standard answer: **track metadata at write time instead of recomputing it at read time.**
    * **The existence check before `new` — the critical catch.** Without `if (!next[i])`, inserting `"app"` after `"apple"` replaces the existing `a → p → p` nodes with fresh ones, cutting off `l → e` (still allocated, now unreachable: a **leak plus a lost word**). That's the same "check before you overwrite" rule as never reassigning a linked-list `next` before saving the old one.
    * **`isDone = true` after the loop — yes, that's the standard.** After the loop, `current` *is* the node for the full word. Inside the loop it's every prefix along the way, and those must stay `false`, otherwise `insert("apple")` would make `search("app")` true.

* **The "Gotcha" — what the interviewer review got right, and what it missed:**
    * **Got right:** `string` by value copies the argument on every call. That's a heap allocation for anything past the 15-char short-string buffer, and LeetCode words go up to 2000 chars. `std::string_view` is zero-copy. **Duplicated walk → one helper** (DRY). `return p && p->isDone;` short-circuits, so `p->isDone` is never read when `p` is null.
    * **Missed — the Rule of Three (a real crash, not a nitpick).** `PrefixTree` owns raw heap pointers and has a destructor, but no copy constructor or copy assignment. So the compiler generates ones that copy `head_` **as a pointer**. `PrefixTree b = a;` gives two objects pointing at the same nodes, and when both go out of scope, both destructors free them: a **double free**. The rule: *if you write a destructor, a copy constructor, or copy assignment, you almost always need all three.* The minimal fix is to forbid copying:
      `PrefixTree(const PrefixTree&) = delete;` and `PrefixTree& operator=(const PrefixTree&) = delete;`.
      LeetCode never copies the trie, so it never shows up there. In a code review, it's the first thing a senior C++ reviewer flags. **Fixed in the follow-up:** both are now `= delete`. Your comment says "delete copy and move semantics", and that's accurate even though only the copy pair is written. Once you declare a copy constructor or copy assignment yourself, even as `= delete`, the compiler stops generating the move constructor and move assignment. A move attempt then falls back to the deleted copy and fails to compile. So the class can be neither copied nor moved, which is the safe default for a raw-pointer owner.
    * **Missed — the iterative destructor is *more* robust than the `unique_ptr` version, not just equivalent.** The LLM said `std::array<std::unique_ptr<TrieNode>, 26>` means "no destructor needed". True, but the cleanup it generates is **recursive**: each node's destructor destroys its children's `unique_ptr`s, which call *their* destructors, and so on, one stack frame per level. Measured: a 2000-deep chain (LeetCode's max word length) is fine, but a 10⁶-deep chain **segfaults** on an 8 MB stack. Your `std::stack` loop keeps the pending nodes on the heap, so its depth is unlimited. That's the same Trees lesson (iterative over recursive when depth isn't bounded), and on a 4 KB MCU stack the difference matters much sooner.
    * **Small things:**
        * `if (head_)` in the destructor is always true (the constructor always allocates the root). Removed in the follow-up, along with renaming `head_` → `root_`.
        * Storing the root **by value** (`TrieNode root_;`) skips one `new` and the null question entirely. The destructor then starts by pushing `root_`'s children instead of `root_` itself.
            * **Your question: wouldn't every `current` then become a `TrieNode` copy?** No. Only the **root** moves into the object; the cursor stays a pointer. Start it with the root's **address**: `TrieNode* current = &root_;`. Every node below the root is still heap-allocated and reached through `next[i]`, so `current = current->next[i]` doesn't change. The cursor never holds a node by value. It's a 64-bit address that gets reassigned at each step, exactly as before.
            * **The trap you sensed is real, but only with `auto`.** If you keep `auto current = root_;`, `auto` deduces `TrieNode` (a by-value **216-byte copy** of the root), not a pointer or reference. `auto` never adds `*` or `&` on its own. You'd find out immediately, though: `current->next` doesn't compile on a non-pointer. Write `&root_` (or `auto* current = &root_;` to make the pointer-ness explicit).
            * **Destructor change, and it's mandatory:** never `delete &root_`. It wasn't allocated with `new`, so `delete` on it is undefined behavior (heap corruption). Push `root_.next[i]` for each non-null child, and let the root die with the object.
            * **Is it worth it?** It saves one heap allocation and one pointer hop on every operation, and removes "can the root be null?" from the design entirely. Its cost is that `PrefixTree` now holds the 216-byte node itself instead of an 8-byte pointer (fine, since it's not being copied anyway). It's a small win; the pointer version is not wrong. **Owner's decision: keep the heap root.** The by-value root makes the destructor asymmetric: either skip the root inside the loop (`nodeToDelete != &root_`), or seed the stack with its 26 children in a separate loop first. Both add a special case to code that's currently uniform ("every node came from `new`, so every node gets `delete`"). One allocation per trie isn't worth breaking that symmetry. (Terminology: a by-value root isn't necessarily on the *stack*. It's a member, so it lives wherever the `PrefixTree` object lives: on the stack, on the heap, or in static storage.)
            * **Related polish:** `returnNode`, `search` and `startsWith` never modify the trie, so they can be marked `const`. Inside a `const` member, `&root_` has type `const TrieNode*`, so the cursor becomes `const TrieNode*` too, and the compiler guarantees the read paths never write.
        * `c - 'a'` assumes lowercase `a–z` (a LeetCode guarantee). Any other character gives an index outside `0..25`, which is **out-of-bounds UB** on `std::array::operator[]`. Validate the input, or `assert`, if it isn't guaranteed.

* **📝 Parked for later — the `std::unique_ptr` version (owner's note: not implemented yet).** What to try when you come back to it:
    * Children become `std::array<std::unique_ptr<TrieNode>, 26> next;`. Create with `next[i] = std::make_unique<TrieNode>();`, and walk with a **raw, non-owning** cursor: `TrieNode* cur = root_.get(); … cur = cur->next[i].get();`. Owning pointers live in the tree; the cursor only observes.
    * **What you gain:** no hand-written destructor, and the class becomes **move-only automatically** (`unique_ptr` can't be copied), which also solves the Rule-of-Three bug above.
    * **What you lose:** the recursive cleanup depth described above. To keep both benefits, write a destructor that detaches the children into a `std::vector<std::unique_ptr<TrieNode>>` worklist (`std::move` them out) and lets them die one at a time, so no recursion is needed. Worth doing once to really learn ownership transfer.

* **Time & Space Complexity:** `insert` / `search` / `startsWith`: $O(L)$ time per call (L = word / prefix length), $O(1)$ extra space for the walk (one cursor, no recursion). Storage: $O(\text{total characters inserted})$ nodes in the worst case (no shared prefixes), each 216 bytes (26 × 8-byte pointers + `bool`, padded). Destructor: $O(\text{nodes})$ time, with the work stack's peak size bounded by the node count.

### [2] Design Add and Search Words Data Structure

> `[1]`'s trie with one twist: `.` in a search matches **any** letter. You spotted the right shape yourself (recursion that forks at `.`), and the coding took a while. The first committed version had a real correctness bug that simple tests don't catch: **a `.` branch returned the first node that matched the pattern's *length*, even when no word ended there.** The fix is in the second version.

* **The Core Pattern:** Trie + **DFS over children** (§2 Pattern 2). `addWord` is `[1]`'s `insert` unchanged. `search` becomes a recursive walk carrying **(node, index into the pattern)**:
    * Letter → follow exactly one slot (or fail if it's empty), the same as `[1]`.
    * `.` → try **every non-null child** with `index + 1`, and stop at the first branch that succeeds.
    * Pattern used up → the answer is whether a word ends at this node.
    * This is the Trees "one path → loop; branching → DFS" rule playing out. `[1]` never branched, so a loop worked. `.` branches, so the "come back and try the next sibling" behavior that recursion gives you for free is exactly what's needed.

* **Your "wild search" worry — "26 searches per dot, more dots means more searches".** The fear is 26ᵈ for d dots. The real bound is much better:
    * **A `.` only follows children that *exist*.** A sparse trie gives a `.` a handful of options, not 26.
    * **A single search can't visit more nodes than the trie holds,** because each node is reached by one path from the root. Whatever the dot pattern, the worst case is O(total nodes).
    * **LeetCode 211 also caps it:** at most 2 dots per query, word length ≤ 25. So the realistic worst case is about 26² × 25 steps per query, and the recursion is at most 25 frames deep, which is safe.
    * Your first idea, "a helper that starts the search from a given node instead of root, called 26 times", **is** the recursion. You just hadn't noticed that the helper can call itself.

* **The "Gotcha" — the prefix-shadowing bug (the lesson of this problem):**
    * **First version:** `findNode` returned "the node where the pattern ends". `search` checked `isDone` on that node **afterwards**. At a `.`, the loop returned the **first** non-null result.
    * **Failing case (compiled and confirmed):** `addWord("abcd")`, `addWord("xbc")`, `search(".bc")`. The `.` tries `'a'` first and reaches the node for `"abc"`. That node exists, since it's the prefix of `"abcd"`, but `isDone == false`. The loop returns it immediately, `search` sees `isDone == false`, and answers **`false`**. The `x → b → c` branch, a real word, is never tried. Correct answer: `true`.
    * **Why tests with only `"xbc"` passed:** without a competing prefix of the same length, the first branch to get to the end *is* the word.
    * **Why it never bit `[1]`:** with no `.`, there's only **one** path, so "the node where the pattern ends" is unique, and checking `isDone` afterwards is equivalent to checking it inside. **Once the search branches, "did this branch succeed?" has to use the *full* success condition (pattern used up *and* a word ends here) at the moment you decide whether to keep searching siblings.** Checking afterwards only sees whichever branch happened to come first.
    * **Your question: "does this only happen with wildcards?"** Yes. Only a `.` makes a choice between siblings, so only a `.` can stop too early. Letter-only searches have nothing to fall back to.
    * **Your fix:** inside the `.` loop, accept a child's result only if it's non-null **and** `isDone`, otherwise keep trying siblings, and return `nullptr` if none succeed. **Verified:** it passes the failing case above and 90,000 random queries checked against a brute-force matcher.

* **Version history — four versions, and why each change happened:**

    | # | State | `findNode` returns | Status |
    | -- | -- | -- | -- |
    | 1 | committed `b1fa5d6` (first pass) | node where the pattern ends | ❌ prefix-shadowing bug (above) |
    | 2 | committed `9511317` | node, but `.` loop accepts only `isDone` nodes | ✅ correct, success check in two places |
    | 3 | uncommitted attempt | `bool` | ❌ `.` loop returned on the first branch |
    | 4 | **final** (current file) | `bool`, null check in one place | ✅ correct (verified: 90k random queries vs brute force) |

    * **v2 → v3: why switch to `bool` at all — the "mixed contract".** In v2, a letter-only path returned "the end node, which might be just a prefix", and `search` still had to check `isDone`. Through a `.`, it returned "a node where a word definitely ends", because the loop had already filtered it. Both were correct, but the success condition lived in **two places**, so a later edit to one could drift from the other. The rule: **return what the caller actually needs.** Nobody needed the node here, only yes/no. (Returning a node was right in `[1]`, where `startsWith` and `search` both used it.) In the `bool` version, the base case "pattern used up → `return node->isDone;`" holds the whole success condition, and `search` is one call.

    * **v3's two bugs (the bool attempt that failed):**
        * **`return findNode(...)` inside the `.` loop.** A `return` in a loop ends the whole function on the first pass. So the loop tried the first existing child and passed up its answer, **true or false**, and the other 25 siblings were never tried. Failing case: `addWord("ab")`, `addWord("cd")`, `search(".d")`. The `.` takes `'a'`, finds no `d` under it, and returns `false`, so `c → d` is never examined. It's the v1 bug in a new form: v1 stopped on a branch that was only a prefix; v3 stopped on *any* branch, outright failures included. **The fix you found:** `.` means "succeed if **any** child succeeds". So only `true` may end the loop early (`if (result) return true;`), and `false` is the answer only **after** every branch has failed (the `return false;` after the loop). It's the same shape as `std::any_of`: short-circuit on the first hit, conclude "no" only after an exhaustive miss.
        * **`if (!searchNode) return true;` — a missing node counted as a match.** It was never reached in v3 (every caller checked null before recursing), so it was a latent bug, not the failure you saw. Corrected to `return false`.

    * **"What should a null node return?" — the question that stayed murky, and why it's `false`.** The confusion was mixing up **a null node** with **an empty trie**:
        * **An empty trie is not null.** The constructor always creates the root. A trie with no words is a single root node with 26 null slots and `isDone == false`.
        * **A null node means "no path continues this way".** The walk tried to step into a slot that was never created, so no stored word spells this pattern. That can never be a successful match, so the answer is `false`.
        * **`search("")` on an empty trie:** the root is non-null, so the null check is skipped. `currentIdx (0) >= size (0)` is true immediately, so it returns `root->isDone`, which is **`false`** unless `addWord("")` was called. Verified: `0` on an empty trie, and still `0` after `addWord("ab")`.
        * **Why `false` is right for `search("")`:** `search` asks "was this exact word inserted?", and `""` counts only if it was added. The "it should be true" intuition belongs to **`startsWith("")`** in `[1]`, which *is* true even on an empty trie: every word starts with `""`, and you're already standing at the root, so there's nothing to walk. It's the §1 prefix-vs-word distinction again, applied to the empty string.

    * **v3 → v4: removing the callers' null checks (one check, one place).** With the top-of-function null check returning `false`, the guards at the call sites (`if (node)` before recursing in the `.` loop, and `if (!next[charIdx]) return false` in the letter branch) were doing the same job twice. v4 deletes them and simply recurses into whatever the slot holds, null included. The callee's first line handles it. That's the Trees `[1]` habit again: **let the base case handle null, don't special-case it in the caller.**
        * **Trade-off:** at a `.`, every empty slot now costs a function call that immediately returns `false` (up to 25 wasted calls per `.` per node visited), where v3 skipped them with an `if`. Under LeetCode's 2-dot limit that's noise. In a hot loop you might keep the caller-side check as a fast path. It's a readability-vs-call-overhead choice, not a correctness one.

* **Other polish:**
    * Renaming the helper parameter from `root` to `searchNode` (v2) fixed the shadowing of the member `root`. 👍
    * `addWord` is `[1]`'s `insert` copied verbatim. If both lived in one codebase, they'd share one trie class.
    * The helper is still called `findNode`, but it no longer finds a node. Something like `matches` / `searchFrom` would now describe it honestly.

* **Time & Space Complexity:** `addWord`: $O(L)$ time. `search`: $O(L)$ for letter-only patterns; with dots, $O(\min(26^d \cdot L,\ \text{total nodes}))$ worst case for d dots, i.e. $O(26^2 \cdot L)$ under LeetCode's 2-dot limit. Space: $O(L)$ recursion depth for `search` (≤ 25 frames), plus the trie's $O(\text{total characters})$ nodes at 216 bytes each.

### [3] Word Search II

> The Hard that closes out Tries: Backtracking `[1]` Word Search, but for a whole **list** of words at once. I solved Word Search I first on purpose, and it paid off: the grid DFS (scanner, guard clauses at the top, mark/unmark) carried over almost unchanged. What took a long time was merging two traversals in my head, then three real bugs, then understanding pruning. First pass (no pruning) is committed in `26ef668`; the second version adds pruning and frees the pruned nodes.

* **The Core Pattern:** Trie as a pruning index for another search (§2 Pattern 3). **Put the *words* into a trie, then run Word Search I's DFS from every cell, carrying a trie node along.** One DFS checks the board against *every* word at the same time, and the trie says the moment the current path stops being a prefix of any word.

* **Why my first two ideas don't work:**
    * **Run Word Search I once per word.** Correct, but every word restarts the full grid search: `words × m·n·4·3^(L−1)`. LeetCode allows up to 3×10⁴ words on a 12×12 board, so it's far too slow. Words that share a prefix (`oath`, `oats`) also redo the same walk over and over, which is exactly the work a trie shares.
    * **Turn the board into a trie, then look up each word.** The trie would need every path that can be walked on the board, and the number of paths grows ×3 per step (§1 of Backtracking). That's exponential memory before searching anything. **Flip it: the words are the small, fixed set, so they go in the trie, and the board is what you search.**

* **"My brain is splitting: I'm traversing two things." How they sync up: the board drives, and the trie is the map.**
    * At cell `(i, j)` you're holding the trie node for the letters walked so far. **The board decides the next letter:** look at `board[i][j]`, and check exactly **one** slot, `node->next[board[i][j] - 'a']`.
    * Slot is `nullptr` → no word continues this way, so return. That one check is the pruning.
    * Slot exists → step into the cell and into that child **together**, then fire the 4 neighbour calls carrying the *child*.
    * **You never loop over the 26 children.** That was `[2]`'s `.` wildcard, where the *query* was fixed and the trie branched. Here it's reversed: the **board** branches (4 neighbours), and the trie answers one yes/no per step: "is this still a prefix?"
    * So `(cell, trie node)` advance in lockstep: one step on the board = one level down the trie.

* **Collecting the words: build the string top-down, not bottom-up.** My worry was "when I find a match, do I rebuild the string from the bottom up?" My idea was an `isMatch` flag on nodes plus a second pass over the trie at the end. It works, but it's an extra traversal. Two standard answers:
    * **Carry the path as a `std::string&`:** `push_back` the letter when stepping in, `pop_back` when leaving. It's the same choose / un-choose pair as `seen`. When a node has `isDone`, the string already spells the word, so copy it into the results. **I used this one.**
    * **Store the word in the node** (`std::string word` instead of `bool isDone`). No string-building at all, but every node carries a `std::string` member, used or not, which makes 216-byte nodes even bigger.
    * **De-duplicate by clearing `isDone` when the word is found.** The same word can be spelled along two different board paths. Unlike `seen`, this mark is **permanent** (never restored): once found, the word is done for the whole search.
    * **Don't stop when a word is found.** `app` and `apple` share a path, so the DFS keeps going below a found word.

* **The three bugs, and what each one teaches:**
    * **DFS started only from `{0, 0}`.** I forgot Word Search I's scanner: a word can start at *any* cell, so `findWords` needs the nested loop over every cell. `stringSoFar` doesn't need clearing between starts: every `push_back` is matched by a `pop_back`, so it's empty again whenever control is back in the loop.
    * **`seen` passed by value.** A missing `&` means each call gets a **full copy** of the grid: an allocation per row on every step, and marks that never reach the other calls. It's the same trap as Trees `[13]` copying the map on every call. Rule: **shared mutable state goes by `&`.**
    * **`insert` used `root` instead of `current`.** `root->next[c]` inside the loop meant every letter was hung directly off the root, and only the *last* letter's node was marked as a word end. `"oath"` became four depth-1 nodes, with the `h` node marked, so any `h` on the board was reported as the word `"h"`. It was a **wrong answer, not a timeout.** Two lessons:
        * **`insert` was retyped from `[1]`, not reused, and that's where the bug got in.** Test a reused piece on its own (insert a word, walk it back) before building on it.
        * **The other LLM was asked to check only compile errors, then called the broken version a "masterpiece" and blamed a TLE.** That diagnosis was a guess, and the bug was in code it had praised. Praise isn't a test.
        * **Term check:** the LLM called this "pointer shadowing". It isn't. **Shadowing** is when an inner declaration with the *same name* hides an outer one, like `[2]`'s helper parameter named `root` hiding the member `root`. This was just the wrong variable: a cursor that was declared but never used.

* **Pruning, "burning the bridge" (the part that broke my brain):**
    * **The problem:** after `"bat"` is found and its `isDone` cleared, the nodes `b → a → t` are still in the trie. Every later `b` on the board walks `b → a → t` again, does all the bounds and `seen` work, and finds nothing. The trie still *looks* like it holds a word there.
    * **The fix:** after a cell's 4 recursive calls return, check whether the child node is now **dead**: no children and not a word end. If so, the parent cuts it off (`root->next[charIdx] = nullptr`). This runs **on the way back up**, so it chains: cut `t`, and now `a` has no children, so cut `a`, then `b`. The whole word disappears from the trie, and the next `b` on the board dies on its first lookup.
    * **My question: "Why not just stop recursing when the node has no children?"** That saves the 4 neighbour calls on *this* path only. The dead nodes stay in the trie, so **every future start cell** walks them again. Pruning changes the trie itself, so the savings apply to every later search, not just this one.
    * **My first pruning attempt had two bugs:**
        * **The empty-check loop set `isEndTree = false` unconditionally.** `nextNode->next` is a `std::array<TrieNode*, 26>`: it **always has 26 elements**, `nullptr` or not, so the loop body runs on element 0 every time. The flag must depend on `if (node)`.
        * **`nextNode = nullptr` only cleared a local copy.** `nextNode` holds a copy of the address. Setting the copy to null leaves the parent's slot still pointing at the node. **The pointer that must change is the one *stored in the parent*, `root->next[charIdx]`.** Same idea as passing a pointer by value: you can change what it points *at*, but not the caller's pointer itself.
    * **The other LLM's "Trap 3" can't happen.** It said a childless node might be an unfound word, so you also need `!nextNode->isDone`. But a node with no children is always the **last letter of an inserted word**, and `insert` marks it. The only way it's childless *and* `isDone == false` is that the word was already found. In my code, `isDone` is also cleared before the check. **My `isEndTree` test is sufficient.** Adding `!isDone` is harmless, but it isn't fixing anything.
    * **The leak: cutting the link isn't freeing the node.** Once the parent's slot is null, the destructor's walk from `root` can never reach that node again, so it's leaked. **Measured with AddressSanitizer on the LeetCode example:** 1512 bytes leaked in 7 allocations = 7 × 216-byte nodes, which is exactly `oath` (4) + `eat` (3), the two words found and pruned. Fixed by `delete nextNode;` before nulling the slot; then no leak is reported. **Why `delete` is safe right there:** the node is a leaf, its own recursive calls have already returned, and every other active frame holds a node at a *different* trie depth (its ancestors). Nothing else can be using it.
    * **What pruning buys (measured, 12×12 board of all `'a'`, `-O2`, this machine):**

        | Words | No pruning (`26ef668`) | Pruning |
        | -- | -- | -- |
        | `a` … `aaaaaaaaaa` (10 findable) | 45 ms | ~0 ms |
        | + `aaaaaaaaab` … `aaaaaaaaaz` (25 that can never be found) | 35 ms | 14 ms |

        * Findable only: the first snake path finds all 10 words and the trie empties completely, so the other 143 start cells die on the first lookup.
        * **Pruning only removes *found* words.** A word that's never found keeps its whole path alive, so the 9-`a` chain under the unfindable words is walked again from every cell. That's why the second row is still 14 ms.
        * The other LLM said the unpruned version passed LeetCode "because C++ is so fast". I can't check LeetCode's tests, but 45 ms on this worst-shape board makes it plausible.

* **Interviewer review (my final code):**
    * **Kept from earlier problems:** deleted copy operations (Rule of Three, `[1]`), the iterative `std::stack` destructor, the `coordinates` alias inside the class (Word Search I), and `seen` / `stringSoFar` by `&`.
    * **Fixed in the second version:** the deleted assignment operator returns `PrefixTrie&` (convention; the compiler accepts either since it's deleted), and `TrieNode*&` became `TrieNode*`. A reference-to-pointer is only needed to **reassign the caller's pointer**, and the helper never does: it changes nodes *through* the pointer.
    * **`if (!root) return;` is now unreachable.** Every caller passes a non-null node: `findWords` passes the root, and the recursion only passes `nextNode` after checking it. It's harmless as a guard; just know it isn't doing work.
    * **The other LLM's "zero heap allocations" claim is slightly off:** each found word is copied into `resultVector` (one allocation per result), and `stringSoFar` grows once, to at most 10 characters. During the search itself, though, it's true.
    * **Verified:** both versions match a brute-force (Word Search I per word) checker on 20,000 random small boards (no mismatches, no duplicates).

* **Time & Space Complexity:** with board `m × n`, word length ≤ `L` (10 on LeetCode), `W` total characters across all words:
    * **Build:** O(W).
    * **Search:** O(m · n · 4 · 3^(L−1)). This is Word Search I's bound, but paid **once for all words** instead of once per word, because the trie caps the depth at L and kills dead paths immediately. Pruning doesn't change the worst case (unfindable words), only the typical one.
    * **Space:** the trie's O(W) nodes at 216 bytes each (shrinks as words are found), O(m · n) for `seen`, and O(L) recursion depth plus `stringSoFar`.
