/*
* Sends Data to the shift register based on
* a string of binary chars
* String is converted to decical and sent to
* register
*/

#include <IRremote.h>

// pin connected to ST_CP of 74HC595
const byte LATCH = 11;
// pin connected to SH_CP of 74HC595
const byte CLOCK = 12;
// pin connected to DS of 74HC595
const byte DATA = 8;

char on[] = "00000011";
char off[] = "00000000";

int blinker = 13;
int BPM = 140;
int upButton = 5;
int downButton = 4;
int upButtonState = 0;
int downButtonState = 0;

//IR Control
int RECEIVE_PIN = 10;
IRrecv irInput(RECEIVE_PIN);
decode_results results;

int irUpButton1 = 2128;
int irUpButton2 = 80;
int irDownButton1 = 2129;
int irDownButton2 = 81;


/*
* Initialisation Method
*/
void setup()
{
  Serial.begin(9600);
  //set pins to output so you can control the shift register
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(blinker, OUTPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);

  // the LEDs don't change while you're sending in bits
  digitalWrite(LATCH, LOW);
  // shift out the bits
  shiftOut(DATA, CLOCK, MSBFIRST, 0);
  //take the latch pin high so the LEDs will light up
  digitalWrite(LATCH, HIGH);

  irInput.enableIRIn(); // Start the receiver
}

/*
* Converts Binary to decimal
* @param binary as a char string
*/
int binaryToDecimal(char binary[])
{
  int sum=0;
  for(int i=0;i<strlen(binary);i++)
  {
    int n = binary[i] - '0';
    sum = (sum * 2) + n;
  }
  return sum;
}

/*
*Writes to the pins based on binary input
*@param binary input as char string
*/
void latchWrite(char binary[])
{
  digitalWrite(LATCH,LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, binaryToDecimal(binary));
  digitalWrite(LATCH,HIGH);
}

//Calculate the BPM in milliseconds
//minus 100 milliseconds for beep length
double calcBPM(int inputBPM)
{
  double divide = (60.00/inputBPM);
  return ((divide*1000)-100);
}

// Metronome output 
void bpmBeep(int BPM)
{
  latchWrite(on);
  digitalWrite(blinker, HIGH);
  delay(100);
  latchWrite(off);
  digitalWrite(blinker, LOW);
  delay(calcBPM(BPM));
}

/*
*Arduino Loop
*/
void loop()
{
  upButtonState = digitalRead(upButton);
  downButtonState = digitalRead(downButton);

  if (irInput.decode(&results)) {
        delay(50);

        if(results.value == irUpButton1 || results.value == irUpButton2)
        {
          BPM = BPM + 10;
        }
        else if (results.value == irDownButton1 || results.value == irDownButton2)
        {
          BPM = BPM - 10;
        }
        irInput.resume();// Receive the next value
    }

  // check if the pushbutton is pressed.
  if (upButtonState == HIGH) {
    // increase BPM by 10
    BPM = BPM + 10;
  }
  // check if the pushbutton is pressed.
  if(downButtonState == HIGH) {
    // decrease BPM by 10  
    BPM = BPM - 10;
  }
  //Play metronome at set BPM
  bpmBeep(BPM);
}