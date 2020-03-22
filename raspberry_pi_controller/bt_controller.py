import bluetooth
import time

# The bluetooth address for the door module
bd_addr = "14:41:05:05:88:77"

port = 1
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

try:
    sock.connect((bd_addr, port))
except:
    raise("Could not connect to bt_addr: " + bd_addr)

print("Connected to HC-05 module, bt address: " + bd_addr)

sock.send("0x1")    # Send the connection event to the arduino
data = sock.recv(4).decode()
if not "OK" in data:
    print("NO OK FROM CONNECTION")
    sock.close()
    exit()

print("Start!")

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
