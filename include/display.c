#include "display.h"
#include "fontNew.h"

uint8_t Width = 160;
uint8_t Height = 128;
uint8_t FGround[]={0x0, 0xF, 0x4};
uint8_t BGround[]={0x4, 0x0, 0xF};

static inline void cs_select() {
	asm volatile("nop \n nop \n nop");
	gpio_put(PIN_CS, 0);
	asm volatile("nop \n nop \n nop");
}
static inline void cs_deselect() {
	asm volatile("nop \n nop \n nop");
	gpio_put(PIN_CS, 1);
	asm volatile("nop \n nop \n nop");
}
void write_cmd(uint8_t *cmd, int len) {
	cs_select();
	spi_write_blocking(SPI_PORT, cmd, 1);
	if(len > 1){
		gpio_put(PIN_A0, 1);
		spi_write_blocking(SPI_PORT, cmd + (uint8_t)1, len - 1);
		gpio_put(PIN_A0, 0);
	}
	cs_deselect();
}
void set_col(int start, int end){
	uint8_t cmd[5];
	cmd[0] = 0x2A;
	cmd[1] = 0x00;
	cmd[2] = start;
	cmd[3] = 0x00;
	cmd[4] = end;
	write_cmd(cmd, 5);
}
void set_row(int start, int end){
	uint8_t cmd[5];
	cmd[0] = 0x2B;
	cmd[1] = 0x00;
	cmd[2] = start;
	cmd[3] = 0x00;
	cmd[4] = end;
	write_cmd(cmd, 5);
}
void fillbg(){
	int len = Width*Height*3/2+1;
	uint8_t area[len];
	area[0] = 0x2C;
	for(int i=1; i<len-2; i=i+3){
		area[i] = BGround[0]<<4 | BGround[1];
		area[i+1] = BGround[2]<<4 | BGround[0];
		area[i+2] = BGround[1]<<4 | BGround[2];
	 }
	set_col(0, Width-1);
	set_row(0, Height-1);
	write_cmd(area, (len));
}
void write_font16x16(uint8_t *pos, uint8_t zeichen){
	//create buffer for font
	int len = 16*16*3/2+1;
	uint8_t buf[len];
	buf[0] = 0x2C;
	int z = 1;
	//write font in buffer
	for(int i=0; i<32; i=i+1){
		for(int k=0; k<7; k=k+2){
			if(FONT16x16[zeichen*32+i] & (0x80 >> k)){
				buf[z] = FGround[0]<<4 | FGround[1];
				buf[z+1] = FGround[2]<<4;
			}
			else{
				buf[z] = BGround[0]<<4 | BGround[1];
				buf[z+1] = BGround[2]<<4;
			}
			if(FONT16x16[zeichen*32+i] & ( 0x80 >> (k+1) )){
				buf[z+1] = buf[z+1] | FGround[0];
				buf[z+2] = FGround[1]<<4 | FGround[2];
			}
			else{
				buf[z+1] = buf[z+1] | BGround[0];
				buf[z+2] = BGround[1]<<4 | BGround[2];
			}
			z=z+3;
		}
	}
	//write buffer to display
	set_col(pos[0],pos[0]+15);
	set_row(pos[1],pos[1]+15);
	write_cmd(buf,len);
}

void Display_init(){
	if(true){//spi setup
		spi_init(SPI_PORT, 100* 1000 * 1000);
		gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
		gpio_set_function(PIN_SDA, GPIO_FUNC_SPI);
		gpio_init(PIN_CS);
		gpio_set_dir(PIN_CS, GPIO_OUT);
		gpio_put(PIN_CS, 1);
		gpio_init(PIN_A0);
		gpio_set_dir(PIN_A0, GPIO_OUT);
		gpio_put(PIN_A0, 0);
		gpio_init(PIN_RST);
		gpio_set_dir(PIN_RST, GPIO_OUT);
		gpio_put(PIN_RST, 1);
	}
	sleep_ms(200);
	if(true){//display init
		uint8_t cmd[2];
		cmd[1] = 0;
		//sleep out
		cmd[0] = 0x11;
		write_cmd(cmd,1);
		sleep_ms(60);
		//display on
		cmd[0]=0x29;
		write_cmd(cmd,1);
		//12-bit colormode
		cmd[0] = 0x3A;
		cmd[1] = 0x03;
		write_cmd(cmd,2);
		sleep_ms(100);
		//flip vertikal&hoizontal
		cmd[0] = 0x36;
		cmd[1] = 0x60;
		write_cmd(cmd,2);
	}
	fillbg();
}
void Display_SetColor(uint8_t *color){
	FGround[0] = color[0];
	FGround[1] = color[1];
	FGround[2] = color[2];
	BGround[0] = color[3];
	BGround[1] = color[4];
	BGround[2] = color[5];
	fillbg();
}
void Display_SetFGround(uint8_t *color){
	FGround[0] = color[0];
	FGround[1] = color[1];
	FGround[2] = color[2];
}
void Display_SetBGround(uint8_t *color){
	BGround[0] = color[0];
	BGround[1] = color[1];
	BGround[2] = color[2];
}
void Display_WriteText(uint8_t *pos, char *text, int len, bool sel){
	if(sel){
		FGround[0] = 0xF;
		FGround[1] = 0xF;
		FGround[2] = 0x0;
	}
	for(int i=0; i<len; i++){
		uint8_t mypos[] = {(pos[1]+i)*16,pos[0]*16};
		write_font16x16(mypos, text[i]);
	}
	if(sel){
		FGround[0] = 0x0;
		FGround[1] = 0xF;
		FGround[2] = 0x4;
	}
}
