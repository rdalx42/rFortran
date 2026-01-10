
# rF - RD fortran 

This is a mini fortran clone I'm updating, it compiles to it's own bytecode.
The main goal for this project is to replicate what modern fortran would look like while also making it memory lightweight, with string pooling included!

<img src = "rflogo.png">

# All features:

 - Hybrid stack-based register-based vm (uses registers to perform operations)
 - String pooling
 - O(1) memory access
 - if-else,while loops and scopes
 - Fast variables, where a variable either holds a value to its number value or an index which points to the string pool.
 - Arrays!
 - Enums!


# Notices
 - Concat can only concat strings, not variables which hold strings, concat operations can't be nested: "y" concat "x" concat "z"
 - Arrays can't initialized as empty, there are 256 array slots available, each array having 256 value slots.

```pascal 
program main

var a = 5
var b = 10
var c = 0

if 1==1 or 1!=1 do
    if 5 and 5+5==10 do
        c = 100
    else 
        c = 50
    end
else 
    c = -1
end

list c

var x = 1
var string = "hello, world"
var string2 = string
var string3 = "bye" concat " world!"

list string 
list string2
list string3

do 
    var z = -3.5+x 
    list z
end

list x

var t = 1000000

a=0
while a < t do 
    a=a+1
   
end 

b = [0]
var i = 0 

while i < 256 do 
    if i < 100 do 
        b[i]=i
    else 
        b[i] = "hello" concat ", bye!"
    end 
    i=i+1
end

list b 

enum cargo [
  BIG,
  SMALL,
  MINI,
  MEDIUM
]

var cargo1 = cargo::SMALL
var cargo2 = cargo::BIG
var cargo3 = cargo::BIG

if cargo1 == cargo2 or cargo3==cargo2 do
    list cargo1
end

end program

```

# Bytecode

```
PUSH 5
STORE 0
PUSH 10
STORE 1
PUSH 0
STORE 2
PUSH 1
PUSH 1
OP =
PUSH 1
PUSH 1
OP ~
OR
GOTO_IF_FALSE 1
PUSH 5
PUSH 5
PUSH 5
OP +
PUSH 10
OP =
AND
GOTO_IF_FALSE 3
PUSH 100
STORE 2
GOTO 2
LABEL 3
PUSH 50
STORE 2
LABEL 2
GOTO 0
LABEL 1
PUSH 1
NEG
STORE 2
LABEL 0
LIST 2
PUSH 1
STORE 3
LOADSTRING 0
STORE 4
LOAD 4
STORE 5
LOADSTRING 1
STORE 6
LIST 4
LIST 5
LIST 6
PUSH 3.5
NEG
LOAD 3
OP +
STORE 7
LIST 7
LIST 3
PUSH 1000000
STORE 7
PUSH 0
STORE 0
LABEL 4
LOAD 0
LOAD 7
OP <
GOTO_IF_FALSE 5
LOAD 0
PUSH 1
OP +
STORE 0
GOTO 4
LABEL 5
PUSH 0
LOAD_ARRAY 0
STORE 1
PUSH 0
STORE 8
LABEL 6
LOAD 8
PUSH 256
OP <
GOTO_IF_FALSE 7
LOAD 8
PUSH 100
OP <
GOTO_IF_FALSE 9
LOAD 8
LOAD 8
SET_ARRAY_AT 0
GOTO 8
LABEL 9
LOADSTRING 2
LOAD 8
SET_ARRAY_AT 0
LABEL 8
LOAD 8
PUSH 1
OP +
STORE 8
GOTO 6
LABEL 7
LIST 1
PUSH 1
STORE_ENUM_VALUE 0
PUSH 1
STORE_ENUM_VALUE 1
PUSH 1
STORE_ENUM_VALUE 2
PUSH 1
STORE_ENUM_VALUE 3
PUSH 1
PUSH_ENUM_VALUE 1
STORE 10
PUSH 1
PUSH_ENUM_VALUE 0
STORE 11
PUSH 1
PUSH_ENUM_VALUE 0
STORE 12
LOAD 10
LOAD 11
OP =
LOAD 12
LOAD 11
OP =
OR
GOTO_IF_FALSE 10
LIST 10
LABEL 10

[memory at 2] Type: NUMBER Value: 100
[memory at 4] Type: STRING Value: hello, world POINTER: 0
[memory at 5] Type: STRING Value: hello, world POINTER: 0
[memory at 6] Type: STRING Value: bye world! POINTER: 1
[memory at 7] Type: NUMBER Value: -2.5
[memory at 3] Type: NUMBER Value: 1
[memory at 1] Type: ARRAY (Addr 0) Elements:
[0]: NUMBER: 0
[1]: NUMBER: 1
....
[253]: STRING: hello, bye!
[254]: STRING: hello, bye!
[memory at 10] Type: ENUM_OBJECT (Type 1, Value 1)
Execution time: 49.6773 ms
```

# Speed Benchmark (For variable accessing)

- rFortran (Compiled with optimizations)
  
```Pascal
program speed_benchmark 

var target = 1000000
var i = 0 

while i < target do 
    i=i+1
end

end program 
```

 - 36.9804 ms

- Python

```python
import time

start = time.perf_counter()  
i=0
t=1000000
while i < t:
    
    i=i+1
end = time.perf_counter()
elapsed_ms = (end - start) * 1000 
print(f"ran in: {elapsed_ms:.3f} ms")
```

- ran in: 120.162 ms

# How to run 

 - Requierments
 - C++ 20
 - G++ (I'm pretty sure you can use other compiler, however rF was only tested with G++)

```bash 
git clone [repo_link]
cd path_to_clone/src
g++  runtime/*.cpp lexer/*.cpp compiler/*.cpp parser/*.cpp -Iinclude -o b
# or you can compile it with extra optimizations
g++ runtime/*.cpp lexer/*.cpp compiler/*.cpp parser/*.cpp -Iinclude -O3 -Ofast -funroll-loops -ffast-math -march=native -flto -fomit-frame-pointer -o b
./b # by default, main.rf will be executed
```















