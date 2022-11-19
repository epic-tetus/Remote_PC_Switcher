# Hardware

## Breadboard Layout
![](https://github.com/epic-tetus/Remote_PC_Switcher/blob/main/harware/Image/Breadboard_View.PNG)

## Parts
* Board: ESP32-DevKitC
* Transistor: 2N2222
* Diode: UF4007 [Not essential, But remcommand]

## Helpful Reference
https://www.instructables.com/arduino-controlled-pc-power-switch

## Idea

* ESP32's Power supply method: Motherboard's 5V pin
* Diverge Motherboard's Power SW+ & SW- pins
  - One of those cables will be connected with PC case's sw + & - pin
  - And the other cable will be connected with transistor's Collector & Emitter
* Using ESP32 control the transistor which connected with Power SW pins

[Pin Map of My MotherBoard (MSI B450M)](https://github.com/epic-tetus/Remote_PC_Switcher/blob/main/harware/M7B89v1.3-ASIA.pdf)


```

                       │MotherBoard (MSI B450M)
                       │ATX_PWR
                       │┌──────────────────┐
                       ││13│14│15│16│...│24│
                       │├──────────────────┤      
┌───────┐              ││ 1│ 2│ 3│ 4│...│12│      
│ ESP32 │              │└───────┬──┬───────┘
│       │              │    GND→│  │←5V
│    GND│───────────────────────┘  │ 
│     5V│──────────────────────────┘
│   CTRL│────────────┐                  
└───────┘           B⊼  
                  ＿/ ↘E
                  │C   └───┐
PC Case SW        │        │
         ─┴─ ┌───∈───▶┃──┴───────┐
       ┌─╴ ╶─┘    └─┬───────────┐  │
       └────────────┘   JFP1    │  │ 6: Power SW+, 8: Power SW-
                       │┌───────┴──┴───┐
                       ││ 2│ 4│ 6│ 8│  │
                       │├──────────────┤
                       ││ 1│ 3│ 5│ 7│ 9│
                       │└──────────────┘ 


```

## Connect pin to Motherboard
__~Sorry for my dusty motherboard~__

### 5V & GND
![Connection of ATX_PWR 5V Pin](https://github.com/epic-tetus/Remote_PC_Switcher/blob/main/harware/Image/ATX_PWR_PIN.jpg)

* __You need to confirm that pin is real 5V pin output using your multimeter.__ For saving your ESP32.
* I don't wanna change my power connector which already connected. So, I inserted a jumper cable into connector like the picture above.

### Power Switch Pins
![Branching Method of Power SW Pins](https://github.com/epic-tetus/Remote_PC_Switcher/blob/main/harware/Image/JFP1_PIN.jpg)

* Normally, Computer case's power sw cable need to connect with Motherboard's JFP1 [6|8].
* However, I diverged two cables, SW + & SW - , and connected them to the transistor.
