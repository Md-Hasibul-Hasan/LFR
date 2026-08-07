#include <U8g2lib.h>
#include <Wire.h>
#include "Button.h"
#include "Globals.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);

Button up(A3); 
Button down(A1);
Button ok(A2);

Button::Event upEvent;
Button::Event downEvent;
Button::Event okEvent;

enum class ItemType
{
	FLOAT,
	INT,
	OPTION,
	INFO,
	ACTION
};

struct OptionItem
{
	const char **options;
	byte *index;
	byte count;
};

const char *llrOptions[] = {"LFT","RGT","STR"};
byte llrIdx = 0;
OptionItem llr ={llrOptions,&llrIdx,3};

struct DashboardItem
{
	const char* title;
	ItemType type;
	void* value;
	OptionItem *option;
	Screen nextScreen;
};


DashboardItem dashboard[] =
{
	{"Calibration",ItemType::ACTION,nullptr,nullptr,Screen::CALIBRATION},
  {"Line Follow",ItemType::ACTION,nullptr,nullptr,Screen::LINE_FOLLOW},
	{"Sensor View",ItemType::ACTION,nullptr,nullptr,Screen::SENSOR_VIEW},
	{"Motor Test",ItemType::ACTION,nullptr,nullptr,Screen::MOTOR_TEST},
	{"Set Timer",ItemType::ACTION,nullptr,nullptr,Screen::SET_TIMER},
	{"kp",ItemType::FLOAT,&kp,nullptr,Screen::DASHBOARD},
	{"ki",ItemType::FLOAT,&ki,nullptr,Screen::DASHBOARD},
	{"Kd",ItemType::FLOAT,&kd,nullptr,Screen::DASHBOARD},
	{"Speed",ItemType::INT,&baseSpeed,nullptr,Screen::DASHBOARD},
	{"Lost Line",ItemType::OPTION,nullptr,&llr,Screen::DASHBOARD},
	// {"Error",ItemType::INFO,&error,nullptr,Screen::DASHBOARD},
	// {"Position",ItemType::INFO,&position,nullptr,Screen::DASHBOARD},

};

const byte ITEM_COUNT = sizeof(dashboard) / sizeof(dashboard[0]);

byte selectedItem = 0;
byte topItem = 0;
const byte visibleItems = 5;
bool editing = false;

void OLEDInit()
{
	u8g2.begin();
	u8g2.setFont(u8g2_font_6x12_tf);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
}

void UpdateButtons()
{
	upEvent = up.read();
	downEvent = down.read();
	okEvent = ok.read();
}

void DrawDashboard(){

	bool blink = (millis() / 500) & 1;

	if (selectedItem < topItem)
		topItem = selectedItem;
	if (selectedItem >= topItem + visibleItems)
		topItem = selectedItem - visibleItems + 1;
	u8g2.firstPage();
	do
	{
		u8g2.setFont(u8g2_font_6x12_tf);
		u8g2.drawStr(28,10,"DASHBOARD");
		u8g2.drawHLine(0,12,128);
		for(byte i=0;i<visibleItems;i++){
			byte index = topItem + i;
			if(index >= ITEM_COUNT)
				break;
			int y = 22 + i * 10;
			if(index == selectedItem)
				u8g2.drawStr(0,y,">");
			u8g2.drawStr(10,y,dashboard[index].title);
			char buf[16];
			if(dashboard[index].type == ItemType::FLOAT)
			{
				dtostrf(*(float*)dashboard[index].value,4,2,buf);
				if(editing && index==selectedItem)
				{
					if(blink){
						u8g2.drawBox(84,y-9,34,11);
						u8g2.setDrawColor(0);
						u8g2.drawStr(86,y,buf);
						u8g2.setDrawColor(1);
					}else
						u8g2.drawStr(86,y,buf);
				}
				else
					u8g2.drawStr(86,y,buf);
			}
			else if(dashboard[index].type == ItemType::INT)
			{
				sprintf(buf,"%d",*(int*)dashboard[index].value);
				if(editing && index==selectedItem)
				{
					if(blink){
						u8g2.drawBox(84,y-9,34,11);
						u8g2.setDrawColor(0);
						u8g2.drawStr(86,y,buf);
						u8g2.setDrawColor(1);
					}else
						u8g2.drawStr(86,y,buf);
				}
				else
					u8g2.drawStr(86,y,buf);
			}
			else if(dashboard[index].type == ItemType::OPTION)
			{
				OptionItem *opt = dashboard[index].option;
				if(editing && index==selectedItem)
				{
					if(blink){
						u8g2.drawBox(84,y-9,34,11);
						u8g2.setDrawColor(0);
						u8g2.drawStr(86,y,opt->options[*opt->index]);
						u8g2.setDrawColor(1);
					}else
						u8g2.drawStr(86,y,opt->options[*opt->index]);
				}
				else
					u8g2.drawStr(86,y,opt->options[*opt->index]);
			}
			else if(dashboard[index].type == ItemType::INFO)
			{
				sprintf(buf,"%d",*(int*)dashboard[index].value);
				u8g2.drawStr(86,y,buf);
			}
			else if(dashboard[index].type == ItemType::ACTION)
				u8g2.drawStr(112,y,">");
		}
		if(ITEM_COUNT > visibleItems)
		{
			int barHeight = (visibleItems * 50) / ITEM_COUNT;
			if(barHeight < 8)
				barHeight = 8;
			int barY = 15 + ((50 - barHeight) * topItem) / (ITEM_COUNT - visibleItems);
			u8g2.drawFrame(124,15,4,50);
			u8g2.drawBox(125,barY,2,barHeight);
		}
	}while(u8g2.nextPage());
}


void HandleDashboard()
{
	if(editing){
		if(upEvent == Button::Event::SHORT ||
		   upEvent == Button::Event::REPEAT)
		{
			if(dashboard[selectedItem].type == ItemType::FLOAT)
				(*(float*)dashboard[selectedItem].value) += 0.05f;
			else if(dashboard[selectedItem].type == ItemType::INT)
				(*(int*)dashboard[selectedItem].value)++;
			else if(dashboard[selectedItem].type == ItemType::OPTION)
			{
				OptionItem *opt = dashboard[selectedItem].option;
				(*opt->index)++;
				if(*opt->index >= opt->count)
					*opt->index = 0;
			}
		}
		if(downEvent == Button::Event::SHORT ||
		   downEvent == Button::Event::REPEAT)
		{
			if(dashboard[selectedItem].type == ItemType::FLOAT)
				(*(float*)dashboard[selectedItem].value) -= 0.05f;
			else if(dashboard[selectedItem].type == ItemType::INT)
				(*(int*)dashboard[selectedItem].value)--;
			else if(dashboard[selectedItem].type == ItemType::OPTION)
			{
				OptionItem *opt = dashboard[selectedItem].option;
				if(*opt->index == 0)
					*opt->index = opt->count - 1;
				else
					(*opt->index)--;
			}
		}
		if(okEvent == Button::Event::SHORT)
			editing = false;
            SaveSettings();
		return;
	}
	if(upEvent == Button::Event::SHORT ||
	   upEvent == Button::Event::REPEAT)
	{
		if(selectedItem == 0)
			selectedItem = ITEM_COUNT - 1;
		else
			selectedItem--;
	}
	if(downEvent == Button::Event::SHORT ||
	   downEvent == Button::Event::REPEAT)
	{
		selectedItem++;
		if(selectedItem >= ITEM_COUNT)
			selectedItem = 0;
	}
	if(selectedItem < topItem)
		topItem = selectedItem;
	if(selectedItem >= topItem + visibleItems)
		topItem = selectedItem - visibleItems + 1;
	if(selectedItem == 0)
		topItem = 0;
	if(selectedItem == ITEM_COUNT - 1 && ITEM_COUNT > visibleItems)
		topItem = ITEM_COUNT - visibleItems;
	if(okEvent == Button::Event::SHORT)
	{
		switch(dashboard[selectedItem].type)
		{
			case ItemType::FLOAT:
			case ItemType::INT:
			case ItemType::OPTION:
				editing = true;
				break;
			case ItemType::ACTION:
				currentScreen = dashboard[selectedItem].nextScreen;
				break;
			case ItemType::INFO:
				break;
		}
	}
}


