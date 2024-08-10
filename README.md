# Lev Lang

Yet another LLVM frontend for a language nobody will use

## Syntax

```
fn fib(n: i32) -> i32 {
    if n == 1 or n == 2 {
        return 1;
    }

    return fib(n-1) + fib(n-2);
}

pub fn main() -> i32 {

    // this is a comment, hello there!
    std.print(fib(40));

    return 0;
}

```
