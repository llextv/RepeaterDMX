# DMX Recorder -> 1 Universe DMX Recorder & Repeater

A DIY DMX recorder and repeater designed to record and replay one complete DMX universe.

This project provides a compact, low-cost and reproducible solution capable of recording DMX lighting shows to a microSD card and replaying them through a DMX output.

The system also includes LTC timecode input, allowing recorded lighting shows to be synchronized with an external show-control or audio system.
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

---

# Mechanical Design

The enclosure was designed to be fully 3D printable, allowing easy reproduction and customization.

## Case
![alt text](Pictures/image-1.png)
![alt text](Pictures/image-3.png)

## CAD Files
[text](cad/AssemblyBoxRepeater.step)
[text](cad/BoxRepeater.step)
[text](cad/TopBoxRepeater.step)


---

# Electronic Design

## Schematic
[text](Scheme/RepeaterDMX.kicad_sch)

---

# Firmware
This is firmware:
[text](Software/repeaterDMX/repeaterDMX.ino)