# Two-Pass Linker

## Introduction

This repository contains the implementation of a two-pass linker designed for an educational purpose, highlighting the fundamental concepts of linking and loading in operating systems. The project simulates the linking process for a hypothetical machine characterized by its unique constraints, such as a word-addressable memory of 512 words and integer representation for valid words. This simplified scenario, though not reflective of modern architectures, serves as an effective teaching tool to understand the core functionalities of linkers, including symbol resolution, relocation, and error handling.

The two-pass linker approach, as implemented in this project, meticulously parses and processes the input to produce a unified executable from separately compiled object modules. The first pass establishes a global symbol table and calculates base addresses for each module, while the second pass resolves references and generates the final memory map. This method, although more complex than a single-pass linker, allows for a thorough error-checking mechanism and adheres to the principles of reentrant code, enhancing the robustness and reliability of the linking process.

## Features

- **Symbol Resolution and Relocation:** The linker efficiently handles symbol definitions and references across multiple modules, assigning absolute addresses and resolving external symbols. This feature is crucial for the creation of a coherent executable that functions correctly when loaded into memory.

- **Error and Warning System:** A comprehensive system for detecting and reporting various errors and warnings is implemented. This includes multiple definitions of symbols, undefined symbols, address space violations, and more. Each error or warning is precisely reported with details such as module number, symbol name, and the nature of the issue, aiding developers in debugging their code.

- **Support for Various Addressing Modes:** The linker understands and processes five addressing modes - Immediate, Absolute, Relative, External, and Module. This flexibility allows for a wide range of instruction types and use cases, mimicking the complexity of real-world linkers.

- **Memory Constraint Enforcement:** Emulating a hypothetical machine with a limited memory size imposes constraints on the addressing and symbol definitions. The linker enforces these constraints, ensuring that the generated executable adheres to the target machine's specifications, thus teaching students about the challenges of real-world system limitations.

- **Robust Parsing Mechanism:** Utilizing a custom tokenizer and strict parsing rules, the linker does not assume the syntactical correctness of the input. It can process input files with varied token delimiters and line structures, enhancing its ability to handle real-world scenarios where input may not be perfectly formatted.

- **Modular and Reentrant Code:** The project is structured to practice reentrant coding principles, where functions do not rely on shared state and can re-execute without side effects. This design choice is aligned with practices in operating system development, promoting code safety and concurrency.

## Compilation

A `Makefile` is provided for easy compilation. Use the following commands in the terminal:

```bash
make         # Compiles the linker
make clean   # Removes compiled files
## Usage

To run the linker program, follow these steps:

1. Compile the program using the provided Makefile with the `make` command.
2. Execute the program with an input file that contains the "object module" definitions as specified by the assignment's format:

```bash
./linker [input_file]
```

The input file should adhere to the structured format described in the assignment, including definition lists, use lists, and program text sections for each module.

## Implementation Details

The linker is implemented in C++ and designed to run on Unix-based systems, specifically tested on NYU's `linserv1.cims.nyu.edu` server. It follows a two-pass approach to process the input file:

- **First Pass:** Reads the input to build a symbol table and compute base addresses for each module. It checks for errors such as multiple symbol definitions and symbols exceeding module sizes.
- **Second Pass:** Uses the symbol table and base addresses from the first pass to resolve external symbol references and generate the final memory map. This pass also handles errors related to addressing modes and operand values.

The program strictly adheres to the requirement of not storing tokens between passes or while parsing lines, practicing reentrant code principles essential in operating systems development.

## Testing

The program includes `runit.sh` and `gradeit.sh` scripts for testing, as provided in the `lab1_assign.tar.Z` archive. To test the program:

1. Extract the archive and navigate to the `lab1_assign` directory.
2. Create a directory for your outputs.
3. Run the `runit.sh` script to generate outputs for all provided input files:
    ```bash
    ./runit.sh <your-outdir> ./linker
    ```
4. Use the `gradeit.sh` script to compare your outputs against reference outputs:
    ```bash
    ./gradeit.sh . <your-outdir>
    ```

The `LOG.txt` in your output directory will detail any discrepancies between your program's output and the expected output, assisting in debugging and validation of your implementation.

These steps ensure your linker correctly processes input files and handles various error conditions as specified in the assignment guidelines.
