#include <VS1053.h>
#include <WiFi.h>
#include <ClickEncoder.h>
#include "config.h"


ClickEncoder encoder = ClickEncoder(ENCODER_S1, ENCODER_S2, ENCODER_KEY, ENCODER_STEPS);
VS1053 player(VS1053_XCS, VS1053_XDCS, VS1053_DREQ);

WiFiClient client;

// перивання викликане обертом енкодера
void IRAM_ATTR isrEncoderRotate()
{
	encoder.service();
	portENTER_CRITICAL_ISR(&mux);
	gv_nVolume += encoder.getValue();
	portEXIT_CRITICAL_ISR(&mux);
}

void SetVolume()
{
	uint8_t nCurrVolume = 0;
	portENTER_CRITICAL(&mux);
	nCurrVolume = gv_nVolume;
	portEXIT_CRITICAL(&mux);

	if(player.getVolume() != nCurrVolume)
	{
        if(nCurrVolume >= VOLUME_MAX)
        {			
			nCurrVolume = VOLUME_MAX;
			portENTER_CRITICAL(&mux);
			gv_nVolume = VOLUME_MAX;
			portEXIT_CRITICAL(&mux);
        }
        if(nCurrVolume <= VOLUME_MIN)
        {
			nCurrVolume = VOLUME_MIN;
			portENTER_CRITICAL(&mux);
			gv_nVolume = VOLUME_MIN;
			portEXIT_CRITICAL(&mux);
        }
        player.setVolume(nCurrVolume);
        Serial.printf("Vol = %d\r\n", nCurrVolume);
	}
}

void setup () {
		Serial.begin(115200);

		// Ініціалізація енкодера
		encoder.setButtonHeldEnabled(true);
		encoder.setDoubleClickEnabled(true);
		encoder.setAccelerationEnabled(false);
		// Enable the button to be on pin 0.  Normally pin 0 is not recognized as a valid pin for a button,
		// this is to maintain backward compatibility with an old version of the library
		// This version can have the button on pin zero, and this call enables the feature.
		// in this version best to use pin -1 instead of 0 to disable button functions
		encoder.setButtonOnPinZeroEnabled(true);

// ініціалізація переривань для енкодера
		pinMode(ENCODER_S1, INPUT);
		attachInterrupt(ENCODER_S1, &isrEncoderRotate, CHANGE);
		pinMode(ENCODER_S2, INPUT);
		attachInterrupt(ENCODER_S2, &isrEncoderRotate, CHANGE);
		// pinMode(ENCODER_KEY, INPUT);
		// attachInterrupt(digitalPinToInterrupt(ENCODER_KEY), &isrOnTimer, CHANGE);


		// Wait for VS1053 and PAM8403 to power up
		// otherwise the system might not start up correctly
		delay(3000);

		// This can be set in the IDE no need for ext library
		// system_update_cpu_freq(160);
		Serial.println("\n\nSimple Radio Node WiFi Radio");

		SPI.begin();

		player.begin();
		player.switchToMp3Mode();
        player.setVolume(VOLUME_DEFAULT);

        portENTER_CRITICAL(&mux);
		gv_nVolume = VOLUME_DEFAULT;
        portEXIT_CRITICAL(&mux);

		Serial.print("Connecting to SSID "); Serial.println(ssid);
		WiFi.begin(ssid, password);

		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
		}

		Serial.println("WiFi connected");
		Serial.println("IP address: ");  Serial.println(WiFi.localIP());

		Serial.print("connecting to ");  Serial.println(host);

		if (!client.connect(host, httpPort)) {
			Serial.println("Connection failed");
			return;
		}

		Serial.print("Requesting stream: ");
		Serial.println(path);
		// client.print("GET /RadioROKS_HardnHeavy HTTP/1.0\r\n"
		//               "Host: online2.radioroks.ua\r\n"
		//               "User-Agent: WinampMPEG/5.66, Ultravox/2.1\r\n"
		//               "Ultravox-transport-type: TCP\r\n"
		//               "Accept: */*\r\n"
		//               "Icy-MetaData:1\r\n"
		//               "Connection: close\r\n");
		client.print(String("GET ") + path + " HTTP/1.1\r\n" +
									"Host: " + host + "\r\n" +
									"Connection: close\r\n\r\n");
}

void loop() {
		if(!client.connected()){
			Serial.println("Reconnecting...");
			if(client.connect(host, httpPort)){
				client.print(String("GET ") + path + " HTTP/1.1\r\n" +
									"Host: " + host + "\r\n" +
									"Connection: close\r\n\r\n");
			}
		}

		if(client.available() > 0){
			uint8_t bytesread = client.read(g_nStreamBuff, BUFF_SIZE);
			player.playChunk(g_nStreamBuff, bytesread);
		}

		SetVolume();
}
// /**
//   A simple example to use ESP_VS1053_Library (plays a test sound every 3s)
//   https://github.com/baldram/ESP_VS1053_Library
//   If you like this project, please add a star.

//   Copyright (C) 2018 Marcin Szalomski (github.com/baldram)
//   Licensed under GNU GPL v3

//   The circuit (example wiring for ESP8266 based board like eg. LoLin NodeMCU V3):
//   ---------------------
//   | VS1053  | ESP8266 |
//   ---------------------
//   |   SCK   |   D5    |
//   |   MISO  |   D6    |
//   |   MOSI  |   D7    |
//   |   XRST  |   RST   |
//   |   CS    |   D1    |
//   |   DCS   |   D0    |
//   |   DREQ  |   D3    |
//   |   5V    |   VU    |
//   |   GND   |   G     |
//   ---------------------

//   Note: It's just an example, you may use a different pins definition.
//   For ESP32 example, please follow the link:
//     https://github.com/baldram/ESP_VS1053_Library/issues/1#issuecomment-313907348

//   To run this example define the platformio.ini as below.

//   [env:nodemcuv2]
//   platform = espressif8266
//   board = nodemcuv2
//   framework = arduino

//   lib_deps =
//     ESP_VS1053_Library

// */

// // This ESP_VS1053_Library
// #include <VS1053.h>
// #include "config.h"
// // Please find SampleMp3.h file here:
// //   github.com/baldram/ESP_VS1053_Library/blob/master/examples/Mp3PlayerDemo/SampleMp3.h
// #include "SampleMp3.h"


// #define VOLUME  100 // volume level 0-100

// VS1053 player(VS1053_XCS, VS1053_XDCS, VS1053_DREQ);

// void setup () {
//     Serial.begin(115200);

//     // initialize SPI
//     SPI.begin();

//     Serial.println("Hello VS1053!\n");
//     // initialize a player
//     player.begin();
//     player.switchToMp3Mode(); // optional, some boards require this
//     player.setVolume(VOLUME);
// }

// void loop() {
//     Serial.println("Playing sound... ");

//     // play mp3 flow each 3s
//     player.playChunk(sampleMp3, sizeof(sampleMp3));
//     delay(3000);
// }
