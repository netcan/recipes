## BrainFuck Compiler
BrainFuck compiler in Rust procedural macro metaprogramming.

## Usage
The BrainFuck code from [https://fatiherikli.github.io/brainfuck-visualizer/](https://fatiherikli.github.io/brainfuck-visualizer/), see it how to work.

The BrainFuck compiler generates the below code to "Hello World!" at compiler time(metaprogramming), and printing it at run time.

Implementing in Rust procedural macro way.

```rust
extern crate brain_fuck;
use brain_fuck::brain_fuck;
fn main() {
    println!("{}", brain_fuck!("
        ++++++++++[>+++++++>++++++++++
        >+++>+<<<<-]>++.>+.+++++++.
        .+++.>++.<<+++++++++++++++.>.
        +++.------.--------.>+.>.
    "));
}
```

emit asm code:
```nasm
brain_fuck::main::h3aaadb2fa73e0763:
...
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	leaq	l___unnamed_2(%rip), %rax
	movq	%rax, -16(%rbp)
	leaq	_$LT$$RF$T$u20$as$u20$core..fmt..Display$GT$::fmt::h45c22d68a23dc67a(%rip), %rax
...
l___unnamed_6:
	.ascii	"Hello World!\__int128"

	.section	__DATA,__const
	.p2align	3
l___unnamed_2:
	.quad	l___unnamed_6
	.asciz	"\r\000\000\000\000\000\000"
```
