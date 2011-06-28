/* This is the C run-time start-off routine.  It's job is to take the */
/* arguments as put on the stack by EXEC, and to parse them and set them up the */
/* way _main expects them. */
/* It also initializes environ when this variable isn't defined by the */
/* programmer.  The detection of whether environ belong to us is rather */
/* simplistic.  We simply check for some magic value, but there is no other */
/* way. */

#include <machine/asm.h>
#include <machine/vm.h>

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

#if defined(__ELF__)
.section .init
#else
.text
#endif

ENTRY(_start)
_crtso:
	xorl	%ebp, %ebp	/* clear for backtrace of core files */
	movl	(%esp), %eax	/* argc */
	leal	4(%esp), %edx	/* argv */
	leal	8(%esp,%eax,4), %ecx	/* envp */

/* Test if environ is in the initialized data area and is set to our */
/* magic number.  If so then it is not redefined by the user. */
	movl	$_C_LABEL(environ), %ebx
	cmpl	$_edata, %ebx	/* within initialized data? */
	jae	0f
	testb	$3, %bl	/* aligned? */
	jne	0f
	cmpl	$0x53535353, (%ebx)	/* is it our environ? */
	jne	0f
	movl	%ebx, _C_LABEL(_penviron)	/* _penviron = &environ; */
0:
	movl	_C_LABEL(_penviron), %ebx
	movl	%ecx, (%ebx)	/* *_penviron = envp; */

	/* Save argv[] and argc. This is so that we can return
	 * argv[0] in the future, without having to check whether
	 * argv can be dereferenced safely now.
	 */
	mov	%edx, (_C_LABEL(__prognamep))
	mov	%eax, (_C_LABEL(__argc))

	push	%ecx	/* push envp */
	push	%edx	/* push argv */
	push	%eax	/* push argc */

	jmp	__minix_mainjump

.balign I386_PAGE_SIZE
__minix_mainjump:
	/* unmap zero pages */
	call	_C_LABEL(_minix_unmapzero)

	call	_C_LABEL(main)	/* main(argc, argv, envp) */

	push	%eax	/* push exit status */
	call	_C_LABEL(exit)

	hlt	/* force a trap if exit fails */

ENTRY(_minix_unmapzero)

	/* unmap 0-page code */
	push	$I386_PAGE_SIZE
	push	$_crtso
	call	_C_LABEL(munmap_text)		/* unmap_text(crtso, I386_PAGE_SIZE) */
	add	$8, %esp

#ifdef __ACK__
	/*
	 * ack uses separate segments for text and data by default. We have a
	 * common segment when compiling using any other compiler
	 */

	/* unmap 0-page data */
	push	$I386_PAGE_SIZE
	push	$romstart
	call	_C_LABEL(munmap)			/* munmap(romstart, I386_PAGE_SIZE) */
	add	$8, %esp
#endif

	ret

#ifdef __ACK__
.rom 
romstart:
.space	I386_PAGE_SIZE
ENTRY(__minix_datastart)
.long	4
#endif
.data
ENTRY(_penviron)
.long	__penvp	/* Pointer to environ, or hidden pointer */
.bss
ENTRY(__prognamep)
.space	4
ENTRY(__argc)
.space	4
.lcomm	__penvp, 4	/* Hidden environment vector */

.extern	endtext	/* Force loading of end labels. */
