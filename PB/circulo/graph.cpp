// FILE: winbgim.cpp
// Version 3.4, December 21, 1999
// This is Konstantin Knizhnik's implementation of Borland's BGI graphics
// to run under Borland C++ 5.02 for Windows, with some mouse additions
// and modifications by Michael Main.
//
// For documentation and modification history please see:
// http://www.cs.colorado.edu/~main/bgi/README.html
// http://www.cs.colorado.edu/~main/bgi/winbgim.h

#include <windows.h>
#include <gdiplus.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream> // put in by MR for error messages 11/30/98
#include <math.h>
#include "graph.h"
#include <gdiplus.h>


#ifndef random
#define random(range) (rand() % (range))
#endif

// Version 3.2:
// Macro to convert one of my RGB colors to a standard RGB color.
// In my RGB colors, the first byte is 0x10 so that it can be
// easily distinguished from the earlier BGI colors.
#define RGB_COLOR(c) (c & 0xFFFFFF)

#define MAX_PAGES 4

static HDC hdc[4];
static HPEN hPen;
static HRGN hRgn;
static HFONT hFont;
static NPLOGPALETTE pPalette;
static PAINTSTRUCT ps;
static HWND hWnd;
static HBRUSH hBrush[USER_FILL+1];
static HBRUSH hBackgroundBrush;
static HPALETTE hPalette;
static HBITMAP hBitmap[MAX_PAGES];
static HBITMAP hPutimageBitmap;
static BITMAPINFO hBitMapInfo={{sizeof(BITMAPINFOHEADER),1,1,1,32,BI_RGB,4}};
       // first parameter is bitmapinfoheader size
       // second parameter is bitmap image width
       // third parameter is bitmap image height
       // fourth is planes of image
       // fifth is compression (BI_RGB is no compression)
       // sixth is image size (number of pixels * 4 bytes per pixel)
static int timeout_expired;

#define PEN_CACHE_SIZE   8
#define FONT_CACHE_SIZE  8
#define BG               16
#define TIMER_ID         1

// Constants added for the key numbers of the key pad extended keys.
// These key numbers appear in the wParam parameter to the window event
// handler with a WM_KEYDOWN event:
// M. Main -- Nov 3, 1998
#define NUMBER_HOME       36
#define NUMBER_UP         38
#define NUMBER_PGUP       33
#define NUMBER_LEFT       37
#define NUMBER_CENTER     12
#define NUMBER_RIGHT      39
#define NUMBER_END        35
#define NUMBER_DOWN       40
#define NUMBER_PGDN       34
#define NUMBER_INSERT     45
#define NUMBER_DELETE     46
#define FUNCTION_F1		  112
#define FUNCTION_F2		  113
#define FUNCTION_F3		  114
#define FUNCTION_F4		  115
#define FUNCTION_F5		  116
#define FUNCTION_F6		  117
#define FUNCTION_F7		  118
#define FUNCTION_F8		  119
#define FUNCTION_F9		  120
#define FUNCTION_F10		  121
#define FUNCTION_F11		  122
#define FUNCTION_F12		  123

// Static variables added for the mouseclick functions:
#define NO_CLICK -1


ULONG_PTR gdiplusToken;


static int x_clicks[WM_MOUSELAST - WM_MOUSEFIRST + 1];
static int y_clicks[WM_MOUSELAST - WM_MOUSEFIRST + 1];
static bool click_ready[WM_MOUSELAST - WM_MOUSEFIRST + 1];

// Original Note:
// When XOR or NOT write modes are used for drawing high BG bit is cleared, so
// drawing colors should be adjusted to preserve this bit.
// Notes from MGM:
// I think understand what this note says. For example, suppse that we are
// drawing with BGI color number 1, which is actually at BG+1 = 17.
// The bit-pattern for this color is 00010001. Suppose that this is
// XORed with a color that is already on the screen. That color might
// be BGI color number 2, which is actually at BG+2 = 18 and has a
// bit pattern of 00010010. In both cases, the leading four bits 0001
// are an artifact of the offset of the index numbers in the palette.
// After the XOR we want this bit to remain on, resulting in
// 00010011. But..., I don't think that the XOR mode is implemented
// correctly. The implementation checks whether we have one of these
// ADJUSTED MODES, and if so, it uses the original BGI color number
// instead of BG+... So, I'm going to take out that programming.
#define ADJUSTED_MODE(mode) ((mode) == XOR_PUT || (mode) == NOT_PUT)

int bgiemu_handle_redraw = TRUE;
int bgiemu_default_mode = VGAHI; //VGAMAX;
static int screen_width;
static int screen_height;
static int window_width;
static int window_height;
static int line_style_cnv[] = {
    PS_SOLID, PS_DOT, PS_DASHDOT, PS_DASH,
    PS_DASHDOTDOT /* if user style lines are not supported */
};
static int write_mode_cnv[] =
{R2_COPYPEN, R2_XORPEN, R2_MERGEPEN, R2_MASKPEN, R2_NOTCOPYPEN};
static int bitblt_mode_cnv[] =
{SRCCOPY, SRCINVERT, SRCPAINT, SRCAND, NOTSRCCOPY};
static int font_weight[] =
{
    FW_BOLD,    // DefaultFont
    FW_NORMAL,  // TriplexFont
    FW_NORMAL,  // SmallFont
    FW_NORMAL,  // SansSerifFont
    FW_NORMAL,  // GothicFont
    FW_NORMAL,  // ScriptFont
    FW_NORMAL,  // SimplexFont
    FW_NORMAL,  // TriplexScriptFont
    FW_NORMAL,  // ComplexFont
    FW_NORMAL,  // EuropeanFont
    FW_BOLD     // BoldFont
};
static int font_family[] =
{
    FIXED_PITCH|FF_DONTCARE,     // DefaultFont
    VARIABLE_PITCH|FF_ROMAN,     // TriplexFont
    VARIABLE_PITCH|FF_MODERN,    // SmallFont
    VARIABLE_PITCH|FF_DONTCARE,  // SansSerifFont
    VARIABLE_PITCH|FF_SWISS,     // GothicFont
    VARIABLE_PITCH|FF_SCRIPT,    // ScriptFont
    VARIABLE_PITCH|FF_DONTCARE,  // SimplexFont
    VARIABLE_PITCH|FF_SCRIPT,    // TriplexScriptFont
    VARIABLE_PITCH|FF_DONTCARE,  // ComplexFont
    VARIABLE_PITCH|FF_DONTCARE,  // EuropeanFont
    VARIABLE_PITCH|FF_DONTCARE   // BoldFont
};

static char* font_name[] =
{
    "Console",          // DefaultFont
    "Times New Roman",  // TriplexFont
    "Small Fonts",      // SmallFont
    "Microsoft Sans Serif",    // SansSerifFont
    "Algerian",         // GothicFont
    "Script",           // ScriptFont
    "Verdana",  		// SimplexFont
    "Mistral",          // TriplexScriptFont
    "Courier New",      // ComplexFont
    "Arial",  			// EuropeanFont
    "Arial Black", // BoldFont
};

static int text_halign_cnv[] = {TA_LEFT, TA_CENTER, TA_RIGHT};
static int text_valign_cnv[] = {TA_BOTTOM, TA_BASELINE, TA_TOP};
static palettetype current_palette;
static struct { int width; int height; } font_metrics[][11] = {
    {{0,0},{8,8},{11,11},{18,18},{26,26},{38,38},{48,48},{56,56},{64,64},{72,72},{80,80}}, // DefaultFont
    {{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, // TriplexFont
    {{0,0},{3,5},{4,6},{4,6},{6,9},{8,12},{10,15},{12,18},{15,22},{18,27},{24,36}}, // SmallFont
    {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // SansSerifFont
    {{0,0},{13,19},{14,21},{16,24},{22,32},{29,42},{36,53},{44,64},{55,80},{66,96},{88,128}}, // GothicFont
// I am not sure about metrics of following fonts
    {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // ScriptFont
    {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // SimplexFont
    {{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, // TriplexScriptFont
    {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // ComplexFont
    //{{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // EuropeanFont
    {{0,0},{8,10},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, // EuropeanFont
    {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}} // BoldFont
};

//struct BGIbitmapinfo {
//    BITMAPINFOHEADER hdr;
//    short            color_table[16];
//};
//static BGIbitmapinfo bminfo = {
//    {sizeof(BITMAPINFOHEADER), 0, 0, 1, 4, BI_RGB}
//};
// The following image_bits isn't used anywhere, but I am reluctant to
// remove it because perhaps the original programmer counts on this
// pointer being in memory after bminfo. Therefore, to get rid of the
// compiler warning, I will later put some reference to image_bits;
// M. Main -- Nov 4, 1998.
static int* image_bits = 0;

// Version 3.2:
// The next three int variables may now be a BGI color (0...MAXCOLORS) or
// an RGI color. Note that the windows_color has dual purpose: it is both
// the color in which Windows will print text and it is the color
// which Windows functions (such as Ellipse) use as the fill color.
// This is because the windows SetTextColor uses its color for filling
// and for text. However, we want our text to print in the drawing
// color (stored in color), and we want things to be filled in
// the fill color (stored in fill_settings.color), so when we output text
// or draw a filled shape, we check to see if we need to call SetTextColor.
static int windows_color;  // Color that Windows plans to use for text & filling.
static int color;          // BGI's color for drawing lines and text.
static int bkcolor;        // Background color
static fillsettingstype fill_settings;
static linesettingstype line_settings;
static int normal_font_size[] = { 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

static int aspect_ratio_x, aspect_ratio_y;
static textsettingstype text_settings;
static viewporttype view_settings;
static int font_mul_x, font_div_x, font_mul_y, font_div_y;
static enum { ALIGN_NOT_SET, UPDATE_CP, NOT_UPDATE_CP } text_align_mode;

//Mouse handler static variables added by Michael Main, Oct 16, 1998.
static int current_mouse_x = 0;
static int current_mouse_y = 0;
typedef void (*Handler)(int,int);
static Handler mouse_handlers[WM_MOUSELAST - WM_MOUSEFIRST + 1] = { 0 };
#define BORDER_WIDTH  8
#define BORDER_HEIGHT 27
static int write_mode;
static int visual_page;
static int active_page;
static arccoordstype ac;

class char_queue {
protected:
    char* buf;
    int   put_pos;
    int   get_pos;
    int   buf_size;
public:
    void  put(char ch)
	{
	    buf[put_pos] = ch;
	    if (++put_pos == buf_size)
		put_pos = 0;

	    if (put_pos == get_pos) // queue is full
		(void)get(); // loose one character
	}

    char get()
	{
	    char ch = buf[get_pos];
	    if (++get_pos == buf_size)
		get_pos = 0;

	    return ch;
	}

    bool is_empty()
	{
	    return get_pos == put_pos;
	}

    char_queue(int buf_size = 256)
	{
	    put_pos = get_pos = 0;
	    this->buf_size = buf_size;
	    buf = new char[buf_size];
	}

    ~char_queue()
	{
	    delete[] buf;
	}
};

static char_queue kbd_queue;
int convert_userbits(DWORD buf[32], unsigned pattern)
{
    int i = 0, j;
    pattern &= 0xFFFF;
    while (true)
    {
   	for (j = 0; pattern & 1; j++) pattern >>= 1;
      	buf[i++] = j;

	if (pattern == 0)
	{
	    buf[i++] = 16 - j;
	    return i;
	}

	for (j = 0; !(pattern & 1); j++) pattern >>= 1;
      	buf[i++] = j;
    }
}

class l2elem {
public:
    l2elem* next;
    l2elem* prev;
    void link_after(l2elem* after)
	{
	    (next = after->next)->prev = this;
	    (prev = after)->next = this;
    	}

    void unlink()
	{
	    prev->next = next;
	    next->prev = prev;
	}

    void prune()
	{
	    next = prev = this;
	}
};

class l2list : public l2elem
{
public:
    l2list() { prune(); }
};

class pen_cache : public l2list
{
    class pen_cache_item : public l2elem
    {
    public:
      	HPEN pen;
	int  color;
	int  width;
	int  style;
	unsigned pattern;
    };
    pen_cache_item* free;
    pen_cache_item  cache[PEN_CACHE_SIZE];

public:
    pen_cache();
    void select(int color);
};

// Moved the two pen_cache methods outside of the class definition to avoid
// warnings about inline functions. M. Main -- Nov 3, 1998
pen_cache::pen_cache()
{
    // Creates a linked list of pen_cache_items, all of which are
    // currently not used.
    for (int i = 0; i < PEN_CACHE_SIZE-1; i++)
   	cache[i].next = &cache[i+1];

    cache[PEN_CACHE_SIZE-1].next = NULL;
    free = cache;
}

void pen_cache::select(int color)
{
    // Version 3.2:
    // The original version required color to be an index into the
    // palette, that has already been adjusted by adding BG.
    // I have changed this so that the color can now be an original
    // BGI color (in the range 0..MAXCOLORS) or one of the new RGB colors.
    for (l2elem* elem = next; elem != this; elem = elem->next)
    {
   	pen_cache_item* ci = (pen_cache_item*)elem;
	if (ci->color == color &&
	    ci->style == line_settings.linestyle &&
            ci->width == line_settings.thickness &&
            (line_settings.linestyle != USERBIT_LINE
	     || line_settings.upattern == ci->pattern))
	{
	    ci->unlink(); // LRU discipline
	    ci->link_after(this);
	    if (hPen != ci->pen)
	    {
         	hPen = ci->pen;
		SelectObject(hdc[0], hPen);
		SelectObject(hdc[1], hPen);
	    }

	    return;
	}
    }
    hPen = NULL;
    if (line_settings.linestyle == USERBIT_LINE)
    {
   	LOGBRUSH lb;
	lb.lbColor = (IS_RGB_COLOR(color)) ? RGB_COLOR(color) : PALETTEINDEX(BG+color);
	lb.lbStyle = BS_SOLID;
	DWORD style[32];
	hPen = ExtCreatePen(PS_GEOMETRIC|PS_USERSTYLE,
			    line_settings.thickness, &lb,
			    convert_userbits(style,line_settings.upattern),
			    style);

    }

    if (hPen == NULL)
    {
   	hPen = CreatePen(line_style_cnv[line_settings.linestyle],
			 line_settings.thickness,
			 IS_RGB_COLOR(color) ? RGB_COLOR(color) : PALETTEINDEX(BG+color));
    }
    SelectObject(hdc[0], hPen);
    SelectObject(hdc[1], hPen);
    pen_cache_item* p;

    if (free == NULL)
    {
   	p = (pen_cache_item*)prev;
	p->unlink();
	DeleteObject(p->pen);
    }
    else
    {
   	p = free;
	free = (pen_cache_item*)p->next;
    }

    p->pen   = hPen;
    p->color = color;
    p->width = line_settings.thickness;
    p->style = line_settings.linestyle;
    p->pattern = line_settings.upattern;
    p->link_after(this);
}

static pen_cache pcache;

// Moved the two font_cache methods outside of the class definition to avoid
// warnings about inline functions. M. Main -- Nov 3, 1998
class font_cache : public l2list
{
    class font_cache_item : public l2elem
    {
    public:
      	HFONT font;
	int   type;
	int   direction;
	int   width, height;
    };
    font_cache_item* free;
    font_cache_item  cache[FONT_CACHE_SIZE];
public:
    font_cache( );
    void select(int type, int direction, int width, int height);
};

font_cache::font_cache()
{
    for (int i = 0; i < FONT_CACHE_SIZE-1; i++)
   	cache[i].next = &cache[i+1];

    cache[FONT_CACHE_SIZE-1].next = NULL;
    free = cache;
}

void font_cache::select(int type, int direction, int width, int height)
{
    for (l2elem* elem = next; elem != this; elem = elem->next)
    {
   	font_cache_item* ci = (font_cache_item*)elem;
	if (ci->type == type && ci->direction == direction &&
	    ci->width == width && ci->height == height)
	{
	    ci->unlink();
	    ci->link_after(this);
	    if (hFont != ci->font)
	    {
         	hFont = ci->font;
		SelectObject(hdc[0], hFont);
		SelectObject(hdc[1], hFont);
	    }
	    return;
	}
    }
    hFont = CreateFont(-height,width,direction*900,(direction&1)*900,
		       font_weight[type],FALSE,FALSE,FALSE,DEFAULT_CHARSET,
		       OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		       font_family[type],font_name[type]);

    SelectObject(hdc[0], hFont);
    SelectObject(hdc[1], hFont);

    font_cache_item* p;
    if (free == NULL)
    {
   	p = (font_cache_item*)prev;
	p->unlink();
	DeleteObject(p->font);
    }
    else
    {
   	p = free;
	free = (font_cache_item*)p->next;
    }
    p->font = hFont;
    p->type = type;
    p->width = width;
    p->height = height;
    p->direction = direction;
    p->link_after(this);
}

static font_cache fcache;

// Version 3.2: I've changed the FLAGS to PC_RESERVED, as specified in
// http://msdn.microsoft.com/library/techart/msdn_palette.htm.
// Elsewhere in the code, I've also changed 
// #define FLAGS         PC_NOCOLLAPSE
#define FLAGS            PC_RESERVED

#define PALETTE_SIZE  256

static PALETTEENTRY BGIcolor[16] = {
    { 0, 0, 0, FLAGS },
    { 0, 0, 255, FLAGS },
    { 0, 255, 0, FLAGS },
    { 0, 255, 255, FLAGS },
    { 255, 0, 0, FLAGS },
    { 255, 0, 255, FLAGS },
    { 165, 42, 42, FLAGS },
    { 211, 211, 211, FLAGS },
    { 47, 79, 79, FLAGS },
    { 173, 216, 230, FLAGS },
    { 32, 178, 170, FLAGS },
    { 224, 255, 255, FLAGS },
    { 240, 128, 128, FLAGS },
    { 219, 112, 147, FLAGS },
    { 255, 255, 0, FLAGS },
    { 255, 255, 255, FLAGS }
};

static  PALETTEENTRY BGIpalette[16];
static short SolidBrushBitmap[8] =
{~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF, ~0xFF};
static short LineBrushBitmap[8] =
{~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0xFF};
static short LtslashBrushBitmap[8] =
{~0x01, ~0x02, ~0x04, ~0x08, ~0x10, ~0x20, ~0x40, ~0x80};
static short SlashBrushBitmap[8] =
{~0x81, ~0x03, ~0x06, ~0x0C, ~0x18, ~0x30, ~0x60, ~0xC0};
static short BkslashBrushBitmap[8] =
{~0xC0, ~0x60, ~0x30, ~0x18, ~0x0C, ~0x06, ~0x03, ~0x81};
static short LtbkslashBrushBitmap[8] =
{~0x80, ~0x40, ~0x20, ~0x10, ~0x08, ~0x04, ~0x02, ~0x01};
static short HatchBrushBitmap[8] =
{~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0x01, ~0xFF};
static short XhatchBrushBitmap[8] =
{~0x81, ~0x42, ~0x24, ~0x18, ~0x18, ~0x24, ~0x42, ~0x81};
static short InterleaveBrushBitmap[8] =
{~0x55, ~0xAA, ~0x55, ~0xAA, ~0x55, ~0xAA, ~0x55, ~0xAA};
static short WidedotBrushBitmap[8] =
{~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x01};
static short ClosedotBrushBitmap[8] =
{~0x44, ~0x00, ~0x11, ~0x00, ~0x44, ~0x00, ~0x11, ~0x00};

char* grapherrormsg(int code)
{
    static char buf[256];

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL, code, 0,
		  buf, sizeof buf, NULL);

    return buf;
}

static int gdi_error_code;
int graphresult()
{
    return gdi_error_code;
}

void setcolor(int c)
{
    // Version 3.2:
    // Parameter c may now be original bgi color or new rgb color.
    // Version 3.4:
    // Fixed bug that forgot to set windows_color to c.
    if (IS_RGB_COLOR(c))
    {
	color = windows_color = c;
	SetTextColor(hdc[0], RGB_COLOR(c));
	SetTextColor(hdc[1], RGB_COLOR(c));
    }
    else if (IS_BGI_COLOR(c))
    {
	color = windows_color = c;
	SetTextColor(hdc[0], PALETTEINDEX(c+BG));
	SetTextColor(hdc[1], PALETTEINDEX(c+BG));
    }
}

int getmaxcolor()
{
    return 15;//WHITE;
}

int getmaxmode()
{
    return VGAMAX;
}

char* getmodename(int mode)
{
    static char mode_str[32];

    sprintf(mode_str, "%d x %d %s", window_width, window_height,
	    mode < 2 ? "EGA" : "VGA");

    return mode_str;
}

int getx()
{
    POINT pos;
    GetCurrentPositionEx(hdc[active_page == visual_page ? 0 : 1], &pos);

    return pos.x;
}

int gety()
{
    POINT pos;
    GetCurrentPositionEx(hdc[active_page == visual_page ? 0 : 1], &pos);

    return pos.y;
}

int getmaxx()
{
    return window_width-1;
}

int getmaxy()
{
    return window_height-1;
}

int getcolor()
{
    // Version 3.2: This return value may now be either a BGI color
    // (from 0...MAXCOLORS) or a bgi color.
    return color;
}

char* getdrivername()
{
    return "EGAVGA";
}

void setlinestyle(int style, unsigned int pattern, int thickness)
{
    line_settings.linestyle = style;
    line_settings.thickness = thickness;
    line_settings.upattern  = pattern;
}

void getlinesettings(linesettingstype* ls)
{
    *ls = line_settings;
}

void setwritemode(int mode)
{
    if ((COPY_PUT <= mode) && (mode <= NOT_PUT))
	write_mode = mode;
}

// The following function added to correct the rgb colors stored in an entry of
// BGIpalette. This is needed because the screen might not be capable of
// displaying the exact color requested. Call correctpalette(i) after each
// change to BGIpalette[i].
// -- M. Main 11/17/98
static void correctpalette(int index)
{
    COLORREF rgb;
    COLORREF rgb_at00;

    int red, blue, green;
    // Save the color currently at location 0,0
    rgb_at00 = GetPixel(hdc[visual_page != active_page ||
			   bgiemu_handle_redraw ? 1 : 0], 0, 0);

    // Change the color at location 0,0 to color number i from the palette.
    putpixel(0,0,index);

    // Get the rgb components of the pixel at 0,0 and put that into the palette.
    rgb = GetPixel(hdc[visual_page != active_page ||
		      bgiemu_handle_redraw ? 1 : 0], 0, 0);

    if (rgb != CLR_INVALID)
    {
	red = GetRValue(rgb);
	blue = GetBValue(rgb);
	green = GetGValue(rgb);
	BGIpalette[index].peRed = (BYTE) (red & 0xFF);
	BGIpalette[index].peGreen = (BYTE) (green & 0xFF);
	BGIpalette[index].peBlue = (BYTE) (blue & 0xFF);

	SetPaletteEntries(hPalette, BG+index, 1, &BGIpalette[index]);
	RealizePalette(hdc[0]);
    }

    // Restore the original color to location 0,0:
    if (bgiemu_handle_redraw || visual_page != active_page)
    	SetPixel(hdc[1], 0, 0, rgb);

    if (visual_page == active_page)
    	SetPixel(hdc[0], 0, 0, rgb_at00);
}

void setpalette(int index, int color)
{
    // Version 3.2: Added test of precondition. Notice that we are only allowed
    // to use this function to change one of the 0..MAXCOLORS locations
    // of the bgi palette to one of the 15 original bgi colors or to an rgb color.

    // Note: The behavior differs from original BGI in that colors already on
    // the screen are not changed by changing the palette, so it is not a
    // real palette.
    
    if ((index < 0) || (index > MAXCOLORS))
	return;

    if (IS_BGI_COLOR(color))
    {   // color is one of the original 15 bgi colors
	BGIpalette[index] = BGIcolor[color];
	current_palette.colors[index] = (signed char) color;
    }
    else
    {   // color is an RGB color constructed with RGB_COLOR macro
	BGIpalette[index].peRed = RED_VALUE(color);
	BGIpalette[index].peGreen = GREEN_VALUE(color);
	BGIpalette[index].peBlue = BLUE_VALUE(color);
	current_palette.colors[index] = (signed char) -1;
    }

    SetPaletteEntries(hPalette, BG+index, 1, &BGIpalette[index]);
    RealizePalette(hdc[0]);

    correctpalette(index); // M. Main, Nov 17, 1998.

    if (index == bkcolor)
    {
	SetBkColor(hdc[0], PALETTEINDEX(BG+bkcolor));
	SetBkColor(hdc[1], PALETTEINDEX(BG+bkcolor));
	DeleteObject(hBackgroundBrush);
        hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG+bkcolor));
    }	    
}

void setrgbpalette(int index, int red, int green, int blue)
{
    // Version 3.2: Added test of precondition. Notice that we are only allowed
    // to use this function to change one of the 0..MAXCOLORS locations
    // of the bgi palette to an explicit rgb color.
    // The rgb values use only the most significant 6 bits of the least significant
    // byte of the int parameters.

    // Note: The behavior differs from original BGI in that colors already on
    // the screen are not changed by changing the palette, so it is not a
    // real palette.

    if ((index < 0) || (index > MAXCOLORS))
	return;

    // Mask RGB values to use only most significant 6 bits from least significant byte:
    BGIpalette[index].peRed = (red & 0xFC);
    BGIpalette[index].peGreen = (green & 0xFC);
    BGIpalette[index].peBlue = (blue & 0xFC);

    current_palette.colors[index] = (signed char) -1;
    
    SetPaletteEntries(hPalette, BG+index, 1, &BGIpalette[index]);
    RealizePalette(hdc[0]);

    correctpalette(index); // M. Main, Nov 17, 1998.

    if (index == bkcolor)
    {
	SetBkColor(hdc[0], PALETTEINDEX(BG+bkcolor));
	SetBkColor(hdc[1], PALETTEINDEX(BG+bkcolor));
	DeleteObject(hBackgroundBrush);
        hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG+bkcolor));
    }	    
}

void setallpalette(palettetype* pal)
{
    // Version 3.2: Notice that we are only allowed
    // to use this function to change some or all of the 0..MAXCOLORS
    // locations of the bgi palette to specific values from original
    // bgi colors.

    // Note: The behavior differs from original BGI in that colors already on
    // the screen are not changed by changing the palette, so it is not a
    // real palette.

    signed char color;
    
    for (int i = 0; (i < pal->size) && (i <= MAXCOLORS); i++)
    {
	color = pal->colors[i];
	if (IS_BGI_COLOR(color))
	{
	    current_palette.colors[i] = color;
	    BGIpalette[i] = BGIcolor[color];
	}
    }

    SetPaletteEntries(hPalette, BG, pal->size, BGIpalette);
    RealizePalette(hdc[0]);

    // Following loop added, M. Main, Nov 17, 1998
    for (int j = 0; (j < pal->size) && (j <= MAXCOLORS); j++)
   	correctpalette(j);

    if (IS_BGI_COLOR(bkcolor) && (bkcolor <= pal->size))
    {
	SetBkColor(hdc[0], PALETTEINDEX(BG+bkcolor));
	SetBkColor(hdc[1], PALETTEINDEX(BG+bkcolor));
	DeleteObject(hBackgroundBrush);
        hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG+bkcolor));
    }	    

}

palettetype* getdefaultpalette()
{
    static palettetype default_palette = { 16,
					   { BLACK, BLUE, GREEN, CYAN, RED, MAGENT, BROWN, LIGHTGRAY,
					     DARKGRAY,LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED,
					     LIGHTMAGENTA, YELLOW, WHITE}};

    return &default_palette;
}

void getpalette(palettetype* pal)
{
    *pal = current_palette;
}

int getpalettesize()
{
    return MAXCOLORS+1;
}

void setbkcolor(int color)
{
    // Version 3.2: Color may now be a bgi color (0 to MAXCOLORS) or an RGB color.

    if (IS_BGI_COLOR(bkcolor))
    {
        SetBkColor(hdc[0], PALETTEINDEX(BG+bkcolor));
		SetBkColor(hdc[1], PALETTEINDEX(BG+bkcolor));
		DeleteObject(hBackgroundBrush);
        hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG+bkcolor));
    }
    else
    {
		SetBkColor(hdc[0], RGB_COLOR(bkcolor));
		SetBkColor(hdc[1], RGB_COLOR(bkcolor));
		DeleteObject(hBackgroundBrush);
        hBackgroundBrush = CreateSolidBrush(RGB_COLOR(bkcolor));
    }
}

int getbkcolor()
{
    // or an RGB color.
    return bkcolor;
}

void setfillstyle(int style, int color)
{
    // Version 3.2: Color may now be bgi color or rgb color.
    if (!IS_BGI_COLOR(color) && !IS_RGB_COLOR(color))
	return;
    
    fill_settings.pattern = style;
    fill_settings.color = color;
    SelectObject(hdc[0], hBrush[style]);
    SelectObject(hdc[1], hBrush[style]);
}

void getfillsettings(fillsettingstype* fs)
{
    *fs = fill_settings;
}

static fillpatterntype userfillpattern ={-1, -1, -1, -1, -1, -1, -1, -1};

void setfillpattern(char const* upattern, int color)
{
    // Version 3.2: Color may now be bgi color or rgb color.
    if (!IS_BGI_COLOR(color) && !IS_RGB_COLOR(color))
	return;
    
    static HBITMAP hFillBitmap;
    static short bitmap_data[8];
    for (int i = 0; i < 8; i++)
    {
        bitmap_data[i] = (unsigned char)~upattern[i];
        userfillpattern[i] = upattern[i];
    }

    HBITMAP h = CreateBitmap(8, 8, 1, 1, bitmap_data);
    HBRUSH hb = CreatePatternBrush(h);
    DeleteObject(hBrush[USER_FILL]);
    if (hFillBitmap)
        DeleteObject(hFillBitmap);

    hFillBitmap = h;
    hBrush[USER_FILL] = hb;
    SelectObject(hdc[0], hb);
    SelectObject(hdc[1], hb);
    fill_settings.color = color;
    fill_settings.pattern = USER_FILL;
}

void getfillpattern(fillpatterntype fp)
{
    memcpy(fp, userfillpattern, sizeof userfillpattern);
}

static void select_fill_color()
{
    if (windows_color != fill_settings.color)
    {
    	windows_color = fill_settings.color;
	if (IS_RGB_COLOR(windows_color))
	{
	    SetTextColor(hdc[0], RGB_COLOR(windows_color));
	    SetTextColor(hdc[1], RGB_COLOR(windows_color));
	}
	else
	{
	    SetTextColor(hdc[0], PALETTEINDEX(windows_color+BG));
	    SetTextColor(hdc[1], PALETTEINDEX(windows_color+BG));
	}
    }
}

void setusercharsize(int multx, int divx, int multy, int divy)
{
    font_mul_x = multx;
    font_div_x = divx;
    font_mul_y = multy;
    font_div_y = divy;
    text_settings.charsize = 0;
}

void moveto(int x, int y)
{
    if (bgiemu_handle_redraw || visual_page != active_page)
        MoveToEx(hdc[1], x, y, NULL);

    if (visual_page == active_page)
   	MoveToEx(hdc[0], x, y, NULL);
}

void moverel(int dx, int dy)
{
    POINT pos;
    GetCurrentPositionEx(hdc[1], &pos);
    moveto(pos.x + dx, pos.y + dy);
}

static void select_font()
{
    if (text_settings.charsize == 0) {
        fcache.select(text_settings.font, text_settings.direction,
		      font_metrics[text_settings.font]
		      [normal_font_size[text_settings.font]].width
		      *font_mul_x/font_div_x,font_metrics[text_settings.font]
		      [normal_font_size[text_settings.font]].height
		      *font_mul_y/font_div_y);
    }
    else
    {
    	fcache.select(text_settings.font, text_settings.direction,
		      font_metrics[text_settings.font][text_settings.charsize].width,
		      font_metrics[text_settings.font][text_settings.charsize].height);
    }
}

static void text_output(int x, int y, const char* str)
{
    select_font();
    if (windows_color != color)
    {
   	windows_color = color;
	if (IS_RGB_COLOR(windows_color))
	{
	    SetTextColor(hdc[0], RGB_COLOR(windows_color));
	    SetTextColor(hdc[1], RGB_COLOR(windows_color));
	}
	else
	{
	    SetTextColor(hdc[0], PALETTEINDEX(windows_color+BG));
	    SetTextColor(hdc[1], PALETTEINDEX(windows_color+BG));
	}
    }

    if (bgiemu_handle_redraw || visual_page != active_page)
   	TextOut(hdc[1], x, y, str, strlen(str));

    if (visual_page == active_page)
        TextOut(hdc[0], x, y, str, strlen(str));
}

void settextstyle(int font, int direction, int char_size)
{
    if (char_size > 10)
        char_size = 10;

    text_settings.direction = direction;
    text_settings.font = font;
    text_settings.charsize = char_size;
    text_align_mode = ALIGN_NOT_SET;
}

void settextjustify(int horiz, int vert)
{
    text_settings.horiz = horiz;
    text_settings.vert = vert;
    text_align_mode = ALIGN_NOT_SET;
}

void gettextsettings(textsettingstype* ts)
{
    *ts = text_settings;
}

int textheight(const char* str)
{
    SIZE ss;
    select_font();
    GetTextExtentPoint32(hdc[0], str, strlen(str), &ss);

    return ss.cy;
}

int textwidth(const char* str)
{
    SIZE ss;
    select_font();
    GetTextExtentPoint32(hdc[0], str, strlen(str), &ss);

    return ss.cx;
}

void outtext(const char* str)
{
    if (text_align_mode != UPDATE_CP)
    {
   	text_align_mode = UPDATE_CP;
	int align = (text_settings.direction == HORIZ_DIR)
	    ? (TA_UPDATECP | text_halign_cnv[text_settings.horiz] |
	       text_valign_cnv[text_settings.vert]): (TA_UPDATECP |
						      text_valign_cnv[text_settings.horiz] |
						      text_halign_cnv[text_settings.vert]);

	SetTextAlign(hdc[0], align);
	SetTextAlign(hdc[1], align);
    }
	
	SetBkMode(hdc[0], TRANSPARENT);
	SetBkMode(hdc[1], TRANSPARENT);
    
	text_output(0, 0, str);
    
	SetBkMode(hdc[0], OPAQUE);
	SetBkMode(hdc[1], OPAQUE);text_output(0, 0, str);
}

void outtextxy(int x, int y, const char* str)
{
    if (text_align_mode != NOT_UPDATE_CP)
    {
   	text_align_mode = NOT_UPDATE_CP;
	int align = (text_settings.direction == HORIZ_DIR)
	    ? (TA_NOUPDATECP | text_halign_cnv[text_settings.horiz] |
	       text_valign_cnv[text_settings.vert]): (TA_NOUPDATECP |
						      text_valign_cnv[text_settings.horiz] |
						      text_halign_cnv[text_settings.vert]);

	SetTextAlign(hdc[0], align);
	SetTextAlign(hdc[1], align);
    }
    
	SetBkMode(hdc[0], TRANSPARENT);
	SetBkMode(hdc[1], TRANSPARENT);
    
	text_output(x, y, str);
    
	SetBkMode(hdc[0], OPAQUE);
	SetBkMode(hdc[1], OPAQUE);	
}

void setviewport(int x1, int y1, int x2, int y2, int clip)
{
    view_settings.left = x1;
    view_settings.top = y1;
    view_settings.right = x2;
    view_settings.bottom = y2;
    view_settings.clip = clip;

    if (hRgn)
   	DeleteObject(hRgn);

    hRgn = clip ? CreateRectRgn(x1, y1, x2, y2) : NULL;
    SelectClipRgn(hdc[1], hRgn);
    SetViewportOrgEx(hdc[1], x1, y1, NULL);
    SelectClipRgn(hdc[0], hRgn);
    SetViewportOrgEx(hdc[0], x1, y1, NULL);
    moveto(0,0);
}

void getviewsettings(viewporttype *viewport)
{
    *viewport = view_settings;
}

const double pi = 3.14159265358979323846;
static void arc_coords(double angle, double rx, double ry, int& x, int& y)
{
    if (rx == 0 || ry == 0)
    {
   	x = y = 0;
	return;
    }

    double s = sin(angle*pi/180.0);
    double c = cos(angle*pi/180.0);
    if (fabs(s) < fabs(c))
    {
   	double tg = s/c;
	double xr = sqrt((double)rx*rx*ry*ry/(ry*ry+rx*rx*tg*tg));
	x = int((c >= 0) ? xr : -xr);
	y = int((s >= 0) ? -xr*tg : xr*tg);
    }
    else
    {
   	double ctg = c/s;
	double yr = sqrt((double)rx*rx*ry*ry/(rx*rx+ry*ry*ctg*ctg));
	x = int((c >= 0) ? yr*ctg : -yr*ctg);
	y = int((s >= 0) ? -yr : yr);
    }
}

void ellipse(int x, int y, int start_angle, int end_angle,int rx, int ry)
{
    ac.x = x;
    ac.y = y;
    arc_coords(start_angle, rx, ry, ac.xstart, ac.ystart);
    arc_coords(end_angle,  rx, ry, ac.xend, ac.yend);
    ac.xstart += x; ac.ystart += y;
    ac.xend += x; ac.yend += y;
    pcache.select(color); // VERSION 3.2: select now passed color.

    if (bgiemu_handle_redraw || visual_page != active_page)
    {
   	Arc(hdc[1], x-rx, y-ry, x+rx, y+ry,
	    ac.xstart, ac.ystart, ac.xend, ac.yend);
    }

    if (visual_page == active_page)
    {
    	Arc(hdc[0], x-rx, y-ry, x+rx, y+ry,
	    ac.xstart, ac.ystart, ac.xend, ac.yend);
    }
}

void fillellipse(int x, int y, int rx, int ry)
{
    pcache.select(color); // VERSION 3.2: select now passed color.
    select_fill_color();
    if (bgiemu_handle_redraw || visual_page != active_page) {
        Ellipse(hdc[1], x-rx, y-ry, x+rx, y+ry);
    }
    if (visual_page == active_page) { 
        Ellipse(hdc[0], x-rx, y-ry, x+rx, y+ry);
    }
}

static void allocate_new_graphic_page(int page)
{
    RECT scr;

    scr.left = -view_settings.left;
    scr.top = -view_settings.top; 
    scr.right = screen_width-view_settings.left-1;
    scr.bottom = screen_height-view_settings.top-1;

    hBitmap[page] = CreateCompatibleBitmap(hdc[0],screen_width,screen_height);
    SelectObject(hdc[1], hBitmap[page]);            
    SelectClipRgn(hdc[1], NULL);
    FillRect(hdc[1], &scr, hBackgroundBrush);
    SelectClipRgn(hdc[1], hRgn);
}

void setactivepage(int page)
{
    if ((page < 0) || (page >= MAX_PAGES))
	return;
    
    if (hBitmap[page] == NULL) { 
        allocate_new_graphic_page(page);
    } else { 
        SelectObject(hdc[1], hBitmap[page]);            
    }

    if (!bgiemu_handle_redraw && active_page == visual_page) {
        POINT pos;
        GetCurrentPositionEx(hdc[0], &pos);
        MoveToEx(hdc[1], pos.x, pos.y, NULL);
    }

    active_page = page;
}

void setvisualpage(int page)
{
    POINT pos;

    if ((page < 0) || (page >= MAX_PAGES))
	return;

    if (hdc[page] == NULL) { 
        allocate_new_graphic_page(page);
    }

    if (!bgiemu_handle_redraw && active_page == visual_page) { 
        SelectObject(hdc[1], hBitmap[visual_page]);            
        SelectClipRgn(hdc[1], NULL);
        BitBlt(hdc[1], -view_settings.left, -view_settings.top, 
               window_width, window_height, 
               hdc[0], -view_settings.left, -view_settings.top, 
               SRCCOPY);
        SelectClipRgn(hdc[1], hRgn);
        GetCurrentPositionEx(hdc[0], &pos);
        MoveToEx(hdc[1], pos.x, pos.y, NULL);
    }

    SelectClipRgn(hdc[0], NULL);
    SelectClipRgn(hdc[1], NULL);
    SelectObject(hdc[1], hBitmap[page]);            
    BitBlt(hdc[0], -view_settings.left, 
           -view_settings.top, window_width, window_height, 
           hdc[1], -view_settings.left, -view_settings.top, SRCCOPY);

    SelectClipRgn(hdc[0], hRgn);
    SelectClipRgn(hdc[1], hRgn);

    if (page != active_page) {
        SelectObject(hdc[1], hBitmap[active_page]);            
    }

    if (active_page != visual_page) { 
        GetCurrentPositionEx(hdc[1], &pos);
        MoveToEx(hdc[0], pos.x, pos.y, NULL);
    }

    visual_page = page;
}

void setaspectratio(int ax, int ay)
{
    aspect_ratio_x = ax;
    aspect_ratio_y = ay;
}

void getaspectratio(int* ax, int* ay)
{
    *ax = aspect_ratio_x;
    *ay = aspect_ratio_y;
}

void circle(int x, int y, int radius)
{
    int ry = (unsigned)radius*aspect_ratio_x/aspect_ratio_y;
    int rx = radius;

    pcache.select(color); // VERSION 3.2: select now passed color.
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Arc(hdc[1], x-rx, y-ry, x+rx, y+ry, x+rx, y, x+rx, y);
    }
    if (visual_page == active_page) { 
        Arc(hdc[0], x-rx, y-ry, x+rx, y+ry, x+rx, y, x+rx, y);
    }    
}

void arc(int x, int y, int start_angle, int end_angle, int radius)
{
    pcache.select(color); // VERSION 3.2: select now passed color.

    ac.x = x;
    ac.y = y;
    ac.xstart = x + int(radius*cos(start_angle*pi/180.0));
    ac.ystart = y - int(radius*sin(start_angle*pi/180.0));
    ac.xend = x + int(radius*cos(end_angle*pi/180.0));
    ac.yend = y - int(radius*sin(end_angle*pi/180.0));

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Arc(hdc[1], x-radius, y-radius, x+radius, y+radius, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend);
    }

    if (visual_page == active_page) { 
        Arc(hdc[0], x-radius, y-radius, x+radius, y+radius, 
            ac.xstart, ac.ystart, ac.xend, ac.yend);
    }
}

void getarccoords(arccoordstype *arccoords)
{
    *arccoords = ac;
}

void pieslice(int x, int y, int start_angle, int end_angle, int radius)
{
    pcache.select(color); // VERSION 3.2: select now passed color.
    select_fill_color();

    ac.x = x;
    ac.y = y;
    ac.xstart = x + int(radius*cos(start_angle*pi/180.0));
    ac.ystart = y - int(radius*sin(start_angle*pi/180.0));
    ac.xend = x + int(radius*cos(end_angle*pi/180.0));
    ac.yend = y - int(radius*sin(end_angle*pi/180.0));

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Pie(hdc[1], x-radius, y-radius, x+radius, y+radius,
            ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
    if (visual_page == active_page) {
        Pie(hdc[0], x-radius, y-radius, x+radius, y+radius, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
}

void sector(int x, int y, int start_angle, int end_angle, int rx, int ry)
{
    ac.x = x;
    ac.y = y;
    arc_coords(start_angle, rx, ry, ac.xstart, ac.ystart);
    arc_coords(end_angle, rx, ry, ac.xend, ac.yend);
    ac.xstart += x; ac.ystart += y;
    ac.xend += x; ac.yend += y;

    pcache.select(color); // VERSION 3.2: select now passed color.

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Pie(hdc[1], x-rx, y-ry, x+rx, y+ry, 
            ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
    if (visual_page == active_page) { 
        Pie(hdc[0], x-rx, y-ry, x+rx, y+ry, 
	    ac.xstart, ac.ystart, ac.xend, ac.yend); 
    }
}

void bar(int left, int top, int right, int bottom)
{
    RECT r;

    if (left > right) {        /* Turbo C corrects for badly ordered corners */   
        r.left = right;
        r.right = left;
    } else {
        r.left = left;
        r.right = right;
    }

    if (bottom < top) {        /* Turbo C corrects for badly ordered corners */   
        r.top = bottom;
        r.bottom = top;
    } else {
        r.top = top;
        r.bottom = bottom;
    }

    pcache.select(color); // VERSION 3.2: select now passed color.
    select_fill_color();

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        FillRect(hdc[1], &r, hBrush[fill_settings.pattern]);
    }

    if (visual_page == active_page) { 
        FillRect(hdc[0], &r, hBrush[fill_settings.pattern]);
    }
}





void bar3d(int left, int top, int right, int bottom, int depth, int topflag)
{
    int temp;
    const double tan30 = 1.0/1.73205080756887729352;

    if (left > right) {     /* Turbo C corrects for badly ordered corners */
        temp = left;
        left = right;
        right = temp;
    }

    if (bottom < top) {
        temp = bottom;
        bottom = top;
        top = temp;
    }

    bar(left+line_settings.thickness, top+line_settings.thickness, 
        right-line_settings.thickness+1, bottom-line_settings.thickness+1);

    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], write_mode_cnv[write_mode]);
        SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 

    pcache.select(color); // VERSION 3.2: select now passed color.

    int dy = int(depth*tan30);
    POINT p[11];

    p[0].x = right, p[0].y = bottom;
    p[1].x = right, p[1].y = top;
    p[2].x = left,  p[2].y = top;
    p[3].x = left,  p[3].y = bottom;
    p[4].x = right, p[4].y = bottom;
    p[5].x = right+depth, p[5].y = bottom-dy;
    p[6].x = right+depth, p[6].y = top-dy;
    p[7].x = right, p[7].y = top;

    if (topflag) {
        p[8].x = right+depth, p[8].y = top-dy;
        p[9].x = left+depth, p[9].y = top-dy;
        p[10].x = left, p[10].y = top;        
    }

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Polyline(hdc[1], p, topflag ? 11 : 8);
    }

    if (visual_page == active_page) { 
        Polyline(hdc[0], p, topflag ? 11 : 8);
    }

    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], R2_COPYPEN);
        SetROP2(hdc[1], R2_COPYPEN);
    }
}

void lineto(int x, int y)
{
    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], write_mode_cnv[write_mode]);
        SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 

    pcache.select(color); // VERSION 3.2: select now passed color.

    if (bgiemu_handle_redraw || visual_page != active_page) {
        LineTo(hdc[1], x, y);
    }
    if (visual_page == active_page) { 
        LineTo(hdc[0], x, y);
    }

    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], R2_COPYPEN);
        SetROP2(hdc[1], R2_COPYPEN);
    }
}

void linerel(int dx, int dy)
{
    POINT pos;
    GetCurrentPositionEx(hdc[1], &pos);
    lineto(pos.x + dx, pos.y + dy);
}

void drawpoly(int n_points, int* points) 
{ 
    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], write_mode_cnv[write_mode]);
        SetROP2(hdc[1], write_mode_cnv[write_mode]);
    } 

    pcache.select(color); // VERSION 3.2: select now passed color.
    
    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Polyline(hdc[1], (POINT*)points, n_points);
    }

    if (visual_page == active_page) { 
        Polyline(hdc[0], (POINT*)points, n_points);
    }

    if (write_mode != COPY_PUT) { 
        SetROP2(hdc[0], R2_COPYPEN);
        SetROP2(hdc[1], R2_COPYPEN);
    }
}

void line(int x0, int y0, int x1, int y1)
{
    POINT line[2];
    line[0].x = x0;
    line[0].y = y0;
    line[1].x = x1;
    line[1].y = y1;
    drawpoly(2, (int*)&line);
}

void rectangle(int left, int top, int right, int bottom)
{
    POINT rect[5];
    rect[0].x = left, rect[0].y = top;
    rect[1].x = right, rect[1].y = top;
    rect[2].x = right, rect[2].y = bottom;
    rect[3].x = left, rect[3].y = bottom;
    rect[4].x = left, rect[4].y = top;
    drawpoly(5, (int*)&rect);
}   

void fillpoly(int n_points, int* points)
{
    pcache.select(color); // VERSION 3.2: select now passed color.
    select_fill_color();

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        Polygon(hdc[1], (POINT*)points, n_points);
    }

    if (visual_page == active_page) { 
        Polygon(hdc[0], (POINT*)points, n_points);
    }
}

void floodfill(int x, int y, int border)
{
    pcache.select(color); // VERSION 3.2: select now passed color.
    select_fill_color();

    if (IS_BGI_COLOR(border))
    {
	if (bgiemu_handle_redraw || visual_page != active_page) { 
	    FloodFill(hdc[1], x, y, PALETTEINDEX(border+BG));
	}
	if (visual_page == active_page) { 
	    FloodFill(hdc[0], x, y, PALETTEINDEX(border+BG));
	}
    }
    else
    {
	if (bgiemu_handle_redraw || visual_page != active_page) { 
	    FloodFill(hdc[1], x, y, RGB_COLOR(border));
	}
	if (visual_page == active_page) { 
	    FloodFill(hdc[0], x, y, RGB_COLOR(border));
	}
    }
}

static int handle_input(int wait = 0)
{
    MSG lpMsg;

    if (wait ? GetMessage(&lpMsg, NULL, 0, 0) 
	: PeekMessage(&lpMsg, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
        return 1;
    }
    return 0;
}

void delay(unsigned msec)
{
    timeout_expired = false;
    SetTimer(hWnd, TIMER_ID, msec, NULL); 
    while (!timeout_expired) handle_input(true);
}

int kbhit()
{
    while (handle_input(0));
    return !kbd_queue.is_empty();
}

int getch()
{
    while (kbd_queue.is_empty()) handle_input(true);
    return (unsigned char)kbd_queue.get();
}

void cleardevice()
{        
	swapbuffers();    
    RECT scr;
    scr.left = -view_settings.left;
    scr.top = -view_settings.top; 
    scr.right = screen_width-view_settings.left-1;
    scr.bottom = screen_height-view_settings.top-1;

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        if (hRgn != NULL) { 
            SelectClipRgn(hdc[1], NULL);
        }
        FillRect(hdc[1], &scr, hBackgroundBrush);
        if (hRgn != NULL) { 
            SelectClipRgn(hdc[1], hRgn);
        }
    }

    if (visual_page == active_page) { 
        if (hRgn != NULL) { 
            SelectClipRgn(hdc[0], NULL);
        }
        FillRect(hdc[0], &scr, hBackgroundBrush);
        if (hRgn != NULL) { 
            SelectClipRgn(hdc[0], hRgn);
        }
    }
    moveto(0,0);
}

void clearviewport()
{
    RECT scr;
    scr.left = 0;
    scr.top = 0; 
    scr.right = view_settings.right-view_settings.left;
    scr.bottom = view_settings.bottom-view_settings.top;

    if (bgiemu_handle_redraw || visual_page != active_page) { 
        FillRect(hdc[1], &scr, hBackgroundBrush);
    }

    if (visual_page == active_page) { 
        FillRect(hdc[0], &scr, hBackgroundBrush);
    }
    moveto(0,0);
}

void detectgraph(int *graphdriver, int *graphmode)
{
    *graphdriver = VGA;
    *graphmode = bgiemu_default_mode;
}

int getgraphmode()
{
    return bgiemu_default_mode;
}

void setgraphmode(int) {}

void putimage(int x, int y, void* image, int bitblt)
{
    BGIimage* bi = (BGIimage*)image;
    HDC tempHDC;
    HBITMAP tempBMP;
    char *bitmapBits;
    char *actualBits;
    unsigned int localImageSize;

    hBitMapInfo.bmiHeader.biWidth=bi->width;
    hBitMapInfo.bmiHeader.biHeight=bi->height;
    localImageSize=(bi->width)*(bi->height)*32/8; //32 bits per pixel, 8 bits per byte
    hBitMapInfo.bmiHeader.biSizeImage=localImageSize;

    actualBits=bi->bits;

    bitmapBits=new char[localImageSize];
    tempHDC=CreateCompatibleDC(hdc[visual_page != active_page|| bgiemu_handle_redraw ? 1 : 0]);
    if (0==SetDIBitsToDevice(tempHDC,0,0,bi->width,bi->height,0,0,0,bi->height-1,&(bi->bits),&hBitMapInfo,DIB_RGB_COLORS))
	std::cerr << "Unable to copy image to offscreen buffer" << std::endl;

    tempBMP=CreateDIBSection(tempHDC,&hBitMapInfo,DIB_RGB_COLORS,(void**)&bitmapBits,0,0);

    if (!SelectObject(tempHDC,tempBMP)) 
	std::cerr << "cannot select bitmap image in offscreen window" << std::endl;
		
    memcpy(bitmapBits,actualBits,localImageSize);

    // Select the bitmaps into the compatible DC.
    if (SelectObject(hdc[0], tempBMP)) // was tempBMP))
	std::cerr << "Unable to synch graphics window with bitmap." << std::endl;

    if (!BitBlt(hdc[0],x,y,bi->width,bi->height,tempHDC,0,0,bitblt_mode_cnv[bitblt])) //bi->hdc was tempHDC
        std::cerr << "Unable to copy bitmap to graphics window." << std::endl;

    if (SelectObject(hdc[1], tempBMP)) // was tempBMP))
	std::cerr << "Unable to synch graphics window with bitmap." << std::endl;

    if (!BitBlt(hdc[1],x,y,bi->width,bi->height,tempHDC,0,0,bitblt_mode_cnv[bitblt])) //bi->hdc was tempHDC
        std::cerr << "Unable to copy bitmap to graphics window." << std::endl;

    DeleteObject(tempBMP);
    //delete bitmapBits;  // To prevent heap leak, should be in,
    // but if it is in, the program will crash.  why?
    DeleteDC(tempHDC);

}

unsigned int imagesize(int x1, int y1, int x2, int y2)
{
    return 2 * sizeof(short) + (x2-x1+1)*(y2-y1+1)*32/8;
    // 2 shorts(2 bytes each) + #of pixels* 32 bits per pixel / 8 bits per byte
    //this was 8 + (((x2-x1+8) & ~7) >> 1)*(y2-y1+1);
}

void getimage(int x1, int y1, int x2, int y2, void* image)
{
    BGIimage* bi = (BGIimage*)image;
    char *actualBits;
    int *tempBits;
    int localImageSize;

    HDC tempHDC;
    HBITMAP tempBMP;


    bi->width = (short)(x2-x1+1);
    bi->height = (short)(y2-y1+1);
    localImageSize=(bi->width)*(bi->height)*32/8; //32 bits per pixel, 8 bits per byte

    tempHDC=CreateCompatibleDC(hdc[visual_page != active_page|| bgiemu_handle_redraw ? 1 : 0]);

    hBitMapInfo.bmiHeader.biWidth=(x2-x1+1);
    hBitMapInfo.bmiHeader.biHeight=(y2-y1+1);
    hBitMapInfo.bmiHeader.biSizeImage=localImageSize;

    tempBMP=CreateDIBSection(tempHDC,&hBitMapInfo,DIB_RGB_COLORS,(void**)&tempBits,0,0);
    if (tempBMP == 0)
	std::cerr << "Unable to create bitmap!" << std::endl;

    if (!SelectObject(tempHDC,tempBMP))
	std::cerr << "Unable to synch offscreen window with bitmap." << std::endl;

    if (!BitBlt(tempHDC,0,0,bi->width,bi->height,hdc[visual_page != active_page|| bgiemu_handle_redraw ? 1 : 0],
		x1,y1,SRCCOPY))
        std::cerr << "Unable to copy bitmap to offscreen window." << std::endl;

    //actualBits=new int[(x2-x1+1)*(y2-y1+1)];
    //bi->bits=actualBits;
    actualBits=bi->bits;
    memcpy(actualBits,tempBits,localImageSize);

    DeleteObject(tempBMP);
    DeleteDC(tempHDC);
}

unsigned int getpixel(int x, int y)
{
    int color;
    COLORREF rgb = GetPixel(hdc[visual_page != active_page|| bgiemu_handle_redraw ? 1 : 0], x, y);

    if (rgb == CLR_INVALID)
        return CLR_INVALID;

    int red = GetRValue(rgb);
    int blue = GetBValue(rgb);
    int green = GetGValue(rgb);
    for (color = 0; color <= MAXCOLORS; color++)
    {
        if (BGIpalette[color].peRed == red && BGIpalette[color].peGreen == green && 
	    BGIpalette[color].peBlue == blue)
	    return color;
    }
    return COLOR(red, blue, green);
}

void putpixel(int x, int y, int c)
{
    if (IS_BGI_COLOR(c))
    {
	if (bgiemu_handle_redraw || visual_page != active_page)
	    SetPixel(hdc[1], x, y, PALETTEINDEX(c+BG));

	if (visual_page == active_page)
	    SetPixel(hdc[0], x, y, PALETTEINDEX(c+BG));
    }
    else
    {
	if (bgiemu_handle_redraw || visual_page != active_page)
	    SetPixel(hdc[1], x, y, RGB_COLOR(c));

	if (visual_page == active_page)
	    SetPixel(hdc[0], x, y, RGB_COLOR(c));
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg,WPARAM wParam, LPARAM lParam)
{
    int i;
    static bool palette_changed = false;
    Handler handler;

    if (messg >= WM_MOUSEFIRST && messg <= WM_MOUSELAST)
    {   // Mouse handler functions added by Michael Main, Oct 17, 1998.
        x_clicks[messg-WM_MOUSEFIRST] = current_mouse_x = (int) (lParam & 0xffff);
	y_clicks[messg-WM_MOUSEFIRST] = current_mouse_y = (int) ((lParam >> 16) & 0xffff);
	click_ready[messg-WM_MOUSEFIRST] = true;
	
        if ((handler = mouse_handlers[messg-WM_MOUSEFIRST]) != 0)
        {
            current_mouse_y = (int) ((lParam >> 16) & 0xffff);
            current_mouse_x = (int) (lParam & 0xffff);
            handler(current_mouse_x, current_mouse_y);
        }
    }
    else switch (messg) { 
    
    
	case WM_ERASEBKGND:
    	return 1;
    
    case WM_PAINT: 
        if (hdc[0] == 0) {
            hdc[0] = BeginPaint(hWnd, &ps);
            SelectPalette(hdc[0], hPalette, FALSE);
            RealizePalette(hdc[0]);
            hdc[1] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[1], hPalette, FALSE);
            hdc[2] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[2], hPalette, FALSE);
            hdc[3] = CreateCompatibleDC(hdc[0]);
            SelectPalette(hdc[3], hPalette, FALSE);
            screen_width = GetDeviceCaps(hdc[0], HORZRES);
            screen_height = GetDeviceCaps(hdc[0], VERTRES);
            hBitmap[active_page] = 
                CreateCompatibleBitmap(hdc[0], screen_width, screen_height);
            SelectObject(hdc[1], hBitmap[active_page]);
            SetTextColor(hdc[0], PALETTEINDEX(windows_color+BG));
            SetTextColor(hdc[1], PALETTEINDEX(windows_color+BG));

	    // Version 3.2: Correct setting of background color:
	    // SetBkColor(hdc[0], PALETTEINDEX(BG));
            // SetBkColor(hdc[1], PALETTEINDEX(BG));
	    if (IS_BGI_COLOR(bkcolor))
	    {
			SetBkColor(hdc[0], PALETTEINDEX(BG+bkcolor));
			SetBkColor(hdc[1], PALETTEINDEX(BG+bkcolor));
	    }
	    else
	    {
			SetBkColor(hdc[0], RGB_COLOR(bkcolor));
			SetBkColor(hdc[1], RGB_COLOR(bkcolor));
	    }

	    SelectObject(hdc[0], hBrush[fill_settings.pattern]);
            SelectObject(hdc[1], hBrush[fill_settings.pattern]);
            RECT scr;
            scr.left = -view_settings.left;
            scr.top = -view_settings.top; 
            scr.right = screen_width-view_settings.left-1;
            scr.bottom = screen_height-view_settings.top-1;
            FillRect(hdc[1], &scr, hBackgroundBrush);
        }

        if (hRgn != NULL) { 
            SelectClipRgn(hdc[0], NULL);
        }

        if (visual_page != active_page) { 
            SelectObject(hdc[1], hBitmap[visual_page]); 
        } 

        BitBlt(hdc[0], -view_settings.left, 
               -view_settings.top, window_width, window_height, 
               hdc[1], -view_settings.left, -view_settings.top, 
               SRCCOPY);

        if (hRgn != NULL) {
            SelectClipRgn(hdc[0], hRgn);
        }

        if (visual_page != active_page) { 
            SelectObject(hdc[1], hBitmap[active_page]); 
        } 

        ValidateRect(hWnd, NULL);
        break;


    // The next two cases should probably be rewritten according to
    // http://msdn.microsoft.com/library/techart/msdn_palette.htm
    // 
	
    case WM_SETFOCUS:
        if (palette_changed) { 
            HPALETTE new_palette = CreatePalette(pPalette);
            SelectPalette(hdc[0], new_palette, FALSE);
            RealizePalette(hdc[0]);
            SelectPalette(hdc[1], new_palette, FALSE);
            SelectPalette(hdc[2], new_palette, FALSE);
            SelectPalette(hdc[3], new_palette, FALSE);
            DeleteObject(hPalette);
            hPalette = new_palette;
            palette_changed = false;
        }
        break;

    case WM_PALETTECHANGED: 
        RealizePalette(hdc[0]);
        UpdateColors(hdc[0]);
        palette_changed = true;
        break;

    case WM_DESTROY: 
        EndPaint(hWnd, &ps);
        hdc[0] = 0;
        DeleteObject(hdc[1]);
        DeleteObject(hdc[2]);
        DeleteObject(hdc[3]);
        if (hPutimageBitmap) { 
            DeleteObject(hPutimageBitmap);
            hPutimageBitmap = NULL;
        }
        for (i = 0; i < MAX_PAGES; i++) { 
            if (hBitmap[i] != NULL) {
                DeleteObject(hBitmap[i]);
                hBitmap[i] = 0;
            }
        }
        DeleteObject(hPalette);
        hPalette = 0;
        PostQuitMessage(0);
        break;

    case WM_SIZE: 
        window_width = LOWORD(lParam);
        window_height = HIWORD(lParam);
        break;

    case WM_TIMER:
        KillTimer(hWnd, TIMER_ID);
        timeout_expired = true;
        break;

    case WM_CHAR:
        kbd_queue.put((TCHAR) wParam);
        break;

    case WM_KEYDOWN:
        // New case to handle keyboard keys. M. Main -- Nov 3, 1998.
        switch(wParam)
        {
	case NUMBER_HOME: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_HOME);
	    break;
	case NUMBER_UP: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_UP);
	    break;
	case NUMBER_PGUP: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_PGUP);
	    break;
	case NUMBER_LEFT: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_LEFT);
	    break;
	case NUMBER_CENTER: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_CENTER);
	    break;
	case NUMBER_RIGHT: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_RIGHT);
	    break;
	case NUMBER_END:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_END);
	    break;
	case NUMBER_DOWN: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_DOWN);
	    break;
	case NUMBER_PGDN: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_PGDN);
	    break;
	case NUMBER_INSERT: 
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_INSERT);
	    break;
	case NUMBER_DELETE:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_DELETE);
	    break;
	case FUNCTION_F1:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F1);
	    break;
	case FUNCTION_F2:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F2);
	    break;
	case FUNCTION_F3:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F3);
	    break;
	case FUNCTION_F4:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F4);
	    break;
	case FUNCTION_F5:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F5);
	    break;
	case FUNCTION_F6:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F6);
	    break;
	case FUNCTION_F7:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F7);
	    break;
	case FUNCTION_F8:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F8);
	    break;
	case FUNCTION_F9:
	    kbd_queue.put((TCHAR) 0);
	    kbd_queue.put((TCHAR) KEY_F9);
	    break;
	default:
	    return DefWindowProc(hWnd, messg, wParam, lParam);
        }
        break;

    default:
        return DefWindowProc(hWnd, messg, wParam, lParam);
    }

    return 0;
}

int mousex( )
{   // Added by Michael Main, Oct 17, 1998.
    return current_mouse_x;
}

int mousey( )
{   // Added by Michael Main, Oct 17, 1998.
    return current_mouse_y;
}

void registermousehandler(UINT kind, void h(int, int))
{   // Added by Michael Main, Oct 17, 1998.
    if (kind >= WM_MOUSEFIRST && kind <= WM_MOUSELAST)
        mouse_handlers[kind-WM_MOUSEFIRST] = h;
}

bool ismouseclick(UINT kind)
{
    return
	(kind >= WM_MOUSEFIRST)
	&&
	(kind <= WM_MOUSELAST)
	&&
	(click_ready[kind-WM_MOUSEFIRST]);
}
    
void getmouseclick(UINT kind, int& x, int& y)
{
    if (ismouseclick(kind))
    {
	do
	{
	    y = y_clicks[kind-WM_MOUSEFIRST];
	    x = x_clicks[kind-WM_MOUSEFIRST];
	}
	while (x != x_clicks[kind-WM_MOUSEFIRST]);
	click_ready[kind-WM_MOUSEFIRST] = false;
    }
    else
    {
	x = y = NO_CLICK;
    }
}

void clearmouseclick(UINT kind)
{
    click_ready[kind-WM_MOUSEFIRST] = false;
}

void closegraph()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
    EndPaint(hWnd, &ps);
    
    DestroyWindow(hWnd);
    while(handle_input(true));
}

static void detect_mode(int* gd, int* gm)
{
    switch (*gd) {
    case CGA:
        window_height = 200;
        switch (*gm) {
	case CGAC0:
	case CGAC1:
	case CGAC2:
	case CGAC3:
            window_width = 320;
            break;
	case CGAHI:
            window_width = 640;
            break;
	default:
            window_width = 320;
            break;
        }
        break;

    case MCGA:
        window_height = 200;
        switch (*gm) {
	case MCGAC0:
	case MCGAC1:
	case MCGAC2:
	case MCGAC3:
            window_width = 320;
            break;
	case MCGAMED:
	    window_width = 640;
            break;
	case MCGAHI:
            window_width = 640;
            window_height = 480;
            break;
	default:
            window_width = 320;
            break;
        }
        break;

    case EGA:
        window_width = 640;
        switch (*gm) {
	case EGALO:
            window_height = 200;
            break;
	case EGAHI:
            window_height = 350;
            break;
	default:
            window_height = 350;
            break;
        }
        break;

    case EGA64:
        window_width = 640;
        switch (*gm) {
	case EGA64LO:
            window_height = 200;
            break;
	case EGA64HI:
            window_height = 350;
            break;
	default:
            window_height = 350;
            break;
        }
        break;

    case EGAMONO:
        window_width = 640;
        window_height = 350;
        break;

    case HERCMONO:
        window_width = 720;
        window_height = 348;
        break;

    case ATT400:
        window_height = 200;
        switch (*gm) {
	case ATT400C0:
	case ATT400C1:
	case ATT400C2:
	case ATT400C3:
            window_width = 320;
            break;
	case ATT400MED:
            window_width = 640;
            break;
	case ATT400HI:
            window_width = 640;
            window_height = 400;
            break;
	default:
            window_width = 320;
            break;
        }
        break;

    default:
    case DETECT:
        *gd = VGA;
        *gm = bgiemu_default_mode;                   

    case VGA:
        window_width = 640;
        switch (*gm) {
	case VGALO:
            window_height = 200;
            break;
	case VGAMED:
            window_height = 350;
            break;
	case VGAHI:
            window_height = 480;
            break;
	default:
            window_height = 480;
            break;
        }
        break;

    case PC3270:
        window_width = 720;
        window_height = 350;
        break;

    case IBM8514:
        switch (*gm) {
	case IBM8514LO:
	    window_width = 640;
            window_height = 480;
            break;
	case IBM8514HI:
            window_width = 1024;
            window_height = 768;
            break;
	default:
            window_width = 1024;
            window_height = 768;
            break;
        }
        break;
    } 
}

static void set_defaults()
{
    color = windows_color = WHITE;
    bkcolor = 0;
    line_settings.thickness = 1;
    line_settings.linestyle = SOLID_LINE;
    line_settings.upattern = ~0;
    fill_settings.pattern = SOLID_FILL;
    fill_settings.color = WHITE;
    write_mode = COPY_PUT;
    text_settings.direction = HORIZ_DIR;
    text_settings.font = DEFAULT_FONT;
    text_settings.charsize = 1;
    text_settings.horiz = LEFT_TEXT;
    text_settings.vert = TOP_TEXT;
    text_align_mode = ALIGN_NOT_SET;
    active_page = visual_page = 0;
    view_settings.left = 0;
    view_settings.top = 0;
    view_settings.right = window_width-1;
    view_settings.bottom = window_height-1;
    aspect_ratio_x = aspect_ratio_y = 10000;
}

// Moved most of the old initgraph into initwork, and introduced a
// second initgraph with explicit height and width parameters.
// Michael Main, Oct 17, 1998.
static void initwork(int height, int width, int* device, int* mode)
{
    int index;
    static WNDCLASS wcApp;
    //+++++++++++++++++++
    static PAINTSTRUCT ps;
    hdc[0] = BeginPaint (hWnd, &ps);
    
	static Gdiplus::GdiplusStartupInput gdiplusStartupInput;    
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//+++++++++++++++++++

    gdi_error_code = grOk;
    if (wcApp.lpszClassName == NULL) {
        wcApp.lpszClassName = "BGIlibrary";
        wcApp.hInstance = 0;
        wcApp.lpfnWndProc = WndProc;
        wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcApp.hIcon = 0;
        wcApp.lpszMenuName = 0;
	// Added HBRUSH typecast. Michael Main, Dec 21, 1999.
        wcApp.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

        // Added CS_DBLCLKS style. Michael Main, Oct 17, 1998.
        wcApp.style = CS_SAVEBITS | CS_DBLCLKS;
        wcApp.cbClsExtra = 0;
        wcApp.cbWndExtra = 0;

        if (!RegisterClass(&wcApp)) {
            gdi_error_code = GetLastError();
            return;
        }
        pPalette = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED,
					    sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*PALETTE_SIZE);

        pPalette->palVersion = 0x300;
        pPalette->palNumEntries = PALETTE_SIZE;
        memset(pPalette->palPalEntry, 0, sizeof(PALETTEENTRY)*PALETTE_SIZE);
        for (index = 0; index < BG; index++) {
            pPalette->palPalEntry[index].peFlags = PC_EXPLICIT;
            // Type casts added from include/win32/wingdi.h MM 10/3/98
            pPalette->palPalEntry[index].peRed = (BYTE) index;
            pPalette->palPalEntry[PALETTE_SIZE-BG+index].peFlags = PC_EXPLICIT;
            pPalette->palPalEntry[PALETTE_SIZE-BG+index].peRed =
                (BYTE) (PALETTE_SIZE-BG+index);
        }

        hBackgroundBrush = CreateSolidBrush(PALETTEINDEX(BG));
	// Added HBRUSH typecast. Michael Main, Dec 21, 1999.
        hBrush[EMPTY_FILL] = (HBRUSH) GetStockObject(NULL_BRUSH);
        hBrush[SOLID_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SolidBrushBitmap));

        hBrush[LINE_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LineBrushBitmap));

        hBrush[LTSLASH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LtslashBrushBitmap));

        hBrush[SLASH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SlashBrushBitmap));

        hBrush[BKSLASH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, BkslashBrushBitmap));

        hBrush[LTBKSLASH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, LtbkslashBrushBitmap));

        hBrush[HATCH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, HatchBrushBitmap));

        hBrush[XHATCH_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, XhatchBrushBitmap));

        hBrush[INTERLEAVE_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, InterleaveBrushBitmap));

        hBrush[WIDE_DOT_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, WidedotBrushBitmap));

        hBrush[CLOSE_DOT_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, ClosedotBrushBitmap));

        hBrush[USER_FILL] =
            CreatePatternBrush(CreateBitmap(8, 8, 1, 1, SolidBrushBitmap));
    }

    memcpy(BGIpalette, BGIcolor, sizeof BGIpalette);
    current_palette.size = MAXCOLORS+1;
    for (index = 10; index <= MAXCOLORS; index++) {
        pPalette->palPalEntry[index] = BGIcolor[0];
    }

    for (index = 0; index <= MAXCOLORS; index++) {
        current_palette.colors[index] = (signed char) index;
        pPalette->palPalEntry[index+BG] = BGIcolor[index];
    }

    hPalette = CreatePalette(pPalette);
    if (height < 10 || width < 10)
        detect_mode(device, mode);
    else
    {
        window_height = height;
        window_width = width;
    }
    set_defaults();
    hWnd = CreateWindow("BGIlibrary", "Windows BGI",
                        WS_OVERLAPPEDWINDOW,
                        0, 0, window_width+BORDER_WIDTH,
                        window_height+BORDER_HEIGHT,
                        (HWND)NULL,  (HMENU)NULL,
			0, NULL);

    if (hWnd == NULL) {
        gdi_error_code = GetLastError();
        return;
    }

    ShowWindow(hWnd, *mode == VGAMAX ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    // Call to correct the BGIpalette added by M. Main 11/17/98:
    for (index = 0; index <= MAXCOLORS; index++)
	correctpalette(index);

    // Set the click locations
    for (index = 0; index < WM_MOUSELAST-WM_MOUSEFIRST; index++)
    {
	x_clicks[index] = y_clicks[index] = NO_CLICK;
	click_ready[index] = false;
    }
}

void initwindow(int width, int height)
{
    int gd = DETECT;
    int gm;

    // See comment above about image_bits.
    int* dummy = image_bits;
    image_bits = dummy;
    initwork(height, width, &gd, &gm);
}

void initgraph(int* graphdriver, int* graphmode, char pathtodriver)
{
    //initwork(0, 0, graphdriver, graphmode);
    WindowData *pWndData;
    int width = 0, height = 0;
    bool valid = true;
    
    if ( *graphdriver == DETECT )
        detectgraph( graphdriver, graphmode );

    switch ( *graphdriver )
    {
    case MCGA:
        switch ( *graphmode )
        {
        case MCGAC0:
        case MCGAC1:
        case MCGAC2:
        case MCGAC3:
            width = 320;
            height = 200;
            break;
        case MCGAMED:
            width = 640;
            height = 200;
            break;
        case MCGAHI:
            width = 640;
            height = 480;
            break;
        }
        break;
    case EGA:
        switch ( *graphmode )
        {
        case EGALO:
            width = 640;
            height = 200;
            break;
        case EGAHI:
            width = 640;
            height = 350;
            break;
        }
        break;
    case EGA64:
        switch ( *graphmode )
        {
        case EGALO:
            width = 640;
            height = 200;
            break;
        case EGAHI:
            width = 640;
            height = 350;
            break;
        }
        break;
    case EGAMONO:
        width = 640;
        height = 350;
        break;
    case HERCMONO:
        width = 720;
        width = 348;
        break;
    case ATT400:
        switch ( *graphmode )
        {
        case ATT400C0:
        case ATT400C1:
        case ATT400C2:
        case ATT400C3:
            width = 320;
            height = 200;
            break;
        case ATT400MED:
            width = 640;
            height = 200;
            break;
        case ATT400HI:
            width = 640;
            height = 400;
            break;
        }
        break;
    case VGA:
        switch ( *graphmode )
        {
        case VGALO:
            width = 640;
            height = 200;
            break;
        case VGAMED:
            width = 640;
            height = 350;
            break;
        case VGAHI:
            width = 640;
            height = 480;
            break;
        }
        break;
    case PC3270:
        width = 720;
        height = 350;
        break;
    case IBM8514:
        switch ( *graphmode )
        {
        case IBM8514LO:
            width = 640;
            height = 480;
            break;
        case IBM8514HI:
            width = 1024;
            height = 768;
            break;
        }
        break;
    default:
	valid = false;
    case CGA:
        switch ( *graphmode )
        {
        case CGAC0:
        case CGAC1:
        case CGAC2:
        case CGAC3:
            width = 320;
            height = 200;
            break;
        case CGAHI:
            width = 640;
            height = 200;
            break;
        }
        break;
    }

    // Create the window with with the specified dimensions
    initwindow( width, height );
    if (!valid)
    {
        //pWndData = BGI__GetWindowDataPtr( );
        pWndData->error_code = grInvalidDriver;

    }
}

void graphdefaults()
{
    // Set fill style, fill pattern, text font, text justifiction:
    set_defaults();

    // Reset the default palette colors:
    for (int i = 0; i <= MAXCOLORS; i++) { 
        current_palette.colors[i] = (signed char) i;
        BGIpalette[i] = BGIcolor[i];
    }
    SetPaletteEntries(hPalette, BG, MAXCOLORS+1, BGIpalette);
    RealizePalette(hdc[0]);
    for (int j = 0; j <= MAXCOLORS; j++)
        correctpalette(j);

    // Reset the background color and drawing color.
    setcolor(WHITE);
    setbkcolor(BLACK);

    // Set viewport to entire screen:
    SelectClipRgn(hdc[0], NULL);
    SelectClipRgn(hdc[1], NULL);
    SetViewportOrgEx(hdc[0], 0, 0, NULL);
    SetViewportOrgEx(hdc[1], 0, 0, NULL);
    SelectObject(hdc[0], hBrush[fill_settings.pattern]);
    SelectObject(hdc[1], hBrush[fill_settings.pattern]);

    // Move current position to 0,0:
    moveto(0,0);
}

void restorecrtmode() {}

// getactivepage and getvisualpage added by M. Main -- Nov 25, 1998
int getactivepage( )
{
    return active_page;
}
int getvisualpage( )
{
    return visual_page;
}

void drawImage(char *rutaImagen, int x, int y){
	
	const size_t cSize = strlen(rutaImagen)+1;
    wchar_t wRutaImagen[cSize];
    mbstowcs (wRutaImagen, rutaImagen, cSize);
    
	/*	
	HDC memDC;
	HBITMAP imagen = (HBITMAP)LoadImage(NULL, rutaImagen, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);		
	BITMAP bmp;
	GetObject(imagen, sizeof(BITMAP), (LPSTR) &bmp);
	
    memDC = CreateCompatibleDC(hdc[0]);
    SelectObject(memDC, imagen);
    BitBlt(hdc[0], x, y, bmp.bmWidth , bmp.bmHeight, memDC, 0, 0, SRCCOPY);
    BitBlt(hdc[1], x, y, bmp.bmWidth , bmp.bmHeight, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC); 
	DeleteObject(imagen);     
	*/
	//++++++++++++++++++++++++++++
	
	
	Gdiplus::Image* image = Gdiplus::Image::FromFile( wRutaImagen );		
	Gdiplus::Graphics graphics(hdc[0]); 
	
	graphics.DrawImage(image, x, y);		
	delete image; 
	//image = 0;   
	
	
}

void drawImageLimit(char *rutaImagen, int x, int y, int w, int h){
	
	const size_t cSize = strlen(rutaImagen)+1;
    wchar_t wRutaImagen[cSize];
    mbstowcs (wRutaImagen, rutaImagen, cSize);
    
	/*
	HDC memDC;
	HBITMAP imagen = (HBITMAP)LoadImage(NULL, rutaImagen, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);		
	
    memDC = CreateCompatibleDC(hdc[0]);
    SelectObject(memDC, imagen);
    BitBlt(hdc[0], x, y, w, h, memDC, 0, 0, SRCCOPY);
    BitBlt(hdc[1], x, y, w, h, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC); 
	DeleteObject(imagen);  
	*/
	Gdiplus::Image* image = Gdiplus::Image::FromFile( wRutaImagen );		
	Gdiplus::Graphics graphics(hdc[0]); 	
	graphics.DrawImage(image, x, y, w, h);		
	
	delete image; 
}

void swapbuffers(){
	int oldv = getvisualpage( );
    int olda = getactivepage( );
    setvisualpage(olda);
    setactivepage(oldv);
}


