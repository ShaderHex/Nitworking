# Nitworking

Nitworking is a lightweight and cross-platform socket server library designed to simplify the creation of basic HTTP servers. It eliminates the complexity often found in existing C++ networking frameworks by offering an easy-to-use and well-documented API. With Nitworking, developers can effortlessly handle client requests, serve HTML files, and manage socket connections on both Windows and Linux.

---

## Why Nitworking?

Most C++ networking frameworks are either unnecessarily complex or lack sufficient documentation. Nitworking was built with the vision of solving these issues by providing:
- **Ease of Use:** A simple API that makes networking tasks intuitive.
- **Lightweight Design:** Focused on functionality without unnecessary overhead.
- **Cross-Platform Support:** Compatible with both Windows and Linux environments.

---

## Features

- Lightweight and beginner-friendly.
- Cross-platform support for Windows and Linux.
- Simplifies HTTP server creation.
- Basic request routing for handling multiple paths.
- Minimal setup and no external dependencies.

---

## Platforms

- **Linux**: Works out of the box with no additional setup.  
- **Windows**: Requires calling the `initialize_winsock();` function to initialize the Winsock library.

---

## Getting Started

### Installation

1. **Download the Library:**  
   Clone this repository or download the `nitworking.h` file directly.  

   ```bash
   git clone https://github.com/ShaderHex/Nitworking.git
   ```
2. **Setup Your Project:**  
   Place the `nitworking.h` file alongside your `main.cpp`.  

3. **Compile Your Code:**  
   On Linux:  
   ```bash
   g++ -o server main.cpp
   ```  
   On Windows:  
   ```bash
   g++ -o server main.cpp -lws2_32
   ```  

---

## Example Usage

The following example demonstrates how to set up a simple HTTP server with multiple routes using Nitworking:

```cpp
#include "nitworking.h"
#include <iostream>
#include <map>

int main() {
#ifdef _WIN32
    initialize_winsock();
#endif

    PathMapping mapping[] = {
        {"/home", html_from_file("example.html")},
        {"/about", "<html><body>Made by ShaderHex (on GitHub)</body></html>"},
        {"/contact", "<html><body>This project doesn't have a Discord server apparently</body></html>"}
    };

    sock server_fd = create_server_socket();
    unsigned int port = 8080;

    bind_socket(server_fd, "127.0.0.1", port);

    listen_for_connections(server_fd);
    std::cout << "Listening on port " << port << std::endl;

    while (true) {
        sock client_fd = accept_connection(server_fd);
        
        html_buffer(client_fd, html_from_file("example.html"), mapping, 3);
        
        close_socket(client_fd);
    }

    close_socket(server_fd);
}
```

### Steps in Example:
1. Initialize Winsock (if on Windows) using `initialize_winsock()`.  
2. Define `PathMapping` to associate URL paths with responses (HTML content or files).  
3. Create a server socket with `create_server_socket()` and bind it to `127.0.0.1` on port `8080`.  
4. Use `listen_for_connections()` to enable the server to accept client requests.  
5. Use `html_buffer()` to serve appropriate responses based on the requested path.  

Visit the server at `http://127.0.0.1:8080/home`, `/about`, or `/contact` in your web browser.

---

## Future Plans

- [] - Simplify the API further for a better developer experience. (WIP)
- [] - Add support for UDP communication.  

---

## License

Nitworking is open-source and licensed under the [MIT License](https://github.com/ShaderHex/Nitworking/wiki/LICENSE). Feel free to use, modify, and distribute it.

---

## Author

This project was created and is maintained by [ShaderHex](https://github.com/ShaderHex). For feedback or contributions, feel free to open an issue or submit a pull request.

---

Ready to create your first HTTP server? Clone the repo and get started with Nitworking today!
