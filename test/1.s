	.text
	.file	"main"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	movl	$1073741824, -104(%rsp) # imm = 0x40000000
	movl	$1, -100(%rsp)
	movl	$0, -96(%rsp)
	movl	$0, -92(%rsp)
	movl	$0, -88(%rsp)
	movl	$0, -84(%rsp)
	xorl	%eax, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2               # -- Begin function f
.LCPI1_0:
	.long	1065353216              # float 1
	.text
	.globl	f
	.p2align	4, 0x90
	.type	f,@function
f:                                      # @f
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -8(%rsp)
	movl	$0, -4(%rsp)
	movss	.LCPI1_0(%rip), %xmm0   # xmm0 = mem[0],zero,zero,zero
	retq
.Lfunc_end1:
	.size	f, .Lfunc_end1-f
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
