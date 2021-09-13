/*
 * Project deploy
 * Description:
 * Author: Rishub Handa 
 * Date:
 */

#include "Connection.h" 
#include "string.h" 
#include "neopixel/neopixel.h" 

/***************
 * STARTUP CONFIGURATION *
 ***************/ 

STARTUP(setUpInternet());
SYSTEM_MODE(SEMI_AUTOMATIC); 
SYSTEM_THREAD(ENABLED); 

/***************
 * PERIPHERALS *
 ***************/ 

int btn1 = D5; 
int btn2 = D6; 
int btn3 = D7; 

unsigned int senderSignalPin = D2; // SENDER SIGNAL (SENDER PLATE)
unsigned int senderGroundPlate = D1; // SENDER GROUND PLATE
unsigned int recVoltage = A4; // RECEIVER VOLTAGE READING
unsigned int recGroundPlate = D3; // RECEIVER GROUND PLATE 

int tray = A3; 
int slider = D4; 
int charger = D0; 
int battery_level = A1; 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, A5); 

/***************
 * GLOBALS *
 ***************/ 

String device_id = ""; 

enum event_id { btn1_id, btn2_id, btn3_id, tray_id, dispense_id }; 

bool btn1pressed = false; 
bool btn2pressed = false; 
bool btn3pressed = false; 
bool tray_opened = false; 
bool slider_opened = false; 
bool slider_LED = false; 

int slider_delay = 1000; 

long slider_time = 0; 
long sleep_time = 0; 

bool charger_connected = false; 

bool is_setup_mode = false; 
int num_setup_btn_presses = 0; 
int CODE_SIZE = 6; 

long check_wifi_time; 
int check_wifi_delay = 3000; 
TCPClient client; 
String server_URL = "aqueous-falls-74814.herokuapp.com"; 
// String server_URL = "medlocksolutions.com"; 
bool is_data_sent = false; 

const int OSM_frequency = 10000;
const int data_array_size = 60;
const int data_history_size = 30; // Number of samples in moving average 
const int strip_delay = 10; 
int data_array[data_array_size];
int data_array_idx = 0; 

struct Sample { 
    float OSMAve; 
    float OSMDev; 
}; 

// DEBUG: 
int zero_strips = 0; 
int one_strip = 0; 
int multi_strips = 0; 
bool is_dispensing = false; 
const int prediction_size = 100; 
const int pred_history_size = 50; 
int pred_array[prediction_size]; 
int pred_array_idx = 0; 

const int ave_spread = 400; 
const int ave_start = 150; 
int ave_array[ave_spread]; 

// std / 100 
const int std_spread = 250; 
const int std_start = 50; 
int std_array[std_spread]; 

enum predictions { zero, one, multi }; 


/*
*****************
* FUNCTION DECLARATIONS *
*****************
*/ 


// Setup functions  
void set_pins(); 
void setup_measure_strips(); 
void check_btns_setup(); 

// Check peripherals 
void check_btns(); 
bool check_btns_special_func(); 
void check_dispense(int ave, int std); 
void check_tray(); 
void check_slider(); 
void check_charger(); 

// Strip measurement 
void measure_OSM(); 
void measure_OSM_test(); 
void strips_signal(int inputFreq); 
void print_dispense_data(); 
void predict_is_dispensing(int ave, int std); 
void track_dispense(int ave, int std); 
void validate_dispense(); 
void handle_dispense(bool is_one); 

// Algorithm functions 
int moving_ave(int data_arr[], int startIdx, int history_size); 
int moving_std(int data_arr[], int startIdx, int ave, int history_size); 
float moving_ave_float(int data_arr[], int startIdx, int history_size); 
float moving_std_float(int data_arr[], int startIdx, int ave, int history_size); 
int summary_ave(int data_arr[], int size); 
int summary_std(int data_arr[], int size); 

// Lights 
void flash_lights(int num, int r, int g, int b); 
void hold_lights(int time, int r, int g, int b); 
void loading_lights(int num, int r, int g, int b); 
void wakeup_lights(bool isSetup); 
void low_battery_lights(); 

// Sleep 
void check_sleep(); 
void reset_sleep_time(); 
void sleep_sequence(); 

// WiFi 
void begin_listening(); 
void exit_listening_isr(); 
void check_auto_listening(); 

// Data Handler 
void store_data(unsigned long value, int type); 
unsigned long encoded(int type, unsigned long value); 
unsigned long timestamp(); 
void print_events(); 
void clear_user_memory(); 
bool is_memory_full(); 

// Data Transmission 
void send_data(); 
void transmit_to_server(String data_str); 
String get_server_res(); 
void handle_server_res(String res); 
String format_json(); 
String decode_name(unsigned long value); 




void setup() {

    // IMPLEMENT: Memory management, EEPROM configuration, setup mode 

    Serial.begin(); 

    int64_t get_id; EEPROM.get(2, get_id); 
    long id_check; EEPROM.get(2, id_check); 
    if(get_id == -1 || id_check == -65536) { 
        EEPROM.clear(); is_setup_mode = true; wakeup_lights(true); Serial.println("Setup mode. "); 
    } else { 
        if(is_memory_full()) { strip.setPixelColor(0, 24, 13, 24); strip.show(); }
        else { wakeup_lights(false); }
    }

    set_pins(); 
    setup_measure_strips(); 

    strip.begin(); strip.clear(); strip.show(); 

    Particle.connect(); 
    WiFi.setListenTimeout(180); 

    if(!WiFi.hasCredentials()) { begin_listening(); } 

    reset_sleep_time(); 

}

void loop() {

    check_auto_listening(); 
    // check_charger(); 

    if(!is_setup_mode) {
        measure_OSM_test(); 
        check_btns(); 
        check_slider(); 
        check_tray(); 
    } else {
        check_btns_setup(); 
    }

    check_sleep(); 
}

/*
*******************
* SETUP FUNCTIONS *
*******************
*/ 

void set_pins() {

    //Sender plates
    pinMode(senderSignalPin, OUTPUT);
    pinMode(senderGroundPlate, OUTPUT);

    //Receiver plates
    pinMode(recGroundPlate, OUTPUT);

    //Receiver circuit pins
    pinMode(recVoltage, INPUT); 

    // Buttons 
    pinMode(btn1, INPUT); 
    pinMode(btn2, INPUT); 
    pinMode(btn3, INPUT); 

    // Other peripherals 
    pinMode(charger, INPUT); 
    pinMode(battery_level, INPUT); 
    pinMode(slider, INPUT); 
    pinMode(tray, INPUT); 

    // TEST: 
    // Attach interrupts to buttons and tray 
    // attachInterrupt(btn1, check_btns, RISING); 
    // attachInterrupt(btn2, check_btns, RISING); 
    // attachInterrupt(btn3, check_btns, RISING); 
    // attachInterrupt(tray, check_tray, RISING); 
    
}

void setup_measure_strips() {

    //Disable all tones
    noTone(senderSignalPin);
    noTone(senderGroundPlate);
    noTone(recGroundPlate);

    //Sender plates
    digitalWrite(senderGroundPlate, LOW);

    //Receiver plates
    digitalWrite(recGroundPlate, LOW);

    //Creates a square wave with the frequency as an argument
    strips_signal(OSM_frequency);

} 

void check_btns_setup() {

    if(check_btns_special_func()) return; 

    if(digitalRead(btn1) == HIGH && !btn1pressed) {  reset_sleep_time();  btn1pressed = true; Serial.println("Button 1 Onclick. "); store_data(timestamp(), btn1_id);   } 
    if(digitalRead(btn2) == HIGH && !btn2pressed) { reset_sleep_time();  btn2pressed = true; Serial.println("Button 2 Onclick. "); store_data(timestamp(), btn2_id); } 
    if(digitalRead(btn3) == HIGH && !btn3pressed) { reset_sleep_time(); btn3pressed = true; Serial.println("Button 3 Onclick. "); store_data(timestamp(), btn3_id); } 

    if(digitalRead(btn1) == LOW && btn1pressed) { 
        // Particle.publish("Button 1 Setup. "); 
        reset_sleep_time(); flash_lights(3, 0, 20, 0); btn1pressed = false; num_setup_btn_presses += 1; 
    } 
    
    if(digitalRead(btn2) == LOW && btn2pressed) {
        // Particle.publish("Button 2 Setup. "); 
        reset_sleep_time(); flash_lights(3, 20, 20, 0); btn2pressed = false; num_setup_btn_presses += 1; 
    } 
    
    if(digitalRead(btn3) == LOW && btn3pressed) {
        // Particle.publish("Button 3 Setup. "); 
        reset_sleep_time(); flash_lights(3, 20, 0, 0); btn3pressed = false; num_setup_btn_presses += 1; 
    } 

    if(num_setup_btn_presses >= CODE_SIZE && WiFi.ready()) {
        Serial.println("Sending data from setup mode. "); 
        Serial.println(num_setup_btn_presses); 
        send_data(); 
    }

}


/*
*******************
* PERIPHERALS *
*******************
*/ 

void check_btns() {

    if(check_btns_special_func()) return; 
    
    if(digitalRead(btn1) == HIGH && !btn1pressed) {  reset_sleep_time();  btn1pressed = true; } 
    if(digitalRead(btn2) == HIGH && !btn2pressed) { reset_sleep_time();  btn2pressed = true; } 
    if(digitalRead(btn3) == HIGH && !btn3pressed) { reset_sleep_time(); btn3pressed = true; } 

    if(digitalRead(btn1) == LOW && btn1pressed) { 
        // Particle.publish("Button 1. "); 
        // long id_check; EEPROM.get(2, id_check); 
        // Particle.publish(String(id_check)); 
        reset_sleep_time(); flash_lights(3, 0, 20, 0); btn1pressed = false; store_data(timestamp(), btn1_id); 
    } 
    
    if(digitalRead(btn2) == LOW && btn2pressed) {
        // Particle.publish("Button 2. "); 
        reset_sleep_time(); flash_lights(3, 20, 20, 0); btn2pressed = false; store_data(timestamp(), btn2_id); 
    } 
    
    if(digitalRead(btn3) == LOW && btn3pressed) {
        // Particle.publish("Button 3. "); 
        reset_sleep_time(); flash_lights(3, 20, 0, 0); btn3pressed = false; store_data(timestamp(), btn3_id); 
    } 
    
}

bool check_btns_special_func() {
    
    int btn_press_time = 25; 

    if(digitalRead(btn1) == HIGH && digitalRead(btn2) == LOW && digitalRead(btn3) == HIGH) {
        int counter = 0; btn1pressed = false; btn2pressed = false; btn3pressed = false; 
        while(digitalRead(btn1) == HIGH && digitalRead(btn2) == LOW && digitalRead(btn3) == HIGH) { 
            counter++; delay(200); 
            if(counter > btn_press_time) break; 
        }
        if(counter > btn_press_time) { begin_listening(); }
        return true; 
    }

    if(digitalRead(btn1) == LOW && digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) {
        int counter = 0; btn1pressed = false; btn2pressed = false; btn3pressed = false; 
        while(digitalRead(btn1) == LOW && digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) { 
            counter++; delay(200); 
            if(counter > btn_press_time) break; 
        }
        // IMPLEMENT: Change for Production to only System.reset() 
        if(counter > btn_press_time) { flash_lights(1, 20, 20, 20); EEPROM.clear(); System.reset(); }
        return true; 
    }

    if(digitalRead(btn1) == HIGH && digitalRead(btn2) == HIGH && digitalRead(btn3) == LOW) {
        int counter = 0; btn1pressed = false; btn2pressed = false; btn3pressed = false; 
        while(digitalRead(btn1) == HIGH && digitalRead(btn2) == HIGH && digitalRead(btn3) == LOW) { 
            counter++; delay(200); 
            if(counter > btn_press_time) break; 
        }
        if(counter > btn_press_time) { flash_lights(2, 20, 20, 20); sleep_sequence(); }
        return true; 
    }

    if(digitalRead(btn1) == HIGH && digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) {
        int counter = 0; btn1pressed = false; btn2pressed = false; btn3pressed = false; 
        while(digitalRead(btn1) == HIGH && digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) { 
            counter++; delay(200); 
            if(counter > 35) break; 
        }
        if(counter > 35) { 
            flash_lights(5, 20, 20, 20); 
            WiFi.clearCredentials(); 
        }
        return true; 
    }

    return false; 
}

void check_slider() {

    if(is_dispensing) return; 
    if(digitalRead(slider) == LOW) {
        if(slider_opened) {
            strip.clear(); strip.show(); 
            slider_opened = false; 
        } 
        return; 
    }
    if(millis() > slider_time + slider_delay) {
        slider_opened = true; 
        reset_sleep_time(); 
        if(slider_LED) {
            strip.clear(); strip.show(); 
            slider_LED = false; 
        } else {
            strip.setPixelColor(0, 0, 0, 20); strip.show(); 
            slider_LED = true; 
        }

        slider_time = millis(); 
    }


}

// IMPLEMENT: This should be continuously checked 
void check_tray() {

    // TEST: using the while loop to continuously check tray rather than loop in the main function 
    // if(digitalRead(tray) == HIGH) {
    while(digitalRead(tray) == HIGH) { 
      reset_sleep_time(); 
      flash_lights(1, 20, 0, 0); 
      if(!tray_opened) { store_data(timestamp(), tray_id); tray_opened = true; } 
    } 

    if(digitalRead(tray) == LOW && tray_opened) { tray_opened = false; } 

} 

void check_charger() { 
    if(digitalRead(charger) == HIGH && charger_connected) {
      charger_connected = false; 
    }
    
    if(digitalRead(charger) == LOW && !charger_connected) {
      reset_sleep_time(); 
      flash_lights(3, 26, 14, 0); 
      charger_connected = true; 
    }
}



/******************
* STRIP MEASUREMENT *
******************/ 


void measure_OSM_test() {

    delay(strip_delay);

    if(data_array_idx >= data_array_size) {
        for(int i = 0; i < data_history_size; i++) {
            data_array[i] = data_array[data_array_idx - data_history_size + i]; 
        } 
        data_array_idx = data_history_size; 

    }

    int raw = analogRead(recVoltage);
    data_array[data_array_idx] = raw;
    int ave = moving_ave(data_array, data_array_idx, data_history_size);
    int std = moving_std(data_array, data_array_idx, ave, data_history_size); 

    data_array_idx++; 

    // DEBUG: visualize the data 
    // Serial.print(millis()); Serial.print(","); Serial.print(raw); Serial.print(","); Serial.print(ave); Serial.print(","); Serial.print(std); Serial.print("\n"); 

    check_dispense(ave, std); 

}


void measure_OSM() {

    setup_measure_strips();
    delay(strip_delay);

    float total_ave = 0; 
    float total_std = 0; 

    for (unsigned int i = 0; i < data_array_size; i++)
    {

        delay(strip_delay);

        unsigned int raw = analogRead(recVoltage);
        data_array[i] = raw;
        unsigned int average = moving_ave(data_array, data_array_size, data_history_size);
        unsigned int deviation = moving_std(data_array, average, data_array_size, data_history_size);

        total_ave += average; 
        total_std += deviation; 

        // DEBUG: visualize the data 
        Serial.print(millis()); Serial.print(","); Serial.print(raw); Serial.print(","); Serial.print(average); Serial.print(","); Serial.println(deviation); 


        // check_dispense(average, deviation); 

    }

    total_ave = (float) total_ave / data_array_size; 
    total_std = (float) total_std / data_array_size; 

}

void strips_signal(int inputFreq) { analogWrite(senderSignalPin, 127, inputFreq); } 

void check_dispense(int ave, int std) {

    predict_is_dispensing(ave, std); 
    float moving_pred = moving_ave_float(pred_array, pred_array_idx, pred_history_size); 

    // Track ave and std distributions 
    if(is_dispensing) { track_dispense(ave, std); }

    if(!is_dispensing && moving_pred >= 0.2) {
        is_dispensing = true; 
        Serial.println("Start dispensing. "); 
        // Particle.publish("Start dispensing. "); 
        strip.setPixelColor(0, 0, 0, 20); strip.show(); 
    } 

    if(is_dispensing && moving_pred < 0.2) {
        validate_dispense(); 
    } 
    
    pred_array_idx++; 

}

void predict_is_dispensing(int ave, int std) {
    if(pred_array_idx >= prediction_size) {
        for(int i = 0; i < pred_history_size; i++) {
            pred_array[i] = pred_array[pred_array_idx - pred_history_size + i]; 
        } 
        pred_array_idx = pred_history_size; 

    }

    // Prediction parameters 
    if(ave < 280) {
        pred_array[pred_array_idx] = zero; 
        if(is_dispensing) zero_strips++; 
    } else if (ave < 350) {
        pred_array[pred_array_idx] = one; 
        if(is_dispensing) one_strip++; 
    } else {
        pred_array[pred_array_idx] = multi; 
        if(is_dispensing) multi_strips++; 
    }
}

void track_dispense(int ave, int std) {
    if(ave >= ave_start && ave < ave_start + ave_spread) { ave_array[ave - ave_start]++; } 
    else if(ave < ave_start) { ave_array[0]++; } 
    else if(ave >= ave_start + ave_spread) { ave_array[ave_spread - 1]++; }

    int std_log = std / 100; 

    if(std_log >= std_start && std_log < std_start + std_spread) { std_array[std_log - std_start]++; } 
    else if(std_log < std_start) { std_array[0]++; } 
    else if(std_log >= std_start + std_spread) { std_array[std_spread - 1]++; }
}

// IMPLEMENT: validate that the event was actually a dispense 
void validate_dispense() {
    is_dispensing = false; 
    Serial.println("Stop dispensing. "); 

    // Print prediction counts 
    Serial.print(zero_strips); Serial.print(", "); 
    Serial.print(one_strip); Serial.print(", "); 
    Serial.println(multi_strips); 
    zero_strips = 0; one_strip = 0; multi_strips = 0; 

    // DEBUG: 
    // print_dispense_data(); 

    int ave_ave = summary_ave(ave_array, ave_spread) + ave_start; 
    int ave_std = summary_std(ave_array, ave_spread); 

    int std_ave = summary_ave(std_array, std_spread) + std_start; 
    int std_std = summary_std(std_array, std_spread); 

    Serial.println("SUMMARY: "); 
    Serial.print("Average: "); Serial.print(ave_ave); Serial.print(", "); Serial.println(ave_std); 
    Serial.print(" Variance: "); Serial.print(std_ave); Serial.print(", "); Serial.println(std_std); 

    // String summary = String(ave_ave) + "," + String(ave_std) + "," + String(std_ave) + "," + String(std_std); 
    // Particle.publish(summary); 
    if(ave_ave > 280) {
        Serial.println("DISPENSE COMPLETE. "); 
        handle_dispense(true); 
        hold_lights(1000, 0, 20, 0); 
    } else {
        Serial.println("Incomplete dispense. "); 
        strip.clear(); strip.show(); 
    }

    for(int i = 0; i < ave_spread; i++) { ave_array[i] = 0; } 
    for(int i = 0; i < std_spread; i++) { std_array[i] = 0; } 
}

void handle_dispense(bool is_one) {

    if(is_one) { 
        reset_sleep_time(); 
        store_data(timestamp(), dispense_id); 
        // Particle.publish("Dispense. "); 
    }
}

void print_dispense_data() {
    // Print ave and std distributions 
    Serial.println("Distribution of Averages"); 
    Serial.println("[ "); 
    for(int i = 0; i < ave_spread; i++) {
        Serial.print(String(ave_start + i)); Serial.print(": "); 
        Serial.print(String(ave_array[i])); Serial.print(", "); 
        for(int j = 0; j < ave_array[i]; j += 2) {
            Serial.print("*"); 
        }
        Serial.println(""); 
    }
    Serial.println("]. "); 

    Serial.println("Distribution of Deviation"); 
    Serial.println("[ "); 
    for(int i = 0; i < std_spread; i++) {
        Serial.print(String(std_start + i)); Serial.print(": "); 
        Serial.print(String(std_array[i])); Serial.print(", "); 
        for(int j = 0; j < std_array[i]; j += 2) {
            Serial.print("*"); 
        }
        Serial.println(""); 
    }
    Serial.println("]. "); 

}



/***************
 * ALGORITHM FUNCTIONS *
 ***************/ 

int moving_ave(int data_arr[], int startIdx, int history_size) { 
    int sum = 0;
    for (int i = 0; i < history_size; i++) { sum += data_arr[startIdx - i]; }
    return (sum / history_size);
}

float moving_ave_float(int data_arr[], int startIdx, int history_size) { 
    int sum = 0;
    for (int i = 0; i < history_size; i++) { sum += data_arr[startIdx - i]; }
    return ((float)sum / (float)history_size);
}

int moving_std(int data_arr[], int startIdx, int ave, int history_size) { 
    int sum = 0;
    int temp = 0;

    for (int i = 0; i < history_size; i++) {
        temp = (data_arr[startIdx - i] - ave);
        temp = temp * temp;
        sum += temp;
    }

    return (sum / history_size);
}

float moving_std_float(int data_arr[], int startIdx, int ave, int history_size) { 
    int sum = 0;
    int temp = 0;

    for (int i = 0; i < history_size; i++) {
        temp = (data_arr[startIdx - i] - ave);
        temp = temp * temp;
        sum += temp;
    }

    return ((float)sum / (float)history_size);
}

int summary_ave(int data_arr[], int size) {
    int res = 0; 
    int n = 0; 
    // Not counting end values 
    for(int i = 0; i < size; i++) {
        res += i * data_arr[i]; 
        n += data_arr[i]; 
    }
    // return ((float)res / (float)n); 
    return res / n; 

}

int summary_std(int data_arr[], int size) { 
    int ave = summary_ave(data_arr, size); 
    int res = 0; 
    int temp = 0; 
    int n = 0; 
    // Not counting end values 
    for(int i = 0; i < size; i++) {
        temp = i - ave; 
        temp = temp * temp; 
        res += temp * data_arr[i]; 
        n += data_arr[i]; 
    }
    // return ((float)res / (float)n); 
    return res / n; 
}



/***************
 * WIFI FUNCTIONS *
 ***************/ 

void check_auto_listening() {
    if(WiFi.listening()) begin_listening(); 
}

void begin_listening() {
    WiFi.on(); 
    attachInterrupt(btn2, exit_listening_isr, RISING); 
    WiFi.listen(); 

    while(WiFi.listening()) { flash_lights(1, 20, 20, 20); }
    while(WiFi.connecting()) { flash_lights(1, 20, 20, 20); flash_lights(1, 0, 20, 20); }

    detachInterrupt(btn2); 

    if(WiFi.ready()) {
        hold_lights(2000, 0, 20, 0); 
    } else { hold_lights(2000, 20, 0, 0); } 


}

void exit_listening_isr() {
    detachInterrupt(btn2); 
    WiFi.listen(false); 
}






/***************
 * DATA HANDLER *
 ***************/ 

/*** EPROM Layout: 
 * 0-1: Current Address
 * 2-25: ID 
 * 26-29: Current Time 
 * 30 + 4i: Value 
 ***/ 

void store_data(unsigned long value, int type) { 

    if(is_memory_full()) return; 

    // Encode Data 
    unsigned long encoded_value = encoded(type, value); 

    // Store Data 
    short curr_address; 
    EEPROM.get(0, curr_address); 
    if(curr_address == -1 || curr_address == 65535 || curr_address == 0) {
        EEPROM.put(0, 30ul); 
        curr_address = 30; 
    }

    if(curr_address > EEPROM.length()) { flash_lights(5, 20, 0, 0); return; }

    EEPROM.put(curr_address, encoded_value); 

    // Update Index 
    curr_address += 4; 
    EEPROM.put(0, curr_address); 

    is_data_sent = false; 

} 

bool is_memory_full() {
    short curr_address; EEPROM.get(0, curr_address); 
    if(curr_address > 2000) return true; 
    else return false; 
}

unsigned long encoded(int type, unsigned long value) {
    unsigned long mask = ~0xF; 
    value &= mask; 
    value |= type; 
    return (unsigned long)value; 	
} 

unsigned long timestamp() {
    // Sync Time 
    // TEST: How long it takes to update time if Time.isValid() is true 
    if(!Time.isValid() && Particle.connected()) {
        Particle.syncTime(); waitUntil(Particle.syncTimeDone); 
        if(Time.isValid()) EEPROM.put(26, Time.now()); 
    }

    return Time.now(); 
}

void print_events() {
    short curr_address; 
    EEPROM.get(0, curr_address); 
    if(curr_address == -1 || curr_address == 65535 || curr_address == 0) { Serial.println("No address stored. "); return; } 

    char get_id[24]; 
    EEPROM.get(2, get_id); 

    unsigned long curr_time; 
    EEPROM.get(26, curr_time); 

    Serial.print("Last Updated: "); Serial.println(curr_time); 
    Serial.print("Data: [ "); 
        for(unsigned int i = 30; i < curr_address; i += 4) { 
            u_int32_t value; 
            EEPROM.get(i, value); 
            Serial.print(value); 
            Serial.print(", "); 
        } 
        Serial.println("]. "); 
} 

void clear_user_memory() {
    print_events(); 
    
    char get_id[24]; 
    EEPROM.get(2, get_id); 
    long curr_time = Time.now(); 

    EEPROM.clear(); 
    EEPROM.put(0, 30ul); 
    EEPROM.put(2, get_id); 
    EEPROM.put(26, curr_time); 

    print_events(); 

} 

/***************
 * DATA TRANSMISSION *
 ***************/ 

void send_data() {

    String json_string = format_json(); 
    String server_res;

    if(!WiFi.ready()) { return; } 

    loading_lights(1, 0, 20, 20); 

    transmit_to_server(json_string); 

    loading_lights(2, 0, 20, 20); 

    if (client.connect(server_URL, 80)) {         
        Serial.println("connected"); 
        transmit_to_server(json_string); 
        server_res = get_server_res(); 
        client.stop();
    }
    else
    { Serial.println("connection failed"); } 

    loading_lights(3, 0, 20, 20); 

    handle_server_res(server_res); 

} 

void transmit_to_server(String data_str) {

    client.println("POST /api/dispense HTTP/1.1");
    client.println("Host: " + server_URL);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(strlen(data_str));
    client.println();
    client.println(data_str);
    client.println();

} 

String get_server_res() {

    int wait_count = 0; 
    while(!client.available()) {
        Serial.println("Waiting. "); 
        wait_count++; 
        delay(500); 
        if(wait_count > 30) {
            Serial.println("Server Connection Failed. "); 
            return ""; 
        }
    } 

    Serial.print("Reading Bytes Available: "); Serial.println(client.available());

    char server_res[512]; 
    int server_res_counter = 0; 

    while(client.available()) {
        char c = client.read();
        server_res[server_res_counter] = c; 
        server_res_counter++; 
        if(server_res_counter > 511) {
            if(String(server_res).indexOf("502 Bad Gateway ") != -1) {
                Serial.println("Server Connection Failed. 502 Bad Gateway. "); 
                return String(server_res); 
            }
            break; 
        }
    }

    return String(server_res); 

}

void handle_server_res(String res) {

    Serial.println(res); 
    if(res.indexOf("200 OK") == -1) { hold_lights(500, 20, 0, 0); sleep_sequence(); return; } 

    if(is_setup_mode) {

        if(res.indexOf("Wrong Code.") != -1) { flash_lights(3, 20, 0, 0); sleep_sequence(); return; } 

        int id_idx; 
        if((id_idx = res.indexOf("Dispenser ID: ")) != -1) {
            int start_idx = id_idx + String("Dispenser ID: ").length(); 
            String id = res.substring(start_idx, start_idx + 24); 

            char id_buf[25]; id.toCharArray(id_buf, 25); 
            EEPROM.put(2, id_buf); 

            char get_id[24]; // get_id[25] = 0; 
            EEPROM.get(2, get_id); 
            Serial.print("Get ID: "); 
            Serial.println(get_id); 

        }

        loading_lights(4, 0, 20, 20); 
        clear_user_memory(); 
        hold_lights(500, 0, 20, 0); 
        System.reset(); 

    } else {

        if(res.indexOf("Dispenser ID: ") == -1) { hold_lights(500, 20, 0, 0); return; } 
        loading_lights(4, 0, 20, 20); 
        clear_user_memory(); 
        hold_lights(500, 0, 20, 0); 
        print_events(); 
    }
}

String format_json() {
    short curr_address; EEPROM.get(0, curr_address); 
    char get_id[24]; EEPROM.get(2, get_id); get_id[25] = 0; 
    String id_str = String(1); 
    if(get_id[0] != 0) { id_str = String(get_id); } // id_str = id_str.substring(0, id_str.length()-1); 

    String res = "{ \"id\": \"" + id_str + "\", \"events\": [ "; 
    if(curr_address != -1) {
        for(unsigned int i = 30; i < curr_address; i += 4) { 
            u_int32_t value; 
            EEPROM.get(i, value); 

            res += " { \"name\": "; 
            res += " \"" + decode_name(value) + "\", "; 
            res += " \"value\": "; 
           // res += "" + String(value >> 4) + " }"; 
            res += "" + String(value) + " }"; 

            if(i < curr_address - 4) { res += ","; }
        } 
    }

    unsigned long last_updated; 
    EEPROM.get(26, last_updated); 
    String time_str = String(1); 
    if(last_updated != 0) time_str = String(last_updated); 
    res += " ], \"current_time\": " + time_str + " }"; 

    Serial.println(res); 
    return res; 

}

String decode_name(unsigned long value) {
    long mask = 0xF; 
    unsigned int code = value & mask; 

    switch(code) {
        case btn1_id: 
            return "Button 1"; break; 
        case btn2_id: 
            return "Button 2"; break; 
        case btn3_id: 
            return "Button 3"; break; 
        case tray_id: 
            return "Col Off"; break; 
        case dispense_id: 
            return "Dispense"; break; 
        default: 
            return "Default"; break; 
    }
}

/***************
 * LIGHT FUNCTIONS *
 ***************/ 


void flash_lights(int num, int r, int g, int b) {

    if(is_memory_full()) return; 

    strip.clear(); 
    strip.show(); 
    for(int j = 0; j < num; j++) {
        strip.setPixelColor(0, r, g, b); strip.show(); 
        delay(250); 
        strip.clear(); strip.show(); 
        delay(250); 
    }
    strip.clear(); strip.show(); 
}

void hold_lights(int time, int r, int g, int b) {
    strip.clear(); 
    strip.setPixelColor(0, r, g, b); 
    strip.show(); 
    delay(time); 
    strip.clear(); strip.show(); 
}

void wakeup_lights(bool isSetup) {
    if(isSetup) {
        flash_lights(3, 20, 20, 20); 
        hold_lights(1000, 20, 20, 20); 
    } else {
        hold_lights(1000, 0, 0, 20); 
    }
}

void low_battery_lights() {
    Serial.print("Battery Level: "); Serial.println(analogRead(battery_level)); 
    // IMPLEMENT: test battery level 
    if(analogRead(battery_level) > 1500) return; 

    strip.clear(); strip.show(); 
    for(int i = 0; i < 3; i++) {
        strip.setPixelColor(0, 20, 0, 0); strip.show(); 
        delay(100); 
        strip.clear(); strip.show(); 
        delay(100); 
    }
    strip.clear(); strip.show(); 

}

void loading_lights(int num, int r, int g, int b) { 
    strip.clear(); 
    for(int i = 0; i < num + 1; i++) {
        strip.setPixelColor(0, 0, 20, 20); strip.show(); 
        delay(500); 
        strip.clear(); strip.show(); 
        delay(500); 
    }
}


/***************
 * SLEEP FUNCTIONS *
 ***************/ 

void check_sleep() {
    if(millis() >= sleep_time) {
        if(WiFi.ready() && !is_setup_mode && !is_data_sent) { 
            is_data_sent = true; 
            send_data(); 
        }

        if(!(digitalRead(charger) == LOW)) sleep_sequence(); 
        else reset_sleep_time(); 
    }
} 

void reset_sleep_time() {
    sleep_time = millis() + 60000; 
}

void sleep_sequence() {
    strip.clear(); strip.show(); 
    Serial.println("Going to sleep. "); 
    System.sleep(SLEEP_MODE_DEEP); 
}

