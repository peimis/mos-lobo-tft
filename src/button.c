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
void TFT_Button_draw( const button_t * const b, const bool inverted )
{
	color_t fill, outline, text;
	outline = *b->outlinecolor;

	if(!inverted) {
		fill    = *b->fillcolor;
		text    = *b->textcolor;
	} else {
		text    = *b->fillcolor;
		fill    = *b->textcolor;
	}

	if (b->r)
  	{
		TFT_fillRoundRect(b->x, b->y, b->w, b->h, b->r, fill);
		TFT_drawRoundRect(b->x, b->y, b->w, b->h, b->r, outline);
  	}
	else
	{
		TFT_fillRect(b->x, b->y, b->w, b->h, outline);
	}


	TFT_setFont(b->font, NULL);
	const int label_w = TFT_getStringWidth(b->label);
	const int label_h = TFT_getfontheight();

	TFT_set_fg(&text);
	TFT_set_bg(&fill);

	TFT_print(b->label, b->x + b->w/2 - label_w/2, b->y + b->h/2 - label_h / 2);
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
