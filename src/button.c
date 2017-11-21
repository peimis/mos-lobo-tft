#include <stdio.h>
#include <time.h>

#include "common/platform.h"
#include "common/cs_file.h"
#include "fw/src/mgos_app.h"
#include "fw/src/mgos_gpio.h"
#include "fw/src/mgos_sys_config.h"
#include "fw/src/mgos_timers.h"
#include "fw/src/mgos_hal.h"
#include "fw/src/mgos_dlsym.h"
#include "fw/src/mgos_sntp.h"

#include "mgos.h"

#include "tftspi.h"
#include "tft.h"

#include "button.h"

/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_GFX library and enhanced to handle any label font
***************************************************************************************/

// TFT_Button_init() function: upper left corner & size
//
void TFT_Button_init( button_t * const b, const int x, const int y, const int w, const int h )
{
	b->currstate = 0;
	b->laststate = 0;
	b->x = x;
	b->y = y;
	b->w = w;
	b->h = h;
}



//
//
void TFT_Button_draw( const button_t * const  button, const bool inverted )
{
	color_t fill, outline, text;
	outline = *button->outlinecolor;

	if(!inverted) {
		fill    = *button->fillcolor;
		text    = *button->textcolor;
	} else {
		text    = *button->fillcolor;
		fill    = *button->textcolor;
	}

  	if (button->r)
  	{
		TFT_fillRoundRect(button->x, button->y, button->w, button->h, button->r, fill);
		TFT_drawRoundRect(button->x, button->y, button->w, button->h, button->r, outline);
  	}
	else
	{
  		TFT_fillRect(button->x, button->y, button->w, button->h, outline);
	}


	TFT_set_fg(&text);
	TFT_setFont(button->font, NULL);
	TFT_print(button->label, button->x, button->y);
}


//
//
bool TFT_Button_contains( const button_t * const b, const uint16_t x, const uint16_t y)
{
  return ((x >= b->x) && (x < (b->x + b->w)) &&
          (y >= b->y) && (y < (b->y + b->h)));
}


//
//
void TFT_Button_press(button_t *b, bool p)
{
	b->time = mg_time();
	b->laststate = b->currstate;
	b->currstate = p;
	if (b->cb) {
		b->cb(p);		
	}
}

bool TFT_Button_isPressed(const button_t * const b)
{
	return b->currstate; 
}

bool TFT_Button_justPressed(const button_t * const b)
{
	return (b->currstate && !b->laststate);
}

bool TFT_Button_justReleased(const button_t * const b)
{
	return (!b->currstate && b->laststate);
}
