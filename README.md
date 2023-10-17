# Lev Lang

Lev is WIP compiled programming language inspired by Python's syntax, leveraging LLVM.

## Syntax

```rust
import std
    
struct CheckedInt<T> {
    T: value

    impl std.traits.Show {
        fn show(self) -> str {
            return std.to_string(value);
        }
    }

    impl std.traits.Construct {
        fn construct(value: i32) -> CheckedInt<T> {
            return CheckedInt<T> { value: value };
        }

        fn construct() -> CheckedInt<T> {
            return CheckedInt<T> { value: 0 };
        }
    }
}

impl std.traits.Add for CheckedInt<T> {
    fn add(n1: CheckedInt<T>, n2: CheckedInt<T>) -> Optional<CheckedInt<T>> {
        if n1.value > 0 and n1.value > std.limits<T>() - n2.value {
            return None;
        } else if n1.value < 0 and n2.value < std.limits<T>() + n2.value {
            return None;
        } else {
            return Some(CheckedInt(n1.value + n2.value));
        }
    }
}

fn fib(n: CheckedInt<i32>) -> CheckedInt<i32> {
    if n == 1 or n == 2 {
        return 1;
    }
    return fib(n-1) + fib(n-2);
}

pub fn main() -> i32 {
    -- this is a comment, hello there!

    -- instantiate a class on the stack
    Animal animal = Animal("Smudge");

    -- print stuff to the terminal
    std.print(animal)
    std.print(fib(100))

    return 0
}
```

