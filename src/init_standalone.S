
	.section .init_code, "ax"
	.arm

	.extern test_main

	.global start
	.global stack_top

@--------------------------------------

  .func start_init_s

start:
	stmfd   sp!, {r0, r1, r2, lr}

	@ set own stack position
	ldr     r0, =stack_top
	mov     sp, r0

	@ clear .bss section (zero init)
	mov     r0, #0
	ldr     r1, =__bss_start__
	ldr     r2, =__bss_end__
clear_bss:
	cmp     r1, r2
	strlo   r0, [r1], #4
	blo     clear_bss

	b       test_main

  .endfunc

@--------------------------------------

  .end

