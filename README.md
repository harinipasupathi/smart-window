# smart-window
Hardware Components:
ESP32 Microcontroller
Function: Central controller to process sensor data and control window and blind operations.
Connectivity: Communicates with the cloud for remote data updates and commands.
L293D Motor Driver
Purpose: Controls two 12V DC motors for independent window and blind movement.
Adafruit BME280 Sensor
Measures: Temperature, humidity, and barometric pressure.
Purpose: Provides environmental data to automate window and blind operations for optimal indoor conditions.
BH1750 Light Sensor
Measures: Light intensity.
Purpose: Helps control blind positioning based on ambient light levels.
Two 12V DC Motors
Function:
Motor 1: Adjusts window position.
Motor 2: Controls blinds for light regulation.
12V Power Supply
Provides power to the motors and other components as needed.
Relay Modules
Purpose: Act as switches to control the operation of high-power components like motors based on commands from the ESP32.
Control Logic:
Automatic Mode:
Temperature & Humidity-Based Adjustment: Windows open or close based on predefined thresholds to maintain indoor air quality and temperature.
Light Intensity Adjustment: Blinds adjust based on ambient light to manage glare and natural light levels.
Remote Control via Cloud:
Users can monitor sensor data and manually control windows and blinds through a web application integrated with Firebase.
