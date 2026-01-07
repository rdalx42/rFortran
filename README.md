
# Rf - RD fortran 

This is a mini fortran clone I'm updating, it compiles to it's own bytecode
The main goal for this project is to replicate what modern fortran would look like while also making it memory lightweight, with string polling included!

<img src = "rflogo.png">

# All features:

```pascal 
program main 

    var x = 1
    var string = "hello, world"
    var string2 = string

    list string 
    list string2

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
LIST 1
LIST 2
PUSH 3.5
NEG
LOAD 0
OP +
STORE 3
LIST 3
LIST 0

[String Hasher] Hashed Strings:
Hash: 0 String: hello, world
[memory at 1] Type: STRING Value: hello, world POINTER: 0
[memory at 2] Type: STRING Value: hello, world POINTER: 0
[memory at 3] Type: NUMBER Value: -2.5
[memory at 0] Type: NUMBER Value: 1
```

# How to run 

```bash 

git clone [repo_link]
cd path_to_clone/src
./b # by default, main.rf will be executed
```



