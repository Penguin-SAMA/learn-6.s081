        # 首次启动说明
        # qemu -kernel 会将内核加载到 0x80000000
        # 并使每个 hart（即 CPU）跳转到那里。
        # kernel.ld 使下面的代码
        # 被放置在 0x80000000。

        # qemu -kernel loads the kernel at 0x80000000
        # and causes each hart (i.e. CPU) to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0           # 将stack0的地址加载到sp寄存器
        li a0, 1024*4           # 将立即数4096加载到a0寄存器
        csrr a1, mhartid        # mhartid是特殊寄存器，存放当前cpu的id
        addi a1, a1, 1          # 将寄存器a1的值加1，为了计算栈的偏移量
        mul a0, a0, a1          # 计算 a0 = a0 * a1，指向每个CPU的栈顶地址
        add sp, sp, a0          # 将栈指针sp移动到当前CPU专属的栈顶地址
        # jump to start() in start.c
        call start
spin:
        j spin
