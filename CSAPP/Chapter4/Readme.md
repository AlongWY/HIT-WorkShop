# 第四章
## 4.46
A: 并非如此，当REG为rsp时，这段代码修改了REG的值
B:
```asm
addq $8, REG
movq -8(%rsp), REG
```

## 4.48
```c
void bubble_p(long* data, long count) {
  long *i, *last;
  for (last = data+count-1; last > data; last--) {
    for (i = data; i < last; i++) {
      if (*(i+1) < *i) {
        /* swap adjacent elements */
        long t = *(i+1);
        *(i+1) = *i;
        *i = t;
      }
    }
  }
}
```

使用gcc编译`gcc -S -Og sort.c`获得如下代码：

```asm
	.file	"sort.c"
	.text
	.globl	bubble_p
	.type	bubble_p, @function
bubble_p:
.LFB0:
	.cfi_startproc
	leaq	-8(%rdi,%rsi,8), %rsi
	jmp	.L2
.L3:
	addq	$8, %rax
.L5:
	cmpq	%rsi, %rax
	jnb	.L7
	movq	8(%rax), %rdx
	movq	(%rax), %rcx
	cmpq	%rcx, %rdx
	jge	.L3
	movq	%rcx, 8(%rax)
	movq	%rdx, (%rax)
	jmp	.L3
.L7:
	subq	$8, %rsi
.L2:
	cmpq	%rdi, %rsi
	jbe	.L8
	movq	%rdi, %rax
	jmp	.L5
.L8:
	rep ret
	.cfi_endproc
.LFE0:
	.size	bubble_p, .-bubble_p
	.ident	"GCC: (GNU) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
```

翻译成Y86代码：

```asm
.pos 0
    irmovq stack, %rsp
    call main
    halt
    
/** 测试数据 */
.align 8
data:
  .quad 0x0000000000000004
  .quad 0x0000000000000003
  .quad 0x0000000000000002
data_end:
  .quad 0x0000000000000001

main:
    irmovq data, %rdi
    irmovq data_end, %rsi
    call sort
    ret
    
sort:
    jmp L2
L3:
    irmovq  $8, %r8
	addq	%r8, %rax
L5:
/** cmpq	%rsi, %rax */
    rrmovq  %rax, %r8
    subq    %rsi, %r8 
	
	jge	L7
	mrmovq	8(%rax), %rdx
	mrmovq	(%rax), %rcx
	
/** cmpq	%rcx, %rdx */
    rrmovq  %rdx, %r8
    subq    %rcx, %r8 
	
	jge	L3
	rmmovq	%rcx, 8(%rax)
	rmmovq	%rdx, (%rax)
	jmp	L3
L7:
    irmovq  $8, %r8
	subq	%r8, %rsi
L2:
/** cmpq	%rdi, %rsi */
	rrmovq  %rsi, %r8
    subq    %rdi, %r8 
  
	jle	L8
	rrmovq	%rdi, %rax
	jmp	L5
L8:
	ret

.pos 500
stack:

```

使用条件传送修改代码

```asm
.pos 0
    irmovq stack, %rsp
    call main
    halt
    
/** 测试数据 */
.align 8
data:
  .quad 0x0000000000000004
  .quad 0x0000000000000003
  .quad 0x0000000000000002
data_end:
  .quad 0x0000000000000001

main:
    irmovq data, %rdi
    irmovq data_end, %rsi
    call sort
    ret
    
sort:
    jmp L2
L3:
    irmovq  $8, %r8
	addq	%r8, %rax
L5:
/** cmpq	%rsi, %rax */
    rrmovq  %rax, %r8
    subq    %rsi, %r8 
	
	jge	L7
	mrmovq	8(%rax), %rdx
	mrmovq	(%rax), %rcx
	
/**--------------------*/
/**     此处进行改写      */
    rrmovq  %rdx, %r8
    subq    %rcx, %r8
	
	cmovl	%rcx, 8(%rax)
	cmovl	%rdx, (%rax)
/**---------------------*/
	jmp	L3
L7:
    irmovq  $8, %r8
	subq	%r8, %rsi
L2:
/** cmpq	%rdi, %rsi */
	rrmovq  %rsi, %r8
    subq    %rdi, %r8 
  
	jle	L8
	rrmovq	%rdi, %rax
	jmp	L5
L8:
	ret

.pos 0x200
stack:

```

## 4.50
```c
#include <stdio.h>

long switchv(long idx) {
    long result = 0;
    switch (idx) {
    case 0:result = 0xaaa;
        break;
    case 2:
    case 5:result = 0xbbb;
        break;
    case 3:result = 0xccc;
        break;
    default:result = 0xddd;
    }
    return result;
}

/** 测试数据 */
#define CNT 8
#define MINVAL -1

int main() {
    long vals[CNT];
    int i;
    for (i = 0; i < cnt; ++i) {
        vals[i] = switchv(i + MINVAL);
        printf("idx = %ld,val = 0x%lx\n", i + MINVAL, vals[i]);
    }
    return 0;
}
```

手写Y86

```asm
/** Switchv.ys */
.pos 0
    irmovq stack, %rsp
    call main
    halt

/** 数组，用于查看返回结果 */
array:
    .quad 0x0000000000000000
    .quad 0x0000000000000000
    .quad 0x0000000000000000
    .quad 0x0000000000000000

main:
    # 使用数字 -1,1,3,5测试
    irmovq array,%rcx

    irmovq $-1,%rdi
    call   switchv
    rmmovq %rax,(%rcx)

    irmovq $1,%rdi
    call   switchv
    rmmovq %rax,8(%rcx)

    irmovq $3,%rdi
    call   switchv
    rmmovq %rax,16(%rcx)

    irmovq $5,%rdi
    call   switchv
    rmmovq %rax,24(%rcx)

    ret

table:
    .quad LD
    .quad L0
    .quad L1
    .quad L2
    .quad L3
    .quad L4
    .quad L5

/** %rdi作为参数 */
switchv:
    /** Default */
    irmovq table,%r8
    irmovq $8,%r9
    irmovq $1,%r10

    /** if rdi > 5 */
    irmovq $5,%rax
    rrmovq %rax,%r11
    subq   %rdi,%r11
    jl     addr

    /** if rdi < 0 */
    irmovq $0,%rax
    rrmovq %rax,%r11
    subq   %rdi,%r11
    jg     addr

loop:
    addq   %r9,%r8
    rrmovq %rax,%r11
    subq   %rdi,%r11
    je     addr
    addq   %r10,%rax
    jmp    loop

addr:
    mrmovq (%r8),%r8
    pushq  %r8
    ret

L0:
    irmovq $0xaaa,%rax
    ret
L1:
    jmp LD
L2:
    jmp L5
L3:
    irmovq $0xccc,%rax
    ret
L4:
    jmp LD
L5:
    irmovq $0xbbb,%rax
    ret
LD:
    irmovq $0xddd,%rax
    ret

.pos 0x200
stack:

```


## 4.52

|阶段|                         变量操作情况                         |
|---|------------------------------------------------------------|
| F | icode:ifun = M1[PC];rA:rB=M1[PC+1];valC=M8[PC+2];valP=PC+10|
| D |                            valB=R[rB]                      |
| E |                       valE=valB + valC;set CC              |
| M |                                                            |
| W |                            R[rB]=valE                      |

```hcl
bool instr_valid = icode in
    { INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
        IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ ,IIADDQ };
#                                               ^^^^^^

# Does fetched instruction require a regid byte?
bool need_regids =
	icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ,
		     IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };
#                                       ^^^^^^

# Does fetched instruction require a constant word?
bool need_valC =
    icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ };
#                                                      ^^^^^^

################ Decode Stage    ###################################

## What register should be used as the B source?
word srcB = [
    icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ } : rB;
#                                      ^^^^^^
    icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
    1 : RNONE;  # Don't need register
];

## What register should be used as the E destination?
word dstE = [
    icode in { IRRMOVQ } && Cnd : rB;
    icode in { IIRMOVQ, IOPQ, IIADDQ } : rB;
#                             ^^^^^^
    icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
    1 : RNONE;  # Don't write any register
];

################ Execute Stage   ###################################

## Select input A to ALU
word aluA = [
    icode in { IRRMOVQ, IOPQ } : valA;
    icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
#                                         ^^^^^^
    icode in { ICALL, IPUSHQ } : -8;
    icode in { IRET, IPOPQ } : 8;
    # Other instructions don't need ALU
];

## Select input B to ALU
word aluB = [
    icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL,
        IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
#                            ^^^^^^
    icode in { IRRMOVQ, IIRMOVQ } : 0;
    # Other instructions don't need ALU
];

## Should the condition codes be updated?
bool set_cc = icode in { IOPQ, IIADDQ };
#                              ^^^^^^
```


## 4.54

```hcl
# Is instruction valid?
    bool instr_valid = f_icode in 
        { INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
        IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ };
#                                               ^^^^^^
  
# Does fetched instruction require a regid byte?
    bool need_regids =
        f_icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
                IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };
#                                          ^^^^^^
# Does fetched instruction require a constant word?
    bool need_valC =
        f_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ  };
#                                                            ^^^^^^
## What register should be used as the B source?
    word d_srcB = [
        D_icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ   } : D_rB;
#                                            ^^^^^^
        D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
        1 : RNONE;  # Don't need register
    ];

## Select input A to ALU
    word aluA = [
        E_icode in { IRRMOVQ, IOPQ } : E_valA;
        E_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ  } : E_valC;
#                                               ^^^^^^
        E_icode in { ICALL, IPUSHQ } : -8;
        E_icode in { IRET, IPOPQ } : 8;
        # Other instructions don't need ALU
        ];

## Select input B to ALU
    word aluB = [
        E_icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
                    IPUSHQ, IRET, IPOPQ, IIADDQ  } : E_valB;
#                                        ^^^^^^
        E_icode in { IRRMOVQ, IIRMOVQ } : 0;
        # Other instructions don't need ALU
    ];


## Should the condition codes be updated?
/** bool set_cc = E_icode in { IOPQ, IIADDQ } &&              */
    bool set_cc = ( E_icode == IOPQ || E_icode == IIADDQ )&&
#                                                 ^^^^^^

```

## 4.56

|阶段|                         变量操作情况                         |
|---|------------------------------------------------------------|
| F |        icode:ifun = M1[PC];valC=M8[PC+1];valP=PC+9         |
| D |                                                            |
| E |                       Cnd = Cond(CC,ifun)                  |
| M |                                                            |
| W |                                                            |

更新PC: PC = Cnd?valC:valP

```hcl
## What address should instruction be fetched at
word f_pc = [
	# Mispredicted branch.  Fetch at incremented PC

	# 后向分支取指错误
	M_icode == IJXX && M_ifun != UNCOND && M_valE < M_valA && !M_Cnd : M_valA;
	# 前向分支未取错误
	M_icode == IJXX && M_ifun != UNCOND && M_valE >= M_valA && M_Cnd : M_valE;

	# Completion of RET instruction
	W_icode == IRET : W_valM;
	# Default: Use predicted value of PC
	1 : F_predPC;
];

# Predict next value of PC
word f_predPC = [
	# BBTFNT: This is where you'll change the branch prediction rule
	# 分支跳转
	f_icode == IJXX && f_ifun != UNCOND && f_valC < f_valP : f_valC;
    f_icode == IJXX && f_ifun != UNCOND && f_valC >= f_valP : f_valP;
	f_icode in { IJXX, ICALL } : f_valC;
	1 : f_valP;
];

## Select input A to ALU
word aluA = [
	E_icode in { IRRMOVQ, IOPQ } : E_valA;
	E_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ } : E_valC;
	E_icode in { ICALL, IPUSHQ } : -8;
	E_icode in { IRET, IPOPQ } : 8;
	# 分支跳转
	E_icode in { IJXX } : E_valC;
	# Other instructions don't need ALU
];

## Select input B to ALU
word aluB = [
	E_icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		     IPUSHQ, IRET, IPOPQ } : E_valB;
	E_icode in { IRRMOVQ, IIRMOVQ } : 0;
	# 分支跳转
	E_icode in { IJXX } : 0;
	# Other instructions don't need ALU
];

bool D_bubble =
	# Mispredicted branch
    # 分支选择错误
    (
    (E_icode == IJXX && E_ifun != UNCOND && E_valC < E_valA && !e_Cnd) ||
    (E_icode == IJXX && E_ifun != UNCOND && E_valC >= E_valA && e_Cnd)
    ) ||
	# BBTFNT: This condition will change
	# Stalling at fetch while ret passes through pipeline
	# but not condition for a load/use hazard
	!(E_icode in { IMRMOVQ, IPOPQ } && E_dstM in { d_srcA, d_srcB }) &&
	  IRET in { D_icode, E_icode, M_icode };

# Should I stall or inject a bubble into Pipeline Register E?
# At most one of these can be true.
bool E_bubble =
	# Mispredicted branch
	# 分支选择错误
	(
	(E_icode == IJXX && E_ifun != UNCOND && E_valC < E_valA && !e_Cnd) ||
	(E_icode == IJXX && E_ifun != UNCOND && E_valC >= E_valA && e_Cnd)
	) ||
	# BBTFNT: This condition will change
	# Conditions for a load/use hazard
	E_icode in { IMRMOVQ, IPOPQ } &&
	 E_dstM in { d_srcA, d_srcB};
```
## 4.58

| 阶段 |    popq rA        |       popq2 rA   |
|-----|-------------------|------------------|
|  F  |   valP = PC       |  valP = PC +2    |
|  D  |   valB = R[rsp]   |  valB = R[rsp]   |
|  E  |   valE = valB+8   |  valE = valB-8   |
|  M  |                   |  valM = M8[valE] |
|  W  |   R[rsp] = valE   |  R[rA] = valM    |

```hcl
word f_icode = [
     imem_error : INOP;
     D_icode == IPOPQ : IPOP2;
     1: imem_icode;
 ];
 
bool instr_valid = f_icode in 
     { INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IPOP2 };
 
word f_predPC = [
     f_icode in { IJXX, ICALL } : f_valC;
     ## 1W: Want to refetch popq one time
     f_icode == IPOPQ : f_pc;
     1 : f_valP;
 ];

word d_srcA = [
     D_icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : D_rA;
     D_icode in { IRET } : RRSP;
     1 : RNONE; # Don't need register
 ];

word d_srcB = [
     D_icode in { IOPQ, IRMMOVQ, IMRMOVQ  } : D_rB;
     D_icode in { IPUSHQ, IPOPQ, IPOP2, ICALL, IRET } : RRSP;
     1 : RNONE;  # Don't need register
 ];

word d_dstM = [
     D_icode in { IMRMOVQ, IPOP2 } : D_rA;
     1 : RNONE;  # Don't write any register
 ];
 
word aluA = [
     E_icode in { IRRMOVQ, IOPQ } : E_valA;
     E_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ } : E_valC;
     E_icode in { ICALL, IPUSHQ, IPOP2 } : -8;
     E_icode in { IRET, IPOPQ } : 8;
     # Other instructions don't need ALU
 ];
 
word aluB = [
     E_icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
              IPUSHQ, IRET, IPOPQ, IPOP2 } : E_valB;
     E_icode in { IRRMOVQ, IIRMOVQ } : 0;
     # Other instructions don't need ALU
 ];
 
word mem_addr = [
     M_icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ, IPOP2 } : M_valE;
     M_icode in { IRET } : M_valA;
     # Other instructions don't need address
 ];
 
bool mem_read = M_icode in { IMRMOVQ, IRET, IPOP2 };

bool F_stall =
     # Conditions for a load/use hazard
     E_icode in { IMRMOVQ, IPOP2 } &&
      E_dstM in { d_srcA, d_srcB } ||
     # Stalling at fetch while ret passes through pipeline
     IRET in { D_icode, E_icode, M_icode };
     
bool D_stall = 
     # Conditions for a load/use hazard
     E_icode in { IMRMOVQ, IPOP2 } &&
      E_dstM in { d_srcA, d_srcB };
      
bool D_bubble =
	# Mispredicted branch
	(E_icode == IJXX && !e_Cnd) ||
	# Stalling at fetch while ret passes through pipeline
	# but not condition for a load/use hazard
	!(E_icode in { IMRMOVQ, IPOP2 } && E_dstM in { d_srcA, d_srcB }) &&
	# 1W: This condition will change
	  IRET in { D_icode, E_icode, M_icode };
	  
bool E_bubble =
	# Mispredicted branch
	(E_icode == IJXX && !e_Cnd) ||
	# Conditions for a load/use hazard
	E_icode in { IMRMOVQ, IPOP2 } &&
	 E_dstM in { d_srcA, d_srcB};
```