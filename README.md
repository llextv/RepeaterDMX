# DMX Recorder -> 1 Universe DMX Recorder & Repeater

A DIY DMX recorder and repeater designed to record and replay one complete DMX universe.

This project provides a compact, low-cost and reproducible solution capable of recording DMX lighting shows to a microSD card and replaying them through a DMX output.

The system also includes LTC timecode input, allowing recorded lighting shows to be synchronized with an external show-control or audio system.

## Why am I doing this?
First of all, I would like to ensure show redundancy in the event of a PC crash; we would record the show on this device (1 universe) and be able to replay it as a backup measure. The primary goal is to guarantee show redundancy, while the secondary objective is to provide redundancy and prototyping capabilities for a larger upcoming project.

cf: end of file !

---

# Overview

## Features

- Fully 3D printable enclosure
- Low-cost and easily reproducible design
- 1 DMX universe input
- 1 DMX universe output
- DMX repeater functionality
- DMX show recording to microSD
- DMX show playback from microSD
- LTC timecode input
- Timecode-synchronized show playback
- Standard DMX XLR connectors
- USB-C power and programming
- Open-source hardware and software

---

# Hardware

The complete Bill of Materials is available in:

[BOM.csv](BOM.csv)
(and at end of the file)
---

# Mechanical Design

The enclosure was designed to be fully 3D printable, allowing easy reproduction and customization.

## Case
![alt text](Pictures/image-1.png)
![alt text](Pictures/image-3.png)

## CAD Files
![alt text](Pictures/image-1.png)
[text](cad/AssemblyBoxRepeater.step)
[text](cad/BoxRepeater.step)
[text](cad/TopBoxRepeater.step)


---

# Electronic Design

## Schematic
![alt text](Pictures/image.png)
[text](Scheme/RepeaterDMX.kicad_sch)

---

# Firmware
This is firmware:
![alt text](Pictures/soft.png)
[text](Software/repeaterDMX/repeaterDMX.ino)


# BOM:
| Categorie     | Article                                 | Quantite | Prix_Unitaire_EUR | Prix_Unitaire_USD | Prix_Total_EUR | Prix_Total_USD | Notes     | URL                                                                |
| ------------- | --------------------------------------- | -------: | ----------------: | ----------------: | -------------: | -------------: | --------- | ------------------------------------------------------------------ |
| Controls      | ESP32-S3 Devkit                         |        1 |              5.99 |              6.97 |           5.99 |           6.97 |           | [AliExpress](https://fr.aliexpress.com/item/1005009330328373.html) |
| Communication | MAX485 Module RS-485                    |        2 |             1.295 |              1.51 |           2.59 |           3.01 |           | [AliExpress](https://fr.aliexpress.com/item/1005005737922222.html) |
| Connectique   | XLR 3-Pin Panel Mount Connector         |        3 |             0.247 |              0.29 |           0.74 |           0.86 |           | [AliExpress](https://fr.aliexpress.com/item/32994485419.html)      |
| Storage       | Micro SD Module                         |        1 |              0.83 |              0.97 |           0.83 |           0.97 |           | [AliExpress](https://fr.aliexpress.com/item/1005009224403161.html) |
| Storage       | 32GB Micro SD Card                      |        1 |              4.89 |              5.69 |           4.89 |           5.69 |           | [AliExpress](https://fr.aliexpress.com/item/1005012294704811.html) |
| Connectique   | Panel Mount USB-C Connector             |        1 |              1.79 |              2.08 |           1.79 |           2.08 |           | [AliExpress](https://fr.aliexpress.com/item/1005009101248402.html) |
| Prototype     | Wire                                    |        1 |              0.88 |              1.02 |           0.88 |           1.02 |           | [AliExpress](https://fr.aliexpress.com/item/1005001698292417.html) |
| LTC           | LM393 Comparator Module                 |        1 |              1.24 |              1.44 |           1.24 |           1.44 |           | [AliExpress](https://fr.aliexpress.com/item/1005012824868572.html) |
| LTC           | 600:600 Ohm Audio Isolation Transformer |       10 |             0.429 |              0.50 |           4.29 |           4.99 | 10 pieces | [AliExpress](https://fr.aliexpress.com/item/1005006340217108.html) |
| Prototype     | PCB Board                               |        1 |              3.49 |              4.06 |           3.49 |           4.06 |           | [AliExpress](https://fr.aliexpress.com/item/1005007977006793.html) |
| **TOTAL**     |                                         |          |                   |                   |      **26.73** |      **31.08** |           |                                                                    |


# How DMX works

## What is DMX?

**DMX512** is a digital communication protocol commonly used in stage lighting to control lighting fixtures such as moving heads, LED bars, dimmers, lasers and other lighting equipment.

A DMX universe contains up to **512 control channels**.

Each channel contains a value from **0 to 255**:

* `0` = minimum value
* `255` = maximum value

For example, a simple RGB fixture could use three DMX channels:

```text
Channel 1 → Red
Channel 2 → Green
Channel 3 → Blue
```

If the controller sends:

```text
Channel 1 = 255
Channel 2 = 0
Channel 3 = 128
```

the fixture would display a strong red/magenta color depending on how its RGB mixing is implemented.

More complex fixtures use many more channels. A moving head could, for example, use channels for:

```text
Channel 1 → Pan
Channel 2 → Pan fine
Channel 3 → Tilt
Channel 4 → Tilt fine
Channel 5 → Dimmer
Channel 6 → Shutter
Channel 7 → Red
Channel 8 → Green
Channel 9 → Blue
...
```

The exact channel assignment depends on the fixture's DMX mode.

---

## How DMX is physically transmitted

DMX512 normally uses an **RS-485 physical layer**.

RS-485 uses a differential signal, meaning that the data is transmitted using the voltage difference between two wires.

A typical DMX connection uses:

```text
DMX XLR
Pin 1 → Ground / Shield
Pin 2 → Data -
Pin 3 → Data +
```

The DMX signal is transmitted at **250 kbit/s**.

A DMX frame contains a start condition followed by the channel data.

Conceptually, a complete DMX universe looks like:

```text
┌────────────┬──────────────┬──────────────┬──────────────┬─────┬──────────────┐
│   BREAK    │ START CODE   │  CHANNEL 1   │  CHANNEL 2   │ ... │ CHANNEL 512  │
└────────────┴──────────────┴──────────────┴──────────────┴─────┴──────────────┘
```

The **start code** is normally `0` for standard lighting data.

The following bytes represent the values of the DMX channels.

---

# How this project receives DMX

The recorder has a dedicated **DMX input**.

The signal goes through several stages:

```text
DMX XLR INPUT
      │
      │ Differential RS-485 signal
      ▼
┌──────────────┐
│   MAX485     │
│ RS-485 → TTL │
└──────┬───────┘
       │
       │ Serial data
       ▼
┌──────────────┐
│   ESP32-S3   │
│              │
│ DMX receiver │
└──────┬───────┘
       │
       ├──────────────► DMX OUTPUT
       │                 │
       │                 ▼
       │              MAX485
       │                 │
       │                 ▼
       │              DMX XLR
       │
       ▼
   microSD card
```

The **MAX485** converts the electrical RS-485 differential signal into a logic-level serial signal that the ESP32 can read.

The ESP32 then interprets the incoming DMX frames.

---

# How a DMX universe is represented in the ESP32

The ESP32 maintains an array containing the current value of all DMX channels.

Conceptually:

```cpp
uint8_t dmx[512];
```

The array represents:

```text
dmx[0]   → DMX channel 1
dmx[1]   → DMX channel 2
dmx[2]   → DMX channel 3
...
dmx[511] → DMX channel 512
```

When a new DMX frame arrives, the ESP32 updates this array.

For example:

```text
DMX channel 1   = 255
DMX channel 2   = 128
DMX channel 3   = 0
DMX channel 4   = 42
...
DMX channel 512 = 255
```

The ESP32 therefore always knows the current state of the entire DMX universe.

---

# How the DMX repeater works

The device can also operate as a **DMX repeater**.

In repeater mode, the ESP32 receives the DMX data from the input and sends the same data to the DMX output.

The basic signal path is:

```text
Lighting Controller
       │
       │ DMX
       ▼
┌───────────────┐
│ DMX Recorder  │
│               │
│   MAX485      │
│      ↓        │
│   ESP32-S3    │
│      ↓        │
│   MAX485      │
└───────┬───────┘
        │
        │ DMX
        ▼
 Lighting Fixtures
```

This means the recorder can be inserted between a lighting console/controller and the fixtures without changing the rest of the DMX installation.

---

# How DMX recording works

The important part of this project is that I do **not simply record the electrical waveform**.

Instead, the ESP32 decodes the DMX data and records the **DMX channel values together with their timing**.

For example, imagine that the controller sends:

```text
Time      CH1    CH2    CH3    CH4
0.000 s   0      0      0      0
0.020 s   10     0      0      0
0.040 s   20     0      0      0
0.060 s   30     0      0      0
0.080 s   40     0      0      0
```

The recorder can store these changes on the microSD card.

Conceptually, the recording becomes:

```text
Timestamp       DMX data
--------------------------------
0 ms            [0,0,0,0,...]
20 ms           [10,0,0,0,...]
40 ms           [20,0,0,0,...]
60 ms           [30,0,0,0,...]
80 ms           [40,0,0,0,...]
```

The exact file format is part of the firmware implementation.

The important information is:

1. **The DMX channel values**
2. **When those values occurred**

This allows the original lighting sequence to be reconstructed later.

---

# Why timing is important

Recording only the final state of the 512 channels would not be enough.

For example:

```text
0 s  → Dimmer = 0
1 s  → Dimmer = 255
2 s  → Dimmer = 0
```

contains completely different information from:

```text
0 s     → Dimmer = 0
0.1 s   → Dimmer = 255
0.2 s   → Dimmer = 0
```

The channel values are the same, but the lighting effect is completely different.

Therefore, the recorder needs to preserve the timing between DMX states.

---

# Recording to the microSD card

When recording is enabled, the ESP32 receives DMX frames and stores the relevant information on the microSD card.

The general process is:

```text
DMX Frame
    │
    ▼
ESP32 receives frame
    │
    ▼
Decode DMX channels
    │
    ▼
Read current timestamp
    │
    ▼
Store DMX data + timing
    │
    ▼
microSD card
```

The microSD card is therefore used as the persistent storage for the recorded show.

A recording can contain a complete sequence of DMX states, potentially lasting several minutes or longer depending on the selected recording format and available storage.

A 32 GB card provides significantly more storage than is required for a typical one-universe recording.

---

# How playback works

During playback, the process is reversed.

The ESP32 reads the recorded data from the microSD card and reconstructs the DMX stream.

```text
microSD card
     │
     │ Recorded DMX data
     ▼
  ESP32-S3
     │
     │ Reconstructed DMX frames
     ▼
   MAX485
     │
     │ RS-485 / DMX
     ▼
DMX OUTPUT
     │
     ▼
Lighting fixtures
```

The ESP32 waits for the appropriate timestamp and sends the corresponding DMX values to the output.

For example:

```text
Recording:

0 ms     → CH1 = 0
20 ms    → CH1 = 10
40 ms    → CH1 = 20
60 ms    → CH1 = 30
80 ms    → CH1 = 40
```

During playback, the ESP32 reproduces the same sequence:

```text
0 ms     → CH1 = 0
20 ms    → CH1 = 10
40 ms    → CH1 = 20
60 ms    → CH1 = 30
80 ms    → CH1 = 40
```

The lighting fixtures therefore see essentially the same DMX sequence that was originally recorded.

---

# LTC Timecode

The recorder also includes an **LTC (Linear Timecode) input**.

LTC is commonly used to synchronize different systems during a show, for example:

```text
Audio
Video
Lighting
Show control
Recording systems
```

Instead of simply starting a recording from time `0`, the DMX show can be associated with a timecode position.

For example:

```text
LTC: 01:23:45:00
        │
        ▼
DMX show position
        │
        ▼
Recorded lighting state
```

During playback, the recorder can use the incoming LTC timecode to determine which position of the recorded show should currently be played.

This makes it possible to synchronize the lighting playback with another system.

---

# Complete system architecture

The complete project can therefore be represented as:

```text
                    RECORDING

       DMX Controller / Lighting Console
                     │
                     │ DMX
                     ▼
              ┌─────────────┐
              │   MAX485    │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │   ESP32-S3  │
              │             │
              │ DMX decoder │
              │   + timing  │
              └──────┬──────┘
                     │
                     ▼
                ┌─────────┐
                │ microSD │
                └─────────┘


                     PLAYBACK

                ┌─────────┐
                │ microSD │
                └────┬────┘
                     │
                     ▼
              ┌─────────────┐
              │   ESP32-S3  │
              │             │
              │ DMX player  │
              └──────┬──────┘
                     │
                     ▼
              ┌─────────────┐
              │   MAX485    │
              └──────┬──────┘
                     │
                     │ DMX
                     ▼
                DMX OUTPUT
                     │
                     ▼
              Lighting fixtures
```

With LTC synchronization:

```text
                LTC SOURCE
                    │
                    ▼
              LTC input stage
                    │
                    ▼
                ESP32-S3
                    │
                    ▼
          Synchronize playback
                    │
                    ▼
              DMX OUTPUT
```

---

# Why this is useful for show redundancy

The main purpose of this project is **show redundancy**.

A computer running lighting software can potentially fail because of:

* Software crashes
* Operating system issues
* USB/network problems
* Driver problems
* Power problems
* Hardware failure

With this recorder, a DMX show can be recorded beforehand.

The primary lighting system can then remain the normal setup:

```text
PC / Lighting Console
        │
        ▼
     DMX system
        │
        ▼
     Fixtures
```

while the recorder provides a backup:

```text
             ┌───────────────┐
             │ Main lighting │
             │    system     │
             └───────┬───────┘
                     │
                     ▼
                  DMX line
                     │
                     ▼
              ┌─────────────┐
              │ DMX Recorder│
              └──────┬──────┘
                     │
                     ▼
                 Fixtures
```

If the main system fails, the recorded show can be played back from the recorder instead.

The project is therefore intended as a **low-cost hardware backup for one DMX universe**, rather than as a replacement for a complete professional lighting console.

---

# Limitations

This version only handles **one DMX universe**, meaning a maximum of 512 DMX channels.

It is therefore suitable for relatively small lighting systems or for backing up a specific universe of a larger lighting system.

A future version could potentially support:

* Multiple DMX universes
* Ethernet / Art-Net input
* sACN input
* Larger show storage
* More advanced timecode synchronization
* Automatic failover
* Multiple playback scenes
* Trigger inputs
* A user interface for selecting recordings
