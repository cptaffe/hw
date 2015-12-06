# files

## main.cc
C++ source file.

## out.txt
Runtime output of result.

# compilation
```sh
# This should compile in Windows by opening main.cc
# in Visual Studio, but for completeness here follows
# my build command.
clang++ --std=c++1z -Wall -Wpedantic main.cc
```

# execution
```sh
# The program logs horse predictions to std log,
# which will not be redirected to out.txt, while
# the expected obsBuSF will be logged.
./a.out > out.txt
```
