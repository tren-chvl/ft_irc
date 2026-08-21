<div align="center">

```
▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄        ▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄        ███  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄
                                ███          ███             
███▄▄▄▄▄▄        ███            ███  ███▄▄▄▄▄███  ███        
███▄▄▄▄▄▄        ███            ███  ███▄▄▄▄▄     ███        
███              ███            ███  ███ ███▌     ███        
███              ███            ███  ███ ▐███     ███▄▄▄▄▄▄▄▄
███              ███            ███  ███  ███▌    ███▄▄▄▄▄▄▄▄
```

### A custom IRC server written in C++98

<br>

<img src="https://upload.wikimedia.org/wikipedia/commons/1/18/C_Programming_Language.svg" height="90">
&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;
<img src="https://upload.wikimedia.org/wikipedia/commons/3/35/Tux.svg" height="90">
<br><br>

![C++98](https://img.shields.io/badge/C%2B%2B-98-00599C?style=for-the-badge\&logo=cplusplus\&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge\&logo=linux\&logoColor=black)
![IRC](https://img.shields.io/badge/IRC-Protocol-5865F2?style=for-the-badge)
![42](https://img.shields.io/badge/42-School-black?style=for-the-badge)

</div>

---

# 📡 Overview

**ft_irc** is a custom **Internet Relay Chat (IRC) server** written in **C++98**.

The goal of this project is to implement a fully functional IRC server compatible with a real IRC client.

The server handles **multiple clients simultaneously** using:

* ⚡ Non-blocking I/O
* 🔄 A single `poll()` instance
* 🌐 TCP/IP communication
* 🔐 Client authentication
* 💬 Private and channel messaging
* 👑 IRC operator commands
* 🏠 Channel management

---

# 🧠 Architecture

```text
                         ┌─────────────────────┐
                         │       IRC CLIENT    │
                         └──────────┬──────────┘
                                    │
                                    │ TCP
                                    ▼
                       ┌────────────────────────┐
                       │       IRC SERVER       │
                       │                        │
                       │       poll()           │
                       └───────────┬────────────┘
                                   │
                  ┌────────────────┼────────────────┐
                  │                │                │
                  ▼                ▼                ▼
             ┌─────────┐      ┌─────────┐      ┌─────────┐
             │ Client  │      │ Client  │      │ Client  │
             │    #1   │      │    #2   │      │    #3   │
             └────┬────┘      └────┬────┘      └────┬────┘
                  │                │                │
                  └────────────────┼────────────────┘
                                   │
                                   ▼
                           ┌──────────────┐
                           │   Channel    │
                           │              │
                           │  Users       │
                           │  Operators   │
                           │  Modes       │
                           │  Topic       │
                           └──────────────┘
```

---

# ⚙️ Compilation

The project is compiled using:

* **C++98**
* `-Wall`
* `-Wextra`
* `-Werror`

### Makefile rules

The Makefile contains:

```text
all
$(NAME)
clean
fclean
re
```

No unnecessary relinking is performed.

### Build

```bash
make
```

---

# 🚀 Usage

Start the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

You can then connect using an IRC client or a simple TCP client:

```bash
nc -C 127.0.0.1 6667
```

---

# 🧱 Technical Constraints

| Constraint         | Implementation          |
| ------------------ | ----------------------- |
| Language           | C++98                   |
| Compiler flags     | `-Wall -Wextra -Werror` |
| External libraries | ❌ None                  |
| Boost              | ❌ Not used              |
| Forking            | ❌ Not used              |
| Sockets            | Non-blocking            |
| Multiplexing       | `poll()`                |
| Protocol           | IRC                     |
| Network            | TCP/IP                  |
| Memory leaks       | ❌ Forbidden             |

---

# 🔐 Authentication

A client must successfully complete:

```text
PASS
  │
  ▼
NICK
  │
  ▼
USER
  │
  ▼
REGISTERED
```

Supported commands:

* `PASS`
* `NICK`
* `USER`

A client is fully registered only after the required authentication steps have been completed.

---

# 💬 Channel Management

Supported commands:

```text
JOIN
PART
PRIVMSG
```

Clients can:

* Join channels
* Leave channels
* Send messages to channels
* Send private messages
* Communicate with multiple users simultaneously

---

# 👑 Operators & Modes

Each channel can contain:

```text
┌─────────────────────────────┐
│          CHANNEL            │
├─────────────────────────────┤
│ 👑 Operators                │
│ 👤 Regular users            │
│                             │
│ Topic                       │
│ Channel modes               │
└─────────────────────────────┘
```

### Operator commands

| Command  | Description          |
| -------- | -------------------- |
| `KICK`   | Remove a user        |
| `INVITE` | Invite a user        |
| `TOPIC`  | View/change topic    |
| `MODE`   | Manage channel modes |

### Supported modes

| Mode | Description                     |
| ---- | ------------------------------- |
| `i`  | Invite-only channel             |
| `t`  | Only operators can change topic |
| `k`  | Channel password                |
| `o`  | Grant/remove operator status    |
| `l`  | User limit                      |

---

# 🖥️ Network Handling

All sockets are configured as **non-blocking**.

The server's `poll()` instance monitors:

```text
                 poll()
                   │
       ┌───────────┼───────────┐
       ▼           ▼           ▼
  Listening     Client #1   Client #2
   socket
                   │
                   ▼
              Read / Write
```

Partial packets are buffered until complete commands can be reconstructed.

This allows the server to correctly handle situations such as:

* Partial packet reception
* Multiple clients
* Slow connections
* Unexpected disconnections
* Rapid connect/disconnect cycles

---

# 🧩 Core Components

### 🖥️ Server

Responsible for:

* Socket creation
* `bind()`
* `listen()`
* `accept()`
* `poll()`
* Connection management

### 👤 Client

Responsible for:

* Authentication state
* Nickname
* Username
* Input buffering
* Client state

### 🏠 Channel

Responsible for:

* Member list
* Operators
* Channel modes
* Topic
* Permissions

---

# 🛡️ Error Handling

The server handles:

* Invalid commands
* Unknown users
* Unknown channels
* Permission errors
* Incorrect passwords
* Channel mode violations
* Unexpected disconnections

The objective is to keep the server stable even under unusual network conditions.

---

# 🧪 Testing

Recommended testing includes:

### IRC client

Connect using a real IRC client and test multiple users simultaneously.

### Netcat

```bash
nc -C 127.0.0.1 6667
```

### Multiple clients

```text
Client #1 ─────┐
Client #2 ─────┼──► IRC SERVER
Client #3 ─────┤
Client #4 ─────┘
```

### Memory checks

Use:

```bash
valgrind ./ircserv 6667 mypassword
```

to investigate memory leaks and invalid memory accesses.

---

# 🎁 Bonus

Optional features may include:

* 📁 File transfer
* 🤖 IRC bot

Bonus features should only be implemented once the mandatory requirements are fully stable.

---

# 📝 Evaluation

During peer evaluation, you may be asked to:

* Modify a function
* Add a small feature
* Adjust a channel mode
* Explain the architecture
* Explain the event loop
* Explain `poll()`
* Explain authentication
* Explain channel management
* Explain buffer management

The goal is not only to have a working server, but to **understand every part of it**.

---

# 🧑‍💻 Author

<div align="center">

### tren-chvl

[![GitHub](https://img.shields.io/badge/GitHub-tren--chvl-181717?style=for-the-badge\&logo=github)](https://github.com/tren-chvl)

**ft_irc**

</div>

---

<div align="center">

```text
     ╔══════════════════════════════════╗
     ║                                  ║
     ║        🌐  ft_irc  🌐            ║
     ║                                  ║
     ║       C++98 • Linux • IRC        ║
     ║                                  ║
     ╚══════════════════════════════════╝
```

</div>
