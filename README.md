## ⚙️ C Pre-Compiler Tool

A command-line tool written in C that parses and pre-processes C source files. Developed as an academic project, this tool simulates key functions of a C pre-compiler, including comment removal, dependency resolution, and basic syntax validation.

The program reads a C file, resolves its #include directives, strips all comments, and checks for basic declaration errors, producing a single, clean output file or printing to stdout.

---
## 🎯 Overview

Based on the project specifications, this tool provides the following features:

- Comment Stripping: Removes all single-line (//) and multi-line (/* ... */) comments.



- Dependency Resolution: Recursively parses #include directives and merges the content of the included files (which are also processed). (Assumes included files are in the CWD).


- Syntax Validation: Checks for invalid variable identifiers (e.g., 5brothers, x-ray).


- Flexible Output: Generates a single output file (using -o/--out) or prints the processed code directly to stdout.

- Verbose Statistics: An optional -v/--verbose flag prints detailed processing statistics, including file sizes, lines, comments removed, and errors found.
---

## 🧩 Core Components
The program is organized into a main executable and a library of functions, adhering to the requirement for a non-monolithic structure.

- src/main.c: The application's entry point. It handles parsing command-line arguments (-i, -o, -v), file I/O setup, and calls the main resolver. It is also responsible for printing the final statistics if verbose mode is enabled.


- src/myPreCompilerFunctions.c: Contains the core logic for parsing, processing, and validating the C code.

- (Header File): A header file is used to define function prototypes and structures for use across the project.
---

## ⚙️ Core Functionality
The program's logic is driven by several key functions described in the documentation:

- Resolver(FILE *input, ...): This is the main function of the project. It scans the input file line by line, coordinating all other tasks:

- Calls levaCommenti(...) to strip comments.

- Calls Include(...) to handle #include directives recursively.

- Calls controlloNomi(...) to perform validation.

- levaCommenti(char *riga, ...): Removes comments from a single line, using a global state (star_commenti) to manage multi-line blocks.


- Include(char *riga, ...): When an #include directive is found, it recursively calls Resolver on the new file and appends the result to the main output.

Validation:

- controlloNomi(...): Splits the line into words and checks for valid type and identifier syntax.

- checkTipi(...) & checkParola(...): Validate that identifiers are valid, not reserved keywords (isReserved(...)), and correctly formatted.


Error Reporting: 
- rigaSbagliata(...) is called on validation failure, reporting the specific file and line number where the error was detected.
--- 

## 💻 How to Run
Clone the repository:

```Bash
git clone https://github.com/simonemazzi/MyPreComplier.git
cd MyPreComplier
```
Compile the project using gcc (as required by the specifications):

```Bash
gcc -o myPreCompiler src/main.c src/myPreCompilerFunctions.c -Wall -Wextra
```
Execute the program with the specified arguments:

- To process a file and output to stdout:

```Bash
./myPreCompiler -i nome_file_input.c
```
- To process a file and output to another file:

```Bash
./myPreCompiler -i nome_file_input.c -o nome_file_output.c
```
- To output to a file with verbose statistics:

```Bash
./myPreCompiler --in=nome_file_input.c --out=nome_file_output.c --verbose
```
--- 
## 🧰 Requirements
- C compiler (e.g., gcc or clang)

- Standard C Libraries

---
## 👥 Authors
- [Simone Mazzi](https://github.com/simonemazzi)

- [Lorenz Narcis Grecu](https://github.com/Lurinz)

- [Felipe Sampedro](https://github.com/Sampyx)
