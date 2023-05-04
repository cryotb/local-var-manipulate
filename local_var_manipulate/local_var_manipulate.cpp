#include <iostream>
#include <intrin.h>
#include <Windows.h>

void local_var_alterer(bool trigger)
{
    if (!trigger) return;
    auto rsp = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + sizeof(uintptr_t);

    /* CAREFUL: don't write more than the LV can hold. */
    *reinterpret_cast<uintptr_t*>(rsp + 0x20) = 1337;
    strcpy(reinterpret_cast<char*>(rsp + 0x30), "i'm evil!");
}

/*
*   Sample function which demonstrates usage of LVs.
*/
void local_var_user(bool trigger)
{
    printf("------------------\n");

    /*
    *   the function sets up it's local variables.
    */
    uint64_t time = GetTickCount64();   // [RSP+20h]
    char text[128];                     // [RSP+30h]
    strcpy(text, "hello, world.");

    /*
    *   the function that uses local vars calls into another one.
    */
    local_var_alterer(trigger);

    /*
    *   the function processes it's local vars.
    */
    printf("Time: %lld\n", time);
    printf("Text: %s\n", text);

    printf("------------------\n");
}

int main()
{
    /* this should print default values. */
    local_var_user(false);

    /* this should print altered values. */
    local_var_user(true);

    return 0;
}
