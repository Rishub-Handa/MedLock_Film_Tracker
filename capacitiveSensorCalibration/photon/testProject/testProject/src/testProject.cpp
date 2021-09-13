/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/home/zethuria/bin/nightingale/photon/testProject/testProject/src/testProject.ino"
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
unsigned int movingAverage(unsigned int masterCaptureArrayInput[120]);
unsigned int movingStandardDeviation(unsigned int masterCaptureArrayInput[120], unsigned int average);
void menuListener();
bool buttonPacer();
bool menuNoInputExitCondition();
#line 15 "/home/zethuria/bin/nightingale/photon/testProject/testProject/src/testProject.ino"
unsigned int senderSignalPin = D0;

//D1 - RECEIVER CAP GROUND
unsigned int recCapGround = D1;

//D2 - RECEIVER INPUT GROUND
unsigned int recInputGround = D2;

//D3 - SENDER INPUT GROUND
unsigned int senderInputGround = D3;

//D4 - SENDER CAP GROUND
unsigned int senderCapGround = D4;

//D5 - SENDER GROUND PLATE
unsigned int senderGroundPlate = D5;

//D6 - RECEIVER GROUND PLATE
unsigned int recGroundPlate = D6;

//A0 - OP AMP POWER
unsigned int opAmpPower = A1;

//A2 - SENDER VOLTAGE READING
unsigned int senderVoltage = A2;

//A3 - RECEIVER VOLTAGE READING
unsigned int recVoltage = A3;

//Variable holds ADC 0-4095 values converted to readable voltage values. For sender.
float senderVoltageConverted = 0;

//Variable holds ADC 0-4095 values converted to readable voltage values. For reciever.
float recVoltageConverted = 0;

/*
**************************
* ADC RAW DATA VARIABLES *
**************************
*/

//Size of capture array
const unsigned int masterCaptureArraySize = 120;

//Array that contains all raw data variables
unsigned int masterCaptureArray[masterCaptureArraySize];

//Global variable that represents the current index
unsigned int masterCaptureArrayIndex = 0;

//Represents the pin that is currently being measured
unsigned int currentADCPin = 0;

const unsigned int stripCaptureArraySize = 120;

unsigned int stripCaptureArray[stripCaptureArraySize];

unsigned int stripDelay = 0;

const unsigned int senderIntCaptureArraySize = 120;

unsigned int senderIntCaptureArray[senderIntCaptureArraySize];

unsigned int senderIntDelay = 0;

const unsigned int recIntCaptureArraySize = 120;

unsigned int recIntCaptureArray[recIntCaptureArraySize];

unsigned int recIntDelay = 0;

unsigned int multiplexSwitchDelay = 0;


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

void setup()
{

    //Serial out encoding
    Serial.begin(9600);

    //Setup code for measuring strips between the plates
    setupMeasureStrips();

    //Setup code for measuring interference from external capacitance on the sender plate
    //setupMeasureSenderInterference();

    //Setup code for measuring interference from external capacitance on the receive plate
    //setupMeasureReceiveInterference();

    //Creates a square wave on the sender plate to measure strips inside
    stripsSignal(10000);

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

    //For hands on experimenting in lab
    //manuelExperimentMode();

    automaticExperimentMode();
}

void automaticExperimentMode() {

   // multiplexPlateMeasurements(true);

}

//Method that
void manuelExperimentMode()
{

    //Serial.println("test");

    //measureStrips();
    //measureSenSETUPderInterference();
    //measureReceiveInterference();

    menuListener();

    //printCalculations(unsigned int pin, char mode, bool rawBool, bool averageBool, bool standardDeviationBool)
    printCalculations(currentADCPin, true, true, true);

    //delay(100);
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
    pinMode(recCapGround, INPUT);
    pinMode(recVoltage, INPUT);

    //Sender circuit pins
    digitalWrite(senderInputGround, LOW);
    digitalWrite(senderCapGround, LOW);

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

    currentADCPin = recVoltage;
}

/*
********************
* SIGNAL FUNCTIONS *
********************
*/

void stripsSignal(int inputFreq)
{

    tone(senderSignalPin, inputFreq, 0);
}

void senderInterferenceSignal(int inputFreq)
{

    tone(senderGroundPlate, inputFreq, 0);
}

void receiveInterferenceSignal(int inputFreq)
{

    tone(recGroundPlate, inputFreq, 0);
}

/*
************************************
* HUMAN-READABLE MEASURE FUNCTIONS *
************************************
*/

void measureStrips()
{

    senderVoltageConverted = ((3.3 / (4096)) * analogRead(recVoltage));
    Serial.println("Strips Voltage:" + String(senderVoltageConverted));
}

void measureSenderInterference()
{

    senderVoltageConverted = ((3.3 / (4096)) * analogRead(recVoltage));

    Serial.println("Sender Interference Voltage:" + String(senderVoltageConverted));
}

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
        stripCaptureArray[stripCaptureArraySize] = raw;
        unsigned int average = movingAverage(stripCaptureArray);
        unsigned int deviation = movingStandardDeviation(stripCaptureArray, average);

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
        senderIntCaptureArray[senderIntCaptureArraySize] = raw;
        unsigned int average = movingAverage(senderIntCaptureArray);
        unsigned int deviation = movingStandardDeviation(senderIntCaptureArray, average);

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

    measureReceiveInterference();

    if (writeToSerialMultiplex)
    {
        Serial.println("RIM");
    }

    delay(multiplexSwitchDelay);

    for (unsigned int index = 0; index < recIntCaptureArraySize; index++)
    {

        delay(recIntDelay);

        unsigned int raw = analogRead(senderVoltage);
        recIntCaptureArray[recIntCaptureArraySize] = raw;
        unsigned int average = movingAverage(recIntCaptureArray);
        unsigned int deviation = movingStandardDeviation(recIntCaptureArray, average);

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

void printCalculations(unsigned int pin, bool rawBool, bool averageBool, bool standardDeviationBool)
{

    unsigned int raw = analogRead(pin);

    if (masterCaptureArrayIndex >= 999)
    {

        masterCaptureArrayIndex = 0;
        masterCaptureArray[masterCaptureArrayIndex] = raw;
    }
    else
    {

        masterCaptureArray[masterCaptureArrayIndex] = raw;
    }

    ++masterCaptureArrayIndex;

    unsigned int average = movingAverage(masterCaptureArray);

    printData(raw, average, movingStandardDeviation(masterCaptureArray, average));
}

/*
***********************
* ALGORITHM FUNCTIONS *
***********************
*/

unsigned int movingAverage(unsigned int masterCaptureArrayInput[120])
{

    unsigned int sum = 0;

    for (unsigned int index = 0; index < 120; index++)
    {

        sum += masterCaptureArrayInput[index];
    }

    return (sum / 120);
}

unsigned int movingStandardDeviation(unsigned int masterCaptureArrayInput[120], unsigned int average)
{

    unsigned int sum = 0;
    unsigned int temp = 0;

    for (unsigned int index = 0; index < 120; index++)
    {

        temp = (masterCaptureArrayInput[index] - average);
        temp = temp * temp;
        sum += temp;
    }

    return (sum / 120);
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

                //
                RGB.color(0, 0, 0);
            }
            else if (menuSelection[menuSelectionIndex] == "addStrip")
            {

                //Serial.println("ADD");
                RGB.color(0, 255, 255);
            }
            else if (menuSelection[menuSelectionIndex] == "removeStrip")
            {

                //Serial.println("SUB");
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
        //Alternatively if the user holds the button down for 2 seconds this will be interpreted as confirming the current menu selection
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
        //Serial.println("HERE");
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