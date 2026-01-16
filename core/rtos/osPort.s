.syntax unified
.cpu cortex-m4
.thumb

.global PendSV_Handler
.global osSchedulerLaunch

.extern currentPt
.extern osSchedulerRRWithSleep

/* ---------------- PendSV_Handler ---------------- */
.type   PendSV_Handler, %function
.thumb_func
PendSV_Handler:
    cpsid   i

    push    {r4-r11}

    ldr     r0, =currentPt
    ldr     r1, [r0]
    str     sp, [r1]

    push    {r0, lr}
    bl      osSchedulerRRWithSleep
    pop     {r0, lr}

    ldr     r1, [r0]
    ldr     sp, [r1]

    pop     {r4-r11}

    cpsie   i
    bx      lr

/* ---------------- osSchedulerLaunch ---------------- */
.type   osSchedulerLaunch, %function
.thumb_func
osSchedulerLaunch:
    ldr     r0, =currentPt
    ldr     r2, [r0]
    ldr     sp, [r2]

    pop     {r4-r11}
    pop     {r12}
    pop     {r0-r3}
    add     sp, sp, #4      /* skip LR */
    pop     {lr}
    add     sp, sp, #4      /* skip xPSR */

    cpsie   i
    bx      lr
