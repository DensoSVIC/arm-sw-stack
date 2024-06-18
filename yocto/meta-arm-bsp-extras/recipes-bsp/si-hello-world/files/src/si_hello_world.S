/*
 * SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: MIT
 */

#define UARTDR 0x000
#define UARTFR 0x018
#define UARTCR 0x030

#define PL011_UARTFR_TXFF_BIT 5         /* Transmit FIFO full bit in UARTFR register */
#define PL011_UARTCR_TXE      (1 << 8)  /* Transmit enable */
#define PL011_UARTCR_UARTEN   (1 << 0)  /* UART Enable */

#define UART_BASE 0x2a400000

.global _start
_start:
    /* Inspect CPU affinity register */
    mrs x0, MPIDR_EL1
    ubfx x1, x0, #8, #8
    /* Only use UART from core 0 */
    cbnz x1, loop

    /* Calculate UART address from CPU affinity */
    ubfx x1, x0, #0, #24
    mov x2, #(UART_BASE)
    add x0, x1, x2

    /* Enable tx and uart overall */
    mov	w1, #(PL011_UARTCR_TXE | PL011_UARTCR_UARTEN)
    str	w1, [x0, #UARTCR]

    /* Load address of output label */
    adr x3, output
loadchar:
    /* Load quad at address */
    ldr x1, [x3]
putc:
    /* Check if the transmit FIFO is full */
    ldr	w2, [x0, #UARTFR]
    tbnz	w2, #PL011_UARTFR_TXFF_BIT, putc
    /* Mask MSB */
    ubfx w4, w1, #0, #8
    /* If null-byte, we've finished so start looping */
    cbz w4, loop
    /* Write data */
    str	w4, [x0, #UARTDR]
    /* Shift quad for next iteration */
    mov x1, x1, lsr 8
    cbnz x1, putc
    /* If no more data in quad, loadchar again */
    add x3, x3, #8
    b loadchar

loop:
    wfi
    b loop

.align 8
output:
    .string "Hello World\r\n"
