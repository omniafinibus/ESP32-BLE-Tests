# ESP32-BLE-Tests
A collection of tests for the ESP32 microchip focussing on BLE connectivity

# Native baudrate tests & NimBLE baudrate tests

## Setup
a set of esp32s are is setup as a client and a server.

The server send a specific amount of packages per second (baudrate) which increases every second.
The package contains the server ID and the number of packages send, this is used as check to confirm if all packages are received at the end of a second.
The server and client were about 50cm from each other, with limited extra Bluetooth devices around (see image)

The client keeps track of how many packages have been received.
If the received baudrate changes (data in packet changes) it compares the received amount of packages with the expected amount of the previous second.
If they are equal it was a success. This will keep going until a baudrate of 1000
This test was run twice.

## Conclusion:
baud 1 has an error in the code which causes it to be unsuccessful. 
however given that the following baudrates are successful, it is fair to say that a baudrate of 1 is also successful.
test 1 started having problems after a baudrate of ~500.
The server actually crashed at a baudrate of 507.
test 2 started having problems around the same baudrate.
The server crashed again but now at a baudrate of 514.

This means that a maximum send delay equals 2ms if only 1 server is connected.
due to the instability at the baudrate of 500 is recommend to stay below baudrate of 450.

Although it is clear that the server crashed before the client did, I can not conclude if the client has a greater receive capacity, than the server has a send capacity.

## Further testing:
- Will a greater distance cause the connection to be less stable at equal or lower baudrates?
- How many servers can connect to a single client with a baudrate of 1?
- what is the maximum baudrate when the maximum amount of servers are connected?

# NimBLE response time tests

## Setup:
Tested the response time between the action of sending new sensor data to the client from a server,
and receiving and processing said data.
- client: ESP32 doit devkit v1
- server: ESP32 doit devkit v1

Upon sending sensor data, the server sets a pin to high to notify the oscilloscope that the data has been send.
The client does the same once it receives that data.

## Conclusion:
The response time is 50ms which is slower than specified in the requirements.
Unfortunately being stuck with NimBLE or BLE libraries  it is not possible to change the BLE stack to improve this.

## Further testing:
- Test the same setup with multiple connection (max 8 servers)
- Test influence of faster input
- See which improvements could be made to the ESP32 firmware (i.e. clock speed)
- Test the same setup with a newer ESP32 board (i.e. ESP32-S3 devkit)

# NimBLE connection count tests

## Setup:
Test to check how many servers can be connected to 1 client and if it is stable
client: ESP32-S3-DevKitC-1
server: ESP32 DOIT devkit v1
        ESP32-DevKit-1 v1.0

8 ESP32's have firmware which simulates a server, each sends a new sensor value packet every 250 ms
1 ESP32-S3 is setup as a client and sends its data and state to a serial port

This test is to see if the BLE stack can handle multiple connections and if it is stable.
A nice extra which was tested during testing was how the client and servers handled connections 
and disconnection routines.

## Conclusion:
The BLE stack held up, although it crashed during 1 disconnect, this is expected to be due
to a bug which causes multiple servers to connect to the same client channel. 
This bug also causes multiple servers to stack on top of each other, causing the reading to be incorrect
thinking a sensor value belongs to a server with a different ID.

One more important note is that the client ESP became relatively hot, either cooling is required or
this could be reduced by changing BLE settings.

## Further testing:
- Test max baudrate with multiple connected servers.
- Fix assigned client bug