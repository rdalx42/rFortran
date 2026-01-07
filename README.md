
# rF - RD fortran 

This is a mini fortran clone I'm updating, it compiles to it's own bytecode
The main goal for this project is to replicate what modern fortran would look like while also making it memory lightweight, with string polling included!

<img src = "rflogo.png">

# All features:

# Notices
 - Concat can only concat strings, not variables which hold strings, concat operations can't be nested: "y" concat "x" concat "z"  

```pascal 
program main

var a = 5
var b = 10
var c = 0

if 1 do
    if 5 do
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
GOTO_IF_FALSE 0
PUSH 5
GOTO_IF_FALSE 2
PUSH 100
STORE 2
GOTO 3
LABEL 2
PUSH 50
STORE 2
LABEL 3
GOTO 1
LABEL 0
PUSH 1
NEG
STORE 2
LABEL 1
LIST 2
PUSH 1
STORE 3
LOADSTRING 0
STORE 4
LOAD 4
STORE 5
LOADSTRING 1
LOADSTRING 2
LOADSTRING 3
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

[String Hasher] Hashed Strings:
Hash: 0 String: hello, world
Hash: 1 String: bye
Hash: 2 String:  world!
Hash: 3 String: bye world!
[GOTO Hasher] Hashed GOTO Positions:
Label: 0 Address: 18
Label: 1 Address: 22
Label: 2 Address: 13
Label: 3 Address: 16
Label: 4 Address: 0
[memory at 2] Type: NUMBER Value: 100
[memory at 4] Type: STRING Value: hello, world POINTER: 0
[memory at 5] Type: STRING Value: hello, world POINTER: 0
[memory at 6] Type: STRING Value: bye world! POINTER: 3
[memory at 7] Type: NUMBER Value: -2.5
[memory at 3] Type: NUMBER Value: 1
```

# How to run 

# Requierments
 - C++ 17
 - G++ (I'm pretty sure you can use other compiler, however rF was only tested with G++)
 - Existing.

```bash 
git clone [repo_link]
cd path_to_clone/src
g++  runtime/*.cpp lexer/*.cpp compiler/*.cpp parser/*.cpp -Iinclude -o b.exe 
./b # by default, main.rf will be executed
```







