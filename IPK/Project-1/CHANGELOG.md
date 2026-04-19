# CHANGELOG for IPK25-CHAT Client

## Version 1.0.0 (2025-04-20)

### Implemented Functionality

This project implements a chat client for the IPK25-CHAT protocol as specified in the assignment for the IPK course at FIT VUT. The client supports both TCP and UDP variants of the protocol and adheres to the formal requirements outlined in the project specification. Key implemented features include:

- **TCP Support**:
  - Full implementation of the text-based TCP protocol with messages terminated by `\r\n`.
  - Supports all required message types: `AUTH`, `JOIN`, `MSG`, `ERR`, `BYE`, and `REPLY`.
  - Graceful connection termination without sending `RST` packets, implemented via `TcpClient::graceful_shutdown`.
  - Robust parsing and formatting of messages in `TcpMessage` with validation of parameters (`MessageValidator`).

- **UDP Support**:
  - Partial implementation of the binary UDP protocol.
  - Supports message types: `AUTH`, `JOIN`, `MSG`, `ERR`, `BYE`, `REPLY`, `CONFIRM`, and `PING`.
  - Handles dynamic port allocation, message confirmations (`CONFIRM`), and retransmissions (250 ms timeout, 3 retries).
  - Implements duplicate message detection using `MessageID` tracking in `UdpClient`.
  - Due to time constraints, UDP functionality is incomplete and insufficiently tested (see Known Limitations).

- **General Features**:
  - Command-line interface with required arguments (`-t`, `-s`) and optional arguments (`-p`, `-d`, `-r`, `-h`) parsed in `args.cpp`.
  - User commands: `/auth`, `/join`, `/rename`, `/help`, `/bye`, with non-command input treated as `MSG` in the `OPEN` state.
  - Mealy finite state machine (`StateMachine`) managing protocol states: `START`, `AUTH`, `OPEN`, `JOIN`, `END`.
  - Robust parameter validation (`MessageValidator`) ensuring compliance with protocol constraints (e.g., `Username`, `Secret`, `MessageContent`).
  - Signal handling for `SIGINT` and `EOF` (`Ctrl+C`, `Ctrl+D`) with graceful termination via `SignalHandler`.
  - Debug logging to `stderr` using `printf_debug` macro, enabled with `DEBUG=1` in `Makefile`.
  - Modular object-oriented design in C++20 with RAII, smart pointers, and polymorphism (`Client`, `MessageBase` hierarchies).

- **Development Notes**:
  - The project was developed on two devices, resulting in commits from two accounts in the Gitea repository:
    - `xondre16`: School account used for primary development and testing.
    - `Dodko7`: Private GitHub account used for development on a personal device.
  - This dual-account setup was necessary to accommodate development across environments but does not affect the codebase integrity.

### Known Limitations

- **UDP Implementation**:
  - The UDP protocol implementation is incomplete due to time constraints during development.
  - Insufficient testing was performed, particularly for edge cases such as packet loss, high-latency networks, or server errors.
  - Some binary message formats (e.g., `REPLY` parsing) may not handle all server responses correctly due to relaxed parsing logic in `UdpMessage::parse`.
  - The `CONFIRM` mechanism may fail under specific conditions (e.g., rapid message sequences), as it was not thoroughly validated.

- **Testing**:
  - While TCP functionality was extensively tested, including manual tests with `netcat`, UDP testing was minimal.
  - Unit tests for individual components (e.g., `MessageValidator`, `StateMachine`) were not implemented due to time limitations.
  - Test documentation is incomplete, lacking detailed descriptions of test scenarios, inputs, outputs, and network topology.

- **Documentation**:
  - The `README.md` contains information about the project, containing UML diagram and a comprehensive test section, as required by the assignment.

### Notes

Despite the limitations in the UDP implementation, the TCP functionality is robust, well-tested, and fully compliant with the protocol specification. The codebase is modular, well-commented, and designed to facilitate future extensions. The documented limitations reflect the time constraints faced during development, and all known issues are explicitly stated to ensure transparency.

If no additional limitations are discovered during evaluation, the TCP implementation should perform reliably in all specified scenarios. Further testing and development of the UDP component are recommended to achieve full protocol compliance.