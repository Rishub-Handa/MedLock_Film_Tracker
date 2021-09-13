/*
 * Project Film_Dispenser
 * Description:
 * Authors: Rishub Handa 
 * Date: 
 */



/* TODO: 
 * Correct Moving Average Algorithm 
 * Write to Server 
 * Format EEPROM CSV 
 * Start New Experiment 
 * Indicate Strip Removal 
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

/*
*****************
* FUNCTION DECLARATIONS *
*****************
*/ 

void setPeripheralPinModes(); 
void setupPins(); 

void printEEPROM(); 
String formatTransmission(); 
void sendData(String data); 
void handleServerRes(String res); 
String decodeEventType(long timestamp); 
long encodeEventType(unsigned long value, int code); 

void postMeasurementSequence(); 
void removedStrip(); 
void newSample(); 
void goToSleep(); 
void exitListening(); 

void checkBtns(); 
bool checkBtnsSpecialFunc(); // IMPLEMENT 
void checkDispense(); // IMPLEMENT 
void checkTray(); // IMPLEMENT 


void flashLights(int r, int g, int b, int num); 
void hold_lights(int time, int r, int g, int b); 
void loading_lights(int num, int r, int g, int b); 
void wakeup_lights(bool isSetup); 
void low_battery_lights(); 
void flash_error_code(); 


/*
*****************
* PIN VARIABLES *
*****************
*/ 

unsigned int senderSignalPin = D2; // SENDER SIGNAL (SENDER PLATE)
unsigned int senderGroundPlate = D1; // SENDER GROUND PLATE
unsigned int recVoltage = A4; // RECEIVER VOLTAGE READING
unsigned int recGroundPlate = D3; // RECEIVER GROUND PLATE 

unsigned int btn1 = D5; 
unsigned int btn2 = D6; 
unsigned int btn3 = D7; 

unsigned int tray = A3; 

unsigned int batteryReading = A1; 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(0, A5); 

/*
*********** 
* GLOBALS * 
*********** 
*/

enum event_id { btn1_id, btn2_id, btn3_id, sample_id, charger_id }; 

TCPClient client; 

bool btn1Pressed = false; 
bool btn2Pressed = false; 
bool btn3Pressed = false; 

/*
**************************
* ADC RAW DATA VARIABLES *
**************************
*/

//Opioid Strip Measurement Frequency
unsigned int OSMFrequency = 10000;

const unsigned int stripCaptureArraySize = 60;
unsigned int stripCaptureArray[stripCaptureArraySize];
unsigned int stripDelay = 10;

unsigned int multiplexSwitchDelay = 10;

// Structure for Sample 
struct Sample { 
    float OSMAve; 
    float OSMDev; 
}; 

// Structure for Timestamped Event 
struct Event {
     long timestamp; // u_int64_t ??? 
}; 

int EEPROMIndex = 0; 
Sample currSample; 


void setup()
{  

    setupPins(); 

    // If not connected to internet, go to sleep 
    // Particle.connect(); 

    // DEBUG 
    //Serial out encoding
    // Serial.begin(9600);
    Serial1.begin(9600); 
    EEPROM.clear(); 

    setupMeasureStrips(); 
    System.buttonMirror(btn1, RISING, true); 

    detachInterrupt(btn2); 

    // TESTING 
    // digitalWrite(senderSignalPin, HIGH); 
    

    // Retrieve Current Index From Memory 
    int currAddress; 
    EEPROM.get(EEPROMIndex, currAddress); 
    if(currAddress == 0xFFFFFFFF) {
        EEPROM.clear(); 
        EEPROMIndex = sizeof(EEPROMIndex); 
        EEPROM.put(0, EEPROMIndex); 
        Serial.println("New Memory. "); 
    } else {
        EEPROMIndex = currAddress; 
    }

    // RGB.control(false); // CHANGE 
    // RGB.color(255, 255, 255); 

    strip.begin(); 
    strip.clear(); 
    strip.show(); 

    for(int i = 0; i < 4; i++) {
        strip.setPixelColor(i, 0, 0, 20); 
        strip.show(); 
        delay(200); 
    } 
    strip.clear(); 
    strip.show(); 

    EEPROM.clear(); 

}

void loop()
{

    // newSample(); 
    // delay(5000);   

    checkBtns(); 

    // Handle Server Response 
    if(client.available()) {
        char c = client.read(); 
        Serial.print(c); 
    }

}

/*
*******************
* SETUP FUNCTIONS *
*******************
*/

void setupPins() {

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

}



void setupMeasureStrips()
{

    //Disable all tones
    noTone(senderSignalPin);
    noTone(senderGroundPlate);
    noTone(recGroundPlate);

    //Sender plates
    digitalWrite(senderGroundPlate, LOW);

    //Receiver plates
    digitalWrite(recGroundPlate, LOW);

    //Creates a square wave with the frequency as an argument
    stripsSignal(OSMFrequency);

}

/*
********************
* SIGNAL FUNCTIONS *
********************
*/

void stripsSignal(int inputFreq)
{

    analogWrite(senderSignalPin, 127, inputFreq);

}

void senderInterferenceSignal(int inputFreq)
{
    analogWrite(senderGroundPlate, 127, inputFreq);

}

void receiveInterferenceSignal(int inputFreq)
{
    analogWrite(recGroundPlate, 127, inputFreq);
}

/*
*****************************
* MACHINE MEASURE FUNCTIONS *
*****************************
*/

void measureOSM() {

    setupMeasureStrips();
    delay(multiplexSwitchDelay);

    float totalAverage = 0; 
    float totalDeviation = 0; 

    for (unsigned int index = 0; index < stripCaptureArraySize; index++)
    {

        delay(stripDelay);

        unsigned int raw = analogRead(recVoltage);
        stripCaptureArray[index] = raw;
        unsigned int average = movingAverage(stripCaptureArray, stripCaptureArraySize);
        unsigned int deviation = movingStandardDeviation(stripCaptureArray, average, stripCaptureArraySize);

        totalAverage += average; 
        totalDeviation += deviation; 

    }

    totalAverage = (float) totalAverage / stripCaptureArraySize; 
    totalDeviation = (float) totalDeviation / stripCaptureArraySize; 

}

/*
************************
* DATA PRINT FUNCTIONS *
************************
*/

void printEEPROM() {

}

/*
***********************
* ALGORITHM FUNCTIONS *
***********************
*/

//Computes the moving average of given array in real time
unsigned int movingAverage(unsigned int arrayInput[], unsigned int arraySize)
{
    unsigned int sum = 0;
    for (unsigned int index = 0; index < arraySize; index++) { sum += arrayInput[index]; }
    return (sum / arraySize);
}

//Computes the standard deviation of raw data relative to the current moving average
unsigned int movingStandardDeviation(unsigned int arrayInput[], unsigned int average, unsigned int arraySize)
{
    unsigned int sum = 0;
    unsigned int temp = 0;

    for (unsigned int index = 0; index < arraySize; index++)
    {
        temp = (arrayInput[index] - average);
        temp = temp * temp;
        sum += temp;
    }

    return (sum / arraySize);
}

/*
******************
* AUXILIARY FUNCTIONS *
******************
*/ 

void postMeasurementSequence() { 
    RGB.color(255,255,255); 

    // String data = formatTransmission(); 
    Particle.connect(); 

    String data = "{ \"OSM Ave\": "; 
    data += String(currSample.OSMAve) + ","; 
    data += " \"OSM Std\":  "; 
    data += String(currSample.OSMDev); 
    data += "}"; 

    Particle.publish(data); 

}

void newSample() {
    currSample = { 0, 0, 0, 0, 0, 0, 0 }; 
} 

void exitListening() {
    WiFi.listen(false); 
    System.reset(); 

}

// IMPLEMENT: 
void goToSleep() {
    // Attempt to send data over WiFi. 
    // If low battery, signal LEDs. 

    // Lights 

    System.sleep(SLEEP_MODE_DEEP); 
}

/*
******************
* PERIPHERALS *
******************
*/ 

void checkBtns() {


    // Put to sleep 
    if(digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) {
        btn2Pressed = false; 
        btn3Pressed = false; 

        int timer_count = 0; 

        while(digitalRead(btn2) == HIGH && digitalRead(btn3) == HIGH) {
            timer_count++; 
            delay(500); 
            if(timer_count > 10) break; 
        }

        if(timer_count > 10) {
            goToSleep(); 
            return; 
        } 
        return; 
    }

    // Listening Mode 
    if(digitalRead(btn1) == HIGH && digitalRead(btn3) == HIGH) {
        System.disableButtonMirror(); 
        btn1Pressed = false; 
        btn3Pressed = false; 

        int timer_count = 0; 

        while(digitalRead(btn1) == HIGH && digitalRead(btn3) == HIGH) {
            timer_count++; 
            delay(500); 
            if(timer_count > 10) break; 
        }

        if(timer_count > 10) {
            flashLights(20, 0, 20, 3); 
            attachInterrupt(btn2, exitListening, RISING); 
            WiFi.listen(); 
            return; 
        } else {
            System.buttonMirror(btn1, RISING, true); 
        }
        return; 
    } 

    // Connect to Internet 
    if(digitalRead(btn3) == HIGH && digitalRead(btn1) == LOW && digitalRead(btn2) == LOW) {
        btn3Pressed = true; 
        int timer_count = 0; 

        while(digitalRead(btn3) == HIGH && digitalRead(btn1) == LOW && digitalRead(btn2) == LOW) {
            timer_count++; 
            delay(500); 
            if(timer_count > 10) break; 
        }

        if(timer_count > 10) {
            flashLights(0, 19, 26, 3); 
            attachInterrupt(btn2, exitListening, RISING); 
            Particle.connect(); 
            btn3Pressed = false; 
            return; 
        } 
        return; 
    }

    if(digitalRead(btn1) == HIGH && !btn1Pressed) {
        Serial.println("Button 1 Pressed. "); 
        btn1Pressed = true; 
    } 
    if(digitalRead(btn2) == HIGH && !btn2Pressed) {
        Serial.println("Button 2 Pressed. "); 
        btn2Pressed = true; 
    } 
    if(digitalRead(btn3) == HIGH && !btn3Pressed) {
        Serial.println("Button 3 Pressed. "); 
        btn3Pressed = true; 
    }
    if(digitalRead(btn1) == LOW && btn1Pressed) {
        Serial.println("Button 1 Released. "); 
        btn1Pressed = false; 
        flashLights(20, 0, 0, 3); 
        Particle.publish("Button 1. "); 
    }
    if(digitalRead(btn2) == LOW && btn2Pressed) {
        Serial.println("Button 2 Released. "); 
        btn2Pressed = false; 
        flashLights(20, 20, 0, 3); 
        Particle.publish("Button 2. "); 
        newSample(); 
    }
    if(digitalRead(btn3) == LOW && btn3Pressed) {
        Serial.println("Button 3 Released. "); 
        btn3Pressed = false; 
        flashLights(0, 20, 0, 3); 
        Particle.publish("Button 3. "); 
        removedStrip(); 
    }
} 

void flashLights(int r, int g, int b, int num) {
    for(int i = 0; i < num; i++) {
        strip.setPixelColor(0, r, g, b); strip.show(); 
        delayMicroseconds(200000); 
        strip.clear(); strip.show();
        delayMicroseconds(200000); 
    }
}

/*
******************
* DATA TRANSMISSION *
******************
*/ 

// Assume connected to internet 
void sendData(String data) {
    if (client.connect("719db19f.ngrok.io", 80)) {         
        Serial.println("connected");
                
        client.println("POST /api/dispense/film HTTP/1.1");
        client.println("Host: 719db19f.ngrok.io");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(strlen(data));
        client.println();
        client.println(data);
        client.println();

        int wait_count = 0; 
        while(!client.available()) {
            Serial.println("Waiting. "); 
            wait_count++; 
            delay(500); 
            if(wait_count > 20) {
                Serial.println("Server Connection Failed. "); 
                return; 
            }
        }

        Serial.print("Reading Bytes Available: "); 
        Serial.println(client.available());

        char server_res[512]; 
        int server_res_counter = 0; 

        while(client.available()) {
            char c = client.read();
            server_res[server_res_counter] = c; 
            server_res_counter++; 
            if(server_res_counter > 511) {
                if(String(server_res).indexOf("502 Bad Gateway ") != -1) {
                    Serial.println("Server Connection Failed. 502 Bad Gateway. "); 
                    return; 
                }
                break; 
            }
        }

        handleServerRes(server_res); 

        client.stop();
    } else {
        Serial.println("Could not connect. "); 
    }
}

// IMPLEMENT 
void handleServerRes(String res) {
    if(res.indexOf("200 OK") != -1) {
        // Clear EEPROM events 
    } else {
        // Indicate error 
    }
}

String formatTransmission() {

    // TEST 

    String JSONStr = "[ "; 

    int currAddress; 
    EEPROM.get(0, currAddress); 

    if(currAddress > 0) {
        int i = 4; 
        while(i < currAddress) {

            String dataJSON = "{ "; 

            Event event; 
            EEPROM.get(i, event); 
            i += sizeof(event); 
            String eventType = decodeEventType(event.timestamp); 

            String sampleData = "{ "; 

            if(eventType == "sample ") { 
                Sample sample; 
                EEPROM.get(i, sample); 
                i += sizeof(sample); 

                sampleData += "\"OSMAve\": " + String(sample.OSMAve) + ", " + "\"OSMDev\": " + String(sample.OSMDev) + ", "; 
                sampleData += "\"RIMAve\": " + String(sample.RIMAve) + ", " + "\"RIMDev\": " + String(sample.RIMDev) + ", "; 
                sampleData += "\"SIMAve\": " + String(sample.SIMAve) + ", " + "\"SIMDev\": " + String(sample.SIMDev); 
            } 

            sampleData += "} "; 

            dataJSON += "\"timestamp\": " + String(event.timestamp) + ","; 
            dataJSON += "\"eventType\": " + eventType + ","; 
            dataJSON += "\"data\": " + sampleData + "} "; 

            dataJSON += "}, "; 
        }
    }
    
    JSONStr += "]"; 

    return JSONStr; 
} 

long encodeEventType(unsigned long value, int code) { 
    long mask = ~0u << 4; 
    value = value << 4; 
    value &= mask; 
    value |= code; 
    
    return (long)value; 	
} 


// IMPLEMENT 
String decodeEventType(long timestamp) {
    return "sample"; 
} 
