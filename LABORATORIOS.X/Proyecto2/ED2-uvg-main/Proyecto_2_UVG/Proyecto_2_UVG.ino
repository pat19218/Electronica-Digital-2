//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   Modificaciones y adaptación: Diego Morales
   IE3027: Electrónica Digital 2 - 2021
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <SPI.h>
#include <SD.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "font.h"
#include "lcd_registers.h"
#include "bitmaps.h"
#include "pitches.h"

#define loserp2 "loserp2.bmp"
#define loserp1 "loserp1.bmp"
#define bmp_offset 150
#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define PAD_WIDTH 30
#define BUZZER_PASIVO 40

const int chipSelect = PA_3; //cs PIN

int game_loops = 0;
int game_speed = 10;

String loser[] = {"L", "O", "S", "E", "R"};
String palabra1;          //variable en la q ira la palabra
String palabra2;          //variable en la q ira la palabra
byte cuenta = 0;
byte espacio = 5;
bool turn = 0;
int x = 10;
int y = 10;
int speed_x = 3;
int speed_y = 2;

int player1 = 0;
int player2 = 0;

int points_1 = 0;
int points_2 = 0;
Sd2Card card;
SdVolume volume;
SdFile root;
File dataFile;

int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

// notes in the melody:
int melody[] = {NOTE_F2, NOTE_C6, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_E1, NOTE_B1, NOTE_E1, NOTE_B1, NOTE_E1, NOTE_B1};

int noteDurations[] = {
  2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1
}; // 11 start screen  notes
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);
void LoadImage(File& file);
void draw_player(unsigned int x, unsigned int y);
void draw_field();
void draw_score();
void draw_ball();
void end_game();

//***************************************************************************************************************************************
// Initialization
//***************************************************************************************************************************************

void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  Serial3.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Start");
  LCD_Init();
  LCD_Clear(0x00);

  // SD Init
  pinMode(PA_3, OUTPUT);            // change this to 53 on a mega
  pinMode(BUZZER_PASIVO, OUTPUT);
  FillRect(160, 5 , 5, 230, 0xFFFFF);

  for (int thisNote = 0; thisNote < 10; thisNote++) {
    int noteDuration = 1500 / noteDurations[thisNote];
    tone(BUZZER_PASIVO, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    noTone(BUZZER_PASIVO);                // stop the tone playing
  }

}
//***************************************************************************************************************************************
// Loop
//***************************************************************************************************************************************
void loop() {

  while (points_2 <= 5 && points_1 <= 5) {
    game_loops += 1;
    if (game_speed <= 1) {
      game_speed = 1;
    } else if (game_speed > 10) {
      game_speed = 10;
    } else {
      game_speed = 2000 / game_loops;
    }

    Serial3.write('h');
    while (Serial3.available() && (cuenta < 3)) {   //Confirmando que la comunicación Serial esté disponible para hacer la lectura del mensaje
      delay(8);                    //para dar estabilidad al programa
      char c = Serial3.read();
      palabra1 += c;
      cuenta++;
    }
    cuenta = 0;
    delay(8);
    Serial3.write('t');
    while (Serial3.available() && (cuenta < 3)) {  //Confirmando que la comunicación Serial esté disponible para hacer la lectura del mensaje
      delay(8);                    //para dar estabilidad al programa
      char c = Serial3.read();
      palabra2 += c;
      cuenta++;
    }

    player1 = palabra1.toInt();
    player2 = palabra2.toInt();

    draw_player(5, player1);
    draw_player(315, player2);
    draw_field();
    draw_score();

    draw_ball();

    cuenta = 0;
    palabra1 = "";
    palabra2 = "";

    if (turn) {
      int noteDuration = 1500 / noteDurations[espacio];
      tone(BUZZER_PASIVO, melody[espacio], noteDuration);
      turn = 0;
      espacio++;
      if (espacio > 11) {
        espacio = 0;
      }
    } else {
      noTone(BUZZER_PASIVO);                // stop the tone playing
      turn = 1;
    }
  }

  end_game();

  while (true) {
    beep(note_a, 500);
    beep(note_a, 500);
    beep(note_a, 500);
    beep(note_ff, 350);
    beep(note_cH, 150);
    beep(note_a, 500);
    beep(note_ff, 350);
    beep(note_cH, 150);
    beep(note_a, 650);
    delay(150);
    //end of first bit
    beep(note_eH, 500);
    beep(note_eH, 500);
    beep(note_eH, 500);
    beep(note_fH, 350);
    beep(note_cH, 150);
    beep(note_gS, 500);
    beep(note_ff, 350);
    beep(note_cH, 150);
    beep(note_a, 650);
    delay(150);
    //end of second bit...
    beep(note_aH, 500);
    beep(note_a, 300);
    beep(note_a, 150);
    beep(note_aH, 400);
    beep(note_gSH, 200);
    beep(note_gH, 200);
    beep(note_fSH, 125);
    beep(note_fH, 125);
    beep(note_fSH, 250);
    delay(250);
    beep(note_aS, 250);
    beep(note_dSH, 400);
    beep(note_dH, 200);
    beep(note_cSH, 200);
    beep(note_cH, 125);
    beep(note_b, 125);
    beep(note_cH, 250);
    delay(250);
    beep(note_ff, 125);
    beep(note_gS, 500);
    beep(note_ff, 375);
    beep(note_a, 125);
    beep(note_cH, 500);
    beep(note_a, 375);
    beep(note_cH, 125);
    beep(note_eH, 650);

    beep(note_aH, 500);
    beep(note_a, 300);
    beep(note_a, 150);
    beep(note_aH, 400);
    beep(note_gSH, 200);
    beep(note_gH, 200);
    beep(note_fSH, 125);
    beep(note_fH, 125);
    beep(note_fSH, 250);
    delay(250);
    beep(note_aS, 250);
    beep(note_dSH, 400);
    beep(note_dH, 200);
    beep(note_cSH, 200);
    beep(note_cH, 125);
    beep(note_b, 125);
    beep(note_cH, 250);
    delay(250);
    beep(note_ff, 250);
    beep(note_gS, 500);
    beep(note_ff, 375);
    beep(note_cH, 125);
    beep(note_a, 500);
    beep(note_ff, 375);
    beep(note_cH, 125);
    beep(note_a, 650);
  };
}


//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k + 1]);
      LCD_DATA(bitmap[k]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  }
  else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }
  }
  digitalWrite(LCD_CS, HIGH);
}


//********************************
//
//
//       Game specific functions
//
//
//********************************


void draw_player(unsigned int x, unsigned int y) {
  FillRect(x, 0, 5, y, 0x0);
  FillRect(x, y, 5, PAD_WIDTH, 0xFFFFF);
  FillRect(x, y + PAD_WIDTH, 5, 20, 0x0);


  //  FillRect(x,y + PAD_WIDTH/3,5, PAD_WIDTH/3, 0x9922); // Debug
}

void draw_field() {
  FillRect(160, 5 , 5, 230, 0xFFFFF);
}

void draw_score() {

  for (int a = 0; a < points_1; a++) {
    LCD_Print(loser[a], 170 + a * 10 , 5, 1, 0x44444, 0x0000);
  }

  for (int b = 0; b < points_2; b++) {
    LCD_Print(loser[b], 100 + b * 10 , 5, 1, 0x88888, 0x0000);
  }
}

void draw_ball() {
  if (game_loops % game_speed == 0) {
    FillRect(x, y, 5, 5, 0x0);

    x += speed_x;
    y += speed_y;

    FillRect(x, y, 5, 5, 0xFFFFF);

    // Handle collisions with the edge
    if (y <= 0 || y >= 235) {
      speed_y = -speed_y;
    }

    // Handle collisions with pads

    if (x <= 10 || x >= 310) {
      speed_x = -speed_x;
      // change ball direction depending on bounce location
      if (y >= player1 && y <= player1 + PAD_WIDTH * (1 / 3)) {
        speed_y -= 1;
      } else if (y >= player1 + PAD_WIDTH * (1 / 3) && y <= player1 + PAD_WIDTH * (2 / 3)) {
        speed_y = 1;
      } else if (y >= player1 + PAD_WIDTH * (2 / 3) && y <= player1 + PAD_WIDTH) {
        speed_y += 1;
      } else {

        // Count pointa
        if (x <= 10) {
          points_2 += 1;
          for (byte e = 0; e <= 9; e++) {
            delay(100);
            LCD_Sprite(5, player1, 30, 30, explosion, 9, e, 0, 0);
          }
          FillRect(5, player1, 30, 30, 0x0);

        } else if (x >= 310) {
          points_1 += 1;
          for (int e = 0; e <= 9; e++) {
            delay(100);
            LCD_Sprite(310, player2, 30, 30, explosion, 9, e, 1, 0);
          }
          FillRect(310, player2, 30, 30, 0x0);
        }
        FillRect(x, y, 5, 5, 0x0);
        x = 155;
        speed_y = 3;
      }
    }
  }
}

void end_game() {
  Serial.println("End game");

  SPI.setModule(0);
  if (!SD.begin(PA_3)) {
    Serial.println("Error reading SD");
  }
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  Serial.println("\nFiles found on the card: ");
  root.openRoot(volume);

  if (points_1 >= 5) dataFile = SD.open(loserp2);
  if (points_2 >= 5) dataFile = SD.open(loserp1);

  LoadImage(dataFile);
}

void LoadImage(File& file) {
  SPI.setModule(0);
  unsigned char buf[SCREEN_WIDTH * 2];
  // Remove BMP offset
  file.read(buf, bmp_offset);
  for (int y = 0; y < SCREEN_HEIGHT && file.available(); y++) {
    file.read(buf, SCREEN_WIDTH * 2);
    LCD_Bitmap(0, SCREEN_HEIGHT - y, SCREEN_WIDTH, 1, buf);
  }
  Serial.println("done displaying image");
}

void beep(int note, int duration)
{
  tone(BUZZER_PASIVO, note, duration / 2);
  delay(duration / 2);
  noTone(BUZZER_PASIVO);
  delay(duration / 2 + 20);
}
