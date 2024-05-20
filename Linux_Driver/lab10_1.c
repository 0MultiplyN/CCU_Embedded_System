#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define CLOCK_DELAY 5
#define AUTO_MODE {0,1,0,0,0,0,0,0} //0x40  (Command: auto increment mode)
#define FIXED_MODE {0,1,0,0,0,1,0,0} //0x44 (Command: fixed position mode)
#define START_ADDR {1,1,0,0,0,0,0,0}  //0xC0 (Command to set the starting position at first digit)
#define GPIO_CLK_PATH "/sys/class/gpio/gpio25/value"
#define GPIO_DIO_PATH "/sys/class/gpio/gpio24/value"

int Digits[17][8] = {
    //H,G,F,E,D,C,B,A
     {0,0,1,1,1,1,1,1}, //0
     {0,0,0,0,0,1,1,0}, //1
     {0,1,0,1,1,0,1,1}, //2
     {0,1,0,0,1,1,1,1}, //3
     {0,1,1,0,0,1,1,0}, //4
     {0,1,1,0,1,1,0,1}, //5
     {0,1,1,1,1,1,0,1}, //6
     {0,0,0,0,0,1,1,1}, //7
     {0,1,1,1,1,1,1,1}, //8
     {0,1,1,0,1,1,1,1}, //9
     {0,1,1,1,0,1,1,1}, //A
     {0,1,1,1,1,1,0,0}, //b
     {0,0,1,1,1,0,0,1}, //C
     {0,1,0,1,1,1,1,0}, //d
     {0,1,1,1,1,0,0,1}, //E
     {0,1,1,1,0,0,0,1},  //F
     {0,0,0,0,0,0,0,0}
};

FILE* clk = NULL;
FILE* dio = NULL;

void writeDIO(int v) {
    dio = fopen(GPIO_DIO_PATH, "w");
    fprintf(dio, "%d", v);
    fclose(dio);
}

void writeCLK(int v) {
    clk = fopen(GPIO_CLK_PATH, "w");
    fprintf(clk, "%d", v);
    fclose(clk);
}

void initialize() {  
    // Setup GPIO files for CLK pin
    clk = fopen("/sys/class/gpio/export", "w");
    fprintf(clk, "%d", 25);
    fclose(clk);
    clk = fopen("/sys/class/gpio/gpio25/direction", "w");
    fprintf(clk, "out");
    fclose(clk);

    // Setup GPIO files for DIO pin
    dio = fopen("/sys/class/gpio/export", "w");
    fprintf(dio, "%d", 24);
    fclose(dio);
    dio = fopen("/sys/class/gpio/gpio24/direction", "w");
    fprintf(dio, "out");
    fclose(dio);

    // Set initial state for CLK and DIO pins
    writeCLK(0); // CLK low
    writeDIO(0); // DIO low
}

void end() {
    // Close GPIO files
    clk = fopen("/sys/class/gpio/unexport", "w");
    fprintf(clk, "%d", 25);
    fclose(clk);
    dio = fopen("/sys/class/gpio/unexport", "w");
    fprintf(dio, "%d", 24);
    fclose(dio);
}

void startWrite(){
    //send start signal to TM1637
    writeCLK(1);    //CLK high
    writeDIO(1);    //DIO high
    usleep(CLOCK_DELAY);
    writeDIO(0);    //DIO low
}

void stopWrite() {
    //send stop signal to TM1637
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
    writeDIO(0);    //DIO low
    usleep(CLOCK_DELAY);
    writeCLK(1);    //CLK high
    usleep(CLOCK_DELAY);
    writeDIO(1);    //DIO high
}

void writeByte(int Byte[8]) {
    for (int i = 7; i >= 0; i--) {        
        writeCLK(0);    //CLK low
        //Set data bit                                 //H,G,F,E,D,C,B,A                    
        if (Byte[i] == 1)       
            writeDIO(1);
        else 
            writeDIO(0);
        usleep(CLOCK_DELAY);
        writeCLK(1);    //CLK high
        usleep(CLOCK_DELAY);
    }
    /*Waiting for ACK */
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
    writeCLK(1);    //CLK high
    usleep(CLOCK_DELAY);
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
}

void showNum(int num) {
    if (num >= 0 && num <= 9999) {
        int mode[8] = AUTO_MODE;
        int addr[8] = START_ADDR;
        int bright[8] = DEFAULT_BRIGHTNESS;

        startWrite(); 
        writeByte(mode); //mode = {0,1,0,0,0,0,0,0} (0x40 Command: auto increment mode �s���J�Ҧ�)
        stopWrite();

        startWrite();
        writeByte(addr); //addr = {1,1,0,0,0,0,0,0} (0xC0 
        writeByte(Digits[num / 1000]);     
        writeByte(Digits[num / 100 % 10]);
        writeByte(Digits[num / 10 % 10]);
        writeByte(Digits[num % 10]);
        stopWrite();

        startWrite();
        writeByte(bright); //bright = {1,0,0,0,1,0,0,1} (0x88+1 Set brightness)   
        stopWrite();
    }
    else {
        printf("Error: Exceed Limit\n");
    }
}

void clear() {
     
    int mode[8] = AUTO_MODE;
    int addr[8] = START_ADDR;
    int bright[8] = DEFAULT_BRIGHTNESS;

    startWrite(); 
    writeByte(mode); //mode = {0,1,0,0,0,0,0,0} (0x40 Command: auto increment mode �s���J�Ҧ�)
    stopWrite();

    startWrite();
    writeByte(addr); //addr = {1,1,0,0,0,0,0,0} (0xC0 
    writeByte(Digits[17]);     
    writeByte(Digits[17]);
    writeByte(Digits[17]);
    writeByte(Digits[17]);
    stopWrite();

    startWrite();
    writeByte(bright); //bright = {1,0,0,0,1,0,0,1} (0x88+1 Set brightness)   
    stopWrite();
    
}

int main(void)
{
    initialize();
    
    int num = 1000;
    while (num >= 0) {
        showNum(num);
        num -= 100;
        sleep(1); // wait for 1 second
    }
    clear();
   
    end();
    exit(0);
}

