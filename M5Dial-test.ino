#include <WiFi.h>

#include <M5Dial.h>

// DOCS: https://docs.m5stack.com/en/quick_start/m5dial/arduino

// see https://github.com/m5stack/M5Dial/tree/master/examples/Basic for other basic examples
// bigger examples:
// https://github.com/Gitshaoxiang/M5Dial-Gui-Demo
// https://github.com/m5stack/M5Dial-UserDemo

#define WIFI_SSID     "YOUR WIFI SSID NAME"
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"
#define NTP_TIMEZONE  "UTC+1"
#define NTP_SERVER    "pool.ntp.org"
//#define NTP_SERVER1   "0.pool.ntp.org"
//#define NTP_SERVER2   "1.pool.ntp.org"
//#define NTP_SERVER3   "2.pool.ntp.org"
//#if __has_include(<esp_sntp.h>)
//#include <esp_sntp.h>
//#define SNTP_ENABLED 1
//#elif __has_include(<sntp.h>)
// --- or ---
//#include <sntp.h>
//#define SNTP_ENABLED 1
//#endif

#ifndef SNTP_ENABLED
#define SNTP_ENABLED 0
#endif

//todo: see RTC and NTP

/*
M5GFX display = M5Dial.Display; //?
M5Canvas canvas(&display); //? use of canvas? to draw on memory before draw to display
*/

long previous_encoder_position = -999;

int previous_touch_x = -1;
int previous_touch_y = -1;
static m5::touch_state_t previous_touch_state;

LGFX_Button = touch_button_A;

/* Use M5.Log.println() ? 
   or
   M5_LOGV(), M5_LOGI(), etc ?
*/

void setup() {
    auto cfg = M5.config();
    M5Dial.begin(
        cfg,
        true, // enables rotary encoder
        false // enables RFID reader
    );

    /* Display */
    M5Dial.Display.fillScreen(BLACK);
    //font color
    M5Dial.Display.setTextColor(WHITE);
    //font alignment
    M5Dial.Display.setTextDatum(middle_center);
    //set font
    M5Dial.Display.setTextFont(&fonts::Orbitron_Light_32);
    //set font size
    M5Dial.Display.setTextSize(1);
    //print text
    M5Dial.Display.drawString(
        "Hello world",
        M5Dial.Display.width() / 2,
        (M5Dial.Display.height() / 2) + 5
    );
    
    touch_button_A.initButton(
        &M5Dial.Display,
        M5Dial.Display.width() / 2,
        (M5Dial.Display.height() / 2) -20,
        30,
        10,
        RED,
        BLACK,
        YELLOW,
        "Btn"
    );
    touch_button_A.drawButton();
}

void loop() {
    M5Dial.update();

    /* Button */
    if (M5Dial.BtnA.wasPressed()) {
        //Serial.println("Btn pressed");
    }
    if (M5Dial.BtnA.pressedFor(1000)) {
        //Serial.println("Btn pressed for 1000ms");
    }
    if (M5Dial.Btn1.wasReleased()) {
        //Serial.println("Btn released");
    }

    /* Rotary Encoder */
    long new_encoder_position = M5Dial.Encoder.read();
    if (new_encoder_position != previous_encoder_position) {
        previous_encoder_position = new_encoder_position;
        //Serial.println(new_encoder_position);
    }

    /* RFID Reader */
    if (M5Dial.Rfid.PICC_IsNewCardPresent() || M5Dial.Rfid.PICC_ReadCardSerial()) { // && or || ?
        //Serial.print(F("PICC type: "));
        uint8_t piccType = M5Dial.Rfid.PICC_GetType(M5Dial.Rfid.uid.sak);
        //Serial.println(M5Dial.Rfid.PICC_GetTypeName(piccType));

        // Check is the PICC of Classic MIFARE type
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
            piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
            piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            //Serial.println(F("Your tag is not of type MIFARE Classic."));
            return;
        }
        String uid = "";
        for (byte i=0; i<M5Dial.Rfid.uid.size; i++) {
            //Serial.printf("%02X", M5Dial.Rfid.uid.uidByte[i]);
            uid += String(M5Dial.Rfid.uid.uidByte[i], HEX);
        }
        //Serial.println();
    }

    /* Touch */
    auto touch = M5Dial.Touch.getDetail();
    if (previous_touch_state != touch.state) {
        previous_touch_state = touch.state;
        static constexpr const char* touch_state_name[16] = {
            "none", "touch", "touch_end", "touch_begin",
            "___", "hold", "hold_end", "hold_begin",
            "___", "drag", "drag_end", "drag_begin"
        };
        //Serial.println(touch_state_name[touch.state]);

        /* Touchscreen Button */
        // touch_button_A.justPressed() ?
        if (touch_button_A.contains(touch.x, touch.y)) {
            touch_button_A.press(true); // can be used after: button.isPressed()
            touch_button_A.drawButton(true);
            //Serial.println("Touchscreen button pressed");
        } else { // ?
            touch_button_A.press(false);
            touch_button_A.drawButton(false, "Test");
        }
    }
    if (previous_touch_x != touch.x || previous_touch_y != touch.y) {
        Serial.println("Touch - X:"+String(touch.x)+" Y:"+String(touch.y));
        previous_touch_x = touch.x;
        previous_touch_y = touch.y;
    }

    /* Power/Sleep */
    /*
    //Under battery power supply, after executing the timerSleep program, the device will shut down immediately, and RTC will generate an IRQ signal after the specified time to turn on the device.
    M5Dial.Power.timerSleep(5);
    // M5Dial.Power.timerSleep(const rtc_time_t& time);
    // M5Dial.Power.timerSleep(const rtc_date_t& date, const rtc_time_t&
    // time);
    // M5Dial.Power.powerOff(); shutdown
    */

    /* Speaker/Buzzer */
    M5Dial.Speaker.tone(4000, 20); // 4000Hz, 20ms ?
}
