/****************************************************************************************************************************
  BlynkSimpleEthernet_WM.h
  For W5x00, ENC28J60 and Teensy 4.1 NativeEthernet shields.

  BlynkEthernet_WM is a library for Teensy, STM32, SAM DUE and SAMD boards, with Ethernet W5x00, ENC28J60 or NativeEthernet shields,
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk.
  AVR Mega is not supported.
  Library modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkEthernet_WM
  Licensed under MIT license

  Original Blynk Library author:
  @file       BlynkSimpleEthernet.h
  @author     Volodymyr Shymanskyy
  @license    This project is released under the MIT License (MIT)
  @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
  @date       Jan 2015
  @brief

  Version: 1.5.2

  Version  Modified By   Date      Comments
  -------  -----------  ---------- -----------
  1.0.4     K Hoang      14/01/2020 Initial coding
  1.0.5     K Hoang      24/01/2020 Change Synch XMLHttpRequest to Async (https://xhr.spec.whatwg.org/)
  1.0.6     K Hoang      20/02/2020 Add support to ENC28J60 Ethernet shields
  1.0.7     K Hoang      20/02/2020 Add support to SAM DUE and SAMD boards
  1.0.8     K Hoang      03/03/2020 Fix bug. Change default macAddress for boards
  1.0.9     K Hoang      10/03/2020 Reduce html and code size. Enhance GUI.
  1.0.10    K Hoang      11/04/2020 Add MultiBlynk, dynamic parameters, special chars input
  1.0.11    K Hoang      14/04/2020 Fix bug
  1.0.12    K Hoang      15/04/2020 Drop W5100 and AVR Mega support because of not enough memory.  Add SAMD51 support.
  1.0.13    K Hoang      29/04/2020 Add ESP32, including u-blox NINA-W10 series (ESP32) and ESP8266 support.
                                    Add Configurable Config Portal Title, Default Config Data and DRD. Update examples.
  1.0.14    K Hoang      01/05/2020 Add support to Adafruit nRF52, including NINA_B302_ublox.
  1.0.15    K Hoang      12/05/2020 Fix bug and Update to use LittleFS for ESP8266 core 2.7.1+.
  1.0.16    K Hoang      15/05/2020 Sync with EthernetWebServer v.1.0.9 to use 25MHz for W5x00 and EthernetWrapper feature.
  1.0.17    K Hoang      25/07/2020 New logic for USE_DEFAULT_CONFIG_DATA. Add support to Seeeduino SAMD21/SAMD51 boards.
  1.0.18    K Hoang      15/09/2020 Add support to new EthernetENC library for ENC28J60.
  1.1.0     K Hoang      13/01/2021 Add support to new NativeEthernet library for Teensy 4.1. Fix compiler warnings.
  1.2.0     K Hoang      29/01/2021 Fix bug. Add feature. Use more efficient FlashStorage_STM32 and FlashStorage_SAMD.
  1.2.1     K Hoang      31/01/2021 To permit autoreset after timeout if DRD/MRD or non-persistent forced-CP
  1.3.0     K Hoang      16/05/2021 Add support to RP2040-based boards such as RASPBERRY_PI_PICO
  1.4.0     K Hoang      18/05/2021 Add support to RP2040-based boards using Arduino-mbed RP2040 core
  1.5.0     K Hoang      05/06/2021 Add LittleFS/WiFiManager support to RP2040-based boards using Arduino-mbed RP2040 core
  1.5.1     K Hoang      05/06/2021 Fix bug
  1.5.2     K Hoang      09/10/2021 Update `platform.ini` and `library.json` 
 *****************************************************************************************************************************/

#ifndef BlynkSimpleEthernet_WM_h
#define BlynkSimpleEthernet_WM_h

#ifndef BLYNK_INFO_CONNECTION
  #define BLYNK_INFO_CONNECTION       "W5X00"
#endif

#ifndef BLYNK_ETHERNET_WM_VERSION
  #define BLYNK_ETHERNET_WM_VERSION     "BlynkEthernet_WM v1.5.2"
#endif

#if USE_NATIVE_ETHERNET
  #include "NativeEthernet.h"
  #warning Using NativeEthernet lib for Teensy 4.1 from BlynkSimpleEthernet_WM.h. Must also use Teensy Packages Patch or error
#elif USE_ETHERNET3
  #include "Ethernet3.h"
  #warning Use Ethernet3 lib from BlynkSimpleEthernet_WM.h
#elif USE_ETHERNET2
  #include "Ethernet2.h"
  #warning Use Ethernet2 lib from BlynkSimpleEthernet_WM.h
#elif USE_ETHERNET_LARGE
  #include "EthernetLarge.h"
  #warning Use EthernetLarge lib from BlynkSimpleEthernet_WM.h
#elif USE_CUSTOM_ETHERNET
  #warning Use Custom Ethernet library from BlynkSimpleEthernet_WM. You must include a library in your sketch or error.
#else
  #define USE_ETHERNET          true
  #include "Ethernet.h"
  #warning Use Ethernet lib from BlynkSimpleEthernet_WM.h
#endif

#if USE_NATIVE_ETHERNET
  #include <NativeEthernetClient.h>
#else
  #include <EthernetClient.h>
#endif

#if ( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
      || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
      || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
      || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
      || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #if defined(ETHERNET_USE_SAMD)
    #undef ETHERNET_USE_SAMD
  #endif
  #define ETHERNET_USE_SAMD      true

#elif ( defined(ARDUINO_SAM_DUE) || defined(__SAM3X8E__) )
  #if defined(ETHERNET_USE_SAM_DUE)
    #undef ETHERNET_USE_SAM_DUE
  #endif
  #define ETHERNET_USE_SAM_DUE      true

#elif ( defined(CORE_TEENSY) && !( defined(__MKL26Z64__) || defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega32U4__) ) )
  #if defined(ETHERNET_USE_TEENSY)
    #undef ETHERNET_USE_TEENSY
  #endif
  #define ETHERNET_USE_TEENSY         true

#elif ( defined(ESP8266) )
  // For ESP8266
  #if defined(ETHERNET_USE_ESP8266)
    #undef ETHERNET_USE_ESP8266
  #endif
  #define ETHERNET_USE_ESP8266         true

#elif ( defined(ESP32) )
  // For ESP32
  #if defined(ETHERNET_USE_ESP32)
    #undef ETHERNET_USE_ESP32
  #endif
  #define ETHERNET_USE_ESP32         true
  
#elif ( defined(NRF52840_FEATHER) || defined(NRF52832_FEATHER) || defined(NRF52_SERIES) || defined(ARDUINO_NRF52_ADAFRUIT) || \
        defined(NRF52840_FEATHER_SENSE) || defined(NRF52840_ITSYBITSY) || defined(NRF52840_CIRCUITPLAY) || defined(NRF52840_CLUE) || \
        defined(NRF52840_METRO) || defined(NRF52840_PCA10056) || defined(PARTICLE_XENON) || defined(NINA_B302_ublox) || defined(NINA_B112_ublox) )
  #if defined(ETHERNET_USE_NRF528XX)
    #undef ETHERNET_USE_NRF528XX
  #endif
  #define ETHERNET_USE_NRF528XX         true

#elif ( defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || defined(ARDUINO_GENERIC_RP2040) )  
  #if defined(ETHERNET_USE_RPIPICO)
    #undef ETHERNET_USE_RPIPICO
  #endif
  #define ETHERNET_USE_RPIPICO      true
  
#endif

#if (ETHERNET_USE_SAMD)
  #include <Adapters/BlynkEthernet_SAMD_WM.h>
#elif (ETHERNET_USE_SAM_DUE)
  #include <Adapters/BlynkEthernet_DUE_WM.h>
#elif (ETHERNET_USE_TEENSY)
  #include <Adapters/BlynkEthernet_WM.h>
#elif (ETHERNET_USE_ESP32)
  #include <Adapters/BlynkEthernet_ESP32_WM.h>
#elif (ETHERNET_USE_ESP8266)
  #include <Adapters/BlynkEthernet_ESP8266_WM.h>
#elif (ETHERNET_USE_NRF528XX)
  #include <Adapters/BlynkEthernet_NRF52_WM.h>
#elif (ETHERNET_USE_RPIPICO) && !defined(ARDUINO_ARCH_MBED)
  #include <Adapters/BlynkEthernet_RP2040_WM.h>
#elif (ETHERNET_USE_RPIPICO) && defined(ARDUINO_ARCH_MBED) 
  #include <Adapters/BlynkEthernet_Mbed_RP2040_WM.h> 
#else
  #error This code for SAMD21, SAMD51, SAM-DUE, Teensy (4.1/4.0, 3.x), ESP8266, ESP32, nRF52, RP2040-based boards, not AVR Mega nor STM32! Please check your Tools->Board setting.
#endif

static EthernetClient _blynkEthernetClient;
static BlynkArduinoClient _blynkTransport(_blynkEthernetClient);
BlynkEthernet Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#endif
