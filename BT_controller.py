import bluetooth
import time

bd_addr = "14:41:05:05:88:77"

#nearby_devices = bluetooth.discover_devices()
##print(nearby_devices)

port = 1
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((bd_addr, port))
print("Connected!")
#sock.settimeout(1.0)

time.sleep(5)
sock.send("0x1")
data = sock.recv(4).decode()
if not "OK" in data:
    print("NO OK FROM CONNECTION")
    exit()
time.sleep(5)

print("Start")
while True:
    data = sock.recv(3).decode()
    print("Incoming: " + str(data))

    if "0x2" in data:
        sock.send("OK\r\n")
        print("Outgoing: OK")
    elif "0x3" in data:
        sock.send("OK\r\n")
        print("Outgoing: OK")
    elif "0x4" in data:
        sock.send("OK\r\n")
        print("Outgoing: OK")


sock.close()



#if __name__ == "__main__":
