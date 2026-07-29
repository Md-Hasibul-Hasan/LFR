
#include "Globals.h"

int minValue[SENSOR_COUNT];
int maxValue[SENSOR_COUNT];

int rawValue[SENSOR_COUNT];
int norValue[SENSOR_COUNT];
bool inverseTrack = false;



const int weight[SENSOR_COUNT] = { -350, -250, -150, -50, 50, 150 , 250, 350};
const int CENTER = 0;
int lastPosition = 0;

int position = 0;
int error = 0;

int ReadMuxChannel(int channel){
    digitalWrite(S0, channel & 0x01);
    digitalWrite(S1, (channel >> 1) & 0x01);
    digitalWrite(S2, (channel >> 2) & 0x01);
    digitalWrite(S3, (channel >> 3) & 0x01);
    delayMicroseconds(5);
    return analogRead(SIG_PIN);
}


// void Calibration(){

//   for(int i=0; i<SENSOR_COUNT; i++){
//     minValue[i]=1023;
//     maxValue[i]=0;
//   }

//   unsigned long startTime = millis();
//   while(millis() - startTime < 5000){

//     for(int i=0; i<SENSOR_COUNT; i++){
//       int val = ReadMuxChannel(i);
//       if(val<minValue[i]) minValue[i]=val;
//       if(val>maxValue[i]) maxValue[i]=val;
//     }

//   }
// }

void Calibration(){
    while (true){

        //==========================
        // Show Previous Result
        //==========================
        while (true){
            UpdateButtons(); 

            if (okEvent == Button::Event::SHORT)
            {
                LoadSettings();
                currentScreen = Screen::DASHBOARD;
                return;
            }

            if (okEvent == Button::Event::LONG)
                break;

            u8g2.firstPage();
            do
            {
                u8g2.setFont(u8g2_font_6x12_tf);

                u8g2.drawStr(30, 10, "CALIBRATION");
                u8g2.drawHLine(0,12,128);

                char buf[20];

                for(byte i=0;i<4;i++)
                {
                    sprintf(buf,"S%d %3d %3d",i,minValue[i],maxValue[i]);
                    u8g2.drawStr(0,22+i*10,buf);

                    sprintf(buf,"S%d %3d %3d",i+4,minValue[i+4],maxValue[i+4]);
                    u8g2.drawStr(64,22+i*10,buf);
                }

                u8g2.drawStr(0, 62, "OK:Back      LOK:Cal");
            }
            while (u8g2.nextPage());
        }

        //==========================
        // Reset Min/Max
        //==========================
        for (byte i = 0; i < SENSOR_COUNT; i++)
        {
            minValue[i] = 1023;
            maxValue[i] = 0;
        }

        //==========================
        // Calibrating...
        //==========================
        unsigned long startTime = millis();

        while (millis() - startTime < 5000)
        {
            UpdateButtons();

            if (okEvent == Button::Event::SHORT)
            {
                LoadSettings();
                currentScreen = Screen::DASHBOARD;
                return;
            }

            for (byte i = 0; i < SENSOR_COUNT; i++)
            {
                int val = ReadMuxChannel(i);

                if (val < minValue[i]) minValue[i] = val;
                if (val > maxValue[i]) maxValue[i] = val;
            }

            u8g2.firstPage();
            do
            {
                u8g2.setFont(u8g2_font_6x12_tf);
                u8g2.drawStr(18, 30, "Calibrating...");
            }
            while (u8g2.nextPage());
        }

        SaveSettings();


        // Calibration শেষ হলে outer while(true) আবার
        // Result Screen দেখাবে।
    }
}





void ReadSensors(){
    for (int i = 0; i < SENSOR_COUNT; i++){
        // Read Raw Value
        rawValue[i] = ReadMuxChannel(i);

        int range = maxValue[i] - minValue[i];

        if(range == 0){
          norValue[i] = 0;
        } else{
          norValue[i] = (rawValue[i] - minValue[i]) * 1000 / range;
        }

        // Normalize (0 - 1000)
        norValue[i] = constrain(norValue[i], 0, 1000);
        if(inverseTrack) norValue[i] = 1000 - norValue[i];
    }

    position = CalculatePosition();
    error = CalculateError(position);
}



int CalculatePosition(){
    long weightedSum = 0;
    long total = 0;

    for (int i = 0; i < SENSOR_COUNT; i++){
        weightedSum += (long)norValue[i] * weight[i];
        total += norValue[i];
    }

    if (total == 0) return lastPosition;

    lastPosition = weightedSum / total;

    return lastPosition;
}



int CalculateError(int position){
  int error = position - CENTER;
  return error;
}