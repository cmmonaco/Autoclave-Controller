# Autoclave Controller

An Arduino based controller for a steam sterilizing autoclave. The Model YX-18LM autoclave does not have any way to automatically regulate it's pressure and so must be regularly switched on and off by unplugging during sterilization. This controller is designed to measure and automatically regulate the pressure of the autoclave to remain at a user specified set point. It does not perform any sophisticated set point control, rather simply switches on and off a relay when pressure is below or above a specific range about the set point. The system is designed with a two-way switch to allow for bypass of the controller.

![Autoclave Controller In Action](https://lh3.googleusercontent.com/hjhUSDKN_Y_cjSZmYmEHPZeCOFlvvOUpz-FHKWYV53bZSQbXxgmAei8X3QrIAqUkm0drJ3cwH7VtLmgw1raXyXeYr3Oxhp6kxUo6TuM4z51gJp8w61oKJByo7g7_NEoKmOwB-_R0HAToWkwO1At6xwWevC3AuSjn53LfpFbMMmSup_wq6CdP-S25GLlfyJ2VpITqRvQwS9_Px7Wf8xi-ZKRFh5qqMU0Q5PT7ExwCv9gGXLo8xJUPd5PZrIt17u-XU93_18fRr2WvXXfdfuqRd2pzfPcFpTpkt3yrDqW1E8HrFeoGlJpH_nGsTcqegl0AaKnGNsdQJSXW6mFzBrvUuRaXvVTBbGxb8Y3BUtNDJ6VaKKQHdPeKWYwfGTJQ9CS8U5CHX6ScHBjzB2_L7yWreHorWQnWlkOuv93pX8pVV2hwtvaz-Ejdb7IYytRFL5TzQAJkw13s4VewyuFzq2NgY1muBBKGji30ZThiGB2pYJlRU7Eq-zVAmkSTORWnaNcJfXz5SNLa3kUkG9ViWfh5QwbvtmGpnqIoOue1MAv551gazrK9cyQsvzA3PFZUH1xDCm7nXYDy9o7iEHe3fGBGEk-76Cb0x4T-5E4_qcAMYG2p74GdxstdC34UJsh7Xm-YRZuYjBIYBkRmeySvNmAaSXvGSw=w1274-h956-no)

## Directories in this Repository

- `src`: contains the Arduino source code. See README in scr directory for more details.
- `enclosure`: contains files and reference related to hardware.

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
