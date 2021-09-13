/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/home/zethuria/bin/nightingale/photon/sensorCode/src/sensorCode.ino"
/*
 * Project Film_Dispenser
 * Description:
 * Author: Rishub Handa 
 * Date: 
 */

/*
*****************
* PIN VARIABLES *
*****************
*/

//D0 - SENDER SIGNAL (SENDER PLATE)
void setup();
void loop();
void automaticExperimentMode();
void manuelExperimentMode();
void setupMeasureStrips();
void setupMeasureSenderInterference();
void setupMeasureReceiveInterference();
void stripsSignal(int inputFreq);
void senderInterferenceSignal(int inputFreq);
void receiveInterferenceSignal(int inputFreq);
void measureStrips();
void measureSenderInterference();
void measureReceiveInterference();
void multiplexPlateMeasurements(bool writeToSerialMultiplex);
void printData(unsigned int raw, unsigned int average, unsigned int deviation);
void printCalculations(unsigned int pin, bool rawBool, bool averageBool, bool standardDeviationBool);
unsigned int movingAverage(unsigned int arrayInput[], unsigned int arraySize);
unsigned int movingStandardDeviation(unsigned int arrayInput[], unsigned int average, unsigned int arraySize);
void menuListener();
bool buttonPacer();
bool menuNoInputExitCondition();
#line 15 "/home/zethuria/bin/nightingale/photon/sensorCode/src/sensorCode.ino"
unsigned int senderSignalPin = D1; //CHECK

//D1 - RECEIVER CAP GROUND
unsigned int recCapGround = D0; //CHECK

//D2 - RECEIVER INPUT GROUND
unsigned int recInputGround = D2; //CHECK

//D3 - SENDER INPUT GROUND
unsigned int senderInputGround = D3; //CHECK

//D4 - SENDER CAP GROUND
unsigned int senderCapGround = D4; //CHECK

//D5 - SENDER GROUND PLATE
unsigned int senderGroundPlate = A4; //CHECK

//D6 - RECEIVER GROUND PLATE
unsigned int recGroundPlate = A5; //CHECK


//A0 - OP AMP POWER
//unsigned int opAmpPower = A1;

//A2 - SENDER VOLTAGE READING
unsigned int senderVoltage = A0; //CHECK

//A3 - RECEIVER VOLTAGE READING
unsigned int recVoltage = A1; //CHECK

//Variable holds ADC 0-4095 values converted to readable voltage values. For sender.
float senderVoltageConverted = 0;

//Variable holds ADC 0-4095 values converted to readable voltage values. For reciever.
float recVoltageConverted = 0;

/*
**************************
* ADC RAW DATA VARIABLES *
**************************
*/

//Opioid Strip Measurement Frequency
unsigned int opioidStripMeasurementFrequency = 10000;


//Sender Ground Plate Frequency
unsigned int senderGroundPlateFrequency = 10000;


//Recevier Ground Plate Frequency
unsigned int recGroundPlateFrequency = 10000;


//Size of capture array
const unsigned int masterCaptureArraySize = 120;

//Array that contains all raw data variables
unsigned int masterCaptureArray[masterCaptureArraySize];

//Global variable that represents the current index
unsigned int masterCaptureArrayIndex = 0;

//Represents the pin that is currently being measured
unsigned int currentADCPin = 0;

const unsigned int stripCaptureArraySize = 60;

unsigned int stripCaptureArray[stripCaptureArraySize];

unsigned int stripDelay = 10;

const unsigned int senderIntCaptureArraySize = 60;

unsigned int senderIntCaptureArray[senderIntCaptureArraySize];

unsigned int senderIntDelay = 10;

const unsigned int recIntCaptureArraySize = 60;

unsigned int recIntCaptureArray[recIntCaptureArraySize];            

unsigned int recIntDelay = 10;

unsigned int multiplexSwitchDelay = 10;


/*
******************
* MENU VARIABLES *
******************
*
* These are the variables that make up menuController(). 
* The menu is composed of the photon's RGB LED as output and the SETUP button as input. 
*
*/

//Size of menu array
const unsigned int menuSelectionSize = 5;

//States that can be selected, each giving context to what is happening to any human or external script
//Dictionary:
//OSM: stripMeasure      : RGB: RED
//SIM: senderIntMeasure  : RGB: YELLOW
//RIM: recIntMeasure     : RGB: CYAN
//ADD: addStrip          : RGB: BLUE
//SUB: removeStrip       :
String menuSelection[menuSelectionSize] = {"stripMeasure", "senderIntMeasure", "recIntMeasurem", "addStrip", "removeStrip"};

//Global variable that represents what menu is currently selected
unsigned int menuSelectionIndex = 0;

//Citeria for how long a user should push the menu button before it opens
unsigned int menuThreshold = 1000;

//Citeria for how long a user should push a button before it is registered as a push in software
unsigned int buttonThreshold = 100;

unsigned int deltaMenuTime = 0;


//automatic
unsigned int automaticTime1 = 0;
unsigned int automaticTime2 = 0;


void setup()
{

    //Serial out encoding
    Serial.begin(9600);

    //pinMode(D1, OUTPUT);

    //tone(D1, 10000);

    //analogWrite(D1, 127, 10000);

    //setupMeasureStrips();

    //digitalWrite(A2, HIGH);

    //Setup code for measuring strips between the plates
    //setupMeasureStrips();

    //Setup code for measuring interference from external capacitance on the sender plate
    //setupMeasureSenderInterference();

    //Setup code for measuring interference from external capacitance on the receive plate
    //setupMeasureReceiveInterference();

    //Creates a square wave on the sender plate to measure strips inside
    //stripsSignal(10000);

    //Enables controlling of RGB light
    RGB.control(true);

    //Creates a square wave on the sender ground plate to measure interference
    //senderInterferenceSignal(10000);

    //Creates a square wave on the receive ground plate to measure interference
    //receiveInterferenceSignal(10000);

    //If set to true the op amp has power, if set to false the op amp power is set to ground



}

void loop()
{

    //For use by humans in the real world
    //useMode();


    //delay(100);
    //Serial.println("A4 Max: " + String(analogWriteMaxFrequency(A4)));

    //delay(100);
    //Serial.println("A5 Max: " + String(analogWriteMaxFrequency(A5)));

    //delay(100);
    //Serial.println("D0 Max: " + String(analogWriteMaxFrequency(D0)));




    //For hands on experimenting in lab
    //manuelExperimentMode();
    //Serial.println("test");
    //automaticExperimentMode();
    
    multiplexPlateMeasurements(true);

    //manuelExperimentMode();

}

void automaticExperimentMode() {

    automaticTime1 = millis();
    multiplexPlateMeasurements(true);

    if(automaticTime2 > 10000) {

        automaticTime2 = 0;
        while(System.buttonPushed() == 0) 
        {

            //Paused
            RGB.color(255,0,0);

        }


    }

    automaticTime2 += millis() - automaticTime1;

}

//Method that
void manuelExperimentMode()
{

    //measureStrips();
    //measureSenSETUPderInterference();
    //measureReceiveInterference();

    //printCalculations(unsigned int pin, char mode, bool rawBool, bool averageBool, bool standardDeviationBool)
    //printCalculations(currentADCPin, true, true, true);
    //delay(100);

    multiplexPlateMeasurements(true);
}

//void useMode()
//{
//}

/*
*******************
* SETUP FUNCTIONS *
*******************
*/

void setupMeasureStrips()
{

    //Disable all tones
    noTone(senderSignalPin);
    noTone(senderGroundPlate);
    noTone(recGroundPlate);

    //Set direction of pins

    //Sender plates
    pinMode(senderSignalPin, OUTPUT);
    pinMode(senderGroundPlate, OUTPUT);

    //Sender circuit pins
    pinMode(senderInputGround, INPUT);
    pinMode(senderCapGround, OUTPUT);
    pinMode(senderVoltage, INPUT);

    //Receiver plates
    pinMode(recGroundPlate, OUTPUT);

    //Receiver circuit pins
    pinMode(recInputGround, OUTPUT);
    pinMode(recCapGround, OUTPUT);
    pinMode(recVoltage, INPUT);

    //Sender plates
    digitalWrite(senderGroundPlate, LOW);

    //Sender circuit plates
    digitalWrite(senderCapGround, LOW);

    //Receiver plates
    digitalWrite(recGroundPlate, LOW);

    //Receiver circuit pins
    digitalWrite(recInputGround, LOW);
    digitalWrite(recCapGround, LOW);

    //Creates a square wave with the frequency as an argument
    stripsSignal(opioidStripMeasurementFrequency);

    currentADCPin = recVoltage;
}

void setupMeasureSenderInterference()
{

    //Disable all tones
    noTone(senderSignalPin);
    noTone(senderGroundPlate);
    noTone(recGroundPlate);

    //Set direction of pins

    //Sender plates
    pinMode(senderSignalPin, INPUT);
    pinMode(senderGroundPlate, OUTPUT);

    //Sender circuit pins
    pinMode(senderInputGround, OUTPUT);
    pinMode(senderCapGround, OUTPUT);
    pinMode(senderVoltage, INPUT);

    //Receiver plates
    pinMode(recGroundPlate, INPUT);

    //Receiver circuit pins
    pinMode(recInputGround, INPUT);
    pinMode(recCapGround, OUTPUT);
    pinMode(recVoltage, INPUT);

    //Sender circuit pins
    digitalWrite(recCapGround, LOW);
    digitalWrite(senderInputGround, LOW);
    digitalWrite(senderCapGround, LOW);

    //Creates a square wave with the frequency as an argument
    senderInterferenceSignal(senderGroundPlateFrequency);

    currentADCPin = senderVoltage;
}

void setupMeasureReceiveInterference()
{

    //Disables all tones
    noTone(senderSignalPin);
    noTone(senderGroundPlate);
    noTone(recGroundPlate);

    //Set direction of pins

    //Sender pins
    pinMode(senderSignalPin, INPUT);
    pinMode(senderGroundPlate, INPUT);

    //Sender circuit pins
    pinMode(senderInputGround, INPUT);
    pinMode(senderCapGround, INPUT);
    pinMode(senderVoltage, INPUT);

    //Receiver plates
    pinMode(recGroundPlate, OUTPUT);

    //Receiver circuit pins
    pinMode(recInputGround, OUTPUT);
    pinMode(recCapGround, OUTPUT);
    pinMode(recVoltage, INPUT);

    //Receiver circuit pins
    digitalWrite(recInputGround, LOW);
    digitalWrite(recCapGround, LOW);

    //Creates a square wave with the frequency as an argument
    receiveInterferenceSignal(recGroundPlateFrequency);

    currentADCPin = recVoltage;
}

/*
********************
* SIGNAL FUNCTIONS *
********************
*/

void stripsSignal(int inputFreq)
{

    //tone(senderSignalPin, inputFreq, 0);
    analogWrite(senderSignalPin, 127, inputFreq);

}

void senderInterferenceSignal(int inputFreq)
{

    //tone(senderGroundPlate, inputFreq, 0);

    analogWrite(senderGroundPlate, 127, inputFreq);

}

void receiveInterferenceSignal(int inputFreq)
{

    //tone(recGroundPlate, inputFreq, 0);

    analogWrite(recGroundPlate, 127, inputFreq);

}

/*
************************************
* HUMAN-READABLE MEASURE FUNCTIONS *
************************************
*/

//Measures the amount of opioid strips in between the plates then prints the converted voltage to screen
void measureStrips()
{

    senderVoltageConverted = ((3.3 / (4096)) * analogRead(recVoltage));
    Serial.println("Strips Voltage:" + String(senderVoltageConverted));
}

//Measures the sender plate interference then prints the converted voltage to screen
void measureSenderInterference()
{

    senderVoltageConverted = ((3.3 / (4096)) * analogRead(recVoltage));

    Serial.println("Sender Interference Voltage:" + String(senderVoltageConverted));
}

//Measures the receiver plate interference then prints the converted voltage to screen
void measureReceiveInterference()
{

    recVoltageConverted = ((3.3 / (4096)) * analogRead(recVoltage));

    Serial.println("Receive Interference Voltage:" + String(recVoltageConverted));
}

/*
*****************************
* MACHINE MEASURE FUNCTIONS *
*****************************
*/

//Takes 
void multiplexPlateMeasurements(bool writeToSerialMultiplex)
{

    setupMeasureStrips();

    delay(multiplexSwitchDelay);


    if (writeToSerialMultiplex)
    {
        Serial.println("OSM");
    }

    for (unsigned int index = 0; index < stripCaptureArraySize; index++)
    {

        delay(stripDelay);

        unsigned int raw = analogRead(recVoltage);
        stripCaptureArray[index] = raw;
        unsigned int average = movingAverage(stripCaptureArray, stripCaptureArraySize);
        unsigned int deviation = movingStandardDeviation(stripCaptureArray, average, stripCaptureArraySize);

        if (writeToSerialMultiplex)
        {

            Serial.print(millis());
            Serial.print(",");
            Serial.print(raw);
            Serial.print(",");
            Serial.print(average);
            Serial.print(",");
            Serial.print(deviation);
            Serial.print("\n");
        }
    }

    setupMeasureSenderInterference();

    if (writeToSerialMultiplex)
    {
        Serial.println("SIM");
    }

    delay(multiplexSwitchDelay);


    for (unsigned int index = 0; index < senderIntCaptureArraySize; index++)
    {

        delay(senderIntDelay);

        unsigned int raw = analogRead(senderVoltage);
        senderIntCaptureArray[index] = raw;
        unsigned int average = movingAverage(senderIntCaptureArray, senderIntCaptureArraySize);
        unsigned int deviation = movingStandardDeviation(senderIntCaptureArray, average, senderIntCaptureArraySize);

        if (writeToSerialMultiplex)
        {

            Serial.print(millis());
            Serial.print(",");
            Serial.print(raw);
            Serial.print(",");
            Serial.print(average);
            Serial.print(",");
            Serial.print(deviation);
            Serial.print("\n");
        }
    }

    setupMeasureReceiveInterference();

    if (writeToSerialMultiplex)
    {
        Serial.println("RIM");
    }

    delay(multiplexSwitchDelay);

    for (unsigned int index = 0; index < recIntCaptureArraySize; index++)
    {

        delay(recIntDelay);

        unsigned int raw = analogRead(recVoltage);
        recIntCaptureArray[index] = raw;
        unsigned int average = movingAverage(recIntCaptureArray, recIntCaptureArraySize);
        unsigned int deviation = movingStandardDeviation(recIntCaptureArray, average, recIntCaptureArraySize);



        if (writeToSerialMultiplex)
        {

            Serial.print(millis());
            Serial.print(",");
            Serial.print(raw);
            Serial.print(",");
            Serial.print(average);
            Serial.print(",");
            Serial.print(deviation);
            Serial.print("\n");
        }
    }
}

/*
************************
* DATA PRINT FUNCTIONS *
************************
*/

void printData(unsigned int raw, unsigned int average, unsigned int deviation)
{

    //Serial.println("");
    //Serial.println(label);
    //Serial.println(millis());
    //Serial.println(input);

    //Prints in CSV format so it can be easily converted into data that can be read by a matlab script
    Serial.print(millis());
    Serial.print(",");
    Serial.print(raw);
    Serial.print(",");
    Serial.print(average);
    Serial.print(",");
    Serial.print(deviation);
    Serial.print("\n");
}

//Old function that is not currently used
void printCalculations(unsigned int pin, bool rawBool, bool averageBool, bool standardDeviationBool)
{

    unsigned int raw = analogRead(pin);

    if (masterCaptureArrayIndex >= masterCaptureArraySize)
    {

        masterCaptureArrayIndex = 0;
        masterCaptureArray[masterCaptureArrayIndex] = raw;
    }
    else
    {

        masterCaptureArray[masterCaptureArrayIndex] = raw;
    }

    ++masterCaptureArrayIndex;

    unsigned int average = movingAverage(masterCaptureArray, masterCaptureArraySize);

    printData(raw, average, movingStandardDeviation(masterCaptureArray, average, masterCaptureArraySize));
}

/*
***********************
* ALGORITHM FUNCTIONS *
***********************
*/

unsigned int movingAverage(unsigned int arrayInput[], unsigned int arraySize)
{

    unsigned int sum = 0;

    for (unsigned int index = 0; index < arraySize; index++)
    {

        sum += arrayInput[index];
    }

    return (sum / arraySize);
}

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
* MENU FUNCTIONS *
******************
*/

//Allows for the setup button on the photon to be used as a menu to give context to what is being measured or happening in the experiment
void menuListener()
{

    deltaMenuTime = millis();

    if (System.buttonPushed() > menuThreshold)
    {

        buttonPacer();
        menuSelectionIndex = 0;

        while (true)
        {

            //Serial.println(System.buttonPushed());

            //String menuSelection[5] = {"stripMeasure", "senderIntMeasure", "recIntMeasure", "addStrip", "removeStrip"};
            if (menuSelection[menuSelectionIndex] == "stripMeasure")
            {

                //Serial.println("OSM");

                //Red for strip measurement
                RGB.color(255, 0, 0);
            }
            else if (menuSelection[menuSelectionIndex] == "senderIntMeasure")
            {

                //Serial.println("SIM");

                //Yellow for sender interference
                RGB.color(255, 255, 0);
            }
            else if (menuSelection[menuSelectionIndex] == "recIntMeasure")
            {

                //Serial.println("RIM");

                //White for receiver interference 
                RGB.color(0, 0, 0);
            }
            else if (menuSelection[menuSelectionIndex] == "addStrip")
            {

                //Serial.println("ADD");

                //
                RGB.color(0, 255, 255);
            }
            else if (menuSelection[menuSelectionIndex] == "removeStrip")
            {

                //Serial.println("SUB");

                //
                RGB.color(0, 0, 255);
            }

            if (menuNoInputExitCondition())
            {
                break;
            }

            if (System.buttonPushed() > buttonThreshold)
            {

                if (buttonPacer())
                {
                    break;
                }

                if (menuSelectionIndex < menuSelectionSize)
                {
                    menuSelectionIndex++;
                    deltaMenuTime = millis();
                }
                else
                {

                    menuSelectionIndex = 0;
                    deltaMenuTime = millis();
                }
            }
        }
    }
}

//A method that prevents
bool buttonPacer()
{

    if (System.buttonPushed() > buttonThreshold)
    {

        //Until the user stops pressing the button this while loop will continue
        //This is to prevent "bouncing" where the button detects multiple button presses in a very short period of time
        while (System.buttonPushed() != 0)
        {
        }

        return false;
    }
    else
    {

        return false;
    }
}

bool menuNoInputExitCondition()
{

    if ((millis() - deltaMenuTime) > 5000)
    {
        deltaMenuTime = 0;
        
        //Sets rgb LED default color to white
        RGB.color(255, 255, 255);

        if (menuSelection[menuSelectionIndex] == "stripMeasure")
        {

            //Configures the pins so it is able to measure the strips
            setupMeasureStrips();
            //Identifier for external python script that the measurement type has changed
            Serial.println("OSM");
            return true;
        }
        else if (menuSelection[menuSelectionIndex] == "senderIntMeasure")
        {

            //Configures the pins so it is able to measure the sender plate interference
            setupMeasureSenderInterference();
            //Identifier for external python script that the measurement type has changed
            Serial.println("SIM");
            return true;
        }
        else if (menuSelection[menuSelectionIndex] == "recIntMeasure")
        {

            //Configures the pins so it is able to measure the receiver plate interference
            setupMeasureReceiveInterference();
            //Identifier for external python script that the measurement type has changed
            Serial.println("RIM");
            return true;
        }
        else if (menuSelection[menuSelectionIndex] == "addStrip")
        {

            //Configures the pins so it is able to measure the strips
            setupMeasureStrips();
            //Identifier for external python script that the measurement type has changed
            Serial.println("ADD");
            return true;
        }
        else if (menuSelection[menuSelectionIndex] == "removeStrip")
        {

            //Configures the pins so it is able to measure the strips
            setupMeasureStrips();
            //Identifier for external python script that the measurement type has changed
            Serial.println("SUB");
            return true;
        }
        else
        {

            return false;
        }
    }
    else
    {

        return false;
    }
}