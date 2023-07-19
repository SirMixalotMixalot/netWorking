#include <stddef.h>
#include <windows.h>
#include <iostream>
#include <string_view>
#include <stdint.h>
#ifndef CONSOLE_LIB 
#define CONSOLE_LIB

#ifndef EVENT_LIST_BUFFER_SIZE
    #define EVENT_LIST_BUFFER_SIZE 20
#endif
typedef struct {
    INPUT_RECORD inputRecord[EVENT_LIST_BUFFER_SIZE];
    int length;
} EventList;
typedef enum {
    Bold = 1,
    Underline = 4,
    Negative = 7,
    NoStyle = 0

}STYLE;
enum class TextStyle {
    Bold = 1,
    Underline = 4,
    Negative = 7,
    NoStyle = 0
};
typedef struct {
    int64_t x;
    int64_t y;
} ConsolePoint;
typedef enum {
    UP,
    DOWN,
    LEFT, 
    RIGHT,
    INVALID_DIR,
}ConsoleDirection;
typedef enum {
    NoSimpleColour = 0,
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    DefaultForeground = 39,
    BGGreen = 42,
    BGWhite = 47,
    DefaultBackground = 49,
} ConsoleColourBasic;
struct ConsoleColour {
    int red = 0;
    int green = 0;
    int blue = 0;
    ConsoleColourBasic fg = DefaultForeground;
    ConsoleColourBasic bg = DefaultBackground;
    ConsoleColour() = default;
    ConsoleColour(ConsoleColourBasic f) : fg{f} {

    }

};
enum class ForeGroundColor {
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    DefaultForeground = 39,

};
enum class BackGroundColor {
    Green = 42,
    White = 47,
    DefaultBackground = 49,

};
struct ConsoleStyle {
    ForeGroundColor fg = ForeGroundColor::DefaultForeground;
    BackGroundColor bg = BackGroundColor::DefaultBackground;
    TextStyle ts = TextStyle::NoStyle;

    ConsoleStyle() = default;
    ConsoleStyle(ForeGroundColor fc) : fg{fc} {}
    
};
ConsoleColour default_console_colour();
int prepare_console(uint64_t output_flags, uint64_t input_flags);
int write_styled_and_coloured_text(const wchar_t * text, ConsoleColour colour, STYLE s);
int write_coloured_text(const wchar_t* text, ConsoleColour col);
int write_styled_text(const wchar_t* text, STYLE s);
void write_styled_text(std::ostream& out, const char* text, ConsoleStyle cs);
EventList get_event_list();
int move_cursor(ConsoleDirection d, uint32_t length);

int set_cursor_position(ConsolePoint p); // 0 means no error
void hide_cursor();
void save_cursor_position();
void restore_cursor_last_saved_position();
void scroll_up(uint8_t n);
void clear_screen();
void set_window_title(const wchar_t* title);
void scroll_down(uint8_t n);
void write_text(const wchar_t* t);
void revert_console_attributes();
#endif // CONSOLE_LIB