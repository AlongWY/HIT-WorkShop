# 第三章作业
## 3.58
```asm
decode2:
    subq    %rdx, %rsi # %rsi = y - z;
    imulq   %rsi, %rdi # %rdi = (y - z) * x;
    movq    %rsi, %rax # %rax = y - z;
    salq    $63,  %rax # %rax = (y - z) << 63;
    sarq    $63,  %rax # %rax = ((y - z) << 63) >> 63;
    xorq    %rdi, %rax # %rax = (((y - z) << 63) >> 63) ^ ((y - z) * x)
    ret
```
所以翻译为C代码即为:
```c
long decode2(long x, long y, long z) {
    return (((y - z) << 63) >> 63) ^ ((y - z) * x);
}
```

## 3.60
```asm
loop:
    movl  %esi, %ecx # %ecx=n;
    movl  $1,   %edx # %edx=1; --> mask 
    movl  $0,   %eax # %eax=0; --> result
    jmp   .L2
.L3:
    movq  %rdi, %r8  # %r8=x;
    andq  %rdx, %r8  # %r8=x&%rdx; -->x&mask
    orq   %r8,  %rax # %rax=%rax | (x&%rdx); -->result |= x & mask
    salq  %cl,  %rdx # %rdx=%rdx<<(n&0xFF);  -->mask = mask<<(n&0xFF)
.L2:
    testq %rdx, %rdx
    jne   .L3.       # if %rdx!=0 goto L3. -->mask!=0
    rep; ret
```
A:

|  变量   |        寄存器      |
|--------|-------------------|
|   x    |     %rdi,%r8      |
|   n    |     %esi,%ecx     |
|  mask  |       %rdx        |
| result |       %rax        |

B: 
```c
result = 0;
mask = 1;
```

C:
```c
mask != 0;
```

D:
```c
mask = mask << (n&0xFF)
```

E:
```c
result |= x & mask
```

F:
```c
long loop(long x, int n)
{
    long result = 0;
    long mask;
    for(mask = 1;mask != 0;mask = mask << (n&0xFF)){
        result |= x & mask;
    }
    return result;
}
```

## 3.62
```c
/* Enumerated type creates set of constants numbered 0 and upward */
typedef enum { MODE_A, MODE_B, MODE_C, MODE_D, MODE_E } mode_t;

long switch3(long *p1, long *p2, mode_t action) {
    long result = 0;
    switch (action) {
    case MODE_A:result = *p2;
        action = *p1;
        *p2 = action;
        break;
    case MODE_B:result = *p1 + *p2;
        *p1 = result;
        break;
    case MODE_C:*p1 = 59;
        result = *p2;
        break;
    case MODE_D:result = *p2;
        *p1 = result;
        result = 27;
        break;
    case MODE_E:result = 27;
        break;
    default:result = 12;
    }
    return result;
}
```

## 3.64
```asm
store_ele:
    leaq  (%rsi, %rsi, 2), %rax  # %rax = 3 * j
    leaq  (%rsi, %rax, 4), %rax  # %rax = 13 * j
    leaq  %rdi, %rsi             # %rsi = i
    salq  $6, %rsi               # %rsi *= 64
    addq  %rsi, %rdi             # %rdi = 65 * i
    addq  %rax, %rdi             # %rdi = 65 * i + 13 * j
    addq  %rdi, %rdx             # %rdx = 65 * i + 13 * j + k
    movq  A(, %rdx, 8), %rax     # %rax = A + 8 * (65 * i + 13 * j + k)
    movq  %rax, (%rcx)           # *dest = A[65 * i + 13 * j + k]
    movl  $3640, %eax            # sizeof(A) = 3640
    ret
```

A:
```c
&D[i][j][k] = XD + L(i * S * T + j * T + k)
```

B:
```c
S * T == 65                   R = 7
T == 13                  ==>  S = 5
S * T * R * 8 == 3640         T = 13
```

## 3.66
```asm
sum_col:
    leaq   1(, %rdi, 4), %r8        # %r8 = 4 * n + 1
    leaq   (%rdi, %rdi, 2), %rax    # result = 3 * n
    movq   %rax, %rdi               # %rdi = 3 * n
    testq  %rax, %rax
    jle    .L4                      # if %rax <= 0, goto L4
    salq   $3, %r8                  # %r8 = 8 * (4 * n + 1)
    leaq   (%rsi, %rdx, 8), %rcx    # %rcx = A[0][j]
    movl   $0, %eax                 # result = 0
    movl   $0, %edx                 # i = 0
.L3:
    addq   (%rcx), %rax             # result = result + A[i][j]
    addq   $1, %rdx                 # i += 1
    addq   %r8, %rcx                # 这里每次+8*(4n+1),说明每一行有4n+1个，因此NC(n)为4*n+1
    cmpq   %rdi, %rdx
    jne    .L3                      # 这里说明一直循环到3*n才结束，所以可以说明一共有3n行，因此NR(n)为3*n
    rep; ret
.L4:
    movl $0, %eax
    ret
```

根据上述代码中的分析，可以得出

```
NR(n) = 3 * n
NC(n) = 4 * n + 1
```

## 3.68
首先,结构体str2类型的最长单位是long,所以按照8位对齐,str1同样,也是按照8位对齐.
```c
ty
```

再来看汇编代码:

```asm
setVal:
    movslq  8(%rsi), %rax       # %rax = *(q + 8) --> q->t
    # t是int类型,占4个字节,为了不让t与array共占8个字节,所以sizeof(array[B])大于4,因此可得5<=B<=8.
    
    addq    32(%rsi), %rax      # %rax = *(%rsi +32) --> q->u
    # 说明str2的u从第32位开始的,因此t与s占了24个字节,可以将2个s与t并在一起,占满8个字节,剩下的s占据16个字节,因此可得7<=A<=10.
    
    movq    %rax, 184(%rdi)     # *(%rdi + 184) = %rax --> p->y
    # 说明str1的y从第184位开始的,因此184-8<A*B*4<=184
```

根据汇编代码推出的三个公式:

```
5<=B<=8
7<=A<=10
184-8<A*B*4<=184
```
可以算出唯一解为:
```
A=9
B=5
```

## 3.70
A:

|  变量   |偏移|
|--------|----|
|e1.p    |  0 |
|e1.y    |  8 |
|e2.x    |  0 |
|e2.next |  8 |

B:

```
16
```

C:
```asm
proc:
  # 可能是next也可能是y
  movq 8(%rdi), %rax    # %rax = *(up + 8) --> up->e2.next
  
  # %rdx = *(%rax), %rax是一个指针,在e2域中
  movq (%rax), %rdx     # %rdx = *(*(up + 8)) --> *(up->e2.next)

  # %rdx 也是一个指针       %rdx = *(up->e2.next).e1.p
  movq (%rdx), %rdx     # %rdx = *(*(up->e2.next).e1.p)

  # 使用subq, %rdx是一个长整形 *(*(up->e2).next+8)也是一个长整形,所以 *(up->e2.next + 8)是 *(up->e2.next).e1.y
  subq 8(%rax), %rdx    # %rdx -= *(up->e2.next+8) --> %rdx = *(*(up->e2.next).e1.p) - *(up->e2.next).e1.y

  # 通过最后一句可以判断(%rdi)是个长整形即up->e2.x
  movq %rdx, (%rdi)
  ret
```

可以得出C代码为:
```c
union ele {
  struct {
    long *p;
    long y;
  } e1;
  struct {
    long x;
    union ele *next;
  } e2;
};

void proc(union ele *up){
    up->e2.x = *(*(up->e2.next).e1.p) - *(up->e2.next).e1.y;
}
```

## 3.72
```asm
aframe:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $16, %rsp           # 将栈顶地址减小16，为i分配了内存                 %rsp=s1
    leaq    30(,%rdi,8), %rax   # %rax = 8 * n + 30
    andq    $-16, %rax          # %rax = (8 * n + 30) & 0xfffffff0          将后4位置0,成为最小的16的倍数
    subq    %rax, %rsp          # 将栈顶地址减小%rax位，是为数组p分配内存          %rsp=s2
    leaq    15(%rsp), %r8
    andq    $-16, %r8           # 这两句保证了p的地址是16的倍数,取最小的16的倍数，%r8 = &p[0]
```
A:
```c
s2 = s1 - ((8 * n + 30) & 0xfffffff0)
if(n % 2 == 0)
    s2 = s1 - (8 * n + 16)
else
    s2 = s1 - (8 * n + 24)
```
B:
```c
p = (s2 + 15) & 0xfffffff0
```

C:

| 变量   |                  偏移                 |  栈顶标记  |
|-------|---------------------------------------|----------|
|       |   s1 - (8 * n + 30) & 0xfffffff0      |   <= s2  |
|  e2   |                 ?                     |          |
| p[n]  |       (s2 + 15) & 0xfffffff0          |   p[0]   |
|  e1   |                 ?                     |          |
|   i   |                -16                    |   <= s1  |
| %rbp  |                 0                     |          |


|  极值  |   e1   |    n    |     s1       |
|-------|--------|---------|--------------|
|  最小  |    1   |   偶数   |   s1 % 16==1 |
|  最大  |   24   |   奇数   |   s1 % 16==0 |


* 使e1最小,则e2最大,这里是要16倍数的最小值,因此p最小则为某个对16取余为1的值,
这时e2是15,e2不可能大于等于16，使e1+e2的和也最小,则n为偶数时,是8n+16-8n,为16,
因此答案是: e1为16-e2=1,此时n为偶数,s1%16=1。

* 使e1最大,则e2最小,e2最小则为p恰好是16的倍数,此时e2为0。然后使e1+e2的和也最大,
则是n为奇数时,是8n+24-8n,为24。
因此答案是: e1为24-e2=24,此时n为奇数,s1%16=0。

D:
s2保证了能容下 8*n 字节的最小的16的倍数。
p保证了自身对16对齐。