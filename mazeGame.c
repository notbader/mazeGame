#include <LiquidCrystal.h>

LiquidCrystal lcd(3, 11, 5, 6, 7, 4);

int latchPin = 8; // connected to pin 12 on shift register
int clockPin = 12; // connected to pin 11 on shift register
int dataPin = 11; // connected to pin 14 on shift register


// assign joystick pins

int joyStickPinY = 5; // vertical Pin
int joyStickPinX = 1; // horizental Pin


// game variables

int counter = 0;
int movement = 8;
int countParts = 0;
boolean gameStart = true;
boolean collideCheck = false;

//player variables

int playerRow = 3;
int playerCol = 3;
boolean playerMovement = 0;

// we create an 8 x 16 matrix to allow the LEDs to scroll, the first 8 will scroll 
// and which LEDs are going to be on next is going to be stored in the next 8
// we use our counter to move it which gives the illusion of the player moving

int matrix[8][16] = {   
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},     
};


// array created for the maze, each byte is translted into 8 bits which will light a sequence of LEDs in the same row
// so if we have 75 bytes, it will be converted to 1001011, since its is in the second place in our array
// it will light the second row, first LED is on then the next two LEDs off then one on, one off and the last two are on

byte maze[10][8] = {
  {16,75,0,164,5,68,64,65}, {74,82,17,8,73,0,65,73}, {16,75,0,164,5,68,64,65}, {74,82,17,8,73,0,65,73},
  {16,75,0,164,5,68,64,65}, {74,82,17,8,73,0,65,73}, {16,75,0,164,5,68,64,65}, {74,82,17,8,73,0,65,73},
  {0,0,0,0,0,0,0,0} };



// because we can only light one LED at a time to create the effect of all the LEDs working in the same row
// we need to create a temporary section in which to store the bytes for lighting up the LEDs then store it in our 8x16

boolean tempArray[8][8] = {   
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
  
};


// this functions process our LEDs, it treats them as bytes,and each byte corresponds to a row
// so this function checks it and then saves it in the temporary array we created and then process the next 8
// the second part of this function just lights up the lEDs and move them by 8
// then initialise the movement to make it ready for the next 8.

void moveLEDs(int part) {
byte bitConvert; // create a byte to store our data into
  // after the LEDs move by eight process the next eight and paste it in the temporary array

  for (int rows = 0; rows < 8; rows++) {
    bitConvert = maze[part][rows];
    for (byte column = 0; column < 8; column++) {
      if (bitConvert & (1<<column)) {// this moves the bits to the left, the AND symbol checks which LED shoud be on
                                   // e.g. if we have a sequence of 1001011 it will be (1001011(1<<1)) for column zero
                                   // so if (1001011(1<<1)) = 1 this is for the first bit, light LED but after two bits we get ((1001011(1<<3)) = 0
                                   // so the LED wont be lit and so on
        tempArray[rows][7 - column] = 1; // this is used to start from the right so if we send a '0' it will start from '7'
      }
      else { tempArray[rows][7 - column] = 0; }
    }
  }  

 for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
    matrix[x][y+8] = tempArray[x][y]; // light up the LEDs pasted into the temporary array
      }
    }
  }



void endingSequence() {

// these array are created to use in the function that tests the LEDs, the reason an int type was chosen instead of byte is that
// it is more straightforward and easier to handle
int matrixrow[8] =    {1,2,4,8,16,32,64,128};  
int matrixcolumn[8] = {1,2,4,8,16,32,64,128};

  for (int a=1; a<9; a++)
  {
    for (int b=1; b<9; b++)
    {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, matrixcolumn[b-1]);
  shiftOut(dataPin, clockPin, MSBFIRST, matrixrow[a-1]);
  digitalWrite(latchPin, HIGH);
  delay(50);    }
  }
  for (int a=1; a<9; a++)
  {
    for (int b=1; b<9; b++)
    {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, matrixcolumn[a-1]);
  shiftOut(dataPin, clockPin, MSBFIRST, matrixrow[b-1]);
  digitalWrite(latchPin, HIGH);
  delay(50);     }
  }
 
}

void playerMovements(int joyStickValueX, int joyStickValueY) {

// get the joystick readings

  joyStickValueX = analogRead(joyStickPinX);
  joyStickValueY = analogRead(joyStickPinY);
  
  
// when testing the joystick, at still it returned valeus between 470 and 530, so if it goes larger than 530 it was moved to the right and if less if it was moved left
// then we make sure that the LED doesn move outside the matrix "playerRow < 7", and then we check if the player LED is not going to collide with the walls
// if the above two conditions are satisfied the LED will move

  if (playerMovement == false) {
    if (joyStickValueX > 520 && playerCol < 7 && matrix[playerCol][playerRow+1] != 1) { playerRow++;  }
      else if (joyStickValueX < 480 && playerCol > 0 && matrix[playerCol][playerRow-1] != 1) { playerRow--; }
            // if bigger than 530 move up, if less then move down

    if (joyStickValueY > 530 && playerRow < 7 && matrix[playerCol+1][playerRow] != 1) { playerCol++;  }
      else if (joyStickValueY < 470 && playerRow > 0 && matrix[playerCol-1][playerRow] != 1) { playerCol--; }
      
    if (joyStickValueX > 520 || joyStickValueX < 480 || joyStickValueY > 530 || joyStickValueY < 470) {
        playerMovement = true; // this variable updates the LED when one movement has been made
      }
    }
  }

void lcdSettings() {
  

lcd.begin(16, 2);
lcd.setCursor(0,1);
lcd.print("Obsticales Game");



}

void setup() {

lcdSettings();

initialiseRegisters();


}

void loop() {
if (gameStart == true)  { // as long as the game is on keep executing the loop
  
if (movement == 8) {
    moveLEDs(countParts); // call the function that processes and moves our LEDs
        countParts++; // the maze is going to be split into parts in the moveLEDs function, we need a counter to keep counting the parts
        movement = 0;
  } 
  
  if (counter % 35 == 0) { // this controls the speed in which the LEDs move
      movement++;

    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 16; y++) {
            matrix[x][y] = matrix[x][y+1]; // move our maze LEDs to the left by one
        }
    }  
    
    playerMovement = false;
  }
    counter++;

  if (countParts == 10) { // the maze created was 10x8 so it will be divided into 10 parts and after all 10 parts have been shown end the game!
            

            gameStart = false;  
  }
  
  
    // detects collision with obstacle
    if (matrix[playerCol][playerRow] == 1 && collideCheck == false) {
        lcd.setCursor(0,1);
        lcd.print("You Lose");
        gameStart = false;
      }
      

   // this is used to clear the last player LED position. if there was an LED lighten that has the value "2" which is the value assigned to our player
   // turn off that LED. assign "0".
  for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
           if (matrix[x][y] == 2) { 
                  matrix[x][y] = 0; 
              }
            }
         }
    
 
playerMovements(0,0); // calls the function for the joystick

matrix[playerCol][playerRow] = 2; // lights up our player LED, this can be changed from the variables list at the beginning


      
      if (gameStart == false) { // when game ends
        resetMatrix();
        delay(1000);
        endingSequence();
        
       // initialise the variables
       
      counter = 0;
      movement = 8;
      countParts = 0;
      playerCol = 3;
      playerRow = 3;
      gameStart = true;
     }

   }
  
  refreshMatrix();
}


void resetMatrix() {

 for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          matrix[x][y] = 0;
        }
      }
      
     refreshMatrix();
}

void initialiseRegisters()

{


    pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  
  
  }




// the method for this function was based on the tutorial from http://www.thebox.myzen.co.uk/Workshop/LED_Matrix.html
void refreshMatrix() {
  byte columnbits = 0; // create byte types to store in the value
  byte rowbits = 0;
  
  int x = 0;
  int y = 0;
  for (x = 0; x < 8; x++) {
      
    for (y = 0; y < 8; y ++) {
      if (matrix[x][y] == 1 || matrix[x][y] == 2) { // if the array multiplication will result in turning on LEDs then assign these bits to rows and columns
                                                    // "1" is assigned to turn on the matrix LEDs and "2" is for the player
        
       bitWrite(columnbits, y, HIGH); // this function writes a bit of numerical value
       bitWrite(rowbits, x, HIGH);

      } 
    }
    
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, rowbits); // goes to second shift register
    shiftOut(dataPin, clockPin, MSBFIRST, columnbits); // goes to first shift register
    digitalWrite(latchPin, HIGH);   
    delay(2);
  }
}
