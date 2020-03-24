import bluetooth
import time

class BT_Controller:

    def __init__(self, bd_address):
        """"""
        self.bd_addr = bd_address
        self.port = 1
        self.sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        self.timeout = 20

        self.check_time = None

    def connect(self):
        """"""
        try:
            self.sock.connect((self.bd_addr, port))
            self.sock.settimeout(self.timeout)
            print("Connected to HC-05 module, bd_addr: " + bd_addr)
            return True
        except:
            print("Could not connect to bd_addr: " + bd_addr)
            return False

    def start_arduino_listening_for_door_status(self):
        """"""
        self.sock.send("0x1")   # Send the connection event to the arduino
        data = self.sock.recv(4).decode()
        if not "OK" in data:
            print("Arduino did not respond to 0x1...")
            return False
        else:
            print("Arduino responded with OK...")
            print("Arduino entered listening for door status...")
            return True

    def run(self):
        """"""
        self.check_time = time.time()

        while True:
            data = self.sock.recv(3).decode()
            print("Incoming: " + str(data))

            if "0x2" in data:
                self.sock.send("OK\r\n")
                print("Outgoing: OK")
            elif "0x3" in data:
                self.sock.send("OK\r\n")
                print("Outgoing: OK")
            elif "0x4" in data:
                self.sock.send("OK\r\n")
                self.check_time = time.time()
                print("Outgoing: OK")

            if (time.time() - self.check_time) > 20:
                print("Time exceeded 20 seconds... exiting...")
                return False

    def close(self):
        """"""
        self.sock.close()


if __name__ == "__main__":
    bd_addr = "14:41:05:05:88:77"       # The bluetooth address for the door module
    bt_ctrl = BT_Controller(bd_addr)
    if not bt_ctrl.connect():
        exit()
    if not bt_ctrl.start_arduino_listening_for_door_status():
        bt_ctrl.close()
        exit()
    if not bt_ctrl.run()
        bt_ctrl.close()
        exit()
