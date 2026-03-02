# ft_irc

## Overview

ft_irc is a custom Internet Relay Chat (IRC) server written in C++98.

The goal of this project is to implement a fully functional IRC server compatible with a real IRC client. The server must handle multiple clients simultaneously using non-blocking I/O and a single poll() (or equivalent) instance.

The server follows the IRC protocol basics and supports authentication, nickname management, channel management, private messaging, and operator-specific commands.

---

## Compilation

The project is compiled using:

* C++98 standard
* Flags: `-Wall -Wextra -Werror`

### Makefile rules

The Makefile contains the following rules:

* `all`
* `$(NAME)`
* `clean`
* `fclean`
* `re`

No unnecessary relinking is performed.

To compile:

```bash
make
```

---

## Usage

```bash
./ircserv <port> <password>
```

* `port`: TCP port used for incoming connections
* `password`: Password required by clients to authenticate

Example:

```bash
./ircserv 6667 mypassword
```

---

## Technical Constraints

* C++98 only
* No external libraries
* No Boost
* No forking
* All file descriptors are non-blocking
* Only one poll() (or equivalent) is used
* Communication via TCP/IP (IPv4 or IPv6)
* The server must never crash
* Memory leaks are forbidden

---

## Core Features (Mandatory)

### Authentication

* PASS
* NICK
* USER

A client must:

1. Provide the correct password
2. Set a nickname
3. Set a username

Only then is the client fully registered.

---

### Channel Management

* JOIN
* PART
* PRIVMSG

Features:

* Clients can join channels
* Messages sent to a channel are broadcast to all members
* Private messages between users are supported

---

### Operators and Modes

Each channel has:

* Regular users
* Channel operators

Operator commands implemented:

* KICK — remove a user from a channel
* INVITE — invite a user to a channel
* TOPIC — view or change the channel topic
* MODE — manage channel modes

Supported channel modes:

* `i` — Invite-only channel
* `t` — Only operators can change topic
* `k` — Channel password
* `o` — Grant/remove operator status
* `l` — User limit

---

## Architecture

### Core Components

* **Server**

  * Socket creation
  * Bind and listen
  * poll() loop
  * Accept new connections

* **Client**

  * Authentication state
  * Buffer management
  * Nickname and username

* **Channel**

  * Member list
  * Operator list
  * Modes
  * Topic

---

## Network Handling

* All sockets are set to non-blocking mode
* poll() monitors:

  * Listening socket
  * Client sockets (read/write)
* Partial packets are buffered
* Commands are processed only once fully reconstructed

Example test using netcat:

```bash
nc -C 127.0.0.1 6667
```

Test partial command sending to ensure correct buffering.

---

## Error Handling

The server handles:

* Invalid commands
* Unknown users
* Unknown channels
* Permission errors
* Incorrect password
* Channel mode violations

The server must remain stable under:

* Partial packet reception
* High number of clients
* Low bandwidth
* Unexpected disconnections

---

## Bonus (Optional)

Implemented only if mandatory part is perfect:

* File transfer
* IRC bot

---

## Testing

Recommended:

* Test with a real IRC client
* Test multiple simultaneous clients
* Test edge cases (invalid commands, rapid connect/disconnect)
* Verify memory leaks with valgrind

---

## Evaluation Notes

During peer evaluation, you may be asked to:

* Modify a function
* Add a small feature
* Adjust a mode behavior
* Explain the architecture

You must be able to explain:

* The event loop
* The poll() usage
* Authentication flow
* Channel mode handling
* Buffer management strategy

---

## Author

Marc Justin Zibrian
