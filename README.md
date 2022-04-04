# GearIndicator
Gear Indicator for Kawasaki and Suzuki bikes using communication with ECU over ISO9141 / K-Bus protocol

This project I implemented couple of years ago. We bought Kawasaki Z750 that time and needed to have gear indication
which apparently Kawasaki didn't have installed. About 30 of these were installed on different Kawasaki an Suzuki bikes
in next years. Just dont want to throw it away and forget. Maybe it will be useful for somebody also these days.

## Project structure
Project has hardware and software part. Content of directories as described below.

### &emsp;__Eagle
&emsp;Hardware scheme and PCB created in CadSoft Eagle (Autodek Eagle now).
&emsp;Indicator has 2 PCBs. DISP - PCB with 7 segment display and Controller. PWR - PCB with voltage regulator and ISO9141 interface IC.

### &emsp;__HWcomponents
&emsp;Datasheets of all HW components used. 

### &emsp;__Manuals
&emsp;We have created couple of installation manuals for Kawasaki and Suzuki bikes. Existing PDFs are here.

### &emsp;__SketchUp
&emsp;3D model of the final product. Prepared to be placed to the potting box. Final product with soldered cable was placed into potting box (type and data in _HWcomponents folder).
&emsp;Then Araldite expoxy was used for potting. Here you will find PCB 3D model as well as 3D model of the completed device.

### &emsp;GearIndicator
&emsp;Source code in C. Design is based on ATmega328p controller. Atmel Studio (Microchip Studio now) was used as a development IDE.




