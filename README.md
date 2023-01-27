# HollowClock
# An analog clock with flying hands controlled from internet-time via ESP8266 
https://github.com/xSnowHeadx/HollowClock

README file  
SnowHead Mar 2021  

![Clock](pictures/clock.jpg)
![Stand](pictures/stand.jpg)

## Introduction
Shiura on [thingiverse](https://www.thingiverse.com/thing:4781365) designed a fascinating hollow clock.<br>
The original is controlled by an Arduino. The firmware here was adapted to an ESP8266 and for the use of the [WTA-timeservice](http://worldtimeapi.org) instead of NTP. So there normally are no firmware-modifications necessary to adapt the timezone and DST because they will be investigated out of the public IP of the request. For special cases (f.e. the use of a foreign proxy server) the timezone can be selected manually by replacing "ip" with the wished [timezone](http://worldtimeapi.org/timezone) in WTAClient.cpp.<br>
<br>
##Used libraries:<br>
- ArduinoJson<br>
- DNSServer<br>
- DoubleResetDetector<br>
- ESP8266HTTPClient<br>
- ESP8266WebServer<br>
- ESP8266WiFi<br>
- WiFiManager<br> 

Some modified mechanic parts are described [here](https://www.prusaprinters.org/prints/140533-hollow-clock-remix-uses-internet-time-with-esp8266)<br>
See informations to arduino-based version on [thingiverse](https://www.thingiverse.com/thing:4781365)
