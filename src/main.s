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
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$32, %rsp
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movl	$1, %edi
	callq	setLogLevel
	cmpl	$2, -36(%rbp)
	jl	.LBB0_2
# %bb.1:                                # %ifThen
	movq	%rsp, %r14
	leaq	-16(%r14), %rsp
	movq	%rsp, %rax
	leaq	-16(%rax), %rsp
	movq	-48(%rbp), %rcx
	movq	8(%rcx), %rcx
	movq	%rcx, -16(%rax)
	movq	%rsp, %r15
	leaq	-16(%r15), %rsp
	movq	-16(%rax), %rdi
	callq	readFile
	movq	%rax, -16(%r15)
	movq	%rsp, %r12
	leaq	-16(%r12), %rsp
	movq	-16(%r15), %rdi
	callq	lex
	movq	%rax, -16(%r12)
	movq	%rsp, %rbx
	leaq	-16(%rbx), %rsp
	movq	-16(%r12), %rdi
	callq	parse
	movq	%rax, -16(%rbx)
	movq	%rax, %rdi
	callq	resolveTypes
	movq	-16(%rbx), %rdi
	callq	codeGenLLVM
	movq	-16(%rbx), %rdi
	callq	delModule
	movq	-16(%r12), %rdi
	callq	delTokens
	movq	-16(%r15), %rdi
	callq	memFree
	callq	memLog
	movl	$0, -16(%r14)
.LBB0_2:                                # %ifExit
	movl	$0, -52(%rbp)
	xorl	%eax, %eax
	leaq	-32(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa %rsp, 8
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
