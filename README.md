
Clang2Dot
=========

If you are looking for a tutorial: https://github.com/loarabia/Clang-tutorial/
contains all the basics.

I am working with Clang, basically transforming Clang AST to another (Sage III
from ROSE Compiler).

I have to share the result of this work ; as if you have try something
similar, you may have noticed that most tutorial are out-of-date and there is 
no Graphviz utility for Clang AST (not that I can find).
This code *is* out-of-date too as I am using LLVM/Clang version 3.0 (even if 3.1
is already out), but at least you know what version you should use...

Also, you will probably have to enable RTTI when compiling LLVM/Clang:
```` bash
  $ # configure LLVM/CLANG
  $ export REQUIRES_RTTI=1
  $ make
````

You can notice that it is a work in progress. But as I need it to move from c 
to c++ support in my project, it should reach a functionnal level after a few
days (today is 06/13/12).

Once the base have been laid down, I would like to have a good doxygen doc and
other LLVM/Clang versions support. If you want to help you are welcome.

I'd like it to be a good tutorial...

