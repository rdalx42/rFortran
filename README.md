
# rF - RD fortran 

<br>

> You don't need a Garbage Collector, everything is pre allocated :3

<br>

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
 - Standard Lib!


# Notices
 - Concat can only concat strings, not variables which hold strings, concat operations can't be nested: "y" concat "x" concat "z"
 - Arrays can't initialized as empty, there are 256 array slots available, each array having 256 value slots.
 - There are 20 valid enum slots, each enum can have at most 20 elements in it
 - Nested function calls aren't allowed; e.g f(x(2+2))

```pascal 

program main 

enum cargo [
  BIG,
  SMALL,
  MINI,
  MEDIUM
]

var Delta = [Time()]
var i = 0 

while i < 1000000 do 
    i=i+1
end

List(Delta)

Delta[1]=Time()

List("Runtime: ",Delta[1]-Delta[0],"ms")
List("delta array : " , Delta)

var rand_number = RandRange(1,500)
List("rand: ",rand_number)


var cargo1 = cargo::SMALL
var cargo2 = Delta
var cargo3 = cargo::BIG
List(cargo1, "  " , cargo3)

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
List(c)

var ilist = [0]
var ilistindex = 0

while ilistindex<10 do 
    var InputChar = GetChar()
    
    ilist[ilistindex]=InputChar
    ilistindex=ilistindex+1
end

List(ilist)
var it = GetType(ilist)
var it2 = GetType(ilist[0])

List(it , "  ", it2)

end program 
```

# Bytecode

```
PUSH 1
STORE_ENUM_VALUE 0
PUSH 1
STORE_ENUM_VALUE 1
PUSH 1
STORE_ENUM_VALUE 2
PUSH 1
STORE_ENUM_VALUE 3
CALL_BUILTIN 1
LOAD_ARRAY 0
STORE 1
PUSH 0
STORE 2
LABEL 0
LOAD 2
PUSH 1000000
OP <
GOTO_IF_FALSE 1
LOAD 2
PUSH 1
OP +
STORE 2
GOTO 0
LABEL 1
LOAD 1
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
CALL_BUILTIN 1
PUSH 1
SET_ARRAY_AT 0
LOADSTRING 0
PUSH_PARAM
PUSH 1
LOAD_ARRAY_AT 0
PUSH 0
LOAD_ARRAY_AT 0
OP -
PUSH_PARAM
LOADSTRING 1
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
LOADSTRING 2
PUSH_PARAM
LOAD 1
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
PUSH 1
PUSH_PARAM
PUSH 500
PUSH_PARAM
CALL_BUILTIN 2
STORE 3
LOADSTRING 3
PUSH_PARAM
LOAD 3
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
PUSH 1
PUSH_ENUM_VALUE 1
STORE 4
LOAD 1
STORE 5
PUSH 1
PUSH_ENUM_VALUE 0
STORE 6
LOAD 4
PUSH_PARAM
LOADSTRING 4
PUSH_PARAM
LOAD 6
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
PUSH 0
STORE 7
PUSH 1
PUSH 1
OP =
PUSH 1
PUSH 1
OP ~
OR
GOTO_IF_FALSE 3
PUSH 5
PUSH 5
PUSH 5
OP +
PUSH 10
OP =
AND
GOTO_IF_FALSE 5
PUSH 100
STORE 7
GOTO 4
LABEL 5
PUSH 50
STORE 7
LABEL 4
GOTO 2
LABEL 3
PUSH 1
NEG
STORE 7
LABEL 2
LOAD 7
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
PUSH 0
LOAD_ARRAY 1
STORE 8
PUSH 0
STORE 9
LABEL 6
LOAD 9
PUSH 10
OP <
GOTO_IF_FALSE 7
CALL_BUILTIN 3
STORE 10
LOAD 10
LOAD 9
SET_ARRAY_AT 1
LOAD 9
PUSH 1
OP +
STORE 9
GOTO 6
LABEL 7
LOAD 8
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0
LOAD 8
PUSH_PARAM
CALL_BUILTIN 4
STORE 10
PUSH 0
LOAD_ARRAY_AT 1
PUSH_PARAM
CALL_BUILTIN 4
STORE 11
LOAD 10
PUSH_PARAM
LOADSTRING 4
PUSH_PARAM
LOAD 11
PUSH_PARAM
STANDALONE_CALL_BUILTIN 0

[ 7.89672e+07 ]
Runtime: 25ms
delta array : [ 7.89672e+07 7.89673e+07 ]
rand: 195.607
1  0
100
a
b
c
d
e
f
g
h
i
j
[ a b c d e f g h i j ]
array  string
```


# Speed Benchmark (For variable accessing)

- rFortran (Compiled with optimizations)
  
```Pascal
program speed_benchmark 

var Ds = Time()
var target = 1000000
var i = 0 

while i < target do 
    i=i+1
end

var De = Time()

List("ran in: "  , De-Ds, "ms")

end program 
```

- ran in: 26ms

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
 - Python 3+ (Optional, for fast building)

```bash 
git clone [repo_link]
cd path_to_clone/src
# build project with my own version of cmake
# make sure you have python 3+ installed
cd rdshell
python rdshell.py
rdshell> run
rdshell> exit
cd ..

# ON LINUX/MACOS
./b
# ON WINDOWS
b.exe

# If you don't have python, just run this after you cd'd into src;
g++ runtime/*.cpp lexer/*.cpp runtime/stl/*.cpp compiler/*.cpp parser/*.cpp -Iinclude -O3 -Ofast -funroll-loops -ffast-math -march=native -flto -fomit-frame-pointer -o b
```


















