#include "ws2812.h"
uint dma_chan;
uint pio_offset;
PIO pio = pio0;
int sm = 0;
//LED
uint32_t *led_buf;
uint pattern = 2, counter = 0, colorcounter = 0;
uint8_t Green = 0, Red = 0, Blue = 0;

/* Parameter welche im Menu angezeigt werden */
#define NUMLED 0
#define HELLIGKEIT 1
#define SPEED 2
#define PROG 3
#define SET 4
#define GREEN 5
#define RED 6
#define BLUE 7

char Para_txt[8][5]={
	"NrLED", // Anzahl der LEDs
	"Helkt", // Helligkeit
	"Speed", // Geschwindigkeit des Farbwechsels
	"Progr", // Das Pattern
	"Set  ", // Uebernahme der Werte
	"Gruen", // Helligkeit des Gruenanteils in einem der Pattern
	"Rot  ", // Helligkeit des Rotanteils in einem der Pattern
	"Blau ", // Helligkeit des Blauanteils in einem der Pattern
};

/* Die (start-) Werte zu den Parametern in Para_txt*/
uint8_t Para_wert[] = {16, 0x1F, 32, 0, 0, 0x09, 0x09, 0x09};

/* Der aktuell ausgewaehlte Parameter */
uint8_t selection = 0;

void write_menu(){
	for(int i=0; i<8; i++){
		bool sel = false;
		if(i==selection) sel = true;
		uint8_t pos[] = {i, 0};
		Display_WriteText(pos, Para_txt[i], 5, sel);
		pos[1] = 6;
		char tmp[4];
		sprintf(tmp,"0x%02x",Para_wert[i]);
		Display_WriteText(pos, tmp, 4, sel);
	}
}

void write_pattern(){
	uint summand = Para_wert[HELLIGKEIT] / 21;
	uint8_t left_bits = 0, middle_bits = 0, right_bits = Para_wert[HELLIGKEIT];
	uint i = 0;
	for(int k=0; k<8; k++){
		for(int l=0; l<8; l++){
			led_buf[PATTERNS[pattern][k][l]] = 0|(left_bits<<24)|(middle_bits<<16)|(right_bits<<8);
			if(i<21 || i>62){
				middle_bits = middle_bits + summand;
				right_bits = right_bits - summand;
			}
			else if(i<42){
				middle_bits = middle_bits - summand;
				left_bits = left_bits + summand;
			}
			else if(i<63){
				left_bits = left_bits - summand;
				right_bits = right_bits + summand;
			}
			i=i+1;
		}
	}
}
void shift_r_pattern(){
	uint8_t tmp = PATTERNS[pattern][7][7];
	for(int i=7; i>=0; i--){
		if(i<7){
			PATTERNS[pattern][i+1][0] = PATTERNS[pattern][i][7];
		}
		for(int k=7; k>=1; k--){
			PATTERNS[pattern][i][k] = PATTERNS[pattern][i][k-1];
		}
	}
	PATTERNS[pattern][0][0] = tmp;
	write_pattern();
}
void shift_pattern(){
	uint8_t tmp = PATTERNS[pattern][0][0];
	for(int i=0; i<8; i++){
		if(i>0){
			PATTERNS[pattern][i-1][7] = PATTERNS[pattern][i][0];
		}
		for(int k=0; k<7; k++){
			PATTERNS[pattern][i][k] = PATTERNS[pattern][i][k+1];
		}
	}
	PATTERNS[pattern][7][7] = tmp;
	write_pattern();
}

void writeRGB(){
	for(int i=0; i<Para_wert[NUMLED]; i++){
		led_buf[i] = Para_wert[GREEN]<<24 | Para_wert[RED]<<16 | Para_wert[BLUE]<<8;
	}
}
void change_grbbuf(){
	if(colorcounter < 1){ Green = Para_wert[HELLIGKEIT]-1; Red = 0; Blue = 0;}
	else if(colorcounter < Para_wert[HELLIGKEIT] && Red < Para_wert[HELLIGKEIT]) Red++;
	else if(colorcounter < 2*Para_wert[HELLIGKEIT] && Green > 0) Green--;
	else if(colorcounter < 3*Para_wert[HELLIGKEIT] && Blue < Para_wert[HELLIGKEIT]) Blue++;
	else if(colorcounter < 4*Para_wert[HELLIGKEIT] && Red > 0) Red--;
	else if(colorcounter < 5*Para_wert[HELLIGKEIT] && Green < Para_wert[HELLIGKEIT]) Green++;
	else if(colorcounter < 6*Para_wert[HELLIGKEIT] && Blue > 0) Blue--;
	for(int i=0; i<Para_wert[NUMLED]; i++){
		led_buf[i] = (Green << 24) | (Red << 16) | (Blue << 8);
	}
	colorcounter++;
	if(colorcounter >= 6*Para_wert[HELLIGKEIT]) colorcounter = 0;
}
void make_pattern(){
	if(Para_wert[PROG]>15 && Para_wert[PROG]<31){
		uint8_t byte_l = Para_wert[HELLIGKEIT], byte_m = Para_wert[HELLIGKEIT], byte_r = 0;
		int z = 7;
		for(int i=0; i<Para_wert[NUMLED]; i++){
			led_buf[z] = (byte_l << 24) | (byte_m << 16) | (byte_r << 8);
			z++;
			if(z>Para_wert[NUMLED]-1) z = 0;
			if(i < Para_wert[NUMLED]/3){
				byte_m = byte_m - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_r = byte_r + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
			else if(i < (Para_wert[NUMLED]/3)*2){
				byte_l = byte_l - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_m = byte_m + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
			else{
				byte_r = byte_r - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_l = byte_l + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
		}
	}
	else{
		uint8_t byte_l = Para_wert[HELLIGKEIT], byte_m = 0, byte_r = 0;
		int z = 7;
		for(int i=0; i<Para_wert[NUMLED]; i++){
			led_buf[z] = (byte_l << 24) | (byte_m << 16) | (byte_r << 8);
			z++;
			if(z>Para_wert[NUMLED]-1) z = 0;
			if(i < Para_wert[NUMLED]/3){
				byte_l = byte_l - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_m = byte_m + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
			else if(i < (Para_wert[NUMLED]/3)*2){
				byte_m = byte_m - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_r = byte_r + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
			else{
				byte_r = byte_r - Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
				byte_l = byte_l + Para_wert[HELLIGKEIT]/(Para_wert[NUMLED]/3);
			}
		}
	}
}
void shift_grbbuf(){
	uint32_t tmp = led_buf[0];
	for(int i=0; i<Para_wert[NUMLED]-1; i++) led_buf[i] = led_buf[i+1];
	led_buf[Para_wert[NUMLED]-1] = tmp;
}
void buf_init(){
	led_buf = realloc(led_buf, Para_wert[NUMLED]*sizeof(uint32_t));
	if(Para_wert[PROG]>0)make_pattern();
	else change_grbbuf();
	dma_channel_set_trans_count(dma_chan, Para_wert[NUMLED], true);
}
void dma_handle(){
	dma_hw->ints0 = 1u << dma_chan;// clear the irq
	pio_sm_exec(pio, sm, ws2812_set_bits(pio_offset));
	dma_channel_set_read_addr(dma_chan, &led_buf[0], true);
	if(counter >= Para_wert[SPEED]){
		counter = 0;
		if(Para_wert[PROG]<1)change_grbbuf();
		else if(Para_wert[PROG]<31)shift_grbbuf();
		else writeRGB();
	}
	counter++;
}
void set_direction(uint gpio, uint32_t events){
		if(gpio == BUTTON_D){
			if(selection < 7) selection++;
			else selection = 0;
		}
		if(gpio == BUTTON_U){
			if(selection > 0) selection--;
			else selection = 7;
		}
		if(gpio == BUTTON_R){
			if(Para_wert[selection] < 0xEF) Para_wert[selection] = Para_wert[selection] + 16;
			else if(Para_wert[selection] < 0xFF) Para_wert[selection] = Para_wert[selection] + 1;
			if(selection==4){
				buf_init();
				Para_wert[SET] = 0;
			}
		 }
		if(gpio == BUTTON_L){
			if(Para_wert[selection] > 0x10 && selection>0){
				Para_wert[selection] = Para_wert[selection] - 16;
			}
			else if(Para_wert[selection] > 0x0) Para_wert[selection] = Para_wert[selection] - 1;
		}
		write_menu();
}
void mygpio_init() {
	if(true){//buttons setup
		gpio_init(BUTTON_R);
		gpio_set_pulls(BUTTON_R, 0, 1);
		gpio_set_irq_enabled_with_callback(BUTTON_R, 0x08, true, &set_direction);
		gpio_init(BUTTON_L);
		gpio_set_pulls(BUTTON_L, 0, 1);
		gpio_set_irq_enabled_with_callback(BUTTON_L, 0x08, true, &set_direction);
		gpio_init(BUTTON_U);
		gpio_set_pulls(BUTTON_U, 0, 1);
		gpio_set_irq_enabled_with_callback(BUTTON_U, 0x08, true, &set_direction);
		gpio_init(BUTTON_D);
		gpio_set_pulls(BUTTON_D, 0, 1);
		gpio_set_irq_enabled_with_callback(BUTTON_D, 0x08, true, &set_direction);
	}
}
int main() {
    stdio_init_all();
    mygpio_init();
    Display_init();
    led_buf = calloc(Para_wert[NUMLED], sizeof(uint32_t));
    buf_init();
    write_menu();

	//set up PIO
    pio_offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, pio_offset, WS2812_PIN, 800000);// 1,25µs/bit
	//setup the dma
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config dma_cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_32);
    channel_config_set_write_increment(&dma_cfg, false);
    channel_config_set_dreq(&dma_cfg, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan, &dma_cfg,
		&pio0_hw->txf[0],	//destination
		NULL,				//source
		Para_wert[NUMLED],			//counts before call irq
		false				//don't start yet
	);
	dma_channel_set_irq0_enabled(dma_chan, true);
	irq_set_exclusive_handler(DMA_IRQ_0, dma_handle);
	irq_set_enabled(DMA_IRQ_0, true);
	//start the DMA tu pull data from led_buf to pin2
	//change_grbbuf();
	dma_handle();

	//do somthing
    while (1) {
		sleep_ms(500);
    }
}
