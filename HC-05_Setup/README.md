# Steps for setup HC-05 module in command mode

1. Setup hardware according to schematics [here](schematics.png).
2. Flash the arduino nano with an empty project (Don't forget to disconnect rx and tx cables while flashing).
3. Start an serial monitor with baudrate 38400 with new line and carrige return enabled.
4. Test to send "AT", you should receive "OK" if everything is connected successfully.


# Retreive module BT address

1. run the command "AT+ADDR?".
2. You will receive the bluetooth address of the module, type down the address of the slave.


# Change the name of the BT module

1. To receive the current name run the command "AT+NAME?"
2. To change the name run the command "AT+NAME=<Param>", where <Param> is the name.


# Change the module role (master/ slave)

1. Run the command "AT+ROLE?" to see the current role
2. Run the command "AT+ROLE=<Param>", where <Param> is the current role. 0 = Slave, 1 = Master


# Change the BT pin code

1. To receive the current pin code of the module run the command "AT+PSWD?"
2. To change the pin code run the command "AT+PSWD=<Param>", where <Param> is the new 4-digit pin code.


# Set connect mode/ bind to fixed address (Master)

1. Run the command "AT+CMODE=0" to set the module to connect to a fixed BT address.
2. Run the command "AT+BIND=<Param>", where <Param> is the slave modules BT address with "," instead of ":".
