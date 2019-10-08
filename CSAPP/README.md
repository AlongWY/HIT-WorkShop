# 深入理解计算机课后作业

编译命令如下：
```bash
mkdir out
cd out
cmake ..
make
```
在主文件夹 `bin` 目录下生成可执行文件。

## 作业

转换命令：`pandoc filename.md -o filename.pdf --pdf-engine=xelatex -V CJKmainfont=WenQuanYi\ Micro\ Hei\ Mono`
latex编译命令： `latexmk --shell-escape -xelatex 实验报告.tex`

## 实验一
showbytes 使用说明：
```bash
.\showbyytes filename
```

生成中间文件
```bash
Gcc –E hello.c –o hello.i  #\item 预处理，生成预编译文件（.i文件）：
Gcc –S hello.i –o hello.s  #编译，生成汇编代码（.s文件）：
Gcc –c hello.s –o hello.o  #汇编，生成目标文件（.o文件）：
Gcc hello.o –o hello.out   #链接，生成可执行文件：
```

## 实验二
使用位运算实现各种功能。

## 实验三
二进制炸弹，使用正确的输入以解除炸弹从。

## 实验四
缓存溢出攻击。

## 实验五
Cache模拟器

## 实验六
TinyShell:小型Shell编写

## 大作业
SEQ处理器，Verilog实现
