TLSR8208B 

Example of a BLE button with a thermometer on TLSR8208B (16KB RAM, 128KB Flash)

<img width="347" height="309" alt="image" src="https://github.com/user-attachments/assets/38d35b24-377f-41cd-979d-ff2da4af2d78" />

* Button: GPIO_PD2, Led: GPIO_PD6
* Beacon format - BTHome v2 (binkey supported)
* Transferring button presses and releases with a counter, SoC temperature and battery parameters
* Connect: Pin-Code supported 
* Telink OTA supported
* Average consumption in operating mode is 6 µA.

## Button

Short press of the button - transmission of the event of pressing and releasing. Switches to fast beacon mode for 80 seconds to connect.

Holding the button for 15 seconds resets settings.


<img width="329" height="566" alt="image" src="https://github.com/user-attachments/assets/1e64b91f-387a-44bb-b268-280bee160c3d" />

<img width="484" height="279" alt="image" src="https://github.com/user-attachments/assets/fe3c06e4-ec1d-49fb-ab6a-2b53a6e8f2b6" />

