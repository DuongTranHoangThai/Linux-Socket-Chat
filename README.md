# Linux Socket Chat via Network

Chat program between Server and Client 

**Compile:**
gcc Socket_chat.c -o Socket_chat

**Run:**
- Run as Server : ./Socket_chat -server
- Run as Clinet : ./Socket_chat -client <server_ip> <port>
  Exampe : ./Socket_chat -client 127.0.0.1 12346

Note: Server need to be run first

**TODO**
- Fix the issue the fget() return immediately when the stdin buffer have a newline character, which is added by user when program is blocked in read().
