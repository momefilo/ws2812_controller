#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define SPI_PORT spi1
#define PIN_SCK	10
#define PIN_SDA	11
#define PIN_A0  12
#define PIN_CS  13
#define PIN_RST 28

/* inits the display
 */
void Display_init();

/* color = {0x0..0xF foregroundRed, Green, blue, backgroundRed, Green, blue}
 * clears the screen
 */
void Display_SetColor(uint8_t *color);

/* color = {0x0..0xF foregroundRed, Green, blue)
 */
void Display_SetFGround(uint8_t *color);
void Display_SetBGround(uint8_t *color);

/* pos= {Zeile 0..7, Spalte 0..9}
 * if sel then FGround change
 */
void Display_WriteText(uint8_t *pos, char *text, int len, bool sel);
