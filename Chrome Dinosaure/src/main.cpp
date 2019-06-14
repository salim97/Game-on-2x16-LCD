#include <Arduino.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Wire.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons()
{
        adc_key_in = analogRead(0); // read the value from the sensor
        if (adc_key_in > 850) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
        if (adc_key_in < 50) return btnRIGHT;
        if (adc_key_in < 250) return btnUP;
        if (adc_key_in < 300) return btnDOWN;
        if (adc_key_in < 500) return btnLEFT;
        if (adc_key_in < 850) return btnSELECT;
        return btnNONE; // when all others fail, return this...
}

int _map[2][16];
struct player
{
        int x;
        int y;
        int score;
        char symbol;
};
typedef struct player Player;
Player salim;
int len, x, y;
void printAllLCD();
void buildMap();
void setup() {
        randomSeed(analogRead(1)); // we need this line to generate random numbers
        delayMicroseconds(random(0, 1000)); // wait random delay before starting game, in order to generate random variables
        buildMap(); // assign obstacel to map
        // init
        Serial.begin(115200);
        lcd.begin(16, 2);           // start the library
        //player....
        salim.x = 0;
        salim.y = 1;
        salim.score = 0;
        salim.symbol = 'X';
        printAllLCD();
}

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 500L; // delay between updates, in milliseconds
bool gameOver = false;
void loop() {
        while(gameOver == true) delay(10);
        if(read_LCD_buttons() == btnUP)
                salim.y = 0;
        if(read_LCD_buttons() == btnDOWN)
                salim.y = 1;
        if(read_LCD_buttons() == btnSELECT)
        {
                lcd.clear();
                lcd.setCursor ( 0, 1 );
                lcd.print("Score: "+String(salim.score));
                while(read_LCD_buttons() == btnSELECT) delay(10);
                printAllLCD();
        }

        if(read_LCD_buttons() == btnRIGHT)
        {

                salim.score += 16 - salim.x;
                salim.x = 0;
                buildMap();
                lastConnectionTime = millis();
                printAllLCD();
                while(read_LCD_buttons() == btnRIGHT) delay(10);
                lastConnectionTime = millis();
        }

        if (millis() - lastConnectionTime > postingInterval)
        {
                lastConnectionTime = millis();

                if(salim.x < 15 ) salim.x++;
                else
                {
                        salim.x = 0;
                        buildMap();
                }


                salim.score++;
                printAllLCD();

        }
}

void buildMap()
{
        x = random(2, 10);
        y = random(0, 2);
        len = random(1, 4);
        //fill the martix
        for(int i = 0; i < 16; i++)
        {
                for(int j = 0; j < 2; j++)
                        _map[j][i] = 0;
        }
        //fill matrix with opstacle
        for(int i = 0; i < len; i++ )
        {
                _map[y][x] = 1;
                x++;
        }
}

void printAllLCD()
{
        lcd.clear();
        if(_map[salim.y][salim.x] == 1)
        {
                gameOver = true;

                lcd.setCursor ( 0, 0 );
                lcd.print("Game is Over");
                lcd.setCursor ( 0, 1 );
                lcd.print("Score: "+String(salim.score));
                return;

        }
        for(int i = 0; i < 16; i++)
        {
                for(int j = 0; j < 2; j++)
                {
                        lcd.setCursor ( i, j );
                        if(_map[j][i] == 0) lcd.print(' ');
                        else lcd.print('#');
                }
        }

        lcd.setCursor(salim.x, salim.y);
        lcd.print(salim.symbol);
}
