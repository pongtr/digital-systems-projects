/*  Chameleon_hobbyist
    EE348 Chameleon Project
    Prawat (Pong) Trairatvorakul
    3/30/17
    Reads M&M color, find closest match, then display
    Direct
*/

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
void ADC_init();              // initialize ADC
void PWM_init();              // initialize PWM
void measureRGB();            // measure PT vals of R,G,B
int measure(int color);       // measure PT val for a single color
void colorDisplay(); // display the given color on the LED

void setup() {
  S_open        // macro to set Serial
  IO_init();    // initialize IO
  ADC_init();   // initialize ADC
  PWM_init();   // initialize PWM
}

void loop() {
  measureRGB();
  colorDisplay();
  delay(100);
}

// initialize the pins for LEDs
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

// Initialize the ADC for PT
void ADC_init() {
  PRR &= 255 - (1 << PRADC); // clear PRADC
  ADMUX = 0;  // clear
  ADCSRA = 0; // clear
  ADCSRB = 0; // clear
  ADMUX = 0;       // select channel
  ADMUX |= (1 << REFS0); // set voltage reference to +5V
  ADMUX |= (1 << ADLAR); // select number format (left adjusted)
  ADCSRA |= (0b111 << ADPS0); // set ADC prescaller to 128
  ADCSRA |= (1 << ADEN); // enable ADC by setting ADEN in ADCSRA to 1
}

// Initialize fastPWM for pin6
void PWM_init() {
  TCCR0A = TCCR0B = 0; // clear Timer0 control registers
  TCCR0A |= (1 << WGM00) + (1 << WGM01); // fast PWM mode
  TCCR0A |= (1 << COM0A1); // clear OC0A on compare match
  DDRD = 64; // set pin6 (PD6) as output
  TCCR0B |= (1 << CS02); // 256 pre-scaler
}

// measure PT value for each RGB 
void measureRGB() {
  // turn off LED
  OCR0A = 0; // digitalWrite(R,0);
  digitalWrite(G,0);
  digitalWrite(B,0);
  // measure each of RGB and store measured value
  m_R = measure(R);
  m_G = measure(G);
  m_B = measure(B);
}

// Measure PT value for one color
int measure(int color) {
  if (color == R)
    OCR0A = 255;
  else
    digitalWrite(color, HIGH);
  delay(1); // wait to stabilize
  ADCSRA |= 1 << ADSC; // start conversion
  while (ADCSRA & (1 << ADSC)); // wait for conversion
  // int val = analogRead(PT) >> 2; // read and scale to 8 bits
  int val = ADCH;
  if (color == R)
    OCR0A = 0;
  else
    digitalWrite(color, LOW);
  return val;
}

// display color by transforming measured RGB values
void colorDisplay() {
  int d_R = ~m_R;
  int d_G = ~m_G;
  int d_B = ~m_B;
  // set RGB to stored color
  // analogWrite(R, learned[(color + n_colors) * 3]);
  OCR0A = d_R;
  analogWrite(G, d_G);
  analogWrite(B, d_B);
}

unsigned char EEPROM_read(unsigned int uiAddress) {
  while (EECR & (1 << EEPE)); // wait for completion of previous write
  EEAR = uiAddress;
  EECR |= (1 << EERE);
  return EEDR; // return EEPROM data
}

