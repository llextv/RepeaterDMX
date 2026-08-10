#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS      10
#define SD_MOSI    11
#define SD_SCK     12
#define SD_MISO    13

#define DMX_RX     7
#define DMX_TX     8
#define DMX_DE_RE  18

#define LTC_PIN    4


HardwareSerial DMXSerial(1);

#define DMX_CHANNELS 512

uint8_t dmxBuffer[DMX_CHANNELS + 1];
uint8_t lastDMX[DMX_CHANNELS + 1];

bool dmxFrameReady = false;

uint32_t lastDMXByteTime = 0;

const uint32_t DMX_FRAME_GAP_US = 1200;


volatile uint32_t ltcLastEdge = 0;

#define LTC_EDGE_BUFFER 256

volatile uint32_t ltcEdges[LTC_EDGE_BUFFER];
volatile uint16_t ltcWriteIndex = 0;
volatile uint16_t ltcReadIndex = 0;

bool ltcAvailable = false;

uint8_t ltcBits[80];

uint8_t ltcHours = 0;
uint8_t ltcMinutes = 0;
uint8_t ltcSeconds = 0;
uint8_t ltcFrames = 0;

uint8_t ltcFPS = 30;

uint32_t currentLTCFrame = 0;

bool ltcLocked = false;


File recordFile;

bool recording = false;
bool playing = false;
bool waitingForLTC = false;

uint32_t recordStartMillis = 0;

uint32_t recordedFrames = 0;

uint32_t lastRecordTime = 0;

const uint32_t RECORD_INTERVAL_MS = 20;


File playbackFile;

uint32_t playbackFrame = 0;

uint32_t playbackStartMillis = 0;

uint32_t playbackLastMillis = 0;


struct DMXRecord {

  uint32_t timecodeFrame;

  uint8_t dmx[DMX_CHANNELS];

};


void IRAM_ATTR ltcISR() {

  uint32_t now = micros();

  uint32_t duration = now - ltcLastEdge;

  ltcLastEdge = now;

  uint16_t next = (ltcWriteIndex + 1) % LTC_EDGE_BUFFER;

  if (next != ltcReadIndex) {

    ltcEdges[ltcWriteIndex] = duration;

    ltcWriteIndex = next;
  }
}


void setDMXOutput(bool output) {

  if (output) {

    digitalWrite(DMX_DE_RE, HIGH);

  } else {

    digitalWrite(DMX_DE_RE, LOW);
  }
}


void sendDMXFrame(uint8_t *data) {

  setDMXOutput(true);

  DMXSerial.flush();

  digitalWrite(DMX_TX, LOW);

  delayMicroseconds(120);

  digitalWrite(DMX_TX, HIGH);

  delayMicroseconds(12);

  DMXSerial.write(0);

  for (int i = 1; i <= DMX_CHANNELS; i++) {

    DMXSerial.write(data[i]);
  }

  DMXSerial.flush();

  setDMXOutput(false);
}

void readDMX() {

  while (DMXSerial.available()) {

    uint8_t value = DMXSerial.read();

    uint32_t now = micros();



    if ((now - lastDMXByteTime) > DMX_FRAME_GAP_US) {
      dmxBuffer[0] = 0;

      dmxBuffer[1] = value;

    } else {

      static uint16_t position = 1;

      if (position <= DMX_CHANNELS) {

        dmxBuffer[position] = value;

        position++;

        if (position > DMX_CHANNELS) {

          memcpy(
            lastDMX,
            dmxBuffer,
            sizeof(dmxBuffer)
          );

          dmxFrameReady = true;

          position = 1;
        }
      }
    }

    lastDMXByteTime = now;
  }
}

bool getLTCEdge(uint32_t &duration) {

  noInterrupts();

  if (ltcReadIndex == ltcWriteIndex) {

    interrupts();

    return false;
  }

  duration = ltcEdges[ltcReadIndex];

  ltcReadIndex =
    (ltcReadIndex + 1) % LTC_EDGE_BUFFER;

  interrupts();

  return true;
}

void processLTC() {

  static uint8_t bitPosition = 0;

  static bool waitingHalf = false;

  static uint32_t halfBitTime = 2000;

  uint32_t duration;

  while (getLTCEdge(duration)) {


    if (duration > 1200 && duration < 2600) {

      halfBitTime =
        (halfBitTime * 7 + duration) / 8;
    }

    uint32_t threshold =
      halfBitTime * 3 / 2;


    if (duration > threshold) {

      ltcBits[bitPosition] = 0;

      bitPosition++;

      waitingHalf = false;
    }

    else {

      if (waitingHalf) {

        ltcBits[bitPosition] = 1;

        bitPosition++;

        waitingHalf = false;

      } else {

        waitingHalf = true;
      }
    }

    if (bitPosition >= 80) {

      bitPosition = 0;

      uint16_t sync = 0;

      for (int i = 64; i < 80; i++) {

        sync <<= 1;

        sync |= ltcBits[i];
      }


      if (sync == 0xBFFC ||
          sync == 0x3FFD) {

        decodeLTCFrame();

        ltcLocked = true;
      }
    }
  }
}

uint8_t decodeBCD(
  uint8_t units,
  uint8_t tens
) {

  return units + tens * 10;
}

void decodeLTCFrame() {

  uint8_t frameUnits =
    ltcBits[0] * 1 +
    ltcBits[1] * 2 +
    ltcBits[2] * 4 +
    ltcBits[3] * 8;

  uint8_t frameTens =
    ltcBits[8] * 1 +
    ltcBits[9] * 2;

  uint8_t secondUnits =
    ltcBits[16] * 1 +
    ltcBits[17] * 2 +
    ltcBits[18] * 4 +
    ltcBits[19] * 8;

  uint8_t secondTens =
    ltcBits[24] * 1 +
    ltcBits[25] * 2 +
    ltcBits[26] * 4;

  uint8_t minuteUnits =
    ltcBits[32] * 1 +
    ltcBits[33] * 2 +
    ltcBits[34] * 4 +
    ltcBits[35] * 8;

  uint8_t minuteTens =
    ltcBits[40] * 1 +
    ltcBits[41] * 2 +
    ltcBits[42] * 4;

  uint8_t hourUnits =
    ltcBits[48] * 1 +
    ltcBits[49] * 2 +
    ltcBits[50] * 4 +
    ltcBits[51] * 8;

  uint8_t hourTens =
    ltcBits[56] * 1 +
    ltcBits[57] * 2;

  ltcFrames =
    frameUnits +
    frameTens * 10;

  ltcSeconds =
    secondUnits +
    secondTens * 10;

  ltcMinutes =
    minuteUnits +
    minuteTens * 10;

  ltcHours =
    hourUnits +
    hourTens * 10;

  currentLTCFrame =
    (((uint32_t)ltcHours * 3600UL) +
     ((uint32_t)ltcMinutes * 60UL) +
     ltcSeconds) *
     ltcFPS +
     ltcFrames;
}


bool startRecording() {

  if (recording) {

    return false;
  }

  if (SD.exists("/record.dmx")) {

    SD.remove("/record.dmx");
  }

  recordFile =
    SD.open("/record.dmx", FILE_WRITE);

  if (!recordFile) {

    Serial.println(
      "ERROR: Cannot create record.dmx"
    );

    return false;
  }

  recording = true;

  recordedFrames = 0;

  recordStartMillis = millis();

  Serial.println(
    "RECORDING STARTED"
  );

  return true;
}

void stopRecording() {

  if (!recording) {

    return;
  }

  recordFile.flush();

  recordFile.close();

  recording = false;

  Serial.print(
    "RECORDING STOPPED - Frames: "
  );

  Serial.println(recordedFrames);
}

void recordDMXFrame() {

  if (!recording) {

    return;
  }

  uint32_t now = millis();

  if ((now - lastRecordTime) <
      RECORD_INTERVAL_MS) {

    return;
  }

  lastRecordTime = now;

  uint32_t tc = currentLTCFrame;

  recordFile.write(
    (uint8_t *)&tc,
    sizeof(tc)
  );

  recordFile.write(
    &lastDMX[1],
    DMX_CHANNELS
  );

  recordedFrames++;


  if ((recordedFrames % 100) == 0) {

    recordFile.flush();

    Serial.print(
      "Recorded: "
    );

    Serial.println(recordedFrames);
  }
}

bool startPlayback() {

  if (playing) {

    return false;
  }

  playbackFile =
    SD.open("/record.dmx", FILE_READ);

  if (!playbackFile) {

    Serial.println(
      "ERROR: record.dmx not found"
    );

    return false;
  }

  playbackFrame = 0;

  playing = true;

  waitingForLTC = ltcLocked;

  playbackStartMillis = millis();

  Serial.println(
    "PLAYBACK STARTED"
  );

  if (waitingForLTC) {

    Serial.println(
      "Waiting for LTC..."
    );
  }

  return true;
}

void stopPlayback() {

  if (!playing) {

    return;
  }

  playbackFile.close();

  playing = false;

  waitingForLTC = false;

  Serial.println(
    "PLAYBACK STOPPED"
  );
}

void playbackInternal() {

  if (!playing) {

    return;
  }

  if (waitingForLTC) {

    return;
  }

  uint32_t now = millis();

  if ((now - playbackLastMillis) <
      RECORD_INTERVAL_MS) {

    return;
  }

  playbackLastMillis = now;

  if (playbackFile.available() <
      sizeof(uint32_t) + DMX_CHANNELS) {

    stopPlayback();

    Serial.println(
      "PLAYBACK FINISHED"
    );

    return;
  }

  uint32_t timecode;

  uint8_t frame[DMX_CHANNELS];

  playbackFile.read(
    (uint8_t *)&timecode,
    sizeof(timecode)
  );

  playbackFile.read(
    frame,
    DMX_CHANNELS
  );

  sendDMXFrame(frame);

  playbackFrame++;
}

void playbackLTC() {

  if (!playing) {

    return;
  }

  if (!waitingForLTC) {

    return;
  }


  uint32_t current =
    currentLTCFrame;

  while (playbackFile.available() >=
         sizeof(uint32_t) + DMX_CHANNELS) {

    uint32_t frameTC;

    uint8_t frame[DMX_CHANNELS];

    uint32_t position =
      playbackFile.position();

    playbackFile.read(
      (uint8_t *)&frameTC,
      sizeof(frameTC)
    );

    playbackFile.read(
      frame,
      DMX_CHANNELS
    );


    if (frameTC > current) {

      playbackFile.seek(position);

      break;
    }

    if (frameTC == current) {

      sendDMXFrame(frame);

      playbackFrame++;
    }
  }
}

void processSerial() {

  if (!Serial.available()) {

    return;
  }

  char command =
    Serial.read();

  switch (command) {

    case 'r':
    case 'R':

      startRecording();

      break;

    case 's':
    case 'S':

      stopRecording();

      break;

    case 'p':
    case 'P':

      startPlayback();

      break;

    case 'x':
    case 'X':

      stopPlayback();

      break;

    case 'd':
    case 'D':

      Serial.print(
        "LTC: "
      );

      Serial.print(
        ltcHours
      );

      Serial.print(":");

      if (ltcMinutes < 10)
        Serial.print("0");

      Serial.print(
        ltcMinutes
      );

      Serial.print(":");

      if (ltcSeconds < 10)
        Serial.print("0");

      Serial.print(
        ltcSeconds
      );

      Serial.print(":");

      if (ltcFrames < 10)
        Serial.print("0");

      Serial.println(
        ltcFrames
      );

      break;

    default:

      Serial.println(
        "Commands:"
      );

      Serial.println(
        "R = Record"
      );

      Serial.println(
        "S = Stop"
      );

      Serial.println(
        "P = Play"
      );

      Serial.println(
        "X = Stop playback"
      );

      Serial.println(
        "D = Show LTC"
      );

      break;
  }
}


void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "DMX RECORDER / LTC PLAYER"
  );

  Serial.println(
    "=============================="
  );


  pinMode(
    DMX_DE_RE,
    OUTPUT
  );

  setDMXOutput(false);

  DMXSerial.begin(
    250000,
    SERIAL_8N2,
    DMX_RX,
    DMX_TX
  );

  Serial.println(
    "DMX initialized"
  );

  pinMode(
    LTC_PIN,
    INPUT
  );

  attachInterrupt(
    digitalPinToInterrupt(LTC_PIN),
    ltcISR,
    CHANGE
  );

  Serial.println(
    "LTC initialized"
  );

  SPI.begin(
    SD_SCK,
    SD_MISO,
    SD_MOSI,
    SD_CS
  );

  if (!SD.begin(
        SD_CS,
        SPI
      )) {

    Serial.println(
      "ERROR: SD CARD FAILED"
    );

  } else {

    Serial.println(
      "SD CARD OK"
    );

    Serial.print(
      "SD size: "
    );

    Serial.print(
      SD.cardSize() /
      (1024 * 1024)
    );

    Serial.println(
      " MB"
    );
  }

  Serial.println();

  Serial.println(
    "Commands:"
  );

  Serial.println(
    "R = Record"
  );

  Serial.println(
    "S = Stop"
  );

  Serial.println(
    "P = Play"
  );

  Serial.println(
    "X = Stop playback"
  );

  Serial.println(
    "D = Show LTC"
  );

  Serial.println();
}

void loop() {

  processSerial();

  readDMX();

  processLTC();

  /*
   * Record latest DMX frame.
   */

  if (dmxFrameReady) {

    dmxFrameReady = false;

    recordDMXFrame();
  }

  /*
   * Playback.
   */

  if (playing) {

    if (waitingForLTC) {

      playbackLTC();

    } else {

      playbackInternal();
    }
  }

  /*
   * Print LTC occasionally.
   */

  static uint32_t lastPrint = 0;

  if (ltcLocked &&
      millis() - lastPrint > 1000) {

    lastPrint = millis();

    Serial.printf(
      "LTC %02d:%02d:%02d:%02d\n",
      ltcHours,
      ltcMinutes,
      ltcSeconds,
      ltcFrames
    );
  }
}