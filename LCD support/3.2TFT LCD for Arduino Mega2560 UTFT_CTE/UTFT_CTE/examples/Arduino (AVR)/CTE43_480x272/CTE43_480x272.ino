// Demo with Font and icon Flash IC 
// (C) 2013 Coldtears electronics
// For use with  CTE 4.3 inch LCD Module w/Font and Icon Flash IC
// in combination with CTE TFT/SD Shield for Arduino Mega2560
// web: http://stores.shop.ebay.com/coldtears-electronics-store
//
// The first part of the demo is to show how to use the font IC to draw font to the LCD,
// The IC also include some commonly used and open source icons
// great for custom application development.
//
// Second part of the demo code is adapted from Henning Karlsen's demo
// Original by
// UTFT_Demo_480x272 (C)2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This demo was made for modules with a screen resolution 
// of 480x272 pixels.
// This sketch requires the UTFT library by Henning Karlsen
// also requires the Flash_IC library by Coldtears electronics
//


#include <UTFT.h>
#include <UTFT_CTE.h>
#include "SPI.h"

extern uint8_t SmallFont[];

// Set up UTFT for CTE 4.3inch on Arduino Mega
UTFT myGLCD(ITDB43,38,39,40,41);
UTFT_CTE CTE_LCD(&myGLCD);

void setup()
{
  delay(1000);
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(VGA_BLACK);
  CTE_LCD.SPI_Flash_init(FLASH_CS_PIN);
}


void loop()
{
  myGLCD.clrScr();
  CTE_LCD.show_color_bar();
  delay(1000);

  myGLCD.fillScr(VGA_BLACK);
 CTE_LCD.Load_image(80,16,237);

  myGLCD.setColor(255, 9, 255);  //Set font color
  myGLCD.setBackColor(0, 0, 0);  //Set background color
  CTE_LCD.Set_character_spacing(1); 
  CTE_LCD.Put_Text("Coldtears electronics",60,226,BVS_15);
  CTE_LCD.Put_Text("TFT LCD w/Font IC Demo",60,242,BVS_15);
  delay(1000);
 
  myGLCD.clrScr();
  CTE_LCD.Set_character_spacing(3); 
  CTE_LCD.Put_Text("Coldtears electronics",10,5,BVS_34);
  CTE_LCD.Set_character_spacing(1);
  myGLCD.setColor(166, 256, 211);  myGLCD.setBackColor(0, 255, 0);
  CTE_LCD.Put_Text("4.3 TFT LCD Module w/Font IC Demo",10,40,BVS_22);
  myGLCD.setColor(255, 255, 255);  myGLCD.setBackColor(0, 0, 0); 
  CTE_LCD.Put_Text("123456789",10,65,BVS_15);  
  myGLCD.setColor(0, 255, 255);
  CTE_LCD.Put_Text("abcdefg",10,80,BVS_34);   
  myGLCD.setColor(255, 255, 0);
  CTE_LCD.Put_Text("hijklmnopgrstu",10,115,BVS_43);
  myGLCD.setColor(255, 0, 255);
  CTE_LCD.Put_Text("12345789",10,165,BVS_74);

  delay(1000);
  myGLCD.clrScr();   
  myGLCD.setColor(255, 255, 255);
  static char buff[33];
  for(unsigned char temp=32; temp<65; temp++)  { buff[temp-32]=temp; }  buff[33]=0;
   CTE_LCD.Put_Text_array(buff,0,0,BVS_22);
  for(unsigned char temp=65; temp<92; temp++)  { buff[temp-65]=temp; }  buff[27]=0;
  CTE_LCD.Put_Text_array(buff,0,30,BVS_22); 
  for(unsigned char temp=92; temp<127; temp++)  { buff[temp-92]=temp; }  buff[33]=0;
  CTE_LCD.Put_Text_array(buff,0,60,BVS_22); 
  for(unsigned char temp=127; temp<154; temp++)  { buff[temp-127]=temp; }  buff[27]=0;
  CTE_LCD.Put_Text_array(buff,0,90,BVS_22);   
  for(unsigned char temp=154; temp<181; temp++)  { buff[temp-154]=temp; }  buff[27]=0;
  CTE_LCD.Put_Text_array(buff,0,120,BVS_22); 
   for(unsigned char temp=181; temp<204; temp++)  { buff[temp-181]=temp; }  buff[23]=0;
  CTE_LCD.Put_Text_array(buff,0,150,BVS_22);   
  for(unsigned char temp=204; temp<227; temp++)  { buff[temp-204]=temp; }  buff[23]=0;
  CTE_LCD.Put_Text_array(buff,0,180,BVS_22); 
   for(unsigned char temp=227; temp<255; temp++)  { buff[temp-227]=temp; }  buff[27]=0;
  CTE_LCD.Put_Text_array(buff,0,210,BVS_22);  
  delay(1000);

unsigned int k=4,location=1;
while(k>0)
{ 
  myGLCD.clrScr(); 
  for (unsigned temp1 =0; temp1<6; temp1++)
  {
    for (unsigned temp2 =0; temp2<10; temp2++)
    {
      CTE_LCD.Load_image(temp2*45,temp1*45,location); 
      location++; 
      if(location==236) break;
    }
  if(location==236) break; 
  }
  delay(1000);
  k--;
}

myGLCD.clrScr();
CTE_LCD.Put_Text("Countdown for LCD Display",90,0,BVS_28);
CTE_LCD.Put_Text("Demo by Henning Karlsen",90,30,BVS_28);
myGLCD.setColor(150, 222, 255);
k=50;
while(k>0)
{  
  k--;
  String test_string = String(k);
  test_string = "000"+ test_string + "       "; 
  CTE_LCD.Put_Text(test_string,120,60,BVS_112);
  int temp;
  if (k%3==2) temp=275;  else if (k%3==1) temp=277;  else temp=279;
  CTE_LCD.Load_image(150,175,temp); CTE_LCD.Load_image(250,175,temp+6);
  delay(100);
}
 
// Default demo by Henning Karlsen Starts Here:

  int buf[478];
  int x, x2;
  int y, y2;
  int r;

// Clear the screen and draw the frame
  myGLCD.clrScr();

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 479, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 258, 479, 271);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("* Universal Color TFT Display Library *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print("<http://electronics.henningkarlsen.com>", CENTER, 259);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 479, 257);

// Draw crosshairs
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(239, 15, 239, 256);
  myGLCD.drawLine(1, 135, 478, 135);
  for (int i=9; i<470; i+=10)
    myGLCD.drawLine(i, 133, i, 138);
  for (int i=15; i<256; i+=10)
    myGLCD.drawLine(237, i, 241, i);

// Draw sin-, cos- and tan-lines  
  myGLCD.setColor(0,255,255);
  myGLCD.print("Sin", 5, 15);
  for (int i=1; i<478; i++)
  {
    myGLCD.drawPixel(i,135+(sin(((i*1.13)*3.14)/180)*95));
  }
  
  myGLCD.setColor(255,0,0);
  myGLCD.print("Cos", 5, 27);
  for (int i=1; i<478; i++)
  {
    myGLCD.drawPixel(i,135+(cos(((i*1.13)*3.14)/180)*95));
  }

  myGLCD.setColor(255,255,0);
  myGLCD.print("Tan", 5, 39);
  for (int i=1; i<478; i++)
  {
    myGLCD.drawPixel(i,135+(tan(((i*1.13)*3.14)/180)));
  }

  delay(2000);
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(239, 15, 239, 256);
  myGLCD.drawLine(1, 135, 478, 135);

// Draw a moving sinewave
  x=1;
  for (int i=1; i<(478*20); i++) 
  {
    x++;
    if (x==479)
      x=1;
    if (i>479)
    {
      if ((x==239)||(buf[x-1]==135))
        myGLCD.setColor(0,0,255);
      else
        myGLCD.setColor(0,0,0);
      myGLCD.drawPixel(x,buf[x-1]);
    }
    myGLCD.setColor(0,255,255);
    y=135+(sin(((i*1.65)*3.14)/180)*(90-(i / 100)));
    myGLCD.drawPixel(x,y);
    buf[x-1]=y;
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some filled rectangles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillRect(150+(i*20), 46+(i*20), 210+(i*20), 106+(i*20));
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some filled, rounded rectangles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillRoundRect(330-(i*20), 46+(i*20), 270-(i*20), 106+(i*20));
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some filled circles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillCircle(180+(i*20),75+(i*20), 30);
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some lines in a pattern
  myGLCD.setColor (255,0,0);
  for (int i=15; i<256; i+=5)
  {
    myGLCD.drawLine(1, i, (i*1.88)-10, 256);
  }
  myGLCD.setColor (255,0,0);
  for (int i=256; i>15; i-=5)
  {
    myGLCD.drawLine(478, i, (i*1.88)-11, 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=256; i>15; i-=5)
  {
    myGLCD.drawLine(1, i, 491-(i*1.88), 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=15; i<256; i+=5)
  {
    myGLCD.drawLine(478, i, 490-(i*1.88), 256);
  }
  
  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some random circles
  for (int i=0; i<150; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=32+random(416);
    y=45+random(178);
    r=random(30);
    myGLCD.drawCircle(x, y, r);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some random rectangles
  for (int i=0; i<150; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(476);
    y=16+random(239);
    x2=2+random(476);
    y2=16+random(239);
    myGLCD.drawRect(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

// Draw some random rounded rectangles
  for (int i=0; i<150; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(476);
    y=16+random(239);
    x2=2+random(476);
    y2=16+random(239);
    myGLCD.drawRoundRect(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

  for (int i=0; i<150; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(476);
    y=16+random(239);
    x2=2+random(476);
    y2=16+random(239);
    myGLCD.drawLine(x, y, x2, y2);
  }

  delay(2000);
  
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,478,256);

  for (int i=0; i<10000; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    myGLCD.drawPixel(2+random(476), 16+random(239));
  }

  delay(2000);

  myGLCD.fillScr(0, 0, 255);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect(160, 70, 319, 169);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("That's it!", CENTER, 93);
  myGLCD.print("Restarting in a", CENTER, 119);
  myGLCD.print("few seconds...", CENTER, 132);
  
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.print("Runtime: (msecs)", CENTER, 243);
  myGLCD.printNumI(millis(), CENTER, 258);
  
  
  delay (1000);
  
  myGLCD.clrScr();
}