	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	$0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	l___const.main.result(%rip), %edi
	movl	%edi, -24(%rbp)
	movb	l___const.main.result+4(%rip), %al
	movb	%al, -20(%rbp)
	leaq	-24(%rbp), %rdi
	callq	__ZNSt3__15arrayIcLm5EE4dataEv
	leaq	-24(%rbp), %rdi
	movq	%rax, -32(%rbp)         ## 8-byte Spill
	callq	__ZNKSt3__15arrayIcLm5EE4sizeEv
	leaq	L_.str(%rip), %rdi
	movq	-32(%rbp), %rsi         ## 8-byte Reload
	movq	%rax, %rdx
	movb	$0, %al
	callq	_printf
	xorl	%ecx, %ecx
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	movl	%ecx, %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function _ZNSt3__15arrayIcLm5EE4dataEv
__ZNSt3__15arrayIcLm5EE4dataEv:         ## @_ZNSt3__15arrayIcLm5EE4dataEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function _ZNKSt3__15arrayIcLm5EE4sizeEv
__ZNKSt3__15arrayIcLm5EE4sizeEv:        ## @_ZNKSt3__15arrayIcLm5EE4sizeEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$5, %eax
                                        ## kill: def $rax killed $eax
	movq	%rdi, -8(%rbp)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
l___const.main.result:                  ## @__const.main.result
	.ascii	"world"

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%s %ld\n"


.subsections_via_symbols
