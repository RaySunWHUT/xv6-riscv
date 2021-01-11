// Saved registers for kernel context switches.
// 保存内核上下文切换寄存器。
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved：调用保存
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

// Per-CPU state.
// 每个CPU的状态
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.：此CPU运行的进程，或者该CPU为空(NULL)
  struct context context;     // swtch() here to enter scheduler().：此处，swtch() --> scheduler()
  int noff;                   // Depth of push_off() nesting.：push_off()的嵌套深度
  int intena;                 // Were interrupts enabled before push_off()?：在 push_off()之前是否启用了中断 ?
};

extern struct cpu cpus[NCPU];

/* 
  per-process data for the trap handling code in trampoline.S.
 sits in a page by itself just under the trampoline page in the
 user page table. not specially mapped in the kernel page table.
 the sscratch register points here.
 uservec in trampoline.S saves user registers in the trapframe,
 then initializes registers from the trapframe's
 kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
 usertrapret() and userret in trampoline.S set up
 the trapframe's kernel_*, restore user registers from the
 trapframe, switch to the user page table, and enter user space.
 the trapframe includes callee-saved user registers like s0-s11 because the
 return-to-user path via usertrapret() doesn't return through
 the entire kernel call stack.


 */
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table：内核页表
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack：进程的内核栈顶
  /*  16 */ uint64 kernel_trap;   // usertrap()：用户陷阱(trap)
  /*  24 */ uint64 epc;           // saved user program counter：保存的用户程序计数器
  /*  32 */ uint64 kernel_hartid; // saved kernel tp：保存的内核陷阱(trap)
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
// 每个进程的状态
struct proc {

  struct spinlock lock;

  // p->lock must be held when using these:
  // 使用以下属性时，必须保证 p->lock 锁定
  enum procstate state;        // Process state：进程状态
  void *chan;                  // If non-zero, sleeping on chan：若非0，chan为sleep状态
  int killed;                  // If non-zero, have been killed：若非0，则进程已被杀死
  int xstate;                  // Exit status to be returned to parent's wait：退出状态返回到父等待状态
  int pid;                     // Process ID：进程Id

  // proc_tree_lock must be held when using this:
  // 使用此属性时，必须保持进程树锁定
  struct proc *parent;         // Parent process: 父进程

  // these are private to the process, so p->lock need not be held.
  // 以下属性为进程私有，故不需要保持 p-> lock 锁定
  uint64 kstack;               // Virtual address of kernel stack：内核堆栈的虚拟地址
  uint64 sz;                   // Size of process memory (bytes)：进程内存大小(字节)
  pagetable_t pagetable;       // User page table：用户页表
  struct trapframe *trapframe; // data page for trampoline.S：trampoline.S的数据页
  struct context context;      // swtch() here to run process：swtch()在这里运行进程
  struct file *ofile[NOFILE];  // Open files：打开的文件
  struct inode *cwd;           // Current directory：当前目录
  char name[16];               // Process name (debugging)：进程名
};
