# Week 2 Examples and Tasks

## 1. **ex1 — Bitwise Negation**
Run the sample with `x = 7`, examine what the `~x` operation computes, determine the numeric result, and explain why it is negative.

## 2. **ex2 — Addition Overflow**
Add two large `int` values (`2,000,000,000 + 2,000,000,000`), observe the overflow, then adjust the program to perform the calculation in a 64-bit type and print the correct result.

## 3. **task01 — Characters and ASCII Codes**
Read or define the characters `'a'`, `'b'`, `'c'`, and output each as `character code`, matching the format:

```
a 97
b 98
c 99
```

## 4. **task02 — (a + b) * c - d**
Read four integers `a`, `b`, `c`, `d`, compute `(a + b) * c - d`, and print the result as a single number.

## 5. **task03 — Splitting Code Across Files**
Place the declaration of `print_smth()` in `function.h`, its definition in `function.cpp`, include the header in `main.cpp`, call the function, and ensure the output includes both `print_smth()`'s message and `Hello, World!`.

## 6. **task04 — Mini Calculator**
Implement four functions `plus`, `minus`, `multiply`, `divide` in separate files, each returning the correct result for two `int` operands, and invoke them from `main.cpp`.

## 7. **task05 — Bitwise Operations**
Read two integers and output the results of `a | b`, `a & b`, `a ^ b`, `a >> 1`, `a << 1`, and `~a`, formatting the output clearly.

## 8. **task06 — Sum of Digits of a Four-Digit Number**
Read a four-digit number and print the sum of its digits.

## 9. **task07 — `<cmath>` Functions**
Read `n` and output, in order: `n`, `sqrt(n)`, `sin(n)`, `cos(n)`, and `pow(n, 2)` using `<cmath>` functions.

## 10. **task08 — Output Manipulators**
Demonstrate `std::setprecision`, `std::fixed`, and base manipulators `std::hex`, `std::oct`, `std::dec`, showcasing both floating-point and integer examples.

## 11. **task09 — Swap Without a Temporary Variable**
Read `a` and `b`, show their initial values, swap them without using a temporary variable (e.g., via sum/difference and additionally via XOR), and display the final values.

## 12. **task10 — Parity Check**
Read an integer `a`, determine its parity using `(a & 1)`, and print a clear result such as `even` or `odd`.

## 13. **task11 — Radius Vector Length**
Read coordinates `a`, `b`, compute `sqrt(a^2 + b^2)`, and print the result rounded to two decimal places.

## 14. **task12 — Square of a Hypotenuse**
Read the lengths of the catheti `a`, `b`, compute `a^2 + b^2`, and output the value with two digits after the decimal point.

## 15. **task13 — Square of Triangle Area**
Read triangle sides `a`, `b`, `c`, compute the semiperimeter `p = (a + b + c) / 2`, and print `p(p - a)(p - b)(p - c)` (Heron's formula squared) with two decimal places.

## 16. **task14 — Next Odd Number Without Conditionals**
Read an integer `a` and output the smallest odd number strictly greater than `a` using arithmetic only (no `if` statements or loops).