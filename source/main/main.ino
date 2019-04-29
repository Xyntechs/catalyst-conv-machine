// On the SD card use 24 bit color BMP files (be sure they are 24-bit!)
// There are examples images in the sketch folder.

// MS Paint can be used to crop, resize and flip and save images in 24bit BMP format

// Updated 31/3/15 to include new drawRAW() function that draws raw 16 bit images.
// Raw images are created with UTFT library tool (libraries\UTFT\Tools\ImageConverter565.exe)
// Save as .raw file as this can be easily piped to the TFT pushColors() function
// without any tedious byte swapping etc

//Load Sensitivity//
/////////////////////////////////////////////////////////////
int LoadSensitivity = 511;               //(Max 512 Low Weight) (Min 507 High Weight)
////////////////////////////////////////////////////////////

#include <TFT_HX8357.h>        // Hardware-specific library
TFT_HX8357 tft = TFT_HX8357(); // Invoke custom library

//HX711 initialization for weight sensor
#include "HX711.h"
HX711 scale;
float Base;

// SD Card library, comment out next line if the SdFat library is used
#include <SD.h>


// We can use the SdFat library instead and it will be a bit faster, uncomment next 2 lines
//#include <SdFat.h>           // Use the SdFat library for the Due
//SdFat SD;                    // Permit SD function call compatibility with no sketch changes :-)


#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
 {'1','2','3','S'},
 {'4','5','6','P'},
 {'7','8','9','A'},
 {'*','0','#','M'}
};



byte rowPins[ROWS] = {2,3,4,5}; //connect to row pinouts
byte colPins[COLS] = {6,7,8,9}; //connect to column pinouts

int TimerH;
int TimerM;
int TimerS;



Keypad keypad = Keypad( makeKeymap(keys), rowPins,
colPins, ROWS, COLS );

#include <EEPROM.h>


// SD chip select
#define SDC_CS 53
// These are used when calling drawBMP() function
#define BU_BMP 1 // Temporarily flip the TFT coords for standard Bottom-Up bit maps
#define TD_BMP 0 // Draw inverted Top-Down bitmaps in standard coord frame





/*************************************************************************************************************************************************/
//initializations 
int ConvMotor = 12;
int RotaryLineMotor = 10;
int CrankMotor = 13;
int Start = 14;
int Stop = 15;

int ConvMotorLoad = A2;
int RotaryLineMotorLoad = A3;
int CrankLineMotorLoad = A4;
char Mode;

//used for calibration
double kgps;
float t, w;
bool isCalibration;
bool isTare;
int BaseTime;
int CurrentTime;
long int OperationWeight = 5000;
 int base;
 int LineI = 0;
 int CrankSpeed = 175;
 int LineIterator=0;
  bool SLine=1;
  bool restart;

/*************************************************************************************************************************************************/
//Declarations

void StartLcd(void);
void PackingCycle();
void StartWeightSensors(void);
double ReadWeight(void);
void StartKeyPad(void);
char GetEnteredKey(void);
char WaitEnteredKey(void);
int WaitEnteredNumber(void);
void ConvMotorSpeed(float);
void LineMotorSpeed(float);
void LineMotorSpeedLoad(float);
void DisplayInterface(void);
void DisplayWeight(void);
void drawCentreNumber(int string, int16_t dX, int16_t poY, int16_t font);
void ChooseMode();
void Calibraion();
void CalibTareCapture();
void StartManualMotors(int& ConvMan, int& LineMan);
void ManDisplayInterface();
void EnterLegnth();
void SetOperationWeight();

/********************************************************************Main Program****************************************************************************************************/

void setup()
{
 
 //Starting LCD and Sensors
  StartLcd();
  StartWeightSensors();
//Pin Mapping
  pinMode(ConvMotor, OUTPUT);
  pinMode(RotaryLineMotor, OUTPUT);
  pinMode(ConvMotorLoad, INPUT);
  pinMode(RotaryLineMotorLoad, INPUT);
  pinMode(CrankLineMotorLoad, INPUT);
  pinMode(Start, OUTPUT);
  pinMode(Stop, OUTPUT);
//Start White Screen
 tft.fillScreen(TFT_WHITE);
 //Initially Calibration and taring are off
 isCalibration = false;
 isTare = false;
 //Knowing BaseTime
 BaseTime = millis();
 CurrentTime = BaseTime;
 //Before we start please choose the mode
 ChooseMode();
//We retrieve Operation Weight for the current mode
 char F = EEPROM.read(100+Mode);
 
 char S = EEPROM.read(101+Mode);
 
 char T = EEPROM.read(102+Mode);

  char F1 = EEPROM.read(103+Mode);
 
 char S1 = EEPROM.read(104+Mode);
 
 char T1 = EEPROM.read(105+Mode);

  
OperationWeight = (F-'0')*100000 + (S-'0')*10000 + (T-'0')*1000 + (F1-'0')*100 + (S1-'0')*10 + (T1-'0');
 
 //Now we start the basic display interface
 
 DisplayInterface();
}



/*************************************************************************************************************************************************/


void loop()
{
PackingCycle();

}

/**************************************************************************************************************************************************************************************/
/********************************************************************************Definitions***************************************************************************/
/**************************************************************************************************************************************************************************************/

void StartLcd()
{
  
  Serial.begin(38400); // For debug messages
  delay(1000);

  tft.init(); // Initialize the display (various parameters configured)
  
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SDC_CS)) {
    Serial.println(F("failed!"));
    //return;
  }

  Serial.println(F("Here we go..."));

   // Landscape mode
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.invertDisplay(1);
  
  // Draw bmp image top left corner at x,y = 0,0
  // Image must fit (one day I will add clipping... but it will slow things down)
  drawBMP("Intro.bmp", 0, 0, BU_BMP);

  delay(1000);
  
}


/*************************************************************************************************************************************************/

void StartWeightSensors()
{
  scale.begin(A1, A0);

  scale.set_scale(86.31772447);//337.5411                      // this value is obtained by calibrating the scale with known weights; see the README for details
  
  scale.tare();

}

/*************************************************************************************************************************************************/

void TareWeight(void)
{
scale.tare();
isTare = false;
}


void Calibraion()
{
   tft.fillScreen(TFT_GREEN);
   tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(3);
   tft.drawCentreString("CALIBRATING", 230, 100, 1);
   base  = 510;
   analogWrite(CrankMotor, CrankSpeed);

   
   int t1 = millis();
   float maxi = ReadWeight();
    int j = 0;
    int a=0;
    bool fin = true;
    ConvMotorSpeed(125);
    while(fin)
   {
    
  if(ReadWeight() <=5)
    {
      a++;

      if(a==50)
      {fin=false;}
      }
LineMotorSpeedLoad(1);


   
     }
   
LineMotorSpeedLoad(0);
analogWrite(CrankMotor, 0);
   

   int t2 = millis();
   float deltaT = abs(t2 - t1);
   Serial.println(deltaT);
  deltaT=deltaT/1000;
  maxi = maxi/1000;

   kgps = maxi/deltaT;

   Serial.println(kgps);
   int StoreSpp = (Mode - '0')*sizeof(double);
    EEPROM.put(StoreSpp, kgps);
   delay(5000);
   isCalibration = false;
   DisplayInterface();
   ConvMotorSpeed(0);
   
  
}


void AutoPacking()
{
  
	int StoreSpp = (Mode - '0')*sizeof(double);
	int a=0;
  bool Sp = 1;
	EEPROM.get(StoreSpp, kgps);
	if(digitalRead(14)==HIGH)
	{
  base  = analogRead(CrankLineMotorLoad)+1;
		

		bool st = true;
		if(ReadWeight()<=(OperationWeight + 100) && ReadWeight()>=(OperationWeight-100))
			{
		
			int spwm = ((float(kgps)*125)/(float(ReadWeight())/(1000.0*t*60.0)));
			if(spwm > 255)
      {
        return;
        }

       if(spwm < 25)
      {
        return;
        } 
			ConvMotorSpeed(spwm);
     analogWrite(CrankMotor, CrankSpeed);
     delay(500);

			}
		else
		{



    TimerH = millis()/1000/60/60;
    TimerM = millis()/1000/60;
    TimerS = millis()/1000;
			 tft.fillScreen(TFT_GREEN);
   
			tft.setTextColor(TFT_BLACK, TFT_GREEN);
			tft.setTextSize(3);
			tft.drawCentreString("The Current Weight ", 230, 100, 1);
			tft.drawCentreString("is less than the", 230, 140, 1);
			tft.drawCentreString("Operation Weight", 230, 180, 1);
			tft.drawCentreString("Weight Offset= +-100 g", 230, 220, 1);
			delay(3000);
			DisplayInterface();
			st = false;
		}
		while(st)
		{
			DisplayWeight();
			if(ReadWeight() ==0)
			{
				a++;

				if(a==50)
				{
					ConvMotorSpeed(0);
          analogWrite(RotaryLineMotor,0);
          analogWrite(CrankMotor, 0);
          Sp=0;
					break;
				}
			}

     
       if(digitalRead(14)==HIGH)
        {
          ConvMotorSpeed(0);
          analogWrite(RotaryLineMotor,0);
          analogWrite(CrankMotor, 0);
          Sp=0;
          delay(500);
          break;
        }

     LineMotorSpeedLoad(Sp);
     
     CalibTareCapture();
      if(isCalibration)
      Calibraion();
    
      if(isTare)
      TareWeight();

      if(restart)
      {
        restart = false;
      break;
      }
		}
    }
}

void CalibTareCapture()
{
  char ekey = GetEnteredKey();
  if(ekey =='#')
  {
    isTare = true;
  }
  if(ekey=='*')
  {
    
    if(ReadWeight()<400)
    {
       tft.fillScreen(TFT_GREEN);
   
     tft.setTextColor(TFT_BLACK, TFT_GREEN);
     tft.setTextSize(3);
     tft.drawCentreString("The Current Weight ", 230, 100, 1);
     tft.drawCentreString("Can't be used", 230, 140, 1);
     tft.drawCentreString("for calibration", 230, 180, 1);
     tft.drawCentreString("at least 400 grams", 230, 220, 1);
     delay(3000);
     DisplayInterface();
    }
    else
     isCalibration = true;
  }
    if(ekey == '0')
      {
        SetOperationWeight();
      }
      if(ekey == '4')
      {
        CrankSpeed--;
        }
        if (ekey == '6')
        {
          CrankSpeed++;
        }
             if (ekey == '1')
     {
       //Start White Screen
 tft.fillScreen(TFT_WHITE);
 //Initially Calibration and taring are off
 isCalibration = false;
 isTare = false;
 //Knowing BaseTime
 BaseTime = millis();
 CurrentTime = BaseTime;
 //Before we start please choose the mode
 ChooseMode();
//We retrieve Operation Weight for the current mode
 char F = EEPROM.read(100+Mode);
 
 char S = EEPROM.read(101+Mode);
 
 char T = EEPROM.read(102+Mode);

  char F1 = EEPROM.read(103+Mode);
 
 char S1 = EEPROM.read(104+Mode);
 
 char T1 = EEPROM.read(105+Mode);

  
OperationWeight = (F-'0')*100000 + (S-'0')*10000 + (T-'0')*1000 + (F1-'0')*100 + (S1-'0')*10 + (T1-'0');
 
 //Now we start the basic display interface
 
 DisplayInterface();
 restart=true;
      }

  
  
  }


void StartKeyPad(void)
{
}


/*************************************************************************************************************************************************/

char GetEnteredKey(void)
{
 char key = keypad.getKey();
  if (key){
    Serial.println(key);
 return key;}
 else
 {
 return 'N';
 }
}

char WaitEnteredKey(void)
{
 char key = keypad.getKey();
 while(!key)
 {
  key = keypad.getKey();
 
  if (key){
    Serial.println(key);
  }
 }
  return key;
}

/*************************************************************************************************************************************************/

double ReadWeight(void)
{
  float Weight = scale.get_units() - Base;
  int approxWeightFactor = Weight/5;
  //if(approxWeightFactor*10 < 0)
  //return 0;
  return approxWeightFactor*5;
}

/*************************************************************************************************************************************************/

void PackingCycle()
{
  if(CrankSpeed<=0)
{
  CrankSpeed=0;
  }
  
	if(Mode=='0')
	{
		//Manual Mode
		//Read from users motors' speed

		int ConvMan, LineMan;
		int LineManI = 0;
   bool StartConv = 0;
   bool StartLine = 0;
		analogWrite(RotaryLineMotor, 0);
		BaseTime = millis();
		StartManualMotors(ConvMan, LineMan);
     Serial.println(ConvMotor);
    base  = analogRead(RotaryLineMotorLoad)-1;
		while(1)
		{
			char In = GetEnteredKey();
			DisplayWeight();
      if(In == '0')
      {
        SetOperationWeight();
      }
			if(In == '#')
			{
				TareWeight();
			}
			if(In == '*')
			{
				ConvMotorSpeed(0);
				analogWrite(RotaryLineMotor, 0);
				LineManI=0;
				StartManualMotors(ConvMan, LineMan);
			  
			}
     if(In == '6')
     {
      CrankSpeed++;
     }
     if (In == '4')
     {
      CrankSpeed--;
     }
     if (In == '1')
     {
       //Start White Screen
 tft.fillScreen(TFT_WHITE);
 //Initially Calibration and taring are off
 isCalibration = false;
 isTare = false;
 //Knowing BaseTime
 BaseTime = millis();
 CurrentTime = BaseTime;
 //Before we start please choose the mode
 ChooseMode();
//We retrieve Operation Weight for the current mode
 char F = EEPROM.read(100+Mode);
 
 char S = EEPROM.read(101+Mode);
 
 char T = EEPROM.read(102+Mode);

  char F1 = EEPROM.read(103+Mode);
 
 char S1 = EEPROM.read(104+Mode);
 
 char T1 = EEPROM.read(105+Mode);

  
OperationWeight = (F-'0')*100000 + (S-'0')*10000 + (T-'0')*1000 + (F1-'0')*100 + (S1-'0')*10 + (T1-'0');
 
 //Now we start the basic display interface
 
 DisplayInterface();
 break;
      }
          if(digitalRead(14))
      {
         StartConv = !StartConv;
            delay(500);
        while (digitalRead(14)==HIGH)
        {
          analogWrite(ConvMotor, 255);
          StartConv=0;
          }
      }
        if(digitalRead(15))
      {
         StartLine=!StartLine;

                  delay(500);
        while (digitalRead(15)==HIGH)
        {
          analogWrite(RotaryLineMotor, 255);
          StartLine=0;
          
          }
      }
      
			if(StartConv)
			{
        analogWrite(CrankMotor, CrankSpeed);
				ConvMotorSpeed(85+ConvMan);
      
         
			}
     else
     {
        ConvMotorSpeed(0);
        analogWrite(CrankMotor, 0);
      }
			if(StartLine)
			{
        LineManI=LineMan;
			}
			else
			{
				digitalWrite(RotaryLineMotor, 0);
				LineManI=0;
			}

     
     
			LineMotorSpeed(LineManI);
		
		}
	}
	
		
	else if(Mode >= '1' && Mode <='4')
	{
    
		EnterTime();

		while(1)
		{
	
		
      
      
			DisplayWeight();
			AutoPacking();
			
			CalibTareCapture();
			if(isCalibration)
			Calibraion();
		
			if(isTare)
			TareWeight();

      if(restart)
      {
        restart = false;
      break;
      }
     
		}	
	}


    } 

/*************************************************************************************************************************************************/

void SetOperationWeight()
{
      again:
   tft.fillRoundRect(10, 40, 460, 170, 10, TFT_BLACK);
   tft.fillRoundRect(20, 50, 440, 150, 10, TFT_GREEN);
   tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(2);
   tft.drawCentreString("Enter Operation Weight (0-30) kg", 230, 100, 1);
   tft.drawCentreString("# -> Enter || * -> Escape", 230, 125, 1);
   
   char T;
   char F = WaitEnteredKey();
   if(F=='*'||F=='#')
   goto again;
   tft.drawChar(F, 225, 150, 1);
   char S = WaitEnteredKey();
   if(S== '#')
  {
    T=F;
    S='0';
    F='0';
    }
    else if(S== '*')
    {
      goto again;
      }
      else
      {
  tft.drawChar(F, 220, 150, 1);
  tft.drawChar(S, 230, 150, 1);
  T = WaitEnteredKey();
    if(T== '#')
  {
    T=S;
    S=F;
    F='0';
    }
    else if(T== '*')
    {
      goto again;
      }
      else
      {
          tft.drawChar(F, 220, 150, 1);
  tft.drawChar(S, 230, 150, 1);
  tft.drawChar(T, 240, 150, 1);
  while(GetEnteredKey() != '#')
  {
    if(GetEnteredKey() == '*')
      goto again;
  }
      }
  
  }


//Now For Grams

  again1:
   tft.fillRoundRect(10, 40, 460, 170, 10, TFT_BLACK);
   tft.fillRoundRect(20, 50, 440, 150, 10, TFT_GREEN);
   tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(2);
   tft.drawCentreString("Enter grams suffix (0-999) gm", 230, 100, 1);
   tft.drawCentreString("# -> Enter || * -> Escape", 230, 125, 1);
   
   char T1;
   char F1 = WaitEnteredKey();
   if(F1=='*'||F1=='#')
   goto again1;
   tft.drawChar(F1, 225, 150, 1);
   char S1 = WaitEnteredKey();
   if(S1== '#')
  {
    T1=F1;
    S1='0';
    F1='0';
    }
    else if(S1== '*')
    {
      goto again1;
      }
      else
      {
  tft.drawChar(F1, 220, 150, 1);
  tft.drawChar(S1, 230, 150, 1);
  T1 = WaitEnteredKey();
    if(T1== '#')
  {
    T1=S1;
    S1=F1;
    F1='0';
    }
    else if(T1== '*')
    {
      goto again1;
      }
      else
      {
          tft.drawChar(F1, 220, 150, 1);
  tft.drawChar(S1, 230, 150, 1);
  tft.drawChar(T1, 240, 150, 1);
  while(GetEnteredKey() != '#')
  {
    if(GetEnteredKey() == '*')
      goto again1;
  }
      }
  
  }





if(((F-'0')*100 + (S-'0')*10 + (T-'0'))>=30)
{
  F='0';
  S='3';
  T='0';
}
  
OperationWeight = (F-'0')*100000 + (S-'0')*10000 + (T-'0')*1000 + (F1-'0')*100 + (S1-'0')*10 + (T1-'0');

   EEPROM.put(100+Mode, F);
   EEPROM.put(101+Mode, S);
   EEPROM.put(102+Mode, T);
   EEPROM.put(103+Mode, F1);
   EEPROM.put(104+Mode, S1);
   EEPROM.put(105+Mode, T1);
   
   

  DisplayInterface();
}

/*************************************************************************************************************************************************/
 
  
void StartManualMotors(int& ConvMan, int& LineMan)
{
	
	//Green Background
	SMMagain1:
	tft.fillScreen(TFT_GREEN);
	tft.setTextColor(TFT_BLACK, TFT_GREEN);
	tft.setTextSize(3);
    tft.drawCentreString("Enter loading", 230, 100, 1);
	tft.drawCentreString("conveyor speed: (00-99)", 230, 130, 1);
	char F1 = WaitEnteredKey();
	if(F1=='*'||F1=='#')
		goto SMMagain1;
	tft.drawChar(F1, 225, 170, 1);
	char S1 = WaitEnteredKey();
	if(S1== '#')
	{
		S1=F1;
		F1='0';
    }
    else if(S1== '*')
    {
		goto SMMagain1;
    }
    else
	{
		tft.drawChar(F1, 220, 170, 1);
		tft.drawChar(S1, 230, 170, 1);
		while(GetEnteredKey() != '#')
		{
			if(GetEnteredKey() == '*')
				goto SMMagain1;
		}
    }
  
  	SMMagain2:
	tft.fillScreen(TFT_GREEN);
	tft.setTextColor(TFT_BLACK, TFT_GREEN);
	tft.setTextSize(3);
	tft.drawCentreString("Enter line speed: (00-99)", 230, 100, 1);
	char F2 = WaitEnteredKey();
	if(F2=='*'||F2=='#')
		goto SMMagain2;
	tft.drawChar(F2, 225, 150, 1);
	char S2 = WaitEnteredKey();
	
	if(S2== '#')
	{
		S2=F2;
		F2='0';
    }
    else if(S2== '*')
    {
		goto SMMagain2;
    }
    else
    {
		tft.drawChar(F2, 220, 150, 1);
		tft.drawChar(S2, 230, 150, 1);
		while(GetEnteredKey() != '#')
		{
			if(GetEnteredKey() == '*')
				goto SMMagain2;
		}
    }
   
	float FS1 = (F1-'0')*10 + (S1-'0');
	float FS2 = (F2-'0')*10 + (S2-'0');
  
	FS1 = FS1 * (170.0/99.0);
	FS2 = FS2 * (255.0/99.0);
  
	ConvMan = FS1;
	LineMan = FS2;
	ManDisplayInterface();
}
	
/*************************************************************************************************************************************************/
	
void EnterTime()
{
	again:
   tft.fillRoundRect(10, 40, 460, 170, 10, TFT_BLACK);
   tft.fillRoundRect(20, 50, 440, 150, 10, TFT_GREEN);
   tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(2);
   tft.drawCentreString("Please Enter Pack Time (00-99 min)", 230, 100, 1);
      tft.drawCentreString("# -> Enter || * -> Escape", 230, 125, 1);
  char F = WaitEnteredKey();
  if(F=='*'||F=='#')
  goto again;
  tft.drawChar(F, 225, 150, 1);
  char S = WaitEnteredKey();
  if(S== '#')
  {
    S=F;
    F='0';
    }
    else if(S== '*')
    {
      goto again;
      }
      else
      {
  tft.drawChar(F, 220, 150, 1);
  tft.drawChar(S, 230, 150, 1);
  while(GetEnteredKey() != '#')
  {
	  if(GetEnteredKey() == '*')
		  goto again;
  }
      }

  t = (F-'0')*10 + (S-'0');

 
   DisplayInterface();
  }


  void EnterLegnth()
  {

  
      again2:
  tft.fillRoundRect(10, 40, 350, 160, 10, TFT_BLACK);
  tft.fillRoundRect(20, 50, 340, 150, 10, TFT_GREEN);
     tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(2);
  tft.drawCentreString("Please Enter Wire Length (meters)", 230, 100, 1);
        tft.drawCentreString("# -> Enter || * -> Escape", 230, 125, 1);
  char F1 = WaitEnteredKey();
  if(F1=='*'||F1=='#')
  goto again2;
  tft.drawChar(F1, 225, 150, 1);
  char S1 = WaitEnteredKey();
  if(S1== '#')
  {
    S1=F1;
    F1='0';
    }
    else if(S1== '*')
    {
      goto again2;
      }
      else
      {
  tft.drawChar(F1, 220, 150, 1);
  tft.drawChar(S1, 230, 150, 1);
  while(GetEnteredKey() != '#')
  {
      if(GetEnteredKey() == '*')
      goto again2;
  }
      }
  
    w = (F1-'0')*10 + (S1-'0');
       DisplayInterface();
  }
  

/*************************************************************************************************************************************************/
  
void ChooseMode()
{
	//Green Background
   tft.fillScreen(TFT_GREEN);
   //Text filling
   tft.setTextColor(TFT_BLACK, TFT_GREEN);
   tft.setTextSize(3);
   tft.drawCentreString("Please Enter a Number", 230, 100, 1);
   tft.drawCentreString("(1-9)", 230, 150, 1);
   tft.setTextSize(2);
   tft.drawCentreString("to retrieve previous", 230, 220, 1);
   tft.drawCentreString(" configuration or set it", 230, 250, 1); 
    tft.drawCentreString(" , press 0 for manual mode", 230, 270, 1); 
   //get entered key and store it 
   Mode = WaitEnteredKey();
   //Clear screen until next frame
   tft.fillScreen(TFT_WHITE);
}

/*************************************************************************************************************************************************/

void DisplayInterface(void)
{  char str[12];
 //On a white background
  tft.fillScreen(TFT_WHITE);
  //Draw basic background interface
   tft. fillCircle(120, 160, 100, TFT_BLACK);
   tft. fillCircle(120, 160, 90, TFT_GREEN);
   tft.fillRoundRect(240, 20, 200, 40, 10, TFT_BLACK);
   tft.fillRoundRect(240, 80, 200, 40,10, TFT_BLACK);
   tft.fillRoundRect(240, 140, 200, 40, 10,TFT_BLACK);
   tft.fillRoundRect(240, 200, 200, 40, 10,TFT_BLACK);
   tft.fillRoundRect(240, 260, 200, 40, 10,TFT_BLACK);
   //Text Filling
   tft.setTextColor(TFT_BLACK, TFT_WHITE);
   tft.setTextSize(2);
    tft.drawCentreString("Current Mode: ", 100, 4, 1);
       tft.drawChar(Mode, 180, 4, 1);
       tft.drawCentreString("* : Calibration", 100, 20, 1);
       tft.drawCentreString("# : Tare Weight ", 100, 40, 1);


   

  
     tft.setTextSize(2);
   tft.drawCentreString("Operation Weight", 340, 2, 1);
     tft.setTextSize(2);
   tft.drawCentreString("Crank Speed", 340, 62, 1);
   tft.setTextSize(2);
   tft.drawCentreString("Conv. Motor Load ", 340, 122, 1);
     tft.setTextSize(2);
   tft.drawCentreString("Line Motor Load ", 340, 182, 1);
    tft.setTextSize(2);
   tft.drawCentreString("Crank Motor Load ", 340, 242, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int val = OperationWeight;
  tft.setTextSize(2); 
  ltoa(val, str, 10);
  tft.drawCentreString(str, 340, 25, 2);
   ltoa(0, str, 10);
  tft.drawCentreString(str, 340, 85, 2);
  tft.drawCentreString(str, 340, 145, 2);
  tft.drawCentreString(str, 340, 205, 2);
  tft.drawCentreString(str, 340, 265, 2);
 
   
   
  
  }
  
/*************************************************************************************************************************************************/ 
 
  void ManDisplayInterface(void)
{  char str[12];
 //On a white background
  tft.fillScreen(TFT_WHITE);
  //Draw basic background interface
   tft. fillCircle(120, 160, 100, TFT_BLACK);
   tft. fillCircle(120, 160, 90, TFT_GREEN);
   tft.fillRoundRect(240, 20, 200, 40, 10, TFT_BLACK);
   tft.fillRoundRect(240, 80, 200, 40,10, TFT_BLACK);
   tft.fillRoundRect(240, 140, 200, 40, 10,TFT_BLACK);
   tft.fillRoundRect(240, 200, 200, 40, 10,TFT_BLACK);
   tft.fillRoundRect(240, 260, 200, 40, 10,TFT_BLACK);
   //Text Filling
   tft.setTextColor(TFT_BLACK, TFT_WHITE);
   tft.setTextSize(2);
    tft.drawCentreString("Current Mode: ", 100, 4, 1);
       tft.drawChar(Mode, 180, 4, 1);
       tft.drawCentreString("*: Set Motors' Speed", 100, 20, 1);
       tft.drawCentreString("# : Tare Weight ", 100, 40, 1);
   tft.setTextSize(3);
   tft.drawCentreString("Current Load", 120, 265, 1);
   tft.drawCentreString("(Grams)", 120, 295, 1);
     tft.setTextSize(2);
   tft.drawCentreString("Operation Weight", 340, 2, 1);
     tft.setTextSize(2);
tft.drawCentreString("Crank Speed", 340, 62, 1);
   tft.setTextSize(2);
   tft.drawCentreString("Conv. Motor Load", 340, 122, 1);
     tft.setTextSize(2);
   tft.drawCentreString("Line Motor Load", 340, 182, 1);
    tft.setTextSize(2);
   tft.drawCentreString("Crank Motor Load", 340, 242, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int val = OperationWeight;
  tft.setTextSize(2); 
  ltoa(val, str, 10);
  tft.drawCentreString(str, 340, 25, 2);
   ltoa(0, str, 10);
  tft.drawCentreString(str, 340, 85, 2);
  tft.drawCentreString(str, 340, 145, 2);
  tft.drawCentreString(str, 340, 205, 2);
  tft.drawCentreString(str, 340, 265, 2);
 
   
   
  
  }

/*************************************************************************************************************************************************/
  
void DisplayWeight()
{
tft.setTextColor(TFT_BLACK, TFT_GREEN);
float val = ReadWeight();
tft.setTextSize(3); 
char str[12];
ltoa(val, str, 10);
tft.drawCentreString("         ", 120, 140, 2);
tft.drawCentreString(str, 120, 140, 2);
tft.setTextSize(2); 
tft.setTextColor(TFT_WHITE, TFT_BLACK);

if(CrankSpeed>=255)
{
CrankSpeed=255;
}
if(CrankSpeed<=0)
{
CrankSpeed=0;
}



   tft.setTextSize(3);



val = (millis()/1000)-TimerS;
ltoa(val, str, 10);
tft.drawCentreString(str, 150, 265, 1);

val = (millis()/1000/60)-TimerM;
ltoa(val, str, 10);
tft.drawCentreString(str, 130, 265, 1);
tft.drawCentreString(":", 140, 265, 1);


val = (millis()/1000/60/60)-TimerH;
ltoa(val, str, 10);
tft.drawCentreString(str, 110, 265, 1);
tft.drawCentreString(":", 120, 265, 1);



val = CrankSpeed;
ltoa(val, str, 10);

tft.drawCentreString("         ", 340, 85, 2);
tft.drawCentreString(str, 340, 85, 2);





val = analogRead(ConvMotorLoad);
//val = -((val-512)/40)*100;
ltoa(val, str, 10);

tft.drawCentreString("         ", 340, 145, 2);
tft.drawCentreString(str, 340, 145, 2);


val = analogRead(RotaryLineMotorLoad);
//val = -(val-512);
ltoa(val, str, 10);
tft.drawCentreString("         ", 340, 205, 2);
tft.drawCentreString(str, 340, 205, 2);


val = analogRead(CrankLineMotorLoad);
//val = -((val-512)/90)*100;
ltoa(val, str, 10);
tft.drawCentreString("         ", 340, 265, 2);
tft.drawCentreString(str, 340, 265, 2);

delay(2);
 
}
/*************************************************************************************************************************************************/

void ConvMotorSpeed(float S)
{

  analogWrite(ConvMotor,S);
  
  }

/*************************************************************************************************************************************************/
  
void LineMotorSpeed(float S)
{ 
  CurrentTime = millis();
  float  deltaT = CurrentTime - BaseTime; 

  if(S==0) return;
  double R = (S/255.0);
 
R=1-R;
R=R*50;
 LineIterator++; 


    if(LineIterator<=int(R))
    {
      SLine = 0;
     
    }
    else
    {
      SLine =1;
      }
     if(LineIterator>=50)
      {
         LineIterator=0;
        }
  
  if (SLine)
  {
  analogWrite(RotaryLineMotor ,80); 
  }
  else
  {
  analogWrite(RotaryLineMotor ,0);
  SLine=0;
  }

  if (deltaT>=20000)
  {
    BaseTime = CurrentTime;
    }

 }

/*************************************************************************************************************************************************/


  
void LineMotorSpeedLoad(float S)
{ 
  CurrentTime = millis();
  float val = analogRead(RotaryLineMotorLoad);
float  deltaT = CurrentTime - BaseTime; 
    analogWrite(RotaryLineMotor, 25);

if(S==0) return;
  
    
    if(val >= base && analogRead(CrankLineMotorLoad) > 500)
    {
      
      for(int i=220; i>15; i--)
      {

  analogWrite(RotaryLineMotor, i);
  delay(5);
 
      }
      BaseTime=CurrentTime;
       base= val;
       if(deltaT<=2500)
       {
             for(int i=220; i>15; i--)
      {

  analogWrite(RotaryLineMotor, i);
  delay(5);
 
      }
        }
    
 
    }

       if(base>=LoadSensitivity)
    {
      base=LoadSensitivity;
      }
  


 }



void DisplayTimer()
{

  
  
  }






