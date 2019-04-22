# ESP32_SPS30_DashBoard

ESP32 with sensors SPS30, BME680 and OLED SSH1106 output + DashBoard WebServer + [Logging to ThingSpeak](https://thingspeak.com/channels/763363/)

## How to use

```cmd
git clone --recursive  https://github.com/mrchoke/ESP32_SPS30_DashBoard
```

## Add to library

copy director `Arduino_ESP32_SPS30` to `arduino/libraries/`

```cmd
cp -r Arduino_ESP32_SPS30 ARDUINO_PATH/libraries/
```

## Hardware / Pins

- SPS30 - pins 16 , 17 ( 5V , GND )
  - UART TX -> IO #16
  - UART RX -> IO #17
  - 5V
  - GND
- SSH1106 - pins:
  - GND
  - 5V (VCC)
  - CLK -> IO #18
    - Clock
  - MOSI -> IO #23
    - Master Out Slave In (the data line).
  - RES -> IO #32
    - This is not actually an SPI Pin and can be set to any pin.
    - Reset
  - DC -> IO #33
    - This is not actually an SPI Pin and can be set to any pin.
    - Data Command
  - CS ->  IO #5
    - This is not actually an SPI Pin and can be set to any pin.
    - Cable Select
- BME680
  - 21
  - 22
  - 3V
  - GND



## Terms

**PM** = Particle Mass (μg/m³)(micrograms per cubic meter)

**NC** = Number Concentration (#/cm³)(count per cubic centimeter)

## Science and Guidelines on Air Quality

The World Health Organization recommends limiting particulate matter to the following values:

- Annual average PM10 20 µg/m³
- Annual average PM2.5 10 µg/m³ per year
- Daily average PM10 50 µg/m³ without permitted days on which exceeding is possible.
- Daily average PM2.5 25 µg/m³ without permitted days on which exceeding is possible

see also: [Air Quality Index](https://en.wikipedia.org/wiki/Air_quality_index)
- https://www.who.int/airpollution/infographics/en/
- https://www.who.int/airpollution/en/