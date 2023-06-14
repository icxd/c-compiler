# c-compiler

## todo

* [ ] parse generics for all structs, functions, etc.
    * [ ] structs
    * [ ] union
    * [ ] enums
    * [ ] functions
* [x] tagged union (might add untagged unions later)
* [x] enums
* [x] structs
* [ ] semantic analysis
    * [ ] type checking and type inference
    * [ ] label checking
    * [ ] control flow checking
* [ ] llvm backend or x64 assembly backend (haven't decided yet...)

## known bugs

* [x] `ast_print` causes the entire program to segfault for some unknown reason, needs to be investigated.