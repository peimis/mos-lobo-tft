//
//
//
#ifndef	TFT_BUTTON_H
#define	TFT_BUTTON_H

typedef void (*button_cb)(bool state);

typedef struct button {
	double		time;
	button_cb	cb;
	int			x;		// Top left
	int			y;		// Top left
	uint16_t	w;		// Width
	uint16_t	h;		// Height
	uint16_t	r;		// Round radius
	const color_t	*outlinecolor;
	const color_t	*fillcolor;
	const color_t	*textcolor;
	int16_t		font;
	uint8_t		textsize;
	const char 	*label;
	char		*image;
	bool  		currstate;
	bool		laststate;
} button_t;

struct tft_button_cb
{
	button_t *button;
	SLIST_ENTRY(tft_button_cb) entries;
};

struct tft_button_list
{
	int count;
	SLIST_HEAD(tft_buttons, tft_button_cb) tft_buttons;
};


void TFT_Button_init( button_t * const button, const int x, const int y, const int w, const int h);
void TFT_Button_draw( const button_t * const  button, const bool inverted );
bool TFT_Button_contains( const button_t * const button, const uint16_t x, const uint16_t y);
void TFT_Button_press( button_t *button, bool p);
bool TFT_Button_isPressed(const button_t * const b);
bool TFT_Button_justPressed(const button_t * const b);
bool TFT_Button_justReleased(const button_t * const b);

void TFT_checkButtons(const uint16_t x, const uint16_t y);
int TFT_Button_add_onEvent( button_t * const b, button_cb fn);

#endif
