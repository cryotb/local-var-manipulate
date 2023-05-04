# local-var-manipulate
This project aims to demonstrate how you can manipulate the local variables of a function from within a CALL it makes. I have built a very simple target function on purpose, in order to make it easier to understand.

**WARNING:** Placement of local variables may differ depending on which architecture and compiler you use. This was tested on 64-bit MSVC. 

# how it works
First let's take a look at the pseudo code for `local_var_user`.
```
void __fastcall local_var_user(bool trigger)
{
  __int64 v1; // rdx
  __int64 v2; // r8
  __int64 v3; // r9
  __int64 TickCount64; // [rsp+20h] [rbp-B8h]
  __int64 v5; // [rsp+20h] [rbp-B8h]
  __int64 v6; // [rsp+28h] [rbp-B0h]
  __int64 v7[16]; // [rsp+30h] [rbp-A8h] BYREF

  printf("------------------\n");
  TickCount64 = GetTickCount64();
  strcpy((char *)TextBuf, "hello, world.");
  local_var_alterer(trigger);
  printf("Time: %lld\n", v5);
  printf("Text: %s\n", (const char *)TextBuf);
  printf("------------------\n");
}
```
We can see that IDA has automatically located our Time and TextBuf LV. <br/>
`Time` is at `RSP+20h` <br/>
`TextBuf` is at `RSP+30h` <br/>

Pretty straight forward, right? Now we are looking at the function doing the actual modification:

```
void local_var_alterer(bool trigger)
{
    if (!trigger) return;
    /* get LVs base of the caller. */
    auto rsp = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + sizeof(uintptr_t);

    /* CAREFUL: don't write more than the LV can hold. */
    *reinterpret_cast<uintptr_t*>(rsp + 0x20) = 1337;
    strcpy(reinterpret_cast<char*>(rsp + 0x30), "i'm evil!");
}
```
Nothing out of the ordinary, it simply uses the above addresses to overwrite the LV contents. <br/>
If trigger is `true`, our alterer function will modify the LV contents and thus it should print the modified contents: <br/>
```
int main()
{
    /* this should print default values. */
    local_var_user(false);

    /* this should print altered values. */
    local_var_user(true);

    return 0;
}
```
Result is:
```
Time: 2133869281 
Text: hello, world. 

Time: 1337 
Text: i'm evil! 

C:\Users\drof\Source\Repos\local_var_manipulate\x64\Release\local_var_manipulate.exe (Prozess "16204") wurde mit Code "0" beendet.
Um die Konsole beim Beenden des Debuggens automatisch zu schließen, aktivieren Sie "Extras" > "Optionen" > "Debuggen" > "Konsole beim Beenden des Debuggings automatisch schließen".
Drücken Sie eine beliebige Taste, um dieses Fenster zu schließen.
```

# use cases
From my experience, there won't be many opportunities where you could be able to utilize this. However, there are still a handful of situations where for example, you have a function that makes use of LVs, calls another function, and then processes those LVs. In that case, you could intercept the call it makes and alter the LVs before they are processed.
