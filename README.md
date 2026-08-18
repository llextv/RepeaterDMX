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
| Categorie | Article | Quantite | Prix_Unitaire_EUR | Prix_Total_EUR | Notes | URL |
|---|---|---:|---:|---:|---|---|
| Controls | ESP32-S3 Devkit | 1 | 5.99 | 5.99 | | [AliExpress](https://fr.aliexpress.com/item/1005009330328373.html) |
| Communication | MAX485 Module RS-485 | 2 | 1.295 | 2.59 | | [AliExpress](https://fr.aliexpress.com/item/1005005737922222.html) |
| Connectique | XLR 3-Pin Panel Mount Connector | 3 | 0.247 | 0.74 | | [AliExpress](https://fr.aliexpress.com/item/32994485419.html) |
| Storage | Micro SD Module | 1 | 0.83 | 0.83 | | [AliExpress](https://fr.aliexpress.com/item/1005009224403161.html) |
| Storage | 32GB Micro SD Card | 1 | 4.89 | 4.89 | | [AliExpress](https://fr.aliexpress.com/item/1005012294704811.html) |
| Connectique | Panel Mount USB-C Connector | 1 | 1.79 | 1.79 | | [AliExpress](https://fr.aliexpress.com/item/1005009101248402.html) |
| Prototype | Wire | 1 | 0.88 | 0.88 | | [AliExpress](https://fr.aliexpress.com/item/1005001698292417.html) |
| LTC | LM393 Comparator Module | 1 | 1.24 | 1.24 | | [AliExpress](https://fr.aliexpress.com/item/1005012824868572.html) |
| LTC | 600:600 Ohm Audio Isolation Transformer | 10 | 0.429 | 4.29 | 10 pieces | [AliExpress](https://fr.aliexpress.com/item/1005006340217108.html) |
| Prototype | PCB Board | 1 | 3.49 | 3.49 | | [AliExpress](https://fr.aliexpress.com/item/1005007977006793.html) |
| **TOTAL** | | | | **26.73** | | |