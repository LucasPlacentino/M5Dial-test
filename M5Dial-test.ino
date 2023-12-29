#include <M5Dial.h>

// DOCS: https://docs.m5stack.com/en/quick_start/m5dial/arduino

// see https://github.com/m5stack/M5Dial/tree/master/examples/Basic for other basic examples
// bigger examples:
// https://github.com/Gitshaoxiang/M5Dial-Gui-Demo
// https://github.com/m5stack/M5Dial-UserDemo

void setup() {
    auto cfg = M5.config();
    M5Dial.begin(
        cfg,
        true, // enables rotzary encoder
        false // enables RFID reader
    );

    /* Display */
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
        M5Dial.Display.height() / 2
    );

}

long previous_encoder_position = 0;
int previous_touch_x = 0;
int previous_touch_y = 0;

static m5::touch_state_t previous_touch_state;

void loop() {
    /* Button */
    M5Dial.update();
    if (M5Dial.BtnA.wasPressed()) {
        //Serial.println("Btn pressed");
    }

    /* Rotary Encoder */
    long new_encoder_position = M5Dial.Encoder.read();
    if (new_encoder_position != previous_encoder_position) {
        previous_encoder_position = new_encoder_position;
        //Serial.println(new_encoder_position);
    }

    /* RFID Reader */
    if (M5Dial.Rfid.PICC_IsNewCardPresent() || M5Dial.Rfid.PICC_ReadCardSerial()) {
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
    }
    if (previous_touch_x != touch.x || previous_touch_y != touch.y) {
        Serial.println("Touch - X:"+String(touch.x)+" Y:"+String(touch.y));
        previous_touch_x = touch.x;
        previous_touch_y = touch.y;
    }

    /* Power */
    /*
    //Under battery power supply, after executing the timerSleep program, the device will shut down immediately, and RTC will generate an IRQ signal after the specified time to turn on the device.
    M5Dial.Power.timerSleep(5);
    // M5Dial.Power.timerSleep(const rtc_time_t& time);
    // M5Dial.Power.timerSleep(const rtc_date_t& date, const rtc_time_t&
    // time);
    // M5Dial.Power.powerOff(); shutdown
    */
}
