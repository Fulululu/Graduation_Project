# Remote Measurement and Control System
This project is my graduation design. A total of three parts.

## I. Environment:
#### Wireless Sensor network
ZStack2.5.1 on CC2530 board with BH1750,SHT10,DS18B20 and GY-69 using IAR Embedded Workbench7.3 for 8051.
#### TCP Communication
Server: Linux on Cloud Platform using python3.6.
Client: Raspbian on Raspberry_Pi_3B+ using Python with pyserial modules.
#### Web Service
Linux on Cloud Platform using Django2.0.4 with django-el-pagination plugin.

  WHAT YOU NEED IS:
  1. copy server directory to your server.
  2. sudo apt install python3.6 (on your server)
  3. python(any version) -m pip install django django-el-pagination.(on your server)
  4. copy raspi directory to your raspberry pi
  5. pip install pyserial (on your raspberry pi)(note that pip is a softlink to python2.7 default)
  6. As for zigbee sensor environment, see other Internet blogs.


## II. Start to Use
Please follow this main startup step: Web service->myserver_asy->myclient->Zigbee Nodes

#### Wireless Sensor Network Detail
1. Open project file
2. Click Menu Project->Edit_Configurations, choose "EndDeviceEB" for Sensor device(choose "CoordinatorEB" for coordinator device)
2. Click Menu Project->clean
3. Click Menu Project->Rebuild_all
4. Click Menu Project->Download_and_Debug
5. Let it run

#### TCP Communication Detail
Server:
1. cd server/socket/
2. python3.6 myserver_asy.py

Client:
1. cd raspi/
2. python myclient.py
3. type your account and password that register in Web server

#### Web Service Detail
1. add your E-mail to EMAIL_HOST_USER and EMAIL_HOST_PASSWORD in server/mysite/mysite/setting.py
2. cd server/mysite/
3. python manage runserver yourserverip:yourserverport
4. open your browser and type the url(ip:port)
5. using "python keygen_client.py" to generate an invitation code
6. register an account and login to use.
