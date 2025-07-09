#include "kernel/types.h"
#include "kernel/param.h"

// 保存内存上下文，用于进程切换和中断处理。
// Saved registers for kernel context switches.
struct context {
    uint64 ra; // 返回地址寄存器，调用函数时保存返回地址
    uint64 sp; // 栈指针，保存当前栈顶地址

    // callee-saved: 被调用者保存的寄存器
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

// 每个CPU的状态信息
// Per-CPU state.
struct cpu {
    struct proc* proc;      // 当前运行的进程（若无则为null)
    struct context context; // 用于进入scheduler()时的上下文切换
    int noff;               // push_off()调用深度(中断禁用计数)
    int intena;             // 中断在push_off()前的状态
};

extern struct cpu cpus[NCPU];

// 用户到内核的上下文切换
// trap 处理代码（trampoline.S）中每个进程的数据。
// 位于用户页表中 trampoline 页正下方的独立页中。
// 在内核页表中没有特殊映射。
// trampoline.S 中的 uservec 会将用户寄存器保存到 trapframe，
// 然后从 trapframe 的 kernel_sp、kernel_hartid、kernel_satp 初始化寄存器，并跳转到 kernel_trap。
// usertrapret() 和 trampoline.S 中的 userret 设置 trapframe 的 kernel_*，
// 从 trapframe 恢复用户寄存器，切换到用户页表，并进入用户空间。
// trapframe 包含像 s0-s11 这样的被调用者保存的用户寄存器
// 因为通过 usertrapret() 返回用户的路径不会经过整个内核调用栈。
struct trapframe {
    /*   0 */ uint64 kernel_satp;   // 内核页表
    /*   8 */ uint64 kernel_sp;     // 内核栈指针
    /*  16 */ uint64 kernel_trap;   // 用户trap入口地址(usertrap函数)
    /*  24 */ uint64 epc;           // 用户态程序计数器（PC），即下一条指令地址
    /*  32 */ uint64 kernel_hartid; // CPU核心编号
    /*  40 */ uint64 ra;            // 返回地址
    /*  48 */ uint64 sp;            // 栈指针
    /*  56 */ uint64 gp;            // 全局指针
    /*  64 */ uint64 tp;            // 线程指针
    /*  72 */ uint64 t0;            // 临时指针
    /*  80 */ uint64 t1;
    /*  88 */ uint64 t2;
    /*  96 */ uint64 s0; // 保存寄存器
    /* 104 */ uint64 s1;
    /* 112 */ uint64 a0; // 函数参数寄存器
    /* 120 */ uint64 a1;
    /* 128 */ uint64 a2;
    /* 136 */ uint64 a3;
    /* 144 */ uint64 a4;
    /* 152 */ uint64 a5;
    /* 160 */ uint64 a6;
    /* 168 */ uint64 a7;
    /* 176 */ uint64 s2; // 额外保存寄存器
    /* 184 */ uint64 s3;
    /* 192 */ uint64 s4;
    /* 200 */ uint64 s5;
    /* 208 */ uint64 s6;
    /* 216 */ uint64 s7;
    /* 224 */ uint64 s8;
    /* 232 */ uint64 s9;
    /* 240 */ uint64 s10;
    /* 248 */ uint64 s11;
    /* 256 */ uint64 t3; // 临时寄存器
    /* 264 */ uint64 t4;
    /* 272 */ uint64 t5;
    /* 280 */ uint64 t6;
};

// 进程状态
enum procstate { UNUSED,
                 USED,
                 SLEEPING,
                 RUNNABLE,
                 RUNNING,
                 ZOMBIE };

// 进程的数据结构
// Per-process state
struct proc {
    struct spinlock lock; // 自旋锁保护该进程数据结构

    // p->lock must be held when using these:
    enum procstate state; // 进程状态
    void* chan;           // 等待的事件(channel)
    int killed;           // 是否被终止
    int xstate;           // 退出状态
    int pid;              // 进程 ID

    // wait_lock must be held when using this:
    struct proc* parent; // 父进程
    int sys_trace_mask;

    // 以下是进程私有，无须锁保护
    uint64 kstack;               // 内核栈虚拟地址
    uint64 sz;                   // 进程内存大小
    pagetable_t pagetable;       // 进程页表
    struct trapframe* trapframe; // trapframe页，用于用户内核切换
    struct context context;      // 上下文，用于用户内核切换
    struct file* ofile[NOFILE];  // 打开的文件描述符
    struct inode* cwd;           // 当前工作目录
    char name[16];               // 进程名
};
