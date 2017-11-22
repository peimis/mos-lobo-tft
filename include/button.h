//
//
//
#ifndef	TFT_BUTTON_H
#define	TFT_BUTTON_H

typedef void (*button_cb)(bool state, void *b);

typedef struct button {
	int			id;
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



/*
 * Initializes a basic button parameters.
 *
 * Params:
 *  button: Pointer to button structure
 *      x : Top right corner.
 *      y : Top right corner.
 *      w : Width of button.
 *      h : Height of button.
 */
//-------------------------------------------------------------------------------------
int TFT_Button_init( button_t * const button, const int x, const int y, const int w, const int h);


/*
 * Draws a button to screen.
 *
 * Params:
 *    button: Pointer to button structure
 *  inverted: Swap fill and text colors.
 *
 */
//-------------------------------------------------------------------------------------
void TFT_Button_draw( const button_t * const  button, const bool inverted );


/*
 * Check if given coordinates are inside a button.
 *
 * Params:
 *    button: Pointer to button structure.
 *         x: X-coordinate to check.
 *         y: Y-coordinate to check.
 *
 * Returns:
 *     true if coordinates are inside of the given button, false otherwise.
 */
//-------------------------------------------------------------------------------------
bool TFT_Button_contains( const button_t * const button, const uint16_t x, const uint16_t y);


/*
 * Update the button state.
 *
 * Params:
 *    button: Pointer to button structure.
 *         p: true if button is pressed.
 *
 * Returns:
 *     None.
 */
//-------------------------------------------------------------------------------------
void TFT_Button_press( button_t *button, bool p);

bool TFT_Button_isPressed(const button_t * const b);
bool TFT_Button_justPressed(const button_t * const b);
bool TFT_Button_justReleased(const button_t * const b);

/*
 * Refresh the state of all buttons added by TFT_Button_add_onEvent().
 *
 * Params:
 *    state: true if touchscreen is touched at the moment.
 *        x: Current touch X-coordinate.
 *        y: Current touch Y-coordinate.
 *
 * Returns:
 *     None.
 */
//-------------------------------------------------------------------------------------
void TFT_Buttons_refresh(const int state, const int x, const int y);

int TFT_Button_add_onEvent( button_t * const b, button_cb fn);
bool TFT_Touch_intr_init(void);

#endif
