import sys
import socket
import threading

def receive_data(client_socket):
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print(f"Received from server: {data.decode('utf-8')}")

def send_data(client_socket, message):
    client_socket.sendall(message.encode('utf-8'))

def main():
    # 服务器的 IP 地址和端口
    server_host = "192.168.1.149"
    server_port = 502

    # 创建套接字
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # 连接到服务器
    client_socket.connect((server_host, server_port))

    # 启动接收线程
    receive_thread = threading.Thread(target=receive_data, args=(client_socket,))
    receive_thread.start()

    # 循环发送消息
    while True:
        user_input = input("Enter message to send (or 'exit' to quit): ")
        if user_input.lower() == 'exit':
            break
        # 发送消息给服务器
        send_data(client_socket, user_input)

    # 关闭套接字和线程
    client_socket.close()
    receive_thread.join()


def test():
    # 服务器的 IP 地址和端口
    server_host = "192.168.1.149"
    server_port = 502

    # print(f"Received from server: {data}")

    # 要发送的消息
    message_to_send = "Hello, server! This is the client."

    # 调用函数发送消息给服务器
    send_data(server_host, server_port, message_to_send)




if __name__ == "__main__":
    main()