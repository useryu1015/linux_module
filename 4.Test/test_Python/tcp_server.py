import socket

def start_server(host, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)

    print(f"Server listening on {host}:{port}")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address}")

        data = client_socket.recv(1024)
        if not data:
            break

        print(f"Received data: {data.decode('utf-8')}")

        client_socket.sendall(b"Server received the data")

    server_socket.close()

if __name__ == "__main__":
    server_host = "127.0.0.1"  # Replace with your desired IP address
    server_port = 8888          # Replace with your desired port
    start_server(server_host, server_port)
