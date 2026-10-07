Library defines communication protocol between CLI and Daemon
If contract is respected, All send and recv have a time out
of IPC_TIMEOUT_SEC seconds.

ipc.h
	ipc_accept() automatically configure new socket with timeout
	ipc_connect() automatically connects to daemon and configure socket with timeout.
	ipc_send() sends entire payload with timeout
	ipc_recv() receives entire payload with timeout
	
ipc.c
	ipc_accept() - Accepts new socket using provided server fd.
		Timeout is defined as IPC_TIMEOUT_SEC in header
	ipc_connect() - Creates client socket, then connects to Daemon
		using Path defined as SOCKET_PATH in header, and sets timeout configuration
	ipc_send() - first sends a header with command and payload_len
		then it sends the payload
	ipc_recv() - First expects a header with command and payload_len
		Then it recv the expect length of payload, saves payload using
		malloc and stores it in the payload argument