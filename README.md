# MHZ19B-PMW-Wemos-D1-mini-Wemos-0.66-inch-OLED-Shield-MQTT-and-node-RED
This CO2 sensor, connect to a MQTTbroker and publish the CO2 level to certain Topic. The MQTT broker is hivemq.com, a free and open (no authentication needed) broker, this is very useful for demo purposes where you can use your smartphone in "tethering mode" as an Acces Point or you can use your own free MQTT Broker like Mosquito. The CO2 level also will be visible on a OLED display and a RGB led.

Clients can subcribe to that Topic and receive the CO2 level. This is done by using a "MQTT out" node in node-RED.
The node-RED flow creates a simple dashboard and gives audio warnings using Text To Speak nodes.

There are 2 ways to read the data from this CO2 sensor; that is UART or PWM. This CO2 is connected to a Wemos D1 mini and uses pin D5 for the PWM reading.
I compared this sensor with 2 other certified CO2 sensors and find a difference of 60-80 ppm which a think is pretty good for this cheap sensor.



![GitHub Logo](/images/logo.png)
MHZ19B CO2 sensor:
<image>www.tinytronics.nl/shop/image/cache/data/product-2453/MH-Z19B%20sensor_1-1000x1000.jpg</image>


MH-Z19B user's manual:
https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf
