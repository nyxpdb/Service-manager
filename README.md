Of course. Here is a professional, English-language README file in Markdown format for the Windows Service Controller utility you created.

-----

# Windows Service Controller

A lightweight, command-line utility for starting and stopping Windows services. This tool is built with modern C++ and interacts directly with the Windows Service Control Manager (SCM) API, providing robust error handling and reliable state management.

-----

## **Important**

This tool modifies the state of system services. It **must be run with Administrator privileges** to function correctly. Stopping critical system services can lead to system instability or unexpected behavior. Use this utility with caution and only on services whose function you understand.

## Features

  - **Start & Stop Services**: Provides simple commands to both start and stop any specified Windows service.
  - **Robust Error Handling**: Reports specific error messages from the Windows API, helping to quickly diagnose issues like "Access Denied" or "Service Not Found."
  - **Synchronous Operation**: After sending a start or stop command, the tool waits and polls the service status to ensure the operation has completed successfully before exiting.
  - **Timeout Protection**: Implements a 30-second timeout to prevent the application from hanging if a service gets stuck in a pending state.
  - **Safe Resource Management**: Uses modern C++ RAII principles (`std::unique_ptr`) to ensure system handles are always released correctly.
  - **No Dependencies**: A single, self-contained executable with no external library requirements.

## Prerequisites

  - **Operating System**: Windows 10 / Windows 11 / Windows Server 2016 or later.
  - **Compiler**: A C++ compiler that supports C++11 or later (e.g., MinGW g++, MSVC from Visual Studio).

## Building the Project

1.  Save the source code as `service_tool.cpp`.

2.  Open a command prompt or terminal with access to your C++ compiler.

3.  Compile the source code using the following command (example with g++):

    ```sh
    g++ -o ServiceTool.exe service_tool.cpp -std=c++11 -static
    ```

    This will create an executable file named `ServiceTool.exe`.

## Usage

The utility is operated from a command line with administrative rights. The command syntax is as follows:

```
ServiceTool.exe <action> <ServiceName>
```

  - `<action>`: Can be either `start` or `stop`.
  - `<ServiceName>`: The short name of the service you wish to control (e.g., "Spooler", "WSearch").

-----

### **Examples**

1.  **Open a Command Prompt or PowerShell as an Administrator.**
2.  Navigate to the directory containing `ServiceTool.exe`.

#### To stop the Print Spooler service:

```sh
.\ServiceTool.exe stop Spooler
```

**Output:**

```
[INFO] Stopping service 'Spooler'...
[SUCCESS] Service 'Spooler' stopped successfully.
```

#### To start the Print Spooler service:

```sh
.\ServiceTool.exe start Spooler
```

**Output:**

```
[INFO] Starting service 'Spooler'...
[SUCCESS] Service 'Spooler' started successfully.
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
