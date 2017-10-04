/////////////////////////////////
// EENG 348: Transmitter.ino
// Created: April 22, 2017
// Creator: Pong Trairatvorakul
//
// Sends prompt to Serial
// Receives TB from Serial
// Receives data byte from Serial, encrypts it
// then transmits through RGB LED as dibits.
/////////////////////////////////

//== MACROS & GLOBALS ===================================

// set up serial
#define S_open  Serial.begin(115200);Serial.setTimeout(10);

// pins of RGB LED & phototransistor
#define R 6
#define G 9
#define B 10
#define PT 0

// number of stop bits
#define N_STOP 5

// wait until wait is false (eg. timer reaches value)
// then immediately change wait back to true and continue
// with rest of code
#define WAIT while(wait); wait = true;

// random seed
#define R_SEED 422

// Global variables
int TB;                     // period of each blink
int db[4];                  // dibits
volatile bool wait;         // true, then wait for ISR
volatile bool transmitting = false; // true when transmitting
                            // to prevent sending data when shouldn't
volatile int  to_send;      // value of dibi to send

//== M&M SETUP ===========================================
// Color: Green
// PT Vals
// RGB of each: 000, 101, 011, 111
// PT values: 1008, 743, 543, 375

//== SETUP ===============================================

void setup() {
  S_open                      // macro to set Serial
  IO_init();                  // set up pinmode
  Serial.println("Transmitter ready. Send TB"); // send prompt
  while(!Serial.available()); // wait for TB
  TB = Serial.parseInt();     // store TB value
  encrypt_init();             // set encryption seed
  timer_init();               // set up timer
}

// set up the RGB LEDs as outputs and initialize to LOW
void IO_init() {
  // pinMode(R, OUTPUT); digitalWrite(R, LOW);
  DDRD |= (1 << PD6); PORTD = 0;
  // pinMode(G, OUTPUT); digitalWrite(G, LOW);
  // pinMode(B, OUTPUT); digitalWrite(B, LOW);
  DDRB |= (1 << PB1) + (1 << PB2); PORTB = 0;
}

// set up timer1 to CTC mode with 256 prescaler
void timer_init() {
  wait = true;
  noInterrupts();           // disable interrupts
  TCCR1A = TCCR1B = 0;      // clear TCCRs
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare A interrupt
  OCR1A = TB * 62.5;        // compare match register value
  TCNT1 = 0;                // clear counter
  interrupts();             // enable interrupts
}

// set random seed
void encrypt_init() {
  randomSeed(R_SEED);
}

//== LOOP ===============================================
void loop() {
  // Read image data bytes from Matlab
  while(!Serial.available());    // wait until data sent
  int data = Serial.parseInt();  // store byte in data

  // encrypt data and break into dibits
  dibit(encrypt(data));

  transmitting = true; // toggle transmitting to true so ISR sends bits
  TCNT1 = 0;           // restart counter
    
  // start bit
  to_send = 3;  // corresponds to all on
  WAIT          // wait until timer equals, then send
  
  // dibits
  for (int i = 0; i < 4; i++) {
    to_send = db[i]; // prepare dibit to send
    WAIT             // wait until timer equals, then send
  }
  
  // stop bits
  for (int i = 0; i < N_STOP; i++) {
    to_send = 0;  // corresponds off (idle channel)
    WAIT          // wait until timer equals, then send
  }
  transmitting = false; // toggle transmitting to false so ISR doesn't send bits
}

// encrypt data by XOR with RBYTE
byte encrypt(int data) {
  return data ^ (rand() % 256);
}

// input data byte and breaks into 4 dibits
void dibit(byte data) {
  for (int i = 0; i < 4; i++) {
    db[i] = data % 4;
    data /= 4;
  }
}

// input dibit, outputs the value on the LED
void send_dibit(int dibit) {
  switch (dibit) {
    case 0: light(0,0,0); break; // all off
    case 1: light(1,0,1); break; // RB
    case 2: light(0,1,1); break; // GB
    case 3: light(1,1,1); break; // RGB
  }
}

// input RGB value (1/0) to display
// displays them on the corresponding LEDs
void light(int r_val, int g_val, int b_val) {
   // digitalWrite(R, r_val);       // red LED
  if (r_val) PORTD |=  (1 << PD6);
  else       PORTD &= ~(1 << PD6);
//   digitalWrite(G, g_val);        // green LED
  if (g_val) PORTB |=  (1 << PB1);
  else       PORTB &= ~(1 << PB1);
//   digitalWrite(B, b_val);        // blue LED
  if (b_val) PORTB |=  (1 << PB2);
  else       PORTB &= ~(1 << PB2);
}

// takes code out of wait when timer reached
ISR(TIMER1_COMPA_vect) {
  if (transmitting) {     // ensure that we want to be sending dibits across
    wait = false;         // frees main code to continue
    send_dibit(to_send);  // send the dibit
  }
}


