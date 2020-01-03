# calc

Calc is a simple command line calculator.

## Installing

### Linux/macos
```
git clone https://github.com/pigcoder3/calc.git
cd calc
make
make install
```
If anything is messed up in the process: `make clean`, and redo steps.

## Guide

### Help Message

Using the -h flag:

```
Usage: calc expression [-s] [-n] [-d] [-c]
       calc [-h] [-v]

Options:
  -h - show this help message.
  -s - show steps to solve.
  -n - show result in scientific notation.
  -d - Show debug messages.
  -c - Disable Syntax checks. WARNING: Disabling syntax checking could cause strange
       errors during calculation or even give an incorrect result, because syntax
       checking stops the program if something is wrong. This should only be used
       if there is a bug in the syntax checker. Disable this warning with -w.
  -w - Disable the warning that comes with disabling syntax checks (-c).
  -v --version - show version.

Notes:
  [1] Quotation marks should be present to make sure that your shell interprets the
      expression as a single argument.
  [2] You must place multiplication symbols when using the distributive property.
      [See example 1]
  [3] Calc follows rules of order of operations.
    [3.1] Order of operations does not state when trigonometric functions should be
          calculated, so calc does it between exponents and multiplication/division.
       [3.1.1] It is strongly advised to use parenthesis around trig functions anyway
               to ensure that everything is calculated the way it is intended.

Specific Syntax: (Just throw these together like is done with real equations)
  Add: +
  Subtract: -
  Divide: /
  Multiply: *
  Parenthesis: ( )
  Absolute Value: | |
  Power: number^power
  Root: ((root)V(number)) Note that the parenthesis should be present to ensure that
        the parser reads it the correct way. Inner parenthesis are not necessary if
        there is only 1 number within.
  Trig function: sin(stuff) [See note 3.1]

Examples: [See note 1]
  4*5*(3+2)        = 100 [See note 2]
  (-4^2)+2/3       = 16.6666666
  |-5-2|           = 7
  2/(3V8)          = 1
  sin(1)/sin(1)    = 1

Exit status:
  -2:   Syntax error
  -1:   Invalid flag
  0:    Everthing is okay
```

### Examples

The following command simply adds 4+4 and prints the answer (8)
```
$ calc "4+4"
8
```

If no expression is supplied through the arguments, calc will take an expression through stdin:
```
$ calc
4+4
8
```

You can also pipe the expression to calc:
```
$ cat largeExpression.txt | calc
6200
```

You can get the steps for solving the expression:
```
$ calc "(4+5)/3" -s
(4+5)/3
  4+5
  Addition: 4+5
9/3
Division: 9/3
3
```

You can get the result in scientific notation:
```
$ calc "(4+5)/3*4000" -n
1.200000e+04
```
