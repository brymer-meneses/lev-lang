# Lev Lang

Lev is WIP compiled programming language inspired by Python's syntax, leveraging LLVM.

# Syntax

```
import std

fn fib(n: i32) -> i32:
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

pub fn main() -> i32:
    let num = 100
    for i in 0..num:
        std.print(i)
    return 0
```


