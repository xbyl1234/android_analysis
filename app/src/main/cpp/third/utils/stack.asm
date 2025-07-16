.text
.arch arm64v8a
.global get_call_stack
.type get_call_stack,%function
get_call_stack:
    stp x29, x30, [sp, #-0x10]!  // 保存帧指针和返回地址
    stp x19, x20, [sp, #-0x10]!  // 保存被调用者寄存器
    stp x21, x22, [sp, #-0x10]!  // 保存被调用者寄存器

    mov x19, x0                  // X19 保存数组地址
    mov x20, #0                  // 初始化计数器
    mov x21, x29                  // 当前帧指针
    mov x22, x30                  // 当前返回地址

loop:
    cmp x20, #10
    bge done                     // 达到 10 层，退出

    mov x0, x21
    bl check_stack
    cbz x0, done                 // 内存不可读，退出

    xpaclri                      // 清除 PAC

    str x22, [x19, x20, lsl #3]     // 保存返回地址到 stack[i]
    ldr x21, [x21]                 // 获取上一帧指针
    ldr x22, [x21, #8]             // 获取上一帧返回地址

    add x20, x20, #1
    b loop

done:
    mov x0, x20
    ldp x21, x22, [sp], 0x10
    ldp x19, x20, [sp], 0x10
    ldp x29, x30, [sp], 0x10
    ret
.end