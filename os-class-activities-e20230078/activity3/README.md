# Class Activity 3 — Socket Communication & Multithreading

- **Student Name:** KHORN VICTOR
- **Student ID:** e20230078
- **Date:** 1/11/2026

---

## Task 1: TCP Socket Communication (C)

### Compilation & Execution

![Socket exchange](screenshots/task1_socket_exchange.png)

### Answers

1. **Role of `bind()` / Why client doesn't call it:**

   > In the server program, bind() ties an unnamed socket file descriptor to a specific local address and port number (such as INADDR_ANY on port 8080). This step is essential because the operating system needs to know which incoming traffic to direct to the server process. In contrast, the client typically does not call bind() because it initiates the communication rather than waiting on a fixed, well-known port. When the client invokes connect(), the operating system automatically assigns an ephemeral, unused local port and the appropriate local IP address to the client's socket.

2. **What `accept()` returns:**
   > When incoming connections arrive, accept() pulls the first completed connection request from the listen queue and returns a brand-new socket file descriptor dedicated exclusively to that client. The original server socket (server_fd) remains in the LISTEN state, acting purely as a gatekeeper that waits for and queues up subsequent connection attempts. In contrast, the newly returned socket (client_fd) is an active, fully connected socket configured for two-way communication, handling all actual data transfer operations like read() and send() with that specific client.

3. **Starting client before server:**
   > If the client is started before the server is running, the client's connect() system call fails immediately with the error connect: Connection refused. This occurs because the client sends a TCP SYN packet to port 8080 to begin the three-way handshake, but the host operating system finds no active process listening on that destination port. Consequently, the operating system's networking stack rejects the attempt by replying with a TCP RST (reset) packet, causing connect() to return -1 and set errno to ECONNREFUSED.

4. **What `htons()` does:**
   > The function htons() (Host to Network Short) converts a 16-bit unsigned integer from host byte order to standard network byte order. This conversion is necessary because different processor architectures store multi-byte numbers using different endianness; for instance, modern x86 and ARM systems commonly use little-endian format (storing least significant bytes first), whereas TCP/IP protocols mandate big-endian format (storing most significant bytes first). Without conversion, a port value such as 8080 (0x1F90) on a little-endian host would be transmitted over the wire in reverse order as 0x901F, which the receiving side would misinterpret as port 36912.

5. **Socket call sequence diagram:**
   > The communication sequence begins with both the server and client creating endpoints via socket(). The server then binds itself to a designated IP address and port using bind(), places the socket into a passive waiting state with listen(), and blocks on accept() awaiting incoming traffic. The client triggers the TCP three-way handshake by calling connect(), which unblocks the server's accept() call and yields a dedicated connection socket. Once connected, the client transmits its message using send(), and the server receives it through read(). The server then issues a response using send(), which the client captures via its own read(). Finally, both sides gracefully terminate the connection by calling close() on their respective socket descriptors.

---

## Task 2: POSIX Threads (C)

### Output — Without Mutex (Race Condition)

![Threads output](screenshots/task2_threads_output.png)

### Output — With Mutex (Correct)

_(Include in the same screenshot or a separate one)_

### Answers

1. **What is a race condition?**
   > _Your answer_

2. **What does `pthread_mutex_lock()` do?**
   > _Your answer_

3. **Removing `pthread_join()`:**
   > _Your answer_

4. **Thread vs Process:**
   > _Your answer_

---

## Task 3: Java Multithreading

### ThreadDemo Output

![Java threading](screenshots/task3_java_output.png)

### RunnableDemo Output

_(Include output or screenshot)_

### PoolDemo Output

_(Include output or screenshot)_

### Answers

1. **Thread vs Runnable:**
   > _Your answer_

2. **Pool size limiting concurrency:**
   > _Your answer_

3. **thread.join() in Java:**
   > _Your answer_

4. **ExecutorService advantages:**
   > _Your answer_

---

## Task 4: Observing Threads

### Linux — `ps -eLf` Output

_(Paste the relevant ps output here)_

### Linux — htop Thread View

![htop threads](screenshots/task4_htop_threads.png)

### Windows — Task Manager

![Task Manager threads](screenshots/task4_taskmanager_threads.png)

### Answers

1. **LWP column meaning:**
   > _Your answer_

2. **/proc/PID/task/ count:**
   > _Your answer_

3. **Extra Java threads:**
   > _Your answer_

4. **Linux vs Windows thread viewing:**
   > _Your answer_

---

## Reflection

> _What did you find most interesting about socket communication and threading? How does understanding threads at the OS level help you write better concurrent programs?_