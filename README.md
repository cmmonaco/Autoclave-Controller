# Autoclave Controller

An Arduino based controller for a steam sterilizing autoclave. The Model YX-18LM autoclave does not have any way to automatically regulate it's pressure and so must be regularly switched on and off by unplugging during sterilization. This controller is designed to measure and automatically regulate the pressure of the autoclave to remain at a user specified set point. It does not perform any sophisticated set point control, rather simply switches on and off a relay when pressure is below or above a specific range about the set point.

## Directories in this Repository

- `src`: contains the Arduino source code. See README in scr directory for more details.

## Bill of Materials

### Electronics

- Arduino Uno R3
- [3.5" TFT Touchscreen](https://www.adafruit.com/product/2050)
- [Pressure Transducer](https://www.amazon.com/gp/product/B00RCPDPRQ/ref=oh_aui_detailpage_o07_s00?ie=UTF8&psc=1)
- [25A DC-AC Relay](https://www.amazon.com/gp/product/B0087ZTN08/ref=oh_aui_detailpage_o09_s01?ie=UTF8&psc=1)

### Enclosure