
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <errno.h>

#include <unistd.h>
#include <sys/mman.h>

void func(int number)
{
    printf("Number value: %d\n", number);
}

typedef void (*FUNCPTR_t)(int);

typedef int (*FUNCWITH6PARAMETERS_t)(int x, int y, int z, const char *dest, const char *src, size_t count);

static FUNCPTR_t funcPtr = nullptr;
static FUNCWITH6PARAMETERS_t func6Ptr = nullptr;

void newFunc(int funcNumber)
{
    puts("\e[32mHook is working\e[0m");
    return funcPtr(funcNumber);
}

volatile int funcWith6Parameters(int x, int y, int z, const char *dest, const char *src, size_t count)
{
    printf("X, Y, Z is equal to {%d, %d, %d}\n", x, y, z);
    printf("dest, src, count is equal to {%s, %s, %ld}\n", dest, src, count);

    return x + y + z;
}

int newFuncWith6Parameters(int x, int y, int z, const char *dest, const char *src, size_t count)
{
    puts("\e[32mIntercepting \'funcWith6Parameters\' function\e[0m");
    printf("X, Y, Z is equal to {%d, %d, %d}\n", x, y, z);
    printf("dest, src, count is equal to {%s, %s, %ld}\n", dest, src, count);

    puts("\e[32mCalling the original function\e[0m");

    return func6Ptr(x, y, z, dest, src, count);
}


uintptr_t Hook32Func(uintptr_t targetFunc, uintptr_t NewFunc, int32_t copyContextSize)
{
    assert(targetFunc && NewFunc);

    constexpr int JMPInstFixedSize = 5;

    assert(copyContextSize >= JMPInstFixedSize);

    uint8_t JMPMachineCode[JMPInstFixedSize];

    /* https://c9x.me/x86/html/file_module_x86_id_147.html */
    JMPMachineCode[0] = 0xe9;

    void* trampoline = mmap(nullptr, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    assert(trampoline != MAP_FAILED);

    /* Copyng N bytes from N instructions (with operands) to the trampoline */
    memcpy(trampoline, (void*)targetFunc, copyContextSize);

    /* Finds out where puts the target function address inside the trampoline */
    uintptr_t trampolineJumpLocation = (uintptr_t)trampoline + copyContextSize;
    
    /* Finds out where's jumps from the trampoline to the original function */
    uintptr_t targetJumpLocation = (uintptr_t)targetFunc + copyContextSize;

    /* Insert inside the trampoline a indirect jump to jump inside the original function */
    *(uint32_t*)&JMPMachineCode[1] = targetJumpLocation - trampolineJumpLocation - JMPInstFixedSize;

    /* Copying the JMP x86 instruction inside the trampoline */
    memcpy((void*)(trampolineJumpLocation), JMPMachineCode, sizeof(JMPMachineCode));

    /* Calculate the indirect JMP location from the original function to the new function */
    *(uint32_t*)&JMPMachineCode[1] = NewFunc - targetFunc - JMPInstFixedSize;

    assert((mprotect((void*)(targetFunc & 0xfffff000), sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE | PROT_EXEC)) == 0);

    /* Writing this information inside the original function */
    memcpy((void*)targetFunc, JMPMachineCode, sizeof(JMPMachineCode));

    memset((void*)(targetFunc + JMPInstFixedSize), 0x90, copyContextSize - JMPInstFixedSize);

    printf("Trampoline patch data for (%p) function: ", targetFunc);
    for (int32_t index = 0; index < copyContextSize; index++)
        printf("%#x ", ((uint8_t*)targetFunc)[index]);
    puts("");
    printf("%p: jmp %#x; ", targetFunc, *(uint32_t*)(targetFunc + 1));
    for (int32_t index = 0; index < copyContextSize - JMPInstFixedSize; index++)
        printf("%p: nop; ", targetFunc, ((uint8_t*)targetFunc)[index]);
    puts("");

    return (uintptr_t)trampoline;

}

int main(void)
{
    /* func() has memory stack allocation, 7 instructions are used for all function prologue */
    funcPtr = (FUNCPTR_t)Hook32Func((uintptr_t)func, (uintptr_t)newFunc, 7);

    func6Ptr = (FUNCWITH6PARAMETERS_t)Hook32Func((uintptr_t)funcWith6Parameters, 
        (uintptr_t)newFuncWith6Parameters, 7);

    /* This function has been hooked */
    func(10);

    func(120);

    func(121221);

    funcWith6Parameters(10, 30, 50, "Hey", "What's up?", 10);

    /* We don't need more the hooks */
    munmap((void*)funcPtr, sysconf(_SC_PAGE_SIZE));

    munmap((void*)func6Ptr, sysconf(_SC_PAGE_SIZE));

}

