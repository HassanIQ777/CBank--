# CBank++

A simple, command-line banking application written in C++.

## Features

- **Deposit and Withdraw:** Easily deposit and withdraw money from your account.
- **Transaction History:** View a detailed history of your transactions.
- **Account Overview:** Get a quick overview of your account balance and user information.
- **Customizable Settings:**
    - Set your user name.
    - Choose your preferred currency.
    - Configure the number of transactions to display.
- **Persistent Storage:** Your account information and transaction history are saved to files.

## Getting Started

### Prerequisites

- A C++ compiler that supports C++20.
- `make`

### Building

To build the application, simply run `make` in the project's root directory:

```bash
make
```

This will create an executable named `CBank++` in the `bin` directory.

## Usage

To run the application, you need to provide a directory where the application can store its data files:

```bash
./CBank++ <data_directory>
```

For example:

```bash
./CBank++ my_bank_data
```

If the directory does not exist, you will get an error.

## The `libutils` Library

CBank++ uses the `libutils` library, which provides a collection of utility functions for C++ development.

### Features of `libutils`

- **`benchmark.hpp`:** A simple benchmarking tool to measure the performance of your code.
- **`binarycache.hpp`:** Save and load data in a binary format.
- **`cliparser.hpp`:** A command-line argument parser.
- **`color.hpp`:** Add color to your terminal output.
- **`file.hpp`:** A comprehensive set of functions for file and directory manipulation.
- **`funcs.hpp`:** A collection of miscellaneous utility functions.
- **`log.hpp`:** A simple logging library.
- **`pager.hpp`:** (Currently empty)
- **`random.hpp`:** A random number generation library.
- **`table.hpp`:** Create and display tables in the terminal.
- **`texteditor.hpp`:** A simple text editor component.
- **`timer.hpp`:** A timer to measure elapsed time.
- **`tokenizer.hpp`:** A string tokenizer.

## Development

The project is structured as follows:

- **`CBank++`:** The main application.
    - `main.cpp`: The main entry point of the application.
    - `bank_operations.hpp`: Functions for bank operations.
    - `declarations.hpp`: Global declarations.
- **`libutils`:** A library of utility functions.
    - `src/`: The source code of the library.

### Building for Debugging

To build the application with debugging symbols, run:

```bash
make debug
```

### Cleaning the Build

To clean the build files, run:

```bash
make clean
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
