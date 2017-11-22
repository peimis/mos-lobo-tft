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



struct tft_button_list b_list;
int bid=0;
mgos_timer_id TFT_Touch_read_timer_id;

// TFT_Button_init() function: upper left corner & size
//
int TFT_Button_init( button_t * const b, const int x, const int y, const int w, const int h )
{
	b->currstate = 0;
	b->laststate = 0;

	b->x = x;
	b->y = y;
	b->w = w;
	b->h = h;

	b->r = 3;

	b->image = NULL;
	b->label = NULL;
	b->cb = NULL;
	b->font = DEFAULT_FONT;

	b->outlinecolor = &TFT_DARKGREY;
	b->fillcolor = &TFT_LIGHTGREY;
	b->textcolor = &TFT_NAVY;

	b->id = ++bid;

	return b->id;
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
		TFT_fillRect(b->x, b->y, b->w, b->h, fill);
		TFT_drawRect(b->x, b->y, b->w, b->h, outline);
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
//	printf("%d:%d ?  %d:%d .. %d:%d\n", x, y, b->x,b->y, b->x+b->w, b->y+b->h);
	return ((x >= b->x) && (x < (b->x + b->w)) && (y >= b->y) && (y < (b->y + b->h)));
}


//
//
void TFT_Button_press(button_t *b, bool p)
{
	b->time = mg_time();

	b->laststate = b->currstate;
	b->currstate = p;
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


//
//
int TFT_Button_add_onEvent( button_t * const b, button_cb fn)
{
	struct tft_button_cb *tb_cb = (struct tft_button_cb *) calloc(1, sizeof(*tb_cb));

	if (tb_cb) {
		tb_cb->button = b;
		b->cb = fn;

		SLIST_INSERT_HEAD(&b_list.tft_buttons, tb_cb, entries);
	}

	return 0;
}


//
//
void TFT_Buttons_refresh(const int state, const int x, const int y)
{
	struct tft_button_cb *tb_cb;
	button_t *b;

	SLIST_FOREACH(tb_cb, &b_list.tft_buttons, entries)
	{
		b = tb_cb->button;
		if (state && TFT_Button_contains(b, x, y))
		{
			TFT_Button_press(b, 1);
			if (TFT_Button_justPressed(b)) {
				if (b->cb) {
					b->cb(1, b);
				}
			}
		}
		else
		{
			TFT_Button_press(b, 0);
			if (TFT_Button_justReleased(b)) {
				if (b->cb) {
					b->cb(0, b);
				}
			}
		}
	}
}


//
//
void TFT_Touch_read_timer_cb(void *arg)
{
	int tx, ty;
	bool touch_state = TFT_read_touch(&tx, &ty, false);

	if (touch_state) {
		TFT_drawCircle(tx, ty, 4, TFT_MAGENTA);
	}

	TFT_Buttons_refresh(touch_state, tx, ty);
}


//
//
void TFT_Touch_intr_handler(const int pin, void *arg)
{
	const bool pin_state = mgos_gpio_read(pin);
	int touch_state = 0;
	int tx=0, ty=0;
	(void)arg;

	if (!pin_state)
	{
		if (!TFT_Touch_read_timer_id) {
			TFT_Touch_read_timer_id = mgos_set_timer(200, 1, TFT_Touch_read_timer_cb, NULL);
		}

		if ((touch_state = TFT_read_touch(&tx, &ty, false))) {
			TFT_drawCircle(tx, ty, 4, TFT_MAGENTA);
		}
	}
	else
	{
		if (TFT_Touch_read_timer_id) {
			mgos_clear_timer(TFT_Touch_read_timer_id);
			TFT_Touch_read_timer_id = 0;
		}
	}
	TFT_Buttons_refresh(touch_state, tx, ty);
}


//
//
bool TFT_Touch_intr_init(void)
{
	const int pin = mgos_sys_config_get_tft_t_irq_pin();

	if (-1 != pin)
	{
		LOG(LL_INFO, ("Set TFT touch intr handler for pin '%d'", pin));
		mgos_gpio_set_int_handler(pin, MGOS_GPIO_INT_EDGE_ANY, TFT_Touch_intr_handler, NULL);
		mgos_gpio_enable_int(pin);
	}
	return true;
}
