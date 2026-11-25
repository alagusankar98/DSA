# Debugging C++ on Linux (WSL): compiler flags, sanitizers, gdb

> A reference to open whenever something crashes, hangs, leaks or gives the wrong answer.
> **Every command and output below was run on this machine** (Ubuntu 22.04 on WSL2, g++ 11.4, gdb 12.1,
> Nov 25, 2026). Outputs are trimmed, not invented. Where WSL behaves differently from plain Linux, it says so.

---

## 0. The map: what is what

| Tool | What it is | Installed separately? | When it acts |
| ---- | ---------- | --------------------- | ------------ |
| **`g++`** | The compiler: turns `.cpp` into a program | Yes (already installed) | Build time |
| **Warnings** (`-Wall -Wextra`) | The compiler pointing at suspicious code | No, part of `g++` | Build time |
| **Sanitizers** (ASan, UBSan, LSan, TSan) | Extra checks the compiler **inserts into your program**, plus a small runtime library that ships with gcc | **No, built into `g++`.** You switch them on with a flag; there's nothing to install or launch | Run time: your program checks itself as it runs |
| **`gdb`** | A separate program that runs *your* program under its control: pause, step, inspect | Yes (already installed) | Run time, or after the fact on a core dump |
| **`valgrind`** | A separate program that runs your **unchanged** program on a simulated CPU and watches every memory access | Yes (already installed) | Run time, ~10–50× slower |
| **Core dump** | A file the OS writes when a program crashes: a snapshot of its memory | No, an OS feature | After a crash |

**So: is ASan built in or a separate tool?** Built in. `-fsanitize=address` makes `g++` add a check before
every memory access in your code. Then you run the program **exactly as normal** (`./prog`). If a check fails,
the program prints a report and stops itself. `gdb` is the opposite: a separate program you launch *around*
yours.

**How they fit together:** sanitizers tell you **what went wrong and where** (the exact bad access). `gdb`
lets you **stop and look around** (the values of variables, the call chain). Most bugs need only one of them.

---

## 1. Compiler flags

A full command, then each piece:

```
g++ -std=c++20 -Wall -Wextra -g -O0 -fsanitize=address,undefined file.cpp -o prog
```

| Flag | What it does | Why you want it |
| ---- | ------------ | --------------- |
| `-std=c++20` | Which C++ standard to compile as | Without it, g++ 11 defaults to C++17, so C++20 features fail to compile |
| `-Wall` | Turn on the common warnings ("all" is a historical name; it isn't all of them) | Catches unused variables, missing `return`, `=` vs `==` and similar |
| `-Wextra` | More warnings on top of `-Wall` | Catches signed/unsigned comparisons, unused parameters and similar |
| `-Werror` | Treat every warning as an error | Optional. Forces you to fix warnings instead of scrolling past them |
| `-g` | Store **debug info** in the program: line numbers, variable names, types | Without it, `gdb` and the sanitizer reports show raw addresses instead of `file.cpp:42` and `head` |
| `-O0` | No optimisation (the default if you don't pass `-O`) | The program runs line by line as written, so stepping and variables in `gdb` make sense |
| `-O2` | Real optimisation | For speed and benchmarks. **Bad for debugging** (see the example below) |
| `-fsanitize=address` | **ASan**: checks every memory access (also turns on **LSan**, the leak checker) | Catches out-of-bounds, use-after-free, double free, leaks |
| `-fsanitize=undefined` | **UBSan**: checks for undefined behaviour | Catches signed overflow, shifts too far, null references, misaligned access and more |
| `-fsanitize=thread` | **TSan**: checks for threading bugs | Data races and inconsistent lock order. **Can't be combined with `address`** |
| `-fno-sanitize-recover=all` | Make every sanitizer error stop the program | UBSan otherwise prints and **keeps going** (see §2.3) |
| `-pthread` | Link the threading library | Needed for `std::thread` programs |
| `-o prog` | Name of the output program | Without it, you get `a.out` |

**Ready-made combinations:**

| Situation | Command |
| --------- | ------- |
| Everyday experiments (the default in `CPP/`) | `g++ -std=c++20 -Wall -Wextra -g -fsanitize=address,undefined f.cpp -o f` |
| Stepping through in `gdb` | `g++ -std=c++20 -Wall -Wextra -g -O0 f.cpp -o f` |
| Threads | `g++ -std=c++20 -Wall -Wextra -g -fsanitize=thread -pthread f.cpp -o f` (run with `setarch -R`, §2.4) |
| Benchmarking | `g++ -std=c++20 -O2 f.cpp -o f`: no `-g` needed, **never** with sanitizers (they slow it down) |

**Why `-O2` makes debugging confusing (measured).** This program divides by `a[2]`, which is 0, inside `work()`:

| Built with | What `gdb` reports |
| ---------- | ------------------ |
| `-g -O0` | `SIGFPE, Arithmetic exception` in **`work (a=..., n=2) at opt.cpp:2`**, called from `main`. Exactly right. |
| `-g -O2` | `SIGILL, Illegal instruction` in **`main () at opt.cpp:3`**. `work()` isn't on the stack at all: the compiler inlined it, saw the division by zero at build time and replaced it with a trap instruction. |

Same bug, a different signal, a different function. Debug at `-O0`.

---

## 2. Sanitizers

You turn them on at compile time and then run the program normally. They cost roughly 2× in speed and more
memory, which is fine for experiments, but never benchmark with them on.

### 2.1 ASan (AddressSanitizer): memory errors

**Catches:** reading or writing past the end of an array or `vector` (heap, stack or global), use-after-free,
double free, use of a stack variable after its function returned.

The bug: `new int[5]`, then a loop `for (int i = 0; i <= 5; ++i) arr[i] = i;` writes `arr[5]`.

- **Without ASan:** it prints `2` and exits with 0. **The bug is invisible.** It silently overwrote whatever
  came after the array.
- **With ASan:**

```
==86992==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x503000000054 ...
WRITE of size 4 at 0x503000000054 thread T0
    #0 0x5be28fd34459 in main heap.cpp:5                         ← the bad write: file and line
    ...
0x503000000054 is located 0 bytes to the right of 20-byte region  ← just past the end of a 20-byte block (5 ints)
allocated by thread T0 here:
    #0 ... in operator new[](unsigned long)
    #1 ... in main heap.cpp:4                                    ← where that block was allocated
SUMMARY: AddressSanitizer: heap-buffer-overflow heap.cpp:5 in main
Shadow bytes around the buggy address: ...                       ← ASan's internal bookkeeping: skip it
==86992==ABORTING
```

**How to read any ASan report:** (1) the first line names the **kind** of bug. (2) `READ`/`WRITE of size N`
plus the **first `#0` line in your own code** is where it happened. (3) "located N bytes to the right of
…-byte region" says how far off you were. (4) "allocated by … here" (or "freed by … here" for use-after-free)
says which object it was. Everything after `SUMMARY` can be ignored.

### 2.2 LSan (LeakSanitizer): memory leaks

Comes free with `-fsanitize=address` on Linux. It checks **when the program exits**:

```
==87049==ERROR: LeakSanitizer: detected memory leaks
Direct leak of 400 byte(s) in 1 object(s) allocated from:
    #0 ... in operator new[](unsigned long)
    #1 ... in main leak.cpp:2                ← the `new int[100]` that was never deleted
SUMMARY: AddressSanitizer: 400 byte(s) leaked in 1 allocation(s).
```

"Direct leak" = nothing points at the block any more. "Indirect leak" = it's only reachable from other leaked
blocks (e.g. the rest of a leaked linked list); fix the direct ones and those go away too.

### 2.3 UBSan (UndefinedBehaviorSanitizer): undefined behaviour

**Catches:** signed integer overflow, shifting by ≥ the bit width, dividing by zero, null references,
misaligned pointers, falling off the end of a non-void function, out-of-range enum values and more.

```
ub.cpp:5:7: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
-2147483648
```

**⚠ The trap:** by default UBSan **prints the error and keeps running**. The program above printed the
wrapped value and **exited with 0**. If its output scrolls past, you'll never know. Make it fatal, either way
works (both checked: exit code 1):

- at build time: add `-fno-sanitize-recover=all`
- at run time: `UBSAN_OPTIONS=halt_on_error=1 ./prog`

### 2.4 TSan (ThreadSanitizer): threading bugs

**Catches:** data races (two threads touching the same variable, at least one writing, with no lock), and
**inconsistent lock order** (a potential deadlock), *even on a run where the deadlock didn't happen*.

- **Separate build:** `-fsanitize=thread` can't be combined with `address`. g++ refuses:
  `error: '-fsanitize=thread' is incompatible with '-fsanitize=address'`.
- **WSL quirk:** run it as `setarch -R ./prog`. Without that, it dies at startup with
  `FATAL: ThreadSanitizer: unexpected memory mapping`. That's because this kernel randomises memory
  addresses more than TSan expects, and `setarch -R` turns that randomisation off for one run.

Measured on this machine:

| Program | TSan says |
| ------- | --------- |
| Two threads each do `++counter` 100 000 times, no lock | `WARNING: ThreadSanitizer: data race ... race.cpp:3 in inc()` |
| Thread A locks `m1` then `m2`; later, thread B locks `m2` then `m1` (run one after the other, so it **didn't** deadlock) | `WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock)` |
| The same two threads running **at the same time**, actually deadlocked | Nothing: the program just hangs. For a real hang, use `gdb` (§4) |

### 2.5 Sanitizer options (environment variables)

You set them for one run by putting them in front of the command, e.g. `ASAN_OPTIONS=detect_leaks=0 ./prog`.
Several options go in one variable, separated by `:`.

| Option | Effect |
| ------ | ------ |
| `ASAN_OPTIONS=abort_on_error=1` | Abort instead of exiting, so `gdb` stops at the error (§3.5) |
| `ASAN_OPTIONS=detect_leaks=0` | Turn off the leak check for this run |
| `UBSAN_OPTIONS=halt_on_error=1` | Stop at the first UB error (§2.3) |
| `UBSAN_OPTIONS=print_stacktrace=1` | Add a backtrace to UBSan errors (by default you only get file:line) |

---

## 3. gdb

### 3.1 Starting it

| Command | Use |
| ------- | --- |
| `gdb ./prog` | The normal start. Then type `run` |
| `gdb --args ./prog in.txt 5` | When the program takes arguments |
| `gdb -tui ./prog` | Text UI: the source code on top, commands below. Easier to follow while stepping |
| `gdb ./prog core-file` | Look at a crash after the fact (§4) |
| `sudo gdb -p <pid>` | Attach to a program that's already running (§4). Needs `sudo` on this machine |

**Typing tips:** pressing **Enter** on an empty line repeats the last command (handy for `next`, `next`, …).
Most commands have a one-letter short form (`b`, `r`, `n`, `s`, `c`, `p`). **Ctrl+C** pauses a running
program and gives you the prompt back. `quit` (or `q`) exits.

### 3.2 Commands

**Look around (after a crash or at a breakpoint):**

| Command | What it does |
| ------- | ------------ |
| `bt` | **Backtrace:** the chain of active calls, from where you stopped (`#0`) up to `main` |
| `frame N` (`f N`) | Select frame `N` from the backtrace; the commands below then refer to that function |
| `up` / `down` | Move one frame towards the caller / back towards `#0` |
| `info locals` | All local variables of the selected frame |
| `info args` | The selected function's arguments |
| `p expr` | Print anything: `p head`, `p *head`, `p head->next->val`, `p v.size()`, `p i * 2` |
| `p *arr@5` | Print 5 elements starting at pointer `arr` (a raw pointer doesn't know its own length) |
| `ptype x` | Show the type of `x` (a struct's members, for example) |
| `list` | Show the source around the current line |

**Control execution:**

| Command | What it does |
| ------- | ------------ |
| `break file.cpp:42` / `break funcName` (`b`) | Stop when that line or function is reached |
| `break file.cpp:42 if i == 7` | Stop only when the condition is true. Great for loops |
| `info breakpoints` / `delete N` | List breakpoints / remove breakpoint `N` |
| `run` (`r`) | Start the program from the beginning |
| `continue` (`c`) | Carry on until the next breakpoint or crash |
| `next` (`n`) | Run the current line; **step over** function calls |
| `step` (`s`) | Run the current line; **step into** a function call |
| `finish` | Run until the current function returns, then show its return value |
| `until N` | Run until line `N` (e.g. jump past the end of a loop) |
| `watch expr` | **Watchpoint:** stop whenever `expr` changes (e.g. `watch total`, `watch arr[3]`) |
| `display expr` | Print `expr` automatically every time the program stops |

**Threads:**

| Command | What it does |
| ------- | ------------ |
| `info threads` | List all threads and where each one is |
| `thread N` | Switch to thread `N` (then `bt`, `frame` and so on apply to it) |
| `thread apply all bt` | Backtrace of **every** thread: the deadlock command (§4) |

### 3.3 A real session

The program: a linked list `1 → 2 → 3 → nullptr`, and a `sumList` that loops `while (head->val != -1)`. It
expects a `-1` sentinel at the end that isn't there. Built with `-g -O0`, it segfaults.

```
$ gdb ./crash
(gdb) run
Program received signal SIGSEGV, Segmentation fault.
0x00005555555551f7 in sumList (head=0x0) at crash.cpp:6
6           while (head->val != -1) {          ← stopped on this line, with head = 0x0 (null)
(gdb) bt
#0  0x00005555555551f7 in sumList (head=0x0) at crash.cpp:6
#1  0x0000555555555277 in main () at crash.cpp:15
(gdb) p *head
Cannot access memory at address 0x0                ← confirms it: dereferencing null
(gdb) frame 1                                      ← go up to the caller
#1  0x0000555555555277 in main () at crash.cpp:15
15          std::cout << sumList(&a) << '\n';
(gdb) info locals
c = {val = 3, next = 0x0}                          ← the last node points at null, and there's no -1 node
b = {val = 2, next = 0x7fffffffd880}
a = {val = 1, next = 0x7fffffffd890}
```

Reading it: frame `#0` shows **what** broke (`head` is null). The caller's data shows **why**: the list ends
in `nullptr`, not in the `-1` the loop waits for. The fix belongs in the loop condition, not in `main`.

### 3.4 You already know this from MATLAB

| MATLAB | gdb | Meaning |
| ------ | --- | ------- |
| `dbstop in file at 42` / clicking the margin | `break file.cpp:42` | Set a breakpoint |
| `dbcont` | `continue` | Run to the next breakpoint |
| `dbstep` | `next` | Step over |
| `dbstep in` | `step` | Step into |
| `dbstep out` | `finish` | Step out |
| `dbstack` / the "Function Call Stack" dropdown | `bt` | The call stack |
| `dbup` / `dbdown` | `up` / `down` | Move between frames |
| hovering over a variable / the Workspace panel | `p x` / `info locals` | Inspect variables |
| `dbquit` | `kill` (stop the program) / `quit` (leave gdb) | Stop debugging |

### 3.5 gdb together with ASan

ASan prints its report and exits on its own, so `gdb` never gets to pause at the bug. To pause there, make
ASan abort instead:

```
$ ASAN_OPTIONS=abort_on_error=1 gdb ./heap_asan
(gdb) run
Program received signal SIGABRT, Aborted.
(gdb) bt
#0  __pthread_kill_implementation (...)           ┐
...                                               │ frames #0–#9 are ASan's own reporting
#8  __asan::ReportGenericError (...)              │ machinery: skip them
#9  __asan::__asan_report_store4 (...)            ┘
#10 0x000055555555645a in main () at heap.cpp:5    ← your code: the bad write
(gdb) frame 10
(gdb) p i                                          ← now inspect the state at the moment of the bug
```

Rule of thumb: **walk down the backtrace until you reach the first line with one of your own files.**

---

## 4. Recipes: symptom → what to do

| Symptom | First move |
| ------- | ---------- |
| **Segfault** (`Segmentation fault (core dumped)`) | Build with `-g -O0`, then `gdb ./prog`, `run`, `bt`, and `frame`/`p` up the stack (§3.3). Or rebuild with ASan, which often names the exact bad access directly |
| **Crash in a strange, unrelated place** | Memory was corrupted earlier, and the crash site is only the victim. **Use ASan**: it stops at the corrupting write itself |
| **Wrong answer, no crash** | Usually it's faster to trace a tiny input by hand or add a print. In `gdb`: `break` at the loop, `break ... if i == k` to skip ahead, `watch` the variable that goes wrong |
| **Hangs (infinite loop)** | `gdb ./prog`, `run`, wait, **Ctrl+C**, `bt` shows where it's spinning. `p` the loop variables |
| **Hangs (deadlock)** | `gdb ./prog`, `run`, **Ctrl+C**, `thread apply all bt`. Each stuck thread shows the `lock` it's waiting in, called from your function (below). For prevention, run it under TSan |
| **Leak** | ASan build: the leak report prints at exit (§2.2). Or `valgrind --leak-check=full ./prog` without rebuilding |
| **"Is this UB?"** | UBSan build plus `halt_on_error=1` (§2.3) |
| **Crashed once and you can't reproduce it** | Load its **core dump** in gdb (below) |

**Deadlock, real output.** Two threads lock `m1`/`m2` in opposite orders. Interrupted with Ctrl+C under `gdb`,
then `thread apply all bt` (trimmed to the frames in my code):

```
Thread 3 ... #7  in b () at dead.cpp:6      ← holds m2, waiting for m1
Thread 2 ... #7  in a () at dead.cpp:5      ← holds m1, waiting for m2
Thread 1 ... #5  in main () at dead.cpp:7   ← waiting in join() for the other two
```

**Attaching to a program that's already running:** `gdb -p <pid>` fails here with
`Could not attach to process ... check the setting of /proc/sys/kernel/yama/ptrace_scope`. Ubuntu only lets
a debugger attach to its own child processes. Use `sudo gdb -p <pid>`, or start the program under `gdb` in
the first place.

**Core dumps on WSL.** Plain Linux writes a `core` file into the current folder (after `ulimit -c unlimited`).
**WSL is different:** it catches every crash and saves the core automatically to Windows, at
`%LOCALAPPDATA%\Temp\wsl-crashes\` (from WSL: `/mnt/c/Users/<you>/AppData/Local/Temp/wsl-crashes/`). The
files are named `wsl-crash-…-<program>-11.dmp`, where 11 is the signal number (SIGSEGV). Despite the `.dmp`
extension, they're normal Linux core files:

```
$ gdb -q ./crash /mnt/c/Users/<you>/AppData/Local/Temp/wsl-crashes/wsl-crash-...-crash-11.dmp
(gdb) bt
#0  0x0000603a913021f7 in sumList (head=0x0) at crash.cpp:6
#1  0x0000603a91302277 in main () at crash.cpp:15
```

Same `bt`, `frame` and `p` as a live session, but you can't step or continue: the program is already dead.
The program file must be the exact build that crashed. That folder keeps growing, so empty it now and then.

---

## 5. valgrind

```
valgrind --leak-check=full ./prog
```

Runs your normal program (no rebuild, though `-g` gives line numbers) on a simulated CPU and checks every
access. Real output for the leak from §2.2:

```
==87056==    at 0x484A2F3: operator new[](unsigned long) (...)
==87056==    by 0x10915E: main (leak.cpp:2)
==87056==    definitely lost: 400 bytes in 1 blocks
```

**ASan or valgrind?** ASan is much faster and catches more stack/global overflows. Valgrind needs no rebuild,
so it works on a program you can't recompile. **Don't combine them**: run valgrind on a build without
sanitizers.

---

## 6. VS Code

The VS Code debugger (C/C++ extension) runs **`gdb` underneath**: the panels show the same frames and
variables as `bt` and `info locals`. Keys: **F9** breakpoint · **F5** start/continue · **F10** step over
(`next`) · **F11** step into (`step`) · **Shift+F11** step out (`finish`). It's fine for everyday use. Learn
the gdb command line too, because servers, core dumps and interviews assume it.

---

## 7. In interviews

**Coding rounds** usually give you no debugger at all. The skill there is tracing a small input by hand,
picking the breaking test case (empty, one element, duplicates, boundaries) and placing one well-chosen print.

**Systems questions** ("how would you debug…?") want a tool-backed workflow. Answer with what you'd actually
run:

| They ask | A good answer names |
| -------- | ------------------- |
| "It segfaults once a day in production." | Core dumps enabled → `gdb prog core` → `bt` → inspect frames. Then reproduce under ASan in a test build |
| "Memory keeps growing." | LSan / valgrind locally; a heap profiler in production (heaptrack, for example). Look for a container that's only ever added to |
| "Two threads hang." | Attach `gdb` → `thread apply all bt` → find the lock cycle. TSan in CI to catch lock-order inversion before it ships |
| "It's wrong only in release builds." | Undefined behaviour that the optimiser exploited → UBSan + ASan builds, `-Wall -Wextra`, compare `-O0` and `-O2` (§1) |

---

## 8. Practice (write these yourself, ~5 lines each)

Each small program should trigger one tool. Predict what the tool will say before you run it.

1. Read one past the end of a `std::vector` with `v[v.size()]` → ASan (heap-buffer-overflow).
2. Return a reference to a local variable and use it → `-Wall` warns at build time
   (`reference to local variable 'x' returned`). **Then a surprise (measured):** g++ replaces that
   reference with **null**, so ASan reports `SEGV on unknown address 0x000000000000`, not a stack error.
   The compiler knew the code was undefined behaviour and turned it into something that crashes for sure.
3. `delete` a pointer twice → ASan (attempting double-free).
4. `int x = INT_MAX; x++;` → UBSan, then make it fatal (§2.3).
5. A linked-list function that segfaults → `gdb`: `run`, `bt`, `frame 1`, `info locals` (§3.3).
6. A loop with a bug at iteration 1000 → `break ... if i == 1000`, then step.
7. Two threads, one shared counter, no mutex → TSan (`setarch -R`).
8. Crash a program, then open its core from `wsl-crashes` (§4).
