
Clang2Dot
=========

I am working with Clang, basically transforming Clang AST to another (Sage III
from ROSE Compiler).

I have to share the result of this work, as if you have try something
similar you may have notice that most tutorial are out-of-date and their is 
no Graphviz utility for Clang AST (not that I can find).
This code is out-of-date too as I am using LLVM/Clang version 3.0 (even if 3.1
is already out), but at least you know what version to use...

Also, you will probably have to enable RTTI when compiling LLVM/Clang:
```` bash
  $ # configure LLVM/CLANG
  $ export REQUIRES_RTTI=1
  $ make
````
