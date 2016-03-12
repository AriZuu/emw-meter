EMW-Meter
=========
This is a simple temperature display based on EMW3165 module and cheap SSD1306 OLED display.
It receives data with MQTT protocol (using potato-bus library) and displays
inside and outside temperatures along with weather forecast symbol. I have
also sensor that measures house electricity usage so that is displayed too.

In addition to current values, it collects history for some hours and
displays a trend/bar graph from it at bottom of display. There is
also a weather symbol that shows forecast for next 8 hours. Forecast
data comes from [Finnish Meteorological Institute][2].

Inside temperature is measured by DS1820.

After building firmware and loading it to WifiMCU, type
"help" at console prompt to get started. "sta" will connect to system
to existing access point. Incoming MQTT messaging is currently
not very configurable, ie. topic names and json attributes
are built into code and thus need to be modified to be useful
to someone else than me.

GPIO connections:

| Module Pin | Pin | GPIO                                    |
|------------|-----|-----------------------------------------|
| DS1820     | D3  | PB10                                    |
| OLED MOSI  | D1  | PA1    SPI4_MOSI                        |
|      -     | D14 | PA11   SPI4_MISO                        |
| OLED CLK   | D12 | PB13   SPI4_CLK                         |
| OLED CS    | D13 | PA5    (not really used by OLED module) |
| OLED C/D   | D7  | PA12                                    |
| OLED RST   | D15 | PB1                                     |
| SWDIO      | D6  |                                         |
| SWCLK      | D5  |                                         |
| RESET      | RST |                                         |

Weather forecast symbol font is created from files available at [fmidev github][1]

To build this following modules are needed:

* Pico]OS 
* picoos-micro
* picoos-micro-spiffs
* picoos-lwip
* picoos-ow
* wiced-driver
* cmsis-ports
* potato-bus
* eshell

[1]: https://github.com/fmidev/opendata-resources/tree/master/symbols
[2]: http://fmi.fi
