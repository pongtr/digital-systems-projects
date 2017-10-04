/*  Chameleon_hobbyist
    EE348 Chameleon Project
    Prawat (Pong) Trairatvorakul
    3/30/17
    Reads M&M color, find closest match, then display
    Hobbyist Version
*/

#include <limits.h>

#define S_open  Serial.begin(9600);Serial.setTimeout(10); // set up serial
// pins of RGB LED & phototransistor
#define R 6
#define G 9
#define B 10
#define PT 0

// define colors in array
int n_colors = 7;
String colors[]   = {"Red", "Green", "Blue", "Orange", "Yellow", "Pink", "Fusia"};

// Global vals
int m_R, m_G, m_B; // measured vals of RGB
int learned[42];   // learned values (from learn.ino)

// function prototypes
void IO_init();               // initialize IO
void load_vals();             // load vals from EEPROM
void measureRGB();            // measure PT vals of R,G,B
int measure(int color);       // measure PT val for a single color
int findMatch();              // find closest match of measured RGB values
void colorDisplay(int color); // display the given color on the LED

void setup() {
  S_open        // macro to set Serial
  IO_init();    // initialize IO
  load_vals();  // load values from EEPROM and store in RAM
}

void loop() {
  measureRGB();                   // measure vals from PT
  int match = findMatch();        // find closest match, given measurements
  colorDisplay(match);            // display matched color on LED
  Serial.println("Color Match: " + colors[match]);
  delay(500);                     // leave color on for 0.5s
}

// initialize the LEDs. Set as output and output low
void IO_init() {
  pinMode(R, OUTPUT); digitalWrite(R, 0);
  pinMode(G, OUTPUT); digitalWrite(G, 0);
  pinMode(B, OUTPUT); digitalWrite(B, 0);
}

// load values from EEPROM and store in RAM
void load_vals() {
  Serial.print("Learning values... ");
  for (int i = 0; i < 6 * n_colors; i++) // iterate through EEPROM
    learned[i] = EEPROM_read(i);         // store val
  Serial.println("Done!");
}

// measure PT vals for R,G,B
void measureRGB() {
  // turn off LED
  digitalWrite(R,0);
  digitalWrite(G,0);
  digitalWrite(B,0);
  // measure each of RGB and store measured value
  m_R = measure(R);
  m_G = measure(G);
  m_B = measure(B);
}

// Measure PT value for one color. Input color. Returns value
int measure(int color) {
  digitalWrite(color, HIGH);      // turn on color
  delay(1);                       // wait to stabilize
  int val = analogRead(PT) >> 2;  // read and scale to 8 bits
  digitalWrite(color, LOW);       // turn off color
  return val;                     // return measured value
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

