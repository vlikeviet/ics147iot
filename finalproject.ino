#include <HX711ADC.h>
#include <string>


#define LOADCELL_DOUT_PIN  4
#define LOADCELL_SCK_PIN  3
#define ROTARY_ANGLE_SENSOR A0
#define ADC_REF 5 //reference voltage of ADC is 5v.If the Vcc switch on the seeeduino
                    //board switches to 3V3, the ADC_REF should be 3.3
#define GROVE_VCC 5 //VCC of the grove interface is normally 5v
#define FULL_ANGLE 300 //full value of the rotary angle is 300 degrees
 
#if defined (PARTICLE)
// Nothing to include if Spark
#else
#include <Wire.h>
#endif


#include "Grove_LCD_RGB_Backlight.h"

HX711ADC scale;
rgb_lcd lcd;

float calibration_factor = -110500; //-7050 worked for my 440lb max scale setup
//float calibration_factor = -9950500; 
const int buttonPin = 2; 
int buttonState = 0;
int cal_per_oz = 0;
void setup() 
{
    
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale();
    scale.tare();	//Reset the scale to 0
    
    long zero_factor = scale.read_average(10); //Get a baseline reading
    Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor);
    Serial.begin(9600);
    //Rotary sensor initiate
    pinMode(ROTARY_ANGLE_SENSOR, INPUT);
    pinMode(buttonPin, INPUT);
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.setRGB(136,41,241);
    lcd.print("Taring...");
    delay(3000);
    lcd.clear();
}

void loop() 
{

    
    buttonState = digitalRead(buttonPin);
    lcd.clear();
    float voltage;
    int sensor_value = analogRead(ROTARY_ANGLE_SENSOR);
    voltage = (float)sensor_value*ADC_REF/1023;
    int degrees = (voltage*FULL_ANGLE)/GROVE_VCC;
    //.println("The angle between the mark and the starting position:");
    //Serial.println(degrees);
     //lcd.print(degrees);
    if (buttonState == LOW) {
        
        lcd.clear();
    }

    
    lcd.print("Select category");
    lcd.setCursor(0, 2);
    if(degrees < 290)
    {//15 calories / oz
        cal_per_oz = 15;
        lcd.print("Fruits");
        //Serial.println("Fruits");
    }
    else if (290< degrees && degrees < 640)
    { //18 calories  / oz
        cal_per_oz = 18;
        lcd.print("Vegetables");//Serial.println("Vegetables");
    }
    else if (640< degrees && degrees < 940)
    {//102 calories  / oz
        cal_per_oz = 102;
        lcd.print("Grains");//Serial.println("Grains");
    }
    else if (940< degrees && degrees < 1200)
    {//12 calories  / oz
        cal_per_oz = 12;
        lcd.print("Dairy");//Serial.println("Dairy");
    }
    else if (1200<= degrees)
    {//71 calories  / oz
        cal_per_oz = 71;
        lcd.print("Protein Foods");//Serial.println("Protein Foods");
    }
    
    
    if (buttonState == HIGH) {
        // turn LED on:
        Serial.println("BUTTON PRESSED");
        lcd.clear();
        lcd.print("Calculating... ");
        scale.set_scale(calibration_factor); //Adjust to this calibration factor
        Serial.print("Reading: ");
        Serial.print(scale.get_units(4)*-16);
        Serial.print(" oz"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
        Serial.print(" calibration_factor: ");
        Serial.print(calibration_factor);
        Serial.println();
        
        float w = scale.get_units(4)*-16;
        if (w <= 0)
        {
            w = 0;
        }
        //delay(2000);//
        lcd.setCursor(0, 2);
        lcd.print(w);
        lcd.print("oz/");

        int  cal = cal_per_oz * w;
        lcd.print(cal);
        lcd.print(" Cal");
        delay(3000);
    }
   
    delay(200);
    
}
