---
title: 第七章
author: 冯云龙
date: 2017/12/27
geometry: margin=2cm
documentclass: ctexart
output: pdf_document
---

# 7.6

|符号|swap.o.symtab条目?|符号类型|定义符号的模块|节|
|----|----------------|-------|------------|----|
|buf  |     是        |  external |   m        |.data|
|bufp0|     是        |  global   |   swap     |.data|
|bufp1|     是        |  local    |   swap     |.bss |
|swap |     是        |  global   |   swap     |.text|
|temp |     否        |     *     |     *      |  *  |
|incr |     是        |    local  |    swap    |.text|
|count|     是        |    local  |    swap    |.bss |

# 7.8
A.  (a) REF(main.1)->DEF(main.1)
    (b) REF(main.2)->DEF(main.2)

B.  (a) REF(x.1)->DEF(未知)
    (b) REF(x.2)->DEF(未知)

C.  (a) REF(x.1)->DEF(错误)
    (b) REF(x.2)->DEF(错误)

# 7.10
A. `linux> gcc p.o libx.a p.0`  
B. `linux> gcc p.o libx.a liby.a libx.a`  
C. `linux> gcc p.o libx.a liby.a libx.a libz.a`

# 7.12
A.
```
ADDR(s) = ADDR(.text) = 0x4004e0
ADDR(r.symbol) = ADDR(swap) = 0x4004f8
refaddr = ADDR(s) + r.offset = 0x4004ea
*refptr = (unsigned) (ADDR(r.symbol) + r.addend - refaddr) = 0xa
```

B.
```
ADDR(s) = ADDR(.text) = 0x4004d0
ADDR(r.symbol) = ADDR(swap) = 0x400500
refaddr = ADDR(s) + r.offset = 0x4004da
*refptr = (unsigned) (ADDR(r.symbol) + r.addend - refaddr) = 0x22
```