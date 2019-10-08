# This is the handout directory for the CS:APP Cache Lab.
## Running the autograders:
Before running the autograders, compile your code:

```bash
    linux> make
```

Check the correctness of your simulator:

```bash
    linux> ./test-csim
```

Check the correctness and performance of your transpose functions:

```bash
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67
```

Check everything at once (this is the program that your instructor runs):

```bash
    linux> ./driver.py
```


## Files:


### You will modifying and handing in these two files

|Filename|Function|
|--------|--------|
|csim.c  |    Your cache simulator|
|trans.c |    Your transpose function|

### Tools for evaluating your simulator and transpose function

|Filename|Function|
|--------|--------|
|Makefile     |Builds the simulator and tools|
|README       |This file|
|driver.py*   |The driver program, runs test-csim and test-trans|
|cachelab.c   |Required helper functions|
|cachelab.h   |Required header file|
|csim-ref*    |The executable reference cache simulator|
|test-csim*   |Tests your cache simulator|
|test-trans.c |Tests your transpose function|
|tracegen.c   |Helper program used by test-trans|
|traces/      |Trace files used by test-csim.c|
