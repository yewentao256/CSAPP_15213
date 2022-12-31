# BombLab

[TOC]

## Set up Environment

Using a docker container is the simplest way, source from yansongsongsong

`docker run --privileged -d -p 1221:22 --name bomb yansongsongsong/csapp:bomblab`

Then using vscode `remote ssh` to connect with it as we described in datalab

password: `THEPASSWORDYOUCREATED`

## Commands We usually use

```bash
gdb -q bomb      # start debugging
b explode_bomb   # help you break before bomb
stepi            # run into next instruction (stepin)
nexti            # run into next instruction (not stepin the funcs)
disas phase_1    # make binary coding into assembly, helpful
x/s 0x402400     # get the string value in address 0x402400
i registers      # print the register infos 
p $rsp           # print the value of variable
```

## Phase_1

assembler code for function phase_1:

```c
0x000400ee0 <+0>:     sub    $0x8,%rsp
0x000400ee4 <+4>:     mov    $0x402400,%esi         // move 0x402400 to %esi
0x000400ee9 <+9>:     callq  0x401338 <strings_not_equal>  
0x000400eee <+14>:    test   %eax,%eax              // judge if eax == 1
0x000400ef0 <+16>:    je     0x400ef7 <phase_1+23>  // jump if equal/zero
0x000400ef2 <+18>:    callq  0x40143a <explode_bomb>
0x000400ef7 <+23>:    add    $0x8,%rsp
0x000400efb <+27>:    retq
```

`strings_not_equal` compares two strings in register `%rdi`, `%rsi`, then saves 0 in `%rax` if they are same, 1 otherwise. If you are interested, `disas strings_not_equal` for more details.

So this phase is to compare the strings, if they are not the same, bomb.

So we can use `x/s 0x402400` to see the string, that is the answer.

```bash
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

## Phase_2

Dump of assembler code for function phase_2:

```c
0x00400efc <+0>:     push   %rbp
0x00400efd <+1>:     push   %rbx
0x00400efe <+2>:     sub    $0x28,%rsp
0x00400f02 <+6>:     mov    %rsp,%rsi
0x00400f05 <+9>:     callq  0x40145c <read_six_numbers>
// considering read_six_numbers a black box
// after calling this, we find that (%rsp) is the first element we input
// so here we know the first element must be number 1

// what's more, by using `x/8w $rsp` we know the number's relation with rsp
// eg: we input 1 2 3 4 5 6, so 
// x/8w $rsp
// 0x7fffffffe1d0: 0x00000001      0x00000002      0x00000003      0x00000004
// 0x7fffffffe1e0: 0x00000005      0x00000006      0x00401431      0x
0x00400f0a <+14>:    cmpl   $0x1,(%rsp)
0x00400f0e <+18>:    je     0x400f30 <phase_2+52>  // jump if (%rsp) == 1
0x00400f10 <+20>:    callq  0x40143a <explode_bomb>
----------------------------------------------------------
0x00400f15 <+25>:    jmp    0x400f30 <phase_2+52>
0x00400f17 <+27>:    mov    -0x4(%rbx),%eax
0x00400f1a <+30>:    add    %eax,%eax
0x00400f1c <+32>:    cmp    %eax,(%rbx)
// here (%rbx) should equal to the 2 * -0x4(%rbx)
// rbx = rsp + 0x4 when first here
// then(second, third ...), rbx = rbx + 0x4
0x00400f1e <+34>:    je     0x400f25 <phase_2+41> 
0x00400f20 <+36>:    callq  0x40143a <explode_bomb>
0x00400f25 <+41>:    add    $0x4,%rbx
0x00400f29 <+45>:    cmp    %rbp,%rbx
// here is a loop, if rbx == rbp(rsp + 0x18), then quit
// else go to the phase_2 + 27 again
0x00400f2c <+48>:    jne    0x400f17 <phase_2+27>
0x00400f2e <+50>:    jmp    0x400f3c <phase_2+64>
0x00400f30 <+52>:    lea    0x4(%rsp),%rbx
0x00400f35 <+57>:    lea    0x18(%rsp),%rbp
0x00400f3a <+62>:    jmp    0x400f17 <phase_2+27>
0x00400f3c <+64>:    add    $0x28,%rsp
0x00400f40 <+68>:    pop    %rbx
0x00400f41 <+69>:    pop    %rbp
0x00400f42 <+70>:    retq
```

Dump of assembler code for function `read_six_numbers`:

```c
0x0040145c <+0>:     sub    $0x18,%rsp
0x00401460 <+4>:     mov    %rsi,%rdx
0x00401463 <+7>:     lea    0x4(%rsi),%rcx
0x00401467 <+11>:    lea    0x14(%rsi),%rax
0x0040146b <+15>:    mov    %rax,0x8(%rsp)
0x00401470 <+20>:    lea    0x10(%rsi),%rax
0x00401474 <+24>:    mov    %rax,(%rsp)
0x00401478 <+28>:    lea    0xc(%rsi),%r9
0x0040147c <+32>:    lea    0x8(%rsi),%r8
0x00401480 <+36>:    mov    $0x4025c3,%esi
0x00401485 <+41>:    mov    $0x0,%eax
0x0040148a <+46>:    callq  0x400bf0 <__isoc99_sscanf@plt>
// Consider scanf is a black box
// after calling scanf, we find that eax = the number of element we input
// so here we know we should input more than 5 numbers
0x0040148f <+51>:    cmp    $0x5,%eax
0x00401492 <+54>:    jg     0x401499 <read_six_numbers+61>  // jump if eax > 5
0x00401494 <+56>:    callq  0x40143a <explode_bomb>
0x00401499 <+61>:    add    $0x18,%rsp
0x0040149d <+65>:    retq
```

Carefully read all of the codes above, we can know:

- we should input more than five numbers
- the first number should be 1
- numbers[i+i] = numbers[i] * 2

So we get the answer: `1 2 4 8 16 32`

You can also type lots of numbers, that doesn't matter: `1 2 4 8 16 32 64 ...`

## Phase_3

Dump of assembler code for function phase_3:

```c
0x00400f43 <+0>:     sub    $0x18,%rsp
0x00400f47 <+4>:     lea    0xc(%rsp),%rcx
0x00400f4c <+9>:     lea    0x8(%rsp),%rdx
// move $0x4025cf to the second argument of sscanf
// by `x/s 0x4025cf` we get "%d %d", so here we know we should input 2 numbers
0x00400f51 <+14>:    mov    $0x4025cf,%esi
0x00400f56 <+19>:    mov    $0x0,%eax
0x00400f5b <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
// according to phase_2, we know %eax is the number of elements we input
// so we should type more than 1 element, which also validates the %d %d above
// what's more, if you command `x/4w $rsp`, you'll find the elements you input
// at 0x8(%rsp), 0xc(%rsp)
0x00400f60 <+29>:    cmp    $0x1,%eax
0x00400f63 <+32>:    jg     0x400f6a <phase_3+39>
0x00400f65 <+34>:    callq  0x40143a <explode_bomb>
// here the first element should not big than 7
0x00400f6a <+39>:    cmpl   $0x7,0x8(%rsp)
0x00400f6f <+44>:    ja     0x400fad <phase_3+106>
---------------------------------------------------------------
0x00400f71 <+46>:    mov    0x8(%rsp),%eax
// calculate address = 8 * rax + 0x402470, then get the value saved in address
// then jump to the value address, usually used in switch table
// 8 means 8 bytes a unit, so we can use `x/8xg 0x402470` to see the table
// 0x402470:       0x00400f7c      0x00400fb9
// 0x402480:       0x00400f83      0x00400f8a
// 0x402490:       0x00400f91      0x00400f98
// 0x4024a0:       0x00400f9f      0x00400fa6
// so we know the first number we input is used to get to the different branch
0x00400f75 <+50>:    jmpq   *0x402470(,%rax,8)
0x00400f7c <+57>:    mov    $0xcf,%eax      // eax = 207
0x00400f81 <+62>:    jmp    0x400fbe <phase_3+123>
0x00400f83 <+64>:    mov    $0x2c3,%eax     // eax = 707
0x00400f88 <+69>:    jmp    0x400fbe <phase_3+123>
0x00400f8a <+71>:    mov    $0x100,%eax     // eax = 256
0x00400f8f <+76>:    jmp    0x400fbe <phase_3+123>
0x00400f91 <+78>:    mov    $0x185,%eax     // eax = 389
0x00400f96 <+83>:    jmp    0x400fbe <phase_3+123>
0x00400f98 <+85>:    mov    $0xce,%eax      // eax = 206
0x00400f9d <+90>:    jmp    0x400fbe <phase_3+123>
0x00400f9f <+92>:    mov    $0x2aa,%eax     // eax = 682
0x00400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
0x00400fa6 <+99>:    mov    $0x147,%eax     // eax = 327
0x00400fab <+104>:   jmp    0x400fbe <phase_3+123>
0x00400fad <+106>:   callq  0x40143a <explode_bomb>
0x00400fb2 <+111>:   mov    $0x0,%eax
0x00400fb7 <+116>:   jmp    0x400fbe <phase_3+123>
0x00400fb9 <+118>:   mov    $0x137,%eax     // eax = 311
// here we compare the second number we input with %eax
// they should be the same
0x00400fbe <+123>:   cmp    0xc(%rsp),%eax
0x00400fc2 <+127>:   je     0x400fc9 <phase_3+134>
0x00400fc4 <+129>:   callq  0x40143a <explode_bomb>
0x00400fc9 <+134>:   add    $0x18,%rsp
0x00400fcd <+138>:   retq
```

Read all of the codes and comments carefully above, we know:

- we should input two numbers
- the first number is used to goto different branches
- the second number should be the same with the value in different branches

So we get the answer, pick one of them:

```c
0 207
1 311
2 707
3 256
4 389
5 206
6 682
7 327
```

## phase_4

Dump of assembler code for function phase_4:

```c
0x0040100c <+0>:     sub    $0x18,%rsp
0x00401010 <+4>:     lea    0xc(%rsp),%rcx
0x00401015 <+9>:     lea    0x8(%rsp),%rdx
0x0040101a <+14>:    mov    $0x4025cf,%esi  // "%d %d", two numbers
0x0040101f <+19>:    mov    $0x0,%eax
0x00401024 <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
0x00401029 <+29>:    cmp    $0x2,%eax       // validates two numbers
0x0040102c <+32>:    jne    0x401035 <phase_4+41>
// Note: 0x8(%rsp) is the first number, 0xc(%rsp) is the second
// here the first number(unsigned) should not big than 0xe
0x0040102e <+34>:    cmpl   $0xe,0x8(%rsp)
0x00401033 <+39>:    jbe    0x40103a <phase_4+46>   // below or equal(unsigned)
0x00401035 <+41>:    callq  0x40143a <explode_bomb>
0x0040103a <+46>:    mov    $0xe,%edx   // third arg = 14
0x0040103f <+51>:    mov    $0x0,%esi   // second arg = 0
0x00401044 <+56>:    mov    0x8(%rsp),%edi  // first arg = first n we input
0x00401048 <+60>:    callq  0x400fce <func4>
// here the return number should be 0, or it will boom
0x0040104d <+65>:    test   %eax,%eax
0x0040104f <+67>:    jne    0x401058 <phase_4+76>
// here we know the second number should be zero
0x00401051 <+69>:    cmpl   $0x0,0xc(%rsp)
0x00401056 <+74>:    je     0x40105d <phase_4+81>
0x00401058 <+76>:    callq  0x40143a <explode_bomb>
0x0040105d <+81>:    add    $0x18,%rsp
0x00401061 <+85>:    retq
```

Dump of assembler code for function func4:

```c
0x00400fce <+0>:     sub    $0x8,%rsp
0x00400fd2 <+4>:     mov    %edx,%eax
0x00400fd4 <+6>:     sub    %esi,%eax
0x00400fd6 <+8>:     mov    %eax,%ecx
0x00400fd8 <+10>:    shr    $0x1f,%ecx      // shift logical right 31
0x00400fdb <+13>:    add    %ecx,%eax
0x00400fdd <+15>:    sar    %eax            // shift arithmetic right, default 1
0x00400fdf <+17>:    lea    (%rax,%rsi,1),%ecx
0x00400fe2 <+20>:    cmp    %edi,%ecx
0x00400fe4 <+22>:    jle    0x400ff2 <func4+36>
0x00400fe6 <+24>:    lea    -0x1(%rcx),%edx
0x00400fe9 <+27>:    callq  0x400fce <func4>
0x00400fee <+32>:    add    %eax,%eax
0x00400ff0 <+34>:    jmp    0x401007 <func4+57>
0x00400ff2 <+36>:    mov    $0x0,%eax
0x00400ff7 <+41>:    cmp    %edi,%ecx
0x00400ff9 <+43>:    jge    0x401007 <func4+57>
0x00400ffb <+45>:    lea    0x1(%rcx),%esi
0x00400ffe <+48>:    callq  0x400fce <func4>
0x00401003 <+53>:    lea    0x1(%rax,%rax,1),%eax
0x00401007 <+57>:    add    $0x8,%rsp
0x0040100b <+61>:    retq
```

It's too complicated, so we translate the `func4` to python:

```py
# x = edi = first n we input
# esi = 0 at first, edx = 14 at first
def func4(x: int = 0, esi: int = 0, edx: int = 14) -> int:
    result = edx - esi
    ecx = result >> 31
    result = (result + ecx) >> 1
    ecx = result + esi
    if ecx <= x:
        result = 0
        if ecx >= x:
            return result
        else:
            # should not entering here! the returning number can't be 0 any more
            # if x > 7, the program will be here
            result = func4(x=x, esi=ecx+1, edx=edx)
            return 2*result + 1
    else:
        result = func4(x=x, esi=esi, edx=ecx-1)
        return 2*result
```

Read all of the codes and comments above carefully, we know:

- we should input two numbers, the first one should not big than 14
- the second number should be zero
- The result of `func4` must be zero
- if x > 7, the recursive function returns a non-zero number, boom!
- we can easily find that if `x == 7`, the `func4` directly return 0

What's more, we can try cases from 0 to 7, and get the answers(pick one of them):

```c
0 0
1 0
3 0
7 0
```

## phase_5(doing)

Dump of assembler code for function phase_5:

```c
0x00401062 <+0>:     push   %rbx
0x00401063 <+1>:     sub    $0x20,%rsp
0x00401067 <+5>:     mov    %rdi,%rbx
// Canary usage: save %fs:0x28 to 0x18(%rsp) at the beginning
// then xor the 0x18(%rsp) at the end to see if someone attacks the program 
0x0040106a <+8>:     mov    %fs:0x28,%rax
0x00401073 <+17>:    mov    %rax,0x18(%rsp)
0x00401078 <+22>:    xor    %eax,%eax   // eax xor eax = 0
// `string_length` returns the number of characters in a string
// the string pointer is passed through %rdi (the content you input before)
// to see your string, type `x/s $rdi`
// if you are interested, `disas string_length` for more details
// you will find how `\0` works at the end of a string in that `string_length`
0x0040107a <+24>:    callq  0x40131b <string_length>
// so here we know we should input 6 characters
0x0040107f <+29>:    cmp    $0x6,%eax
0x00401082 <+32>:    je     0x4010d2 <phase_5+112>
0x00401084 <+34>:    callq  0x40143a <explode_bomb>
0x00401089 <+39>:    jmp    0x4010d2 <phase_5+112>
----------------------------------------------------------
// Note: rbx is the rdi, namely the element we input before
// movzbl: move byte to long (zero expanding)
// and whatever we input, we only use the final byte
0x0040108b <+41>:    movzbl (%rbx,%rax,1),%ecx
0x0040108f <+45>:    mov    %cl,(%rsp)
0x00401092 <+48>:    mov    (%rsp),%rdx
0x00401096 <+52>:    and    $0xf,%edx
// x/s 0x4024b0 and get 
// "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
0x00401099 <+55>:    movzbl 0x4024b0(%rdx),%edx
0x004010a0 <+62>:    mov    %dl,0x10(%rsp,%rax,1)
0x004010a4 <+66>:    add    $0x1,%rax
0x004010a8 <+70>:    cmp    $0x6,%rax
0x004010ac <+74>:    jne    0x40108b <phase_5+41>
0x004010ae <+76>:    movb   $0x0,0x16(%rsp)
0x004010b3 <+81>:    mov    $0x40245e,%esi
0x004010b8 <+86>:    lea    0x10(%rsp),%rdi
0x004010bd <+91>:    callq  0x401338 <strings_not_equal>
0x004010c2 <+96>:    test   %eax,%eax
0x004010c4 <+98>:    je     0x4010d9 <phase_5+119>
0x004010c6 <+100>:   callq  0x40143a <explode_bomb>
0x004010cb <+105>:   nopl   0x0(%rax,%rax,1)
0x004010d0 <+110>:   jmp    0x4010d9 <phase_5+119>
-------------------------------------------------------
0x004010d2 <+112>:   mov    $0x0,%eax
0x004010d7 <+117>:   jmp    0x40108b <phase_5+41>
0x004010d9 <+119>:   mov    0x18(%rsp),%rax
0x004010de <+124>:   xor    %fs:0x28,%rax
0x004010e7 <+133>:   je     0x4010ee <phase_5+140>
0x004010e9 <+135>:   callq  0x400b30 <__stack_chk_fail@plt>
0x004010ee <+140>:   add    $0x20,%rsp
0x004010f2 <+144>:   pop    %rbx
0x004010f3 <+145>:   retq
```

## The results for copy

```c
Border relations with Canada have never been better.
1 2 4 8 16 32
6 682
7 0
```
