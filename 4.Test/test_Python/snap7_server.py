import snap7
import threading
import time

class Snap7Server:
    def __init__(self, rack=0, slot=2, port=102, area=0x84, db_number=1):
        self.server = snap7.server.Server()
        self._set_server_params(port)

        # 设置对等地址
        self.server.PeerAddress = '127.0.0.1'

        # 创建并启动服务器线程
        self.server_thread = threading.Thread(target=self.run_server)

    def _set_server_params(self, port):
        """设置服务器参数"""
        self.server.TCP_Port = port

    # def add_cyclic_address_area_bak(self, area, db_number, start, size, interval):
    #     """添加一个S7连接"""
    #     self.server.register_area(area, db_number, self.cyclic_callback, start, size)
    #     self.server.set_cyclic_callback(area, interval, None)

    def add_cyclic_address_area(self, area, db_number, start, size, interval, user_data):
        """添加一个S7连接"""
        # 使用 lambda 函数将 user_data 传递给回调函数
        self.server.register_area(area, self.cyclic_callback, start, size)
        self.server.set_cyclic_callback(area, interval, None)

    def cyclic_callback(self, area, db_number, start, size, data):
        """循环调用的回调函数"""
        # 在这里添加对数据的处理逻辑
        pass

    def start_server(self):
        """启动服务器"""
        self.server_thread.start()

    def run_server(self):
        self.server.start()

    def stop_server(self):
        """停止服务器"""
        self.server.stop()
        self.server_thread.join()

if __name__ == "__main__":
    # 在需要的地方导入 Snap7Server 类
    snap7_server = Snap7Server()
    # snap7_server.add_cyclic_address_area(0x84, 1, 0, 256, 1000)  # 1000 毫秒更新一次
    snap7_server.add_cyclic_address_area(0x84, 1, 0, 256, 1000, 10086)  # 1000 毫秒更新一次

    try:
        # 启动服务器
        snap7_server.start_server()

        # 让服务器运行一段时间
        time.sleep(30)
    except KeyboardInterrupt:
        pass
    finally:
        # 停止服务器
        snap7_server.stop_server()
