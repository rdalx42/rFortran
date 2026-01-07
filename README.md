
# Rf - RD fortran 

This is a mini fortran clone I'm updating, it compiles to it's own bytecode
The main goal for this project is to replicate what modern fortran would look like while also making it memory lightweight

<img src = "rflogo.png">

# All features:

```pascal 
program main 

    var x = 1

    do 
        var z = 3+x 
        list z
    end

    list x

end program 
```

# Bytecode

```
PUSH 1
STORE 0
PUSH 3
LOAD 0
OP +
STORE 1
LIST 1
LIST 0
```

# How to run 

```bash 

git clone [repo_link]
cd path_to_clone/src
./b # by default, main.rf will be executed
```


