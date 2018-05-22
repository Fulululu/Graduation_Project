# Remote Measurement and Control System
This is a design for my graduation project. A total of three parts.

## Environment
#### Wireless Sensor network
ZStack2.5.1 on CC2530 board with BH1750,SHT10,DS18B20 and GY-69 using IAR Embedded Workbench7.3 for 8051.

#### TCP Communication
Server: Linux on Cloud Platform using python3.6.
Client: Raspbian on Raspberry_Pi_3B+ using Python with pyserial modules.

#### Web Service
Linux on Cloud Platform using Django2.0.4.

## Start
Main Startup Step:myserver_asy->Web server->Zigbee Nodes->myclient

#### Wireless Sensor Network Detail
1. Open project file
2. Click Menu Project->Edit_Configurations, choose "EndDeviceEB" for Sensor device(choose "CoordinatorEB" for coordinator device)
2. Click Menu Project->clean
3. Click Menu Project->Rebuild_all
4. Click Menu Project->Download_and_Debug
5. Let it run

#### TCP Communication Detail
Server:
python3.6 myserver_asy.py

Client:
1. python myclient.py
2. type your account and password that register in Web server

#### Web Service Detail
1. python manage runserver yourserverip:yourserverport
2. open your browser and type the url(ip:port)
3. using "python keygen_client.py" to generate an invitation code
4. register an account and login to use.