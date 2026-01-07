
# rF - RD fortran 

This is a mini fortran clone I'm updating, it compiles to it's own bytecode
The main goal for this project is to replicate what modern fortran would look like while also making it memory lightweight, with string polling included!

<img src = "rflogo.png">

# All features:

# Notices
 - Concat can only concat strings, not variables which hold strings, concat operations can't be nested: "y" concat "x" concat "z"  

```pascal 
program main 

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
PUSH 1
STORE 0
LOADSTRING 0
STORE 1
LOAD 1
STORE 2
LOADSTRING 1
LOADSTRING 2
LOADSTRING 3
STORE 3
LIST 1
LIST 2
LIST 3
PUSH 3.5
NEG
LOAD 0
OP +
STORE 4
LIST 4
LIST 0

[String Hasher] Hashed Strings:
Hash: 0 String: hello, world
Hash: 1 String: bye
Hash: 2 String:  world!
Hash: 3 String: bye world!
[memory at 1] Type: STRING Value: hello, world POINTER: 0
[memory at 2] Type: STRING Value: hello, world POINTER: 0
[memory at 3] Type: STRING Value: bye world! POINTER: 3
[memory at 4] Type: NUMBER Value: -2.5
[memory at 0] Type: NUMBER Value: 1
```

# How to run 

# Requierments
 - C++ 17
 - G++ (I'm pretty sure you can use other compiler, however rF was only tested with G++)
 - Existing.

```bash 
git clone [repo_link]
cd path_to_clone/src
g++  runtime/*.cpp lexer/*.cpp compiler/*.cpp parser/*.cpp -Iinclude -o b.exe # compile, optional, only if you made some actual changes
./b # by default, main.rf will be executed
```





