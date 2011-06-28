/* This is the Modula-2 run-time start-off routine.  It's job is to take the */
/* arguments as put on the stack by EXEC, and to parse them and set them up the */
/* way _m_a_i_n expects them. */

.text; .data; .data; .bss

.globl	begtext, begdata, begbss
.text
begtext:
.data
begrom:
.data
begdata:
.bss
begbss:

.globl	m2rtso, hol0, _penviron, _penvp, _fpu_present
.text
m2rtso:
	xorl	%ebp, %ebp	/* clear for backtrace of core files */
	movl	(%esp), %eax	/* argc */
	leal	4(%esp), %edx	/* argv */
	leal	8(%esp,%eax,4), %ecx	/* envp */

/* Test if environ is in the initialized data area and is set to our */
/* magic number.  If so then it is not redefined by the user. */
	movl	$_environ, %ebx
	cmpl	$__edata, %ebx	/* within initialized data? */
	jae	0f
	testb	$3, %bl	/* aligned? */
	jne	0f
	cmpl	$0x53535353, (%ebx)	/* is it our environ? */
	jne	0f
	movl	%ebx, __penviron	/* _penviron = &environ; */
0:
	movl	__penviron, %ebx
	movl	%ecx, (%ebx)	/* *_penviron = envp; */

	push	%ecx	/* push envp */
	push	%edx	/* push argv */
	push	%eax	/* push argc */

/* Test the EM bit of the MSW to determine if an FPU is present and */
/* set __fpu_present if one is found. */
	smsw	%ax
	testb	$0x4, %al	/* EM bit in MSW */
	seteb	__fpu_present	/* True if not set */

	call	_m_a_i_n	/* run Modula-2 program */

	push	%eax	/* push exit status */
	call	_exit

	hlt	/* force a trap if exit fails */

.data
.long	0	/* Separate I&D: *NULL == 0 */
/* Also keeps the first string in the */
/* program from appearing at location 0! */
.data
_penviron:
.long	__penvp	/* Pointer to environ, or hidden pointer */

.bss
.lcomm	__penvp, 4	/* Hidden environment vector */
.lcomm	__fpu_present, 4	/* FPU present flag */

.extern	endtext	/* Force loading of end labels. */
