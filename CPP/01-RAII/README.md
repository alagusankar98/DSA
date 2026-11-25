# RAII + Rule of 0 / 3 / 5

> **Topic 1 of the Modern C++ track** (`roadmap.md` §4). Notes grow as I learn. Experiments are the `.cpp`
> files in this folder, each with an entry in §3.
>
> **Build any experiment with:** `g++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined file.cpp && ./a.out`

**Questions I should be able to answer cold by the end:**
* What is RAII, and why does it make cleanup automatic even when an early `return` or an exception skips the rest of the function?
* What exactly happens, and in what order, when an object goes out of scope (members, base classes, arrays)?
* Which special member functions does the compiler generate on its own, and when does it stop generating them?
* Rule of 3 / 5 / 0: what each one says, and *why* owning a raw resource forces it.
* What goes wrong with a copied raw-pointer owner (shallow copy → double free), and how `= delete` or a smart-pointer member fixes it.
* What it costs: does RAII add any runtime overhead compared with a manual `free` / `delete`?

**Already seen in DSA (fold these in first):**
* `NeetCode/09-Tries/README.md` `[1]` Implement Trie: the missing Rule of Three (a raw-pointer owner with a destructor but compiler-generated copies), fixed with `= delete`. Also why `delete &root_` on a by-value root is UB, and the iterative vs `unique_ptr` destructor trade-off.
* `NeetCode/09-Tries/README.md` `[3]` Word Search II: pruning a node by nulling the parent's pointer **leaks** it. Cutting the link isn't freeing the memory.
* `NeetCode/01-Linked-List/readme.md` `[8]` LRU Cache: deleting the copy and move operations on a class that owns nodes.
* `NeetCode/01-Linked-List/readme.md` `[5]` Remove Nth Node: a stack-allocated dummy node (automatic cleanup) vs a `delete` you have to write yourself.
* `NeetCode/08-Trees/README.md` Serialize/Deserialize: `new`-ed nodes the caller must free, and how `unique_ptr` children would make that automatic.

**Already seen in C++:** none in this track yet. **But outside this repo** I already wrote RAII lock guards in a
C++ gRPC distributed-file-system project (`ScopedFileLock`, `ScopedAsyncLock`: release a lock in the destructor
so a client dying mid-transfer can't deadlock a file). Stage 1 can start from that code: what it got right, and
whether it survives the two traps below and a copy.

**Anchor for stage 1:** my own cleanup problem from a C project. A mutex was
locked and a key added, then 10 failure paths each needed a different mix of cleanups, handled with a pile of
`goto` labels. Rewrite it with RAII, and run into the two traps that a "lock + add key in one constructor" design
hides: **(a)** if adding the key throws, the destructor never runs (the object never finished constructing), so
the mutex stays locked; **(b)** the key must be removed only on *failure*, so the guard needs a
commit/dismiss step (the same idea as `unique_ptr::release()`).

**Carry-forward:** none yet.

**Sources:** learncpp.com (main text) · cppreference (exact rules) · CppCon "Back to Basics" talks (tired blocks) · an LLM as quizmaster, with every claim checked by an experiment.

---

## 1. Core Fundamentals

*(Filled in as I learn: concepts first, then what the compiler generates, then what it costs in hardware.)*

## 2. Quick Reference

*(Rules and a cheat sheet, filled in as I learn.)*

### My answer in 2–3 sentences

*(Cold interview answers to the questions at the top, written only once I can say them without notes.)*

---

## 3. Experiments & Lessons

*(One `### [N] Name` block per `.cpp` file or quiz: what I tested, my prediction, what actually happened, why, and the lesson.)*

---

## 4. Capstone

*(Added at the end of the topic: build from scratch, debug, and/or a "why it's necessary" scenario. The problem statement goes here, then the review outcome.)*
