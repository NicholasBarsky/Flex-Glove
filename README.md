# Flex-Glove
This is the repository of C++ code in arduino for using flex sensors and an analog gyro sensor on a esp32 microcontroller to control a rc car with digital pins

the way the code works is the sensors act as the triggers to move the RC car. when tilted left or tight with the gyro sensor it triggers the output to be turned on causing it to move. the same with the flex sensors on each finger. 

THe reason HIGH and LOW are reversed is due to the fact the old controller board had reverse output.
