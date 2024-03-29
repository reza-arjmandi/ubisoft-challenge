## Prerequisites

1. **CMake**
2. **C++17 Compiler** 

## Compile and Run Instructions

Follow these steps to compile and run the project:
```sh
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install boost-asio
.\vcpkg\vcpkg.exe install boost-json
cmake -B build -S .
cmake --build build
auction-house.exe 8080
``````

## Overview

This project leverages the `boost.asio` library to manage network connections. Upon user connection, a session is created to handle state management through a finite state machine. Each page represents a state, and state transitions are determined by the current state. The design embraces a robust `boost` asynchronous approach, utilizing a single main thread to manage all sessions, ensuring a lock-free environment without race conditions.

In real-time, the server continuously updates and dumps user and item information to a JSON file. To handle the heavy task of serializing and writing to the file, the project employs the Active Object pattern, enhancing efficiency and performance.
