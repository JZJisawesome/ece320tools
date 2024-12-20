# ece320tools

"This guy's doing *another* autograder?"

Yep, along with Nick Chan! :)

We didn't make this earlier on in the term because, until now, the golden traces we've been provided have been adequate for testing.

However, especially with PD4 and beyond, dealing with don't care values in the traces can be a bit painful. That's where ece320tools comes in!

ece320tools are in part based off of [IRVE](https://github.com/angry-goose-initiative/irve), the first part of our Linux-capable RISC-V emulator and CPU implementation project. If you're interested in learning more about it, [check out our Github organization here!](https://github.com/angry-goose-initiative)

Whelp hope this helps people out!

\- JZJ

## PD6

### pd6simdiff

An upgraded version of pd5diff meant for comparing your Verilator simulation traces against golden_sim ones.

Doesn't work for post-synthesis/route or actual traces from the board; see `pd6boarddiff` for that.

Example usage:

```bash

$ ./pd6simdiff.sh path/to/golden_sim_trace.trace path/to/your_sim_trace.trace

```

### pd6boarddiff

An upgraded version of pd5diff meant for comparing Xsim post-synthesis/route and FPGA traces against golden_sim ones.

Doesn't work for Verilator simulation traces (expects that your trace only has `[W]` lines).

Also, the provided golden_board traces don't provide enough information for the autograder to properly do it's job.

You must instead provide a golden_sim trace as the first argument (the `[W]` lines of these are exactly the same as the golden_board ones
but also have other lines that are needed for analysis).

Example usage:

```bash

$ ./pd6boarddiff.sh path/to/golden_sim_trace.trace path/to/your_board_trace.trace

```

## PD5

### pd5diff

Based on the PD4 decoder in Rust, `pd5diff` should have minimal (if any) false positives.

While you can invoke cargo directly to use it, I've provided a convenience `pd5diff.sh` script
which will take care of it for you **and enable some compiler optimizations.**

Example usage:

```bash

$ ./pd5diff.sh path/to/golden_trace.trace path/to/your_trace.trace
    Finished `release` profile [optimized] target(s) in 0.02s
     Running `target/release/pd5diff path/to/golden_trace.trace path/to/your_trace.trace`

           _ ____      _ _  __  __ 
 _ __   __| | ___|  __| (_)/ _|/ _|
| '_ \ / _` |___ \ / _` | | |_| |_ 
| |_) | (_| |___) | (_| | |  _|  _|
| .__/ \__,_|____/ \__,_|_|_| |_|  
|_|                                  for ECE 320

pd5diff v0.2.1 by JZJ :)
"Now with colour! Whoop whoop!"

Path to golden trace: path/to/golden_trace.trace
Path to your trace:   path/to/your_trace.trace
Successfully loaded both traces!
Comparing traces...
At least one error on clock cycle #61 containing lines 361 thru 366 (inclusive):
  Golden                                      | Yours
    [F] 010000c0 00a00193                     |   [F] 010000c0 00a00193
    [D] 010000bc 13 1d 1d 00 0 3f 000007fe 1e |   [D] 010000bc 13 1d 1d 00 0 3f 000007fe 1e
    [R] 1d 00 7fffffff 00000000               |   [R] 1d 00 7fffffff 00000000
    [E] 010000b8 80000000 0                   |   [E] 010000b8 80000000 0
    [M] 010000b4 800007fe 0 0 00000000        |   [M] 010000b4 deadbeef 0 0 00000000
    [W] 010000b0 1 01 7fffffff                |   [W] 010000b0 0 01 7fffffff
  Golden Disassembly:
    [F]     is processing instruction @PC 010000c0: addi x3, x0, 10
    [D]/[R] is processing instruction @PC 010000bc: addi x29, x29, 2046
    [E]     is processing instruction @PC 010000b8: lui x29, 524288
    [M]     is processing instruction @PC 010000b4: addi x30, x1, 2047
    [W]     is processing instruction @PC 010000b0: addi x1, x1, -1
  Error(s):
    Error 1: [W] Write enable line does not match!

[...]

Found 1 error(s)!
pd5diff encountered at least one error!

```

Note how `pd5diff` detects that the rd write-enable for the addi in the writeback stage
is 0 when it should be 1, while ignoring the invalid memory address for the addi in the
memory stage because it's a don't care value! (addi doesn't access memory!)

### Autotesting

If you want to automatically check against all of the benchmarks (`.x`) files you've placed in `verif/data`, you can use the `autotest.sh` script, which will automatically simulate all of the benchmarks in your `verif/data` directory, and invoke the autograder to compare them!

Usage:
```bash
$ source pd5autotest.sh <path-to-project-root>
```
e.g. 
```bash
$ source pd5autotest.sh ~/ece_320/rhvisram-pd5
```

Since my pd5 is located at `~/ece_320/rhvisram-pd5`.

To test against all the benchmarks, simply copy the `.x` files in the `rv32-benchmarks` repo into your `verif/data` directory, and invoke `pd5autotest.sh`!

## PD4

### New Rust decoder-based Trace Comparison

This should (hopefully) have less false positives than the IRVE-based trace comparison, since this software decoder is more general purpose than
one just torn out of an emulator.

Simply do `cargo run --bin betterpd4diff path/to/golden_trace.trace path/to/your_trace.trace` from the root of the repo (you must have Rust installed)!

Note the golden trace must be first and your trace must be second.

Example usage:

```bash
cargo run --bin betterpd4diff ~/example_traces/golden_bubble_sort.trace ~/example_traces/our_bubble_sort.trace
   Compiling ece320tools v0.0.0
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.79s
     Running `target/debug/betterpd4diff /home/jzj/example_traces/golden_BubbleSort.trace /home/jzj/example_traces/bad_BubbleSort.trace`
At least one error on line 14:
  Golden: [D] 01000008 37 0f 00 10 0 00 01000000 10
  Yours:  [D] 01000008 37 0e aa 10 0 00 01000000 00
  Golden Disassembly: lui x15, 4096
  Errors:
    Error 1: RDs do not match!
End of error report for line 14.
Done! If you didn't see any errors above, then you (should) be good!
```

Notice how it complains about the rd mismatch, but not about rs1: because lui doesn't have an rs1! :)

### IRVE-based Trace Comparison

Simply use the `pd4tracecompare.sh` script in place of doing a diff. Your trace must be first and the golden trace must be second (opposite of the Rust decoder for some reason haha).

This will build a helper tool called `irvedecoder` that uses code from our emulator to compare the two traces. You need to have a new enough `g++` installed that can compile C++17 code. (Also you need `diff`, `make`, and `bash`, which I'd assume you have...)

As opposed to ignoring don't care values, this tries to replace them as they appear in the golden trace file with more realistic "garbage" values.
So it's possible there may be some false positives, you have been warned...

Example usage:

```bash

$ ./pd4tracecompare.sh path/to/your/pd4_trace.trace path/to/corresponding/golden_trace.trace
Making irvedecoder
~/local_work/ece320/ece320megarepo/ece320tools/irvedecoder ~/local_work/ece320/ece320megarepo/ece320tools
g++ -std=c++17 -fsanitize=address -o irvedecoder decode.cpp main.cpp
[...]

```

### Autotesting

If you want to automatically check against all of the benchmarks (`.x`) files you've placed in `verif/data`, you can use the `autotest.sh` script, which will automatically simulate all of the benchmarks in your `verif/data` directory, and invoke the autograder to compare them!

Usage:
```bash
$ source autotest.sh <path-to-project-root>
```
e.g. 
```bash
$ source autotest.sh ~/ece_320/rhvisram-pd4
```

Since my pd4 is located at `~/ece_320/rhvisram-pd4`.

To test against all the benchmarks, simply copy the `.x` files in the `rv32-benchmarks` repo into your `verif/data` directory, and invoke `autotest.sh`!
