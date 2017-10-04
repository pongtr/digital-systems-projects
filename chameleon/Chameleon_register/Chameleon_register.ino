/*  Chameleon_hobbyist
    EE348 Chameleon Project
    Prawat (Pong) Trairatvorakul
    3/30/17
    Reads M&M color, find closest match, then display
    Register commands
*/

#include <limits.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define S_open  Serial.begin(9600);Serial.setTimeout(10); // on one line
// pins of RGB LED & phototransistor
#define R 6
#define G 9
#define B 10
#define PT 0

// define colors in array
int n_colors = 7;
String colors[]   = {"Red", "Green", "Blue", "Yellow", "Orange", "Pink", "Fusia"};

// Global vals
int m_R, m_G, m_B; // measured vals of RGB
int learned[42];   // learned values (from learn.ino)

// function prototypes
void IO_init();               // initialize IO
void load_vals();             // load vals from EEPROM
void ADC_init();              // initialize ADC
void measureRGB();            // measure PT vals of R,G,B
int measure(int color);       // measure PT val for a single color
int findMatch();              // find closest match of measured RGB values
void colorDisplay(int color); // display the given color on the LED

void setup() {
  S_open        // macro to set Serial
  IO_init();    // initialize IO
  ADC_init();   // initialize ADC
  load_vals();  // load values from EEPROM and store in RAM
}

void loop() {
  measureRGB();
  int match = findMatch();
  colorDisplay(match);
  // printHex();
  Serial.println("Color Match: " + colors[match]);
  delay(500);
}

// initialize the LEDs. Set as output and output low
void IO_init() {
//  pinMode(R, OUTPUT); digitalWrite(R, 0);
//  pinMode(G, OUTPUT); digitalWrite(G, 0);
//  pinMode(B, OUTPUT); digitalWrite(B, 0);
  DDRD |= (1 << DDD6);
  DDRB |= (1 << DDB1);
  DDRB |= (1 << DDB2);
  PORTD = 0; // turn all D pins to 0
  PORTB = 0; // turn all B pins to 0
}

// load values from EEPROM and store in RAM
void load_vals() {
  Serial.print("Learning values... ");
  for (int i = 0; i < 6 * n_colors; i++)
    learned[i] = EEPROM_read(i);
  Serial.println("Done!");
}

void ADC_init() {
  PRR &= 255 - (1 << PRADC);  // clear PRADC
  ADMUX = 0;                  // clear ADMUX
  ADCSRA = 0;                 // clear ADCSRA
  ADCSRB = 0;                 // clear ADCSRB
  ADMUX = 0;                  // select channel
  ADMUX |= (1 << REFS0);      // set voltage reference to +5V
  ADMUX |= (1 << ADLAR);      // select number format (left adjusted)
  ADCSRA |= (0b111 << ADPS0); // set ADC prescaller to 128
  ADCSRA |= (1 << ADEN);      // enable ADC by setting ADEN in ADCSRA to 1
}

// print out values of hexadecimals for each number
void printHex() {
  Serial.print(m_R); Serial.print("\t");
  Serial.print(m_G); Serial.print("\t");
  Serial.print(m_B); Serial.print("\t");
}

// measure PT value for each RGB 
void measureRGB() {
  // turn off LED -- turn off PWM for pin then write 1 to PORT
  // digitalWrite(R,0);
  TCCR0A &= ~(1 << COM0B1);
  PORTD  &= ~(1 << PORTD6);
  // digitalWrite(G,0);
  TCCR1A &= ~(1 << COM1A1);
  PORTB  &= ~(1 << PORTB1); 
  // digitalWrite(B,0);
  TCCR1A &= ~(1 << COM1B1);
  PORTB  &= ~(1 << PORTB2); 
  
  // measure each of RGB and store measured value
  m_R = measure(R);
  m_G = measure(G);
  m_B = measure(B);
}

// Measure PT value for one color
int measure(int color) {
  // turn on the color (digitalWrite(color, HIGH))
  switch(color){
    case R:
      PORTD |= (1 << PORTD6);
      break;
    case G:
      PORTB |= (1 << PORTB1);
      break;
    case B:
      PORTB |= (1 << PORTB2);
      break;
  }
  delay(1); // wait to stabilize
  ADCSRA |= 1 << ADSC; // start conversion
  while (ADCSRA & (1 << ADSC)); // wait for conversion
  // int val = analogRead(PT) >> 2; // read and scale to 8 bits
  int val = ADCH;
  // turn off the color (digitalWrite(color, LOW))
  switch(color){
    case R:
      PORTD &= ~(1 << PORTD6);
      break;
    case G:
      PORTB &= ~(1 << PORTB1);
      break;
    case B:
      PORTB &= ~(1 << PORTB2);
      break;
  }
  return val;
}

// find closest matching color to measured val
int findMatch() {
  // initialize start
  long min_distance = LONG_MAX; // current minimum distance
  int closest = 0;              // the closest match so far

  // iterate through possible colors
  for (int i = 0; i < n_colors; i++) {
    // calculate cartesian distance
    long distance = 0;
    distance += pow(m_R - learned[3 * i], 2);
    distance += pow(m_G - learned[3 * i + 1], 2);
    distance += pow(m_B - learned[3 * i + 2], 2);
    // test if distance is closer than current closest
    if (distance < min_distance) {
      // if closer, then store
      min_distance = distance;  // current distance becomes minimum distance
      closest = i;              // current color becomes closest color
    }
  }
  return closest; // return closest distance (as index of color array)
}

// display the given color
void colorDisplay(int color) {
  // set RGB to stored color
  analogWrite(R, learned[(color + n_colors) * 3]);
  analogWrite(G, learned[(color + n_colors) * 3 + 1]);
  analogWrite(B, learned[(color + n_colors) * 3 + 2]);
}

unsigned char EEPROM_read(unsigned int uiAddress) {
  while (EECR & (1 << EEPE)); // wait for completion of previous write
  EEAR = uiAddress;           // provide address from EEPROM to read from
  EECR |= (1 << EERE);        // read enable
  return EEDR;                // return EEPROM data
}

