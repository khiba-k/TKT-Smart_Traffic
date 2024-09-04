/****************************************************************************************************************************
  BlynkEthernet_ESP32_WM.h
  For ESP32 or ESP32-based boards running W5x00, ENC28J60 Ethernet shields

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

#ifndef BlynkEthernet_ESP32_WM_h
#define BlynkEthernet_ESP32_WM_h

#if !defined(ESP32)
  #error This code is designed to run on ESP32 and ESP32-based boards! Please check your Tools->Board setting.
#endif

//////////////////////////////////////////////
// From v1.3.0 to display correct BLYNK_INFO_DEVICE

#define BLYNK_USE_128_VPINS

#if defined(BLYNK_INFO_DEVICE)
  #undef BLYNK_INFO_DEVICE
#endif
#define BLYNK_BUFFERS_SIZE    4096

#if defined(BLYNK_INFO_DEVICE)
  #undef BLYNK_INFO_DEVICE
#endif

#if defined(BOARD_NAME)
  #define BLYNK_INFO_DEVICE   BOARD_NAME
#elif defined(BOARD_TYPE)
  #define BLYNK_INFO_DEVICE   BOARD_TYPE
#else
  #define BLYNK_INFO_DEVICE   "ESP32"
#endif

//////////////////////////////////////////////

#define BLYNK_SEND_ATOMIC

#ifndef BLYNK_WM_DEBUG
#define BLYNK_WM_DEBUG      0
#endif

#ifndef BLYNK_INFO_CONNECTION
#define BLYNK_INFO_CONNECTION "W5000"
#endif

#ifdef BLYNK_USE_SSL
#define BLYNK_SERVER_PORT BLYNK_DEFAULT_PORT_SSL
#else
#define BLYNK_SERVER_PORT BLYNK_DEFAULT_PORT
#endif

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <EthernetWebServer.h>

// LittleFS has higher priority than SPIFFS. 
// But if not specified any, use SPIFFS to not forcing user to install LITTLEFS library
//#if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
//  #define USE_SPIFFS      true
//#endif

#if USE_LITTLEFS
  // Use LittleFS
  #include "FS.h"

  // The library will be depreciated after being merged to future major Arduino esp32 core release 2.x
  // At that time, just remove this library inclusion
  #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
  
  FS* filesystem =      &LITTLEFS;
  #define FileFS        LITTLEFS
  #warning Using LittleFS in BlynkEthernet_ESP32_WM.h
#elif USE_SPIFFS
  #include "FS.h"
  #include <SPIFFS.h>
  FS* filesystem =      &SPIFFS;
  #define FileFS        SPIFFS
  #warning Using SPIFFS in BlynkEthernet_ESP32_WM.h
#else
  #include <EEPROM.h>
  #warning Using EEPROM in BlynkEthernet_ESP32_WM.h
#endif

///////// NEW for DRD /////////////
// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#define ESP8266_DRD_USE_RTC     false   //true

#if USE_LITTLEFS
  #define ESP_DRD_USE_LITTLEFS    true
  #define ESP_DRD_USE_SPIFFS      false
  #define ESP_DRD_USE_EEPROM      false
#elif USE_SPIFFS
  #define ESP_DRD_USE_LITTLEFS    false
  #define ESP_DRD_USE_SPIFFS      true
  #define ESP_DRD_USE_EEPROM      false
#else
  #define ESP_DRD_USE_LITTLEFS    false
  #define ESP_DRD_USE_SPIFFS      false
  #define ESP_DRD_USE_EEPROM      true
#endif

#ifndef DOUBLERESETDETECTOR_DEBUG
  #define DOUBLERESETDETECTOR_DEBUG     false
#endif

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

//DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector* drd;

///////// NEW for DRD /////////////

#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())

//NEW
#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

typedef struct
{
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;
//

#if USE_DYNAMIC_PARAMETERS
  extern uint16_t NUM_MENU_ITEMS;
  extern MenuItem myMenuItems [];
  bool *menuItemUpdated = NULL;
#endif

#define HEADER_MAX_LEN      16

#define BLYNK_SERVER_MAX_LEN      32
#define BLYNK_TOKEN_MAX_LEN       36

#define STATIC_IP_MAX_LEN         16
#define BOARD_NAME_MAX_LEN        24

typedef struct
{
  char blynk_server[BLYNK_SERVER_MAX_LEN];
  char blynk_token [BLYNK_TOKEN_MAX_LEN];
}  Blynk_Credentials;

#define NUM_BLYNK_CREDENTIALS     2

// Configurable items besides fixed Header
#define NUM_CONFIGURABLE_ITEMS    ( 3 + (2 * NUM_BLYNK_CREDENTIALS) )

typedef struct Configuration
{
  char header         [HEADER_MAX_LEN];
  Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
  int  blynk_port;
  char static_IP      [STATIC_IP_MAX_LEN];
  char board_name     [BOARD_NAME_MAX_LEN];
  int  checkSum;
} Blynk_Configuration;

// Currently CONFIG_DATA_SIZE  =  ( 64 + (68 * NUM_BLYNK_CREDENTIALS) ) = 200
uint16_t CONFIG_DATA_SIZE = sizeof(Blynk_Configuration);

///New from v1.0.13
extern bool LOAD_DEFAULT_CONFIG_DATA;
extern Blynk_Configuration defaultConfig;

// -- HTML page fragments
const char BLYNK_WM_HTML_HEAD[]     /*PROGMEM*/ = "<!DOCTYPE html><html><head><title>BlynkEthernet_ESP32_WM</title><style>div,input{padding:2px;font-size:1em;}input{width:95%;}\
body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.5rem;margin:0px;}\
</style></head><div style=\"text-align:left;display:inline-block;min-width:260px;\">\
<fieldset><div><label>Blynk Server</label><input value=\"[[sv]]\"id=\"sv\"><div></div></div>\
<div><label>Token</label><input value=\"[[tk]]\"id=\"tk\"><div></div></div>\
<div><label>Blynk Server1</label><input value=\"[[sv1]]\"id=\"sv1\"><div></div></div>\
<div><label>Token1</label><input value=\"[[tk1]]\"id=\"tk1\"><div></div></div>\
<div><label>Port</label><input value=\"[[pt]]\"id=\"pt\"><div></div></div>\
<div><label>StaticIP</label><input value=\"[[ip]]\"id=\"ip\"><div></div></div></fieldset>\
<fieldset><div><label>Board Name</label><input value=\"[[nm]]\"id=\"nm\"><div></div></div></fieldset>";

const char BLYNK_WM_FLDSET_START[]  /*PROGMEM*/ = "<fieldset>";
const char BLYNK_WM_FLDSET_END[]    /*PROGMEM*/ = "</fieldset>";
const char BLYNK_WM_HTML_PARAM[]    /*PROGMEM*/ = "<div><label>{b}</label><input value='[[{v}]]'id='{i}'><div></div></div>";
const char BLYNK_WM_HTML_BUTTON[]   /*PROGMEM*/ = "<button onclick=\"sv()\">Save</button></div>";
const char BLYNK_WM_HTML_SCRIPT[]   /*PROGMEM*/ = "<script id=\"jsbin-javascript\">\
function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);request.open('GET',url,false);request.send(null);}\
function sv(){udVal('sv',document.getElementById('sv').value);udVal('tk',document.getElementById('tk').value);\
udVal('sv1',document.getElementById('sv1').value);udVal('tk1',document.getElementById('tk1').value);\
udVal('pt',document.getElementById('pt').value);udVal('ip',document.getElementById('ip').value);\
udVal('nm',document.getElementById('nm').value);";

const char BLYNK_WM_HTML_SCRIPT_ITEM[]  /*PROGMEM*/ = "udVal('{d}',document.getElementById('{d}').value);";
const char BLYNK_WM_HTML_SCRIPT_END[]   /*PROGMEM*/ = "alert('Updated');}</script>";
const char BLYNK_WM_HTML_END[]          /*PROGMEM*/ = "</html>";
///

#define BLYNK_SERVER_HARDWARE_PORT    8080

class BlynkEthernet
  : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
  public:
    BlynkEthernet(BlynkArduinoClient& transp)
      : Base(transp)
    {}
    
    //////////////////////////////////////////

    void config(const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_SERVER_PORT)
    {
      // Base == BlynkProtocol.h
      Base::begin(auth);
      // conn == BlynkArduinoClient
      this->conn.begin(domain, port);
    }
    
    //////////////////////////////////////////

    void config(const char* auth,
                IPAddress   ip,
                uint16_t    port = BLYNK_SERVER_PORT)
    {
      Base::begin(auth);
      this->conn.begin(ip, port);
    }

    // DHCP with domain
    void begin( const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t port      = BLYNK_SERVER_PORT,
                const byte mac[]   = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("GetIP:"));
#endif

      if (!Ethernet.begin(SelectMacAddress(auth, mac))) {
        BLYNK_FATAL(BLYNK_F("DHCP 0"));
      }
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, domain, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

    // Static IP with domain
    void begin( const char* auth,
                const char* domain,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                const byte mac[] = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("UseStatIP"));
#endif

      Ethernet.begin(SelectMacAddress(auth, mac), local, dns);
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, domain, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

    // Static IP with domain, gateway, etc
    void begin( const char* auth,
                const char* domain,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const byte mac[] = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("UseStatIP"));
#endif

      Ethernet.begin(SelectMacAddress(auth, mac), local, dns, gateway, subnet);
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, domain, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

    // DHCP with server IP
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port    = BLYNK_SERVER_PORT,
                const byte mac[] = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("GetIP:"));
#endif

      if (!Ethernet.begin(SelectMacAddress(auth, mac))) {
        BLYNK_FATAL(BLYNK_F("DHCP 0"));
      }
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, addr, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

    // Static IP with server IP
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port,
                IPAddress local,
                const byte mac[] = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("UseStatIP"));
#endif

      Ethernet.begin(SelectMacAddress(auth, mac), local);
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, addr, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

    // Static IP with server IP, DNS, gateway, etc
    void begin( const char* auth,
                IPAddress addr,
                uint16_t port,
                IPAddress local,
                IPAddress dns,
                IPAddress gateway,
                IPAddress subnet,
                const byte mac[] = NULL)
    {
#if (BLYNK_WM_DEBUG > 1)
      BLYNK_LOG1(BLYNK_F("UseStatIP"));
#endif

      Ethernet.begin(SelectMacAddress(auth, mac), local, dns, gateway, subnet);
      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);
      IPAddress myip = Ethernet.localIP();
      BLYNK_LOG_IP("IP:", myip);

      config(auth, addr, port);
      while (this->connect() != true) {}
    }
    
    //////////////////////////////////////////

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif

    // For ESP32
#define LED_OFF     LOW
#define LED_ON      HIGH

    //////////////////////////////////////////

    void begin()
    {   
      //Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);
      
      //// New DRD ////
      drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS); 
      
      bool noConfigPortal = true;
   
      if (drd->detectDoubleReset())
      {
#if ( BLYNK_WM_DEBUG > 1)
        BLYNK_LOG1(BLYNK_F("DRD. Run ConfigPortal"));
#endif        
        noConfigPortal = false;
      }
      //// New DRD ////
      
#if ( BLYNK_WM_DEBUG > 2)    
      if (LOAD_DEFAULT_CONFIG_DATA) 
      {   
        BLYNK_LOG1(BLYNK_F("======= Start Default Config Data ======="));
        displayConfigData(defaultConfig);
      }
#endif

      hadConfigData = getConfigData();

      connectEthernet();

      isForcedConfigPortal = isForcedCP();
      
      //// New DRD/MRD ////
      //  noConfigPortal when getConfigData() OK and no MRD/DRD'ed
      //if (getConfigData() && noConfigPortal)
      if (hadConfigData && noConfigPortal && (!isForcedConfigPortal) )
      {
        hadConfigData = true;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG1(noConfigPortal? BLYNK_F("bg: noConfigPortal = true") : BLYNK_F("bg: noConfigPortal = false"));
#endif 

        if (ethernetConnected)
        {
          BLYNK_LOG1(BLYNK_F("bg:ECon.TryB"));

          int i = 0;
          while ( (i++ < 10) && !this->connectMultiBlynk() )
          {
          }

          if  (connected())
          {
            BLYNK_LOG1(BLYNK_F("bg:EBCon"));
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("bg:ECon,BNo"));
            // failed to connect to Blynk server, will start configuration mode
            startConfigurationMode();
          }
        }
        else
        {
          // Can't do anything here
          BLYNK_LOG1(BLYNK_F("bg:E&BNoCon"));
          // failed to connect to Blynk server, will start configuration mode
          //startConfigurationMode();
        }
      }
      else
      { 
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG1(isForcedConfigPortal? BLYNK_F("bg: isForcedConfigPortal = true") : BLYNK_F("bg: isForcedConfigPortal = false"));
#endif
                                
        // If not persistent => clear the flag so that after reset. no more CP, even CP not entered and saved
        if (persForcedConfigPortal)
        {
          BLYNK_LOG2(BLYNK_F("bg:Stay in CP "), isForcedConfigPortal ? BLYNK_F("forever : Forced-Persistent") : (noConfigPortal ? BLYNK_F("forever : No ConfigDat") : BLYNK_F("DRD/MRD")));
        }
        else
        {
          BLYNK_LOG2(BLYNK_F("bg:Stay in CP:"), isForcedConfigPortal ? BLYNK_F("forever : Forced-non-Persistent") : (noConfigPortal ? BLYNK_F("forever : No ConfigDat") : BLYNK_F("DRD/MRD")));
          clearForcedCP();
        }
          
        hadConfigData = isForcedConfigPortal ? true : (noConfigPortal ? false : true);
        
        // failed to connect to Blynk server, will start configuration mode
        startConfigurationMode();
      }
    }
    
    //////////////////////////////////////////

    void run()
    {
#if RESET_IF_CONFIG_TIMEOUT    
      static int retryTimes = 0;
#endif
      
      //// New DRD ////
      // Call the double reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call drd.stop() when you wish to no longer
      // consider the next reset as a double reset.
      drd->loop();
      //// New DRD ////

      // Be sure to reconnect Ethernet first
      if (!ethernetConnected)
      {
        connectEthernet();
      }

      // Lost connection in running. Give chance to reconfig.
      if ( !connected() )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect Blynk.
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
#if RESET_IF_CONFIG_TIMEOUT        
          retryTimes = 0;
#endif

          if (server)
          {
            server->handleClient();
          }

          return;
        }
        else
        {
#if RESET_IF_CONFIG_TIMEOUT
          // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
          // to permit user another chance to config.
          if ( configuration_mode && (configTimeout != 0) )
          {
            if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
            {
              BLYNK_LOG2(BLYNK_F("r:BLost,TOut.TryB.#:"), retryTimes);
            }
            else
            {
              //BlynkReset();
              resetFunc();
            }
          }
#endif

          // Not in config mode, try reconnecting before forcing to config mode
          BLYNK_LOG1(BLYNK_F("r:BLost.TryB"));

          if (connectMultiBlynk())
          {
            // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
            digitalWrite(LED_BUILTIN, LED_OFF);

            BLYNK_LOG1(BLYNK_F("r:BRecon"));
          }
        }
      }
      else if (configuration_mode)
      {
        configuration_mode = false;
        BLYNK_LOG1(BLYNK_F("r:E&B OK"));
        
        // Turn the LED_BUILTIN OFF when out of configuration mode. 
        //ESP32 LED_BUILDIN is correct polarity, LED_OFF to turn OFF
        digitalWrite(LED_BUILTIN, LED_OFF);
      }

      //if (connected())
      {
        Base::run();
      }
    }
    
    //////////////////////////////////////////

    String getBoardName()
    {
      return (String(BlynkEthernet_WM_config.board_name));
    }
    
    //////////////////////////////////////////

    String getServerName(uint8_t index = 255)
    {
      if (index == 255)
      {
        // Current connected Blynk Server
        index = currentBlynkServerIndex;
      }
        
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkEthernet_WM_config.Blynk_Creds[index].blynk_server));
    }
    
    //////////////////////////////////////////

    String getToken(uint8_t index = 255)
    {
      if (index == 255)
      {
        // Current connected Blynk Server
        index = currentBlynkServerIndex;
      }
          
      if (index >= NUM_BLYNK_CREDENTIALS)
        return String("");

      if (!hadConfigData)
        getConfigData();

      return (String(BlynkEthernet_WM_config.Blynk_Creds[index].blynk_token));
    }
    
    //////////////////////////////////////////
    
    int getHWPort()
    {
      return (BlynkEthernet_WM_config.blynk_port);
    }
    
    //////////////////////////////////////////

    Blynk_Configuration* getFullConfigData(Blynk_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &BlynkEthernet_WM_config, sizeof(BlynkEthernet_WM_config));

      return (configData);
    }
    
    //////////////////////////////////////////
    
    void clearConfigData()
    {
      memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));

#if USE_DYNAMIC_PARAMETERS      
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
      }
#endif
      
      saveConfigData();
    }
    
    //////////////////////////////////////////
    
    // Forced CP => Flag = 0xBEEFBEEF. Else => No forced CP
    // Flag to be stored at (EEPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE) 
    // to avoid corruption to current data
    //#define FORCED_CONFIG_PORTAL_FLAG_DATA              ( (uint32_t) 0xDEADBEEF)
    //#define FORCED_PERS_CONFIG_PORTAL_FLAG_DATA         ( (uint32_t) 0xBEEFDEAD)
    
    const uint32_t FORCED_CONFIG_PORTAL_FLAG_DATA       = 0xDEADBEEF;
    const uint32_t FORCED_PERS_CONFIG_PORTAL_FLAG_DATA  = 0xBEEFDEAD;
    
    #define FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE     4
    
    //////////////////////////////////////////
    
    void resetAndEnterConfigPortal()
    {
      persForcedConfigPortal = false;
      
      setForcedCP(false);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      resetFunc();
    }
    
    //////////////////////////////////////////
    
    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    void resetAndEnterConfigPortalPersistent()
    {
      persForcedConfigPortal = true;
      
      setForcedCP(true);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      resetFunc();
    }
    
    //////////////////////////////////////////
    
    void resetFunc()
    {
      delay(1000);
      ESP.restart();
    }
    
    //////////////////////////////////////////

  private:

    // Initialize the Ethernet server library
    // with the IP address and port you want to use
    // (port 80 is default for HTTP):
    EthernetWebServer *server;

    bool ethernetConnected = false;

    bool configuration_mode = false;

    unsigned long configTimeout;
    bool hadConfigData = false;
    
    bool isForcedConfigPortal   = false;
    bool persForcedConfigPortal = false;

    Blynk_Configuration BlynkEthernet_WM_config;
    
    uint16_t totalDataSize = 0;
    
    uint8_t currentBlynkServerIndex = 255;

#define RFC952_HOSTNAME_MAXLEN      24
    char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];
    
    //////////////////////////////////////////

    void setRFC952_hostname(const char* iHostname = "")
    {
      if (iHostname[0] == 0)
      {
        String _hostname = "W5X00-XXXXXX";    // + String(macAddress, HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());

      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

#if (BLYNK_WM_DEBUG > 0)
      BLYNK_LOG2(BLYNK_F("Hname="), RFC952_hostname);
#endif
    }
    
    //////////////////////////////////////////

    char* getRFC952_hostname(const char* iHostname)
    {
      memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

      size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

      size_t j = 0;

      for (size_t i = 0; i < len - 1; i++)
      {
        if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
        {
          RFC952_hostname[j] = iHostname[i];
          j++;
        }
      }
      // no '-' as last char
      if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
        RFC952_hostname[j] = iHostname[len - 1];

      return RFC952_hostname;
    }
    
    //////////////////////////////////////////

    void displayConfigData(Blynk_Configuration configData)
    {
      BLYNK_LOG4(BLYNK_F("Hdr="),       configData.header,
                 BLYNK_F(",BName="),    configData.board_name);
      BLYNK_LOG4(BLYNK_F("Svr="),       configData.Blynk_Creds[0].blynk_server,
                 BLYNK_F(",Tok="),      configData.Blynk_Creds[0].blynk_token);
      BLYNK_LOG4(BLYNK_F("Svr1="),      configData.Blynk_Creds[1].blynk_server,
                 BLYNK_F(",Tok1="),     configData.Blynk_Creds[1].blynk_token);
      BLYNK_LOG4(BLYNK_F("Prt="),       configData.blynk_port,
                 BLYNK_F(",SIP="),      configData.static_IP);
                 
#if (USE_DYNAMIC_PARAMETERS && ( BLYNK_WM_DEBUG > 2))
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        BLYNK_LOG6("i=", i, ",id=", myMenuItems[i].id, ",data=", myMenuItems[i].pdata);
      }      
#endif                 
    }
    
    //////////////////////////////////////////

#define BLYNK_BOARD_TYPE      BLYNK_INFO_CONNECTION
#define WM_NO_CONFIG          "blank"
    
    //////////////////////////////////////////
    
    int calcChecksum()
    {
      int checkSum = 0;
      for (uint16_t index = 0; index < (sizeof(BlynkEthernet_WM_config) - sizeof(BlynkEthernet_WM_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &BlynkEthernet_WM_config ) + index);
      }

      return checkSum;
    }
    
    //////////////////////////////////////////

#if ( USE_LITTLEFS || USE_SPIFFS )

#define  CONFIG_FILENAME              BLYNK_F("/wm_config.dat")
#define  CONFIG_FILENAME_BACKUP       BLYNK_F("/wm_config.bak")

#define  CREDENTIALS_FILENAME         BLYNK_F("/wm_cred.dat")
#define  CREDENTIALS_FILENAME_BACKUP  BLYNK_F("/wm_cred.bak")

#define  CONFIG_PORTAL_FILENAME           BLYNK_F("/wm_cp.dat")
#define  CONFIG_PORTAL_FILENAME_BACKUP    BLYNK_F("/wm_cp.bak")

    //////////////////////////////////////////////
    
    void saveForcedCP(uint32_t value)
    {
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "w");
      
      BLYNK_LOG1(BLYNK_F("SaveCPFile "));

      if (file)
      {
        file.write((uint8_t*) &value, sizeof(value));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant CP file
      file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "w");
      
      BLYNK_LOG1(BLYNK_F("SaveBkUpCPFile "));

      if (file)
      {
        file.write((uint8_t *) &value, sizeof(value));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }
    }
    
    //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(isPersistent ? BLYNK_F("setForcedCP Persistent") : BLYNK_F("setForcedCP non-Persistent"));
#endif
      
      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////
    
    void clearForcedCP()
    {
      uint32_t readForcedConfigPortalFlag = 0;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("clearForcedCP"));
#endif
      
      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("Check if isForcedCP"));
#endif
      
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCPFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCPFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
       }

      file.readBytes((char *) &readForcedConfigPortalFlag, sizeof(readForcedConfigPortalFlag));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      char* readBuffer;
           
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
      
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      uint16_t maxBufferLength = 0;
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > maxBufferLength)
          maxBufferLength = myMenuItems[i].maxlen;
      }
      
      if (maxBufferLength > 0)
      {
        readBuffer = new char[ maxBufferLength + 1 ];
        
        // check to see NULL => stop and return false
        if (readBuffer == NULL)
        {
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error can't allocate buffer."));
          return false;
        }
#if ( BLYNK_WM_DEBUG > 2)          
        else
        {
          BLYNK_LOG2(BLYNK_F("ChkCrR: Buffer allocated, sz="), maxBufferLength + 1);
        }
#endif             
      }
     
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;

        // Actual size of pdata is [maxlen + 1]
        memset(readBuffer, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      // Free buffer
      if (readBuffer != NULL)
      {
        free(readBuffer);
        BLYNK_LOG1(BLYNK_F("Buffer freed"));
      }
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }
    
    //////////////////////////////////////////////

    bool loadDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      totalDataSize = sizeof(BlynkEthernet_WM_config) + sizeof(readCheckSum);
      
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCredFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCredFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return false;
        }
      }
     
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CrR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }
    
    //////////////////////////////////////////////

    void saveDynamicData()
    {
      int checkSum = 0;
    
      File file = FileFS.open(CREDENTIALS_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCredFile "));

      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)          
        BLYNK_LOG4(F("CW1:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
           
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
      
      // Trying open redundant Auth file
      file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCredFile "));

      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;

#if ( BLYNK_WM_DEBUG > 2)         
        BLYNK_LOG4(F("CW2:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          BLYNK_LOG1(BLYNK_F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));    
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }   
    }
#endif
    
    //////////////////////////////////////////////
    
    void NULLTerminateConfig()
    {
      //#define HEADER_MAX_LEN      16
      //#define BLYNK_SERVER_MAX_LEN      32
      //#define BLYNK_TOKEN_MAX_LEN       36
      //#define STATIC_IP_MAX_LEN         16
      //#define BOARD_NAME_MAX_LEN        24
      
      // NULL Terminating to be sure
      BlynkEthernet_WM_config.header[HEADER_MAX_LEN - 1] = 0;
      BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server[BLYNK_SERVER_MAX_LEN - 1] = 0;
      BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token [BLYNK_TOKEN_MAX_LEN - 1]  = 0;
      BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server[BLYNK_SERVER_MAX_LEN - 1] = 0;
      BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token [BLYNK_TOKEN_MAX_LEN - 1]  = 0;
      BlynkEthernet_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
      BlynkEthernet_WM_config.static_IP[STATIC_IP_MAX_LEN - 1] = 0;
      BlynkEthernet_WM_config.board_name [BOARD_NAME_MAX_LEN - 1]  = 0;
    }

    //////////////////////////////////////////////

    void loadConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "r");
      BLYNK_LOG1(BLYNK_F("LoadCfgFile "));

      if (!file)
      {
        BLYNK_LOG1(BLYNK_F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_FILENAME_BACKUP, "r");
        BLYNK_LOG1(BLYNK_F("LoadBkUpCfgFile "));

        if (!file)
        {
          BLYNK_LOG1(BLYNK_F("failed"));
          return;
        }
      }

      file.readBytes((char *) &BlynkEthernet_WM_config, sizeof(BlynkEthernet_WM_config));

      BLYNK_LOG1(BLYNK_F("OK"));
      file.close();
    }
    
    //////////////////////////////////////////////

    void saveConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "w");
      BLYNK_LOG1(BLYNK_F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      BlynkEthernet_WM_config.checkSum = calChecksum;
      BLYNK_LOG2(BLYNK_F("WCSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &BlynkEthernet_WM_config, sizeof(BlynkEthernet_WM_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

      // Trying open redundant Auth file
      file = FileFS.open(CONFIG_FILENAME_BACKUP, "w");
      BLYNK_LOG1(BLYNK_F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &BlynkEthernet_WM_config, sizeof(BlynkEthernet_WM_config));
        file.close();
        BLYNK_LOG1(BLYNK_F("OK"));
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("failed"));
      }

#if USE_DYNAMIC_PARAMETERS      
      saveDynamicData();
#endif      
    }
       
    //////////////////////////////////////////////
    
    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&BlynkEthernet_WM_config, &defaultConfig, sizeof(BlynkEthernet_WM_config));
      strcpy(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE);
      
      // Including config and dynamic data, and assume valid
      saveConfigData();
      
#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
      displayConfigData(BlynkEthernet_WM_config);
#endif      
    }
    
    //////////////////////////////////////////////

    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;
      
      hadConfigData = false;
      
       // Format LittleFS/SPIFFS if not yet
      if (!FileFS.begin(true))
      {
        BLYNK_LOG1(BLYNK_F("SPIFFS/LittleFS failed! Formatting."));
        
        if (!FileFS.begin())
        {
          BLYNK_LOG1(BLYNK_F("SPIFFS/LittleFS failed! Pls use EEPROM."));
          return false;
        }
      }

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        loadAndSaveDefaultConfigData();
        
        // Don't need Config Portal anymore
        return true; 
      }
#if USE_DYNAMIC_PARAMETERS      
      else if ( ( FileFS.exists(CONFIG_FILENAME)      || FileFS.exists(CONFIG_FILENAME_BACKUP) ) &&
                ( FileFS.exists(CREDENTIALS_FILENAME) || FileFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
#else
      else if ( FileFS.exists(CONFIG_FILENAME) || FileFS.exists(CONFIG_FILENAME_BACKUP) )
#endif 
      {
        // if config file exists, load
        loadConfigData();
        
#if ( BLYNK_WM_DEBUG > 2)      
        BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
        displayConfigData(BlynkEthernet_WM_config);
#endif

        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkEthernet_WM_config.checkSum, HEX));
                 
#if USE_DYNAMIC_PARAMETERS                 
        // Load dynamic data
        dynamicDataValid = loadDynamicData();
        
        if (dynamicDataValid)
        {
  #if ( BLYNK_WM_DEBUG > 2)      
          BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
  #endif          
        }
  #if ( BLYNK_WM_DEBUG > 2)  
        else
        {
          BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Ignored"));
        }
  #endif
#endif
      }
      else    
      {
        // Not loading Default config data, but having no config file => Config Portal
        return false;
      }      

      if ( (strncmp(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkEthernet_WM_config.checkSum) || !dynamicDataValid )
      {       
        // Including Credentials CSum
        BLYNK_LOG2(BLYNK_F("InitCfgFile,sz="), sizeof(BlynkEthernet_WM_config));

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkEthernet_WM_config, &defaultConfig, sizeof(BlynkEthernet_WM_config));
        }
        else
        {
          memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));

#if USE_DYNAMIC_PARAMETERS
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
          }
#endif
              
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token,    WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token,    WM_NO_CONFIG);
          BlynkEthernet_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkEthernet_WM_config.static_IP,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.board_name,  WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS          
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif          
        }
    
        strcpy(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE);
        
        #if (USE_DYNAMIC_PARAMETERS && ( BLYNK_WM_DEBUG > 2))
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
        #endif
        
        // Don't need
        BlynkEthernet_WM_config.checkSum = 0;

        saveConfigData();
        
        return false;
      }
      else if ( !strncmp(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server, WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token,  WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server, WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token,  WM_NO_CONFIG, strlen(WM_NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
#if ( BLYNK_WM_DEBUG > 2)        
        displayConfigData(BlynkEthernet_WM_config);
#endif        
        return false;
      }
      else
      {
        displayConfigData(BlynkEthernet_WM_config);
      }

      return true;
    }

#else

  #ifndef EEPROM_SIZE
    #define EEPROM_SIZE     2048
  #else
    #if (EEPROM_SIZE > 2048)
      #warning EEPROM_SIZE must be <= 2048. Reset to 2048
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     2048
    #endif
    // FLAG_DATA_SIZE is 4, to store DRD flag
    #if (EEPROM_SIZE < FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      #warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     2048
    #endif
  #endif

  #ifndef EEPROM_START
    #define EEPROM_START     0      //define 256 in DRD/MRD
  #else
    #if (EEPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE)
      #error EPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE. Please adjust.
    #endif
  #endif

  // Stating positon to store Blynk8266_WM_config
  #define BLYNK_EEPROM_START    (EEPROM_START + FLAG_DATA_SIZE)
  
  //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("setForcedCP"));
#endif
      
      EEPROM.put(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      EEPROM.commit();
    }
    //////////////////////////////////////////////
    
    void clearForcedCP()
    {
#if ( BLYNK_WM_DEBUG > 2)    
      BLYNK_LOG1(BLYNK_F("clearForcedCP"));
#endif
      
      EEPROM.put(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, 0);
      EEPROM.commit();
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("Check if isForcedCP"));
#endif
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      EEPROM.get(BLYNK_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////
    
#if USE_DYNAMIC_PARAMETERS
    
    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      
      #define BUFFER_LEN      128
      char readBuffer[BUFFER_LEN + 1];
      
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkEthernet_WM_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          BLYNK_LOG1(BLYNK_F("ChkCrR: Error Small Buffer."));
          return false;
        }
      }
         
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;
        
        // Prepare buffer, more than enough
        memset(readBuffer, 0, sizeof(readBuffer));
        
        // Read more than necessary, but OK and easier to code
        EEPROM.get(offset, readBuffer);
        // NULL terminated
        readBuffer[myMenuItems[i].maxlen] = 0;

#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);
#endif          
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }   
        
        offset += myMenuItems[i].maxlen;    
      }

      EEPROM.get(offset, readCheckSum);
           
      BLYNK_LOG4(F("ChkCrR:CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
           
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    //////////////////////////////////////////////
    
    bool EEPROM_getDynamicData()
    {
      int readCheckSum;
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkEthernet_WM_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
           
      totalDataSize = sizeof(BlynkEthernet_WM_config) + sizeof(readCheckSum);
      
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;
        
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          *_pointer = EEPROM.read(offset);
          
          checkSum += *_pointer;  
         }    
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif             
      }
      
      EEPROM.get(offset, readCheckSum);
      
      BLYNK_LOG4(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;
    }
    
    //////////////////////////////////////////////

    void EEPROM_putDynamicData()
    {
      int checkSum = 0;
      uint16_t offset = BLYNK_EEPROM_START + sizeof(BlynkEthernet_WM_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        
#if ( BLYNK_WM_DEBUG > 2)        
        BLYNK_LOG4(F("CW:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
#endif
                            
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          EEPROM.write(offset, *_pointer);
          
          checkSum += *_pointer;     
         }
      }
      
      EEPROM.put(offset, checkSum);
      //EEPROM.commit();
      
      BLYNK_LOG2(F("CrWCSum=0x"), String(checkSum, HEX));
    }
#endif

    //////////////////////////////////////////////

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      BlynkEthernet_WM_config.checkSum = calChecksum;
      BLYNK_LOG2(BLYNK_F("Save,WCSum=0x"), String(calChecksum, HEX));

      EEPROM.put(BLYNK_EEPROM_START, BlynkEthernet_WM_config);
      
#if USE_DYNAMIC_PARAMETERS        
      EEPROM_putDynamicData();
#endif 
      
      EEPROM.commit();
    }

    //////////////////////////////////////////////
    
    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&BlynkEthernet_WM_config, &defaultConfig, sizeof(BlynkEthernet_WM_config));
      strcpy(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE);
      
      // Including config and dynamic data, and assume valid
      saveConfigData();
      
#if ( BLYNK_WM_DEBUG > 2)      
      BLYNK_LOG1(BLYNK_F("======= Start Loaded Config Data ======="));
      displayConfigData(BlynkEthernet_WM_config);
#endif      
    }
    
    //////////////////////////////////////////////
    
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;
      
      hadConfigData = false; 
      
      EEPROM.begin(EEPROM_SIZE);
      BLYNK_LOG2(BLYNK_F("EEPROMsz:"), EEPROM_SIZE);
      
      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        loadAndSaveDefaultConfigData();
        
        // Don't need Config Portal anymore
        return true;            
      }
      else
      {   
        // Load stored config from EEPROM-simulated FlashStorage
        EEPROM.get(BLYNK_EEPROM_START, BlynkEthernet_WM_config);
                
        // Verify ChkSum
        calChecksum = calcChecksum();

        BLYNK_LOG4(BLYNK_F("CCSum=0x"), String(calChecksum, HEX),
                   BLYNK_F(",RCSum=0x"), String(BlynkEthernet_WM_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS        
        // Load stored dynamic data from dueFlashStorage
        dynamicDataValid = checkDynamicData();
#endif
        // If checksum = 0 => DueFlashStorage has been cleared (by uploading new FW, etc) => force to CP
        // If bad checksum = 0 => force to CP
        if ( (calChecksum != 0) && (calChecksum == BlynkEthernet_WM_config.checkSum) )
        {                   
          if (dynamicDataValid)
          {
  #if USE_DYNAMIC_PARAMETERS
            // CkSum verified, Now get valid dynamic data
            EEPROM_getDynamicData();
            
    #if ( BLYNK_WM_DEBUG > 2 )
            BLYNK_LOG1(BLYNK_F("Valid Stored Dynamic Data"));
    #endif
  #endif  
                 
            BLYNK_LOG1(BLYNK_F("======= Start Stored Config Data ======="));
            displayConfigData(BlynkEthernet_WM_config);
            
            // Don't need Config Portal anymore
            return true;
          }
          else
          {
            // Invalid Stored config data => Config Portal
            BLYNK_LOG1(BLYNK_F("Invalid Stored Dynamic Data. Load default from Sketch"));
            
            // Load Default Config Data from Sketch, better than just "blank"
            loadAndSaveDefaultConfigData();
                             
            // Need Config Portal here as data can be just dummy
            // Even if you don't open CP, you're OK on next boot if your default config data is valid 
            return false;
          }
        }   
      } 

      if ( (strncmp(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE, strlen(BLYNK_BOARD_TYPE)) != 0) ||
           (calChecksum != BlynkEthernet_WM_config.checkSum) || !dynamicDataValid || 
           ( (calChecksum == 0) && (BlynkEthernet_WM_config.checkSum == 0) ) )
      {
        // Including Credentials CSum
        BLYNK_LOG2(F("InitCfgFile,Sz="), sizeof(BlynkEthernet_WM_config));

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&BlynkEthernet_WM_config, &defaultConfig, sizeof(BlynkEthernet_WM_config));
        }
        else
        {  
          memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));

#if USE_DYNAMIC_PARAMETERS
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
          }
#endif
          
          // Including Credentials CSum
          BLYNK_LOG2(BLYNK_F("InitCfgFile,DataSz="), totalDataSize);

          // doesn't have any configuration
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token,    WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token,    WM_NO_CONFIG);
          BlynkEthernet_WM_config.blynk_port = BLYNK_SERVER_HARDWARE_PORT;
          strcpy(BlynkEthernet_WM_config.static_IP,   WM_NO_CONFIG);
          strcpy(BlynkEthernet_WM_config.board_name,  WM_NO_CONFIG);

#if USE_DYNAMIC_PARAMETERS          
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif          
        }
    
        strcpy(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE);
        
#if ( BLYNK_WM_DEBUG > 2) && USE_DYNAMIC_PARAMETERS
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          BLYNK_LOG4(BLYNK_F("g:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
        }
#endif
                
        // Don't need
        BlynkEthernet_WM_config.checkSum = 0;

        saveConfigData();

        return false;
      }
      else if ( !strncmp(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server, WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token,  WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server, WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token,  WM_NO_CONFIG, strlen(WM_NO_CONFIG) ) )
      {
        // If SSID, PW, Server,Token ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(BlynkEthernet_WM_config);
      }

      return true;
    }

#endif      // SPIFFS

    //////////////////////////////////////////////

    bool connectMultiBlynk()
    {
#define BLYNK_CONNECT_TIMEOUT_MS      10000L

      for (uint16_t i = 0; i < NUM_BLYNK_CREDENTIALS; i++)
      {
        config(BlynkEthernet_WM_config.Blynk_Creds[i].blynk_token,
               BlynkEthernet_WM_config.Blynk_Creds[i].blynk_server, BlynkEthernet_WM_config.blynk_port);

        if (this->connect(BLYNK_CONNECT_TIMEOUT_MS) )
        {
          BLYNK_LOG4(BLYNK_F("Connected to Blynk Server = "), BlynkEthernet_WM_config.Blynk_Creds[i].blynk_server,
                     BLYNK_F(", Token = "), BlynkEthernet_WM_config.Blynk_Creds[i].blynk_token);
                     
          currentBlynkServerIndex = i;           
          return true;
        }
      }

      BLYNK_LOG1(BLYNK_F("Blynk not connected"));

      return false;
    }

    //////////////////////////////////////////////
    
    void createHTML(String& root_html_template)
    {
      String pitem;
      
      root_html_template = String(BLYNK_WM_HTML_HEAD);

#if USE_DYNAMIC_PARAMETERS     
      if (NUM_MENU_ITEMS > 0)
      {
        root_html_template += String(BLYNK_WM_FLDSET_START);
           
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          pitem = String(BLYNK_WM_HTML_PARAM);

          pitem.replace("{b}", myMenuItems[i].displayName);
          pitem.replace("{v}", myMenuItems[i].id);
          pitem.replace("{i}", myMenuItems[i].id);
          
          root_html_template += pitem;      
        }
#endif
            
        root_html_template += String(BLYNK_WM_FLDSET_END);
      }
      
      root_html_template += String(BLYNK_WM_HTML_BUTTON) + String(BLYNK_WM_HTML_SCRIPT);   

#if USE_DYNAMIC_PARAMETERS           
      if (NUM_MENU_ITEMS > 0)
      {        
        for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          pitem = String(BLYNK_WM_HTML_SCRIPT_ITEM);
          
          pitem.replace("{d}", myMenuItems[i].id);
          
          root_html_template += pitem;        
        }
      }
#endif
      
      root_html_template += String(BLYNK_WM_HTML_SCRIPT_END) + String(BLYNK_WM_HTML_END);
      
      return;      
    }
    
    //////////////////////////////////////////////
    
    void handleRequest()
    {
      if (server)
      {
        String key = server->arg("key");
        String value = server->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          String result;
          createHTML(result);

#if (BLYNK_WM_DEBUG > 2)
          BLYNK_LOG2(BLYNK_F("h:Repl:"), result);
#endif

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;
          
          if ( RFC952_hostname[0] != 0 )
          {
            // Replace only if Hostname is valid
            result.replace("BlynkEthernet_ESP32_WM", RFC952_hostname);
          }
          else if ( BlynkEthernet_WM_config.board_name[0] != 0 )
          {
            // Or replace only if board_name is valid.  Otherwise, keep intact
            result.replace("BlynkEthernet_ESP32_WM", BlynkEthernet_WM_config.board_name);
          }

          if (hadConfigData)
          {
            result.replace("[[sv]]",     BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server);
            result.replace("[[tk]]",     BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token);
            result.replace("[[sv1]]",    BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server);
            result.replace("[[tk1]]",    BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token);
            result.replace("[[pt]]",     String(BlynkEthernet_WM_config.blynk_port));
            result.replace("[[ip]]",     BlynkEthernet_WM_config.static_IP);
            result.replace("[[nm]]",     BlynkEthernet_WM_config.board_name);
          }
          else
          {            
            // Better than garbage
            result.replace("[[sv]]",     "blynk_server");
            result.replace("[[tk]]",     "blynk_token");
            result.replace("[[sv1]]",    "blynk_server1");
            result.replace("[[tk1]]",    "blynk_token1");
            result.replace("[[pt]]",     "8080");
            result.replace("[[ip]]",     "0");
            result.replace("[[nm]]",     BLYNK_BOARD_TYPE);
          }

#if USE_DYNAMIC_PARAMETERS
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            String toChange = String("[[") + myMenuItems[i].id + "]]";
            result.replace(toChange, myMenuItems[i].pdata);

#if ( BLYNK_WM_DEBUG > 2)                 
            BLYNK_LOG4(BLYNK_F("h1:myMenuItems["), i, BLYNK_F("]="), myMenuItems[i].pdata );
#endif            
          }
#endif

          server->send(200, "text/html", result);
          
          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&BlynkEthernet_WM_config, 0, sizeof(BlynkEthernet_WM_config));
          strcpy(BlynkEthernet_WM_config.header, BLYNK_BOARD_TYPE);
        }

#if USE_DYNAMIC_PARAMETERS
        if (!menuItemUpdated)
        {
          // Don't need to free
          menuItemUpdated = new bool[NUM_MENU_ITEMS];
          
          if (menuItemUpdated)
          {
            for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
            {           
              // To flag item is not yet updated
              menuItemUpdated[i] = false;           
            }
  #if ( BLYNK_WM_DEBUG > 2)                 
            BLYNK_LOG1(BLYNK_F("h: Init menuItemUpdated" ));
  #endif                        
          }
          else
          {
            BLYNK_LOG1(BLYNK_F("h: Error can't alloc memory for menuItemUpdated" ));
          }
        }  
#endif

        static bool sv_Updated  = false;
        static bool tk_Updated  = false;
        static bool sv1_Updated = false;
        static bool tk1_Updated = false;     
        static bool pt_Updated  = false;
        static bool ip_Updated  = false;
        static bool nm_Updated  = false;
   
        if (!sv_Updated && (key == String("sv")))
        {
#if (BLYNK_WM_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("h:sv"));
#endif
          sv_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server) - 1)
            strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server, value.c_str(), sizeof(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_server) - 1);
        }
        else if (!tk_Updated && (key == String("tk")))
        {
#if ( BLYNK_WM_DEBUG > 2)        
          BLYNK_LOG1(BLYNK_F("h:tk"));
#endif
          tk_Updated = true;         
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token) - 1)
            strcpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token, value.c_str(), sizeof(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token) - 1);
        }    
        else if (!sv1_Updated && (key == String("sv1")))
        {
#if (BLYNK_WM_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("h:sv1"));
#endif
          sv1_Updated = true;
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server) - 1)
            strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server, value.c_str(), sizeof(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_server) - 1);
        }
        else if (!tk1_Updated && (key == String("tk1")))
        {
#if ( BLYNK_WM_DEBUG > 2)        
          BLYNK_LOG1(BLYNK_F("h:tk1"));
#endif
          tk1_Updated = true;         
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token) - 1)
            strcpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token, value.c_str(), sizeof(BlynkEthernet_WM_config.Blynk_Creds[1].blynk_token) - 1);
        }               
        else if (!pt_Updated && (key == String("pt")))
        {
#if (BLYNK_WM_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("h:pt"));
#endif
          pt_Updated = true;
          number_items_Updated++;
          BlynkEthernet_WM_config.blynk_port = value.toInt();
        }
        else if (!ip_Updated && (key == String("ip")))
        {
#if ( BLYNK_WM_DEBUG > 2)        
          BLYNK_LOG1(BLYNK_F("h:ip"));
#endif
          ip_Updated = true;         
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.static_IP) - 1)
            strcpy(BlynkEthernet_WM_config.static_IP, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.static_IP, value.c_str(), sizeof(BlynkEthernet_WM_config.static_IP) - 1);
        }
        else if (!nm_Updated && (key == String("nm")))
        {
#if ( BLYNK_WM_DEBUG > 2)        
          BLYNK_LOG1(BLYNK_F("h:nm"));
#endif
          nm_Updated = true;         
          number_items_Updated++;
          
          if (strlen(value.c_str()) < sizeof(BlynkEthernet_WM_config.board_name) - 1)
            strcpy(BlynkEthernet_WM_config.board_name, value.c_str());
          else
            strncpy(BlynkEthernet_WM_config.board_name, value.c_str(), sizeof(BlynkEthernet_WM_config.board_name) - 1);
        }

#if USE_DYNAMIC_PARAMETERS  
        else
        {      
          for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            if ( !menuItemUpdated[i] && (key == myMenuItems[i].id) )
            {
              BLYNK_LOG4(BLYNK_F("h:"), myMenuItems[i].id, BLYNK_F("="), value.c_str() );
              
              menuItemUpdated[i] = true;
              
              number_items_Updated++;

              // Actual size of pdata is [maxlen + 1]
              memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

              if ((int) strlen(value.c_str()) < myMenuItems[i].maxlen)
                strcpy(myMenuItems[i].pdata, value.c_str());
              else
                strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);
                
              break;  
            }
          }
        }
#endif
        
#if ( BLYNK_WM_DEBUG > 2)   
        BLYNK_LOG2(F("h:items updated ="), number_items_Updated);
        BLYNK_LOG4(F("h:key ="), key, ", value =", value);
#endif
        
        server->send(200, "text/html", "OK");

#if USE_DYNAMIC_PARAMETERS        
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
#else
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
#endif
        {
#if USE_LITTLEFS
          BLYNK_LOG1(BLYNK_F("h:Updating LittleFS"));     
#elif USE_SPIFFS
          BLYNK_LOG1(BLYNK_F("h:Updating SPIFFS"));
#else
          BLYNK_LOG1(BLYNK_F("h:Updating EEPROM. Please wait for reset"));
#endif

          saveConfigData();
          
          // Done with CP, Clear CP Flag here if forced
          if (isForcedConfigPortal)
            clearForcedCP();

#if (BLYNK_WM_DEBUG > 2)
          BLYNK_LOG1(BLYNK_F("h:Rst"));
#endif

          // Delay then reset after save data
          delay(200);
          resetFunc();
        }
      }     // if (server)
    }
    
    //////////////////////////////////////////////

#ifndef CONFIG_TIMEOUT
  #warning Default CONFIG_TIMEOUT = 60s
  #define CONFIG_TIMEOUT			60000L
#endif

    void startConfigurationMode()
    {
      // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);

      BLYNK_LOG2(BLYNK_F("CfgIP="), Ethernet.localIP() );

      if (!server)
        server = new EthernetWebServer;

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1

      if (server)
      {
        server->on("/", [this]() { handleRequest(); } );
        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      // or SSID, PW, Server,Token ="nothing"
      if (hadConfigData)
      {
        configTimeout = millis() + CONFIG_TIMEOUT;
        
#if ( BLYNK_WM_DEBUG > 2)                   
        BLYNK_LOG4(BLYNK_F("s:millis() = "), millis(), BLYNK_F(", configTimeout = "), configTimeout);
#endif
      }
      else
      {
        configTimeout = 0;
#if ( BLYNK_WM_DEBUG > 2)                   
        BLYNK_LOG1(BLYNK_F("s:configTimeout = 0"));
#endif        
      }  

      configuration_mode = true;
    }
    
    //////////////////////////////////////////

    bool connectEthernet()
    {
      // Check go see if static IP is required
      IPAddress staticIP;

      if (staticIP.fromString(BlynkEthernet_WM_config.static_IP))
      {
        // Use static IP
        Ethernet.begin(SelectMacAddress(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token, NULL), staticIP);
        ethernetConnected = true;
      }
      else
      {
        // If static_IP ="nothing"  or NULL, use DHCP dynamic IP
        ethernetConnected = Ethernet.begin(SelectMacAddress(BlynkEthernet_WM_config.Blynk_Creds[0].blynk_token, NULL));
      }

      // give the Ethernet shield a second to initialize:
      BlynkDelay(1000);

      if (ethernetConnected)
      {
        //IPAddress myip = Ethernet.localIP();
        //BLYNK_LOG_IP("IP:", myip);
        BLYNK_LOG_IP("IP:", Ethernet.localIP());
      }
      else
      {
        BLYNK_LOG1(BLYNK_F("DHCPFailed"));
      }

      return ethernetConnected;
    }
    
    //////////////////////////////////////////////

    byte* SelectMacAddress(const char* token, const byte mac[])
    {
      if (mac != NULL) 
      {
        return (byte*)mac;
      }

      macAddress[0] = 0xFE;
      macAddress[1] = 0xAB;
      macAddress[2] = 0xCD;
      macAddress[3] = 0xEF;
      macAddress[4] = 0xED;
      macAddress[5] = 0xBA;

      int len = strlen(token);
      int mac_index = 1;

      for (uint16_t i = 0; i < len; i++)
      {
        macAddress[mac_index] ^= token[i];

        if (++mac_index > 5) {
          mac_index = 1;
        }
      }
      BLYNK_LOG("MAC:%02X-%02X-%02X-%02X-%02X-%02X",
                macAddress[0], macAddress[1],
                macAddress[2], macAddress[3],
                macAddress[4], macAddress[5]);

      return macAddress;
    }

    byte macAddress[6];

};

#endif    // BlynkEthernet_ESP32_WM_h
