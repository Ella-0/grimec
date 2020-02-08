	.text
	.file	"grimec_main"
	.globl	grimec_main_main        # -- Begin function grimec_main_main
	.p2align	4, 0x90
	.type	grimec_main_main,@function
grimec_main_main:                       # @grimec_main_main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %rbp, -16
	movq	%rsi, %rbx
	movl	%edi, %ebp
	movl	$1, %edi
	callq	setLogLevel
	cmpl	$2, %ebp
	jl	.LBB0_2
# %bb.1:                                # %ifThen
	movq	8(%rbx), %rdi
	callq	readFile
	movq	%rax, %r14
	movq	%rax, %rdi
	callq	lex
	movq	%rax, %rbp
	movq	%rax, %rdi
	callq	parse
	movq	%rax, %rbx
	movq	%rax, %rdi
	callq	resolveTypes
	movq	%rbx, %rdi
	callq	codeGenLLVM
	movq	%rbx, %rdi
	callq	delModule
	movq	%rbp, %rdi
	callq	delTokens
	movq	%r14, %rdi
	callq	memFree
	callq	memLog
.LBB0_2:                                # %ifExit
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	grimec_main_main, .Lfunc_end0-grimec_main_main
	.cfi_endproc
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	callq	grimec_main_main
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
