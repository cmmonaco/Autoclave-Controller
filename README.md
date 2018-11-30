# Autoclave Controller

An Arduino based controller for a steam sterilizing autoclave. The Model YX-18LM autoclave does not have any way to automatically regulate it's pressure and so must be regularly switched on and off by unplugging during sterilization. This controller is designed to measure and automatically regulate the pressure of the autoclave to remain at a user specified set point. It does not perform any sophisticated set point control, rather simply switches on and off a relay when pressure is below or above a specific range about the set point. The system is designed with a two-way switch to allow for bypass of the controller.

## Directories in this Repository

- `src`: contains the Arduino source code. See README in scr directory for more details.

## Bill of Materials

### Electronics

- Arduino Uno R3
- 16x2 Character LCD
- 10k Potentiometer
- Pushbutton
- Heavy Duty Two Way Switch
- [Pressure Transducer](https://www.amazon.com/gp/product/B00RCPDPRQ/ref=oh_aui_detailpage_o07_s00?ie=UTF8&psc=1)
- [25A DC-AC Relay](https://www.amazon.com/gp/product/B0087ZTN08/ref=oh_aui_detailpage_o09_s01?ie=UTF8&psc=1)

### Enclosure

- [ABS Project Box](https://www.amazon.com/Hammond-1591ESBK-ABS-Project-Black/dp/B0002BSRIO/ref=sr_1_4?s=lamps-light&ie=UTF8&qid=1543547356&sr=1-4&keywords=project+box)
- 1/8" Acrylic