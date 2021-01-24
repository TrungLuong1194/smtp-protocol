# smtp_protocol

## SMTP Fundamentals 
SMTP is an application layer protocol. The client who wants to send the mail opens a TCP connection to the SMTP server and then sends the mail across the connection. The SMTP server is always on listening mode. As soon as it listens for a TCP connection from any client, the SMTP process initiates a connection on that port (25). After successfully establishing the TCP connection the client process sends the mail instantly.

## SMTP Client
- Using poll() api. It waits for one of a set of file descriptors to become ready to perform I/O.
- How to apply poll():
	+ standard input's file description => Send data.
	+ socket's files description => Receive data.
	
## SMTP Server
- Using poll() api for: Handling multiple clients on server without multithreading.
- Each file description corresponding to each client.
- Create Maildir for store mails.

## Documents
- https://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
- https://blog.mailtrap.io/smtp-commands-and-responses/
- https://en.wikipedia.org/wiki/Maildir
- https://www.systoolsgroup.com/maildir/
- https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
- https://man7.org/linux/man-pages/man2/poll.2.html
- https://www.geeksforgeeks.org/simple-mail-transfer-protocol-smtp/
