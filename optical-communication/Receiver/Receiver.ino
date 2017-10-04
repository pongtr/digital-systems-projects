/////////////////////////////////
// EENG 348: Receiver.ino
// Created: April 22, 2017
// Creator: Pong Trairatvorakul
//
// Sends prompt to Serial
// Receives TB from Serial
// Receive dibit data via optical link using Transmitter.ino
// combine dibits, decrypts, then print decrypted byte to Serial
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
int TB;             // period of each blink
int db[4];          // dibits
volatile bool wait; // true, then wait for ISR
volatile bool receiving = false; // true when receiving
volatile int read_val; // value read from ADC

//== M&M SETUP ===========================================
// Color: Green
// PT Vals corresponding to each dibit
int PT_vals[] = {1008, 743, 543, 375};
// halfway between each value
int thresholds[] = {
  (PT_vals[0] + PT_vals[1])/2,
  (PT_vals[1] + PT_vals[2])/2,
  (PT_vals[2] + PT_vals[3])/2
};
                  

//== SETUP ===============================================

void setup() {
  S_open                      // macro to set Serial
  IO_init();                  // set up pinmode
  Serial.println("Receiver ready. Send TB"); // send prompt
  while(!Serial.available()); // wait for TB
  TB = Serial.parseInt();     // store TB value
  decrypt_init();             // set decryption seed
  timer_init();               // set up timer
}

// set up the phototransistor as input
void IO_init() {
  pinMode(PT, INPUT);
}

// set up timer1 to CTC mode with 256 prescaler
void timer_init() {
  wait = true;
  noInterrupts();           // disable interrupts
  TCCR1A = TCCR1B = 0;      // clear TCCRs
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare A interrupt
  OCR1A = TB * 62.5 * 1.5;  // compare match register value
                            //   set to 1.5TB
  TCNT1 = 0;                // clear counter
  interrupts();             // enable interrupts
}

// generate seed for decryption
void decrypt_init() {
  randomSeed(R_SEED);
}

//== LOOP ===============================================
void loop() {
  while (analogRead(PT) > 700); // wait for start bit
  OCR1A = TB * 62.5 * 1.5;    // compare match register val to 1.5TB
  receiving = true;
  wait = true;
  TCNT1 = 0;                  // reset counter
  WAIT                        // wait macro: wait until counter ISR
  db[0] = PT_read(read_val);  // get dibit 0
  OCR1A = TB * 62.5;          // compare match register value to TB
  for (int i = 1; i < 4; i++) {   // read the rest of the bits
    WAIT                          // wait macro
    db[i] = PT_read(read_val);    // get dibit
  }
  receiving = false;
  int data = decrypt(combine());  // combine dibits and decrypt
  Serial.println(data);           // send data to matlab
  WAIT                            // wait macro
}

// decrypt data by XOR with RBYTE
byte decrypt(int data) {
  return data ^ (rand() % 256);
}

// combine all the dibits db into a byte to return
int combine() {
  int val = 0;
  for (int i = 0; i < 4; i++) {
    val += db[i] << (2 * i);
  }
  return val;
}

// input an ADC value
// return the corresponding dibit val
int PT_read(int val) {
  if      (val > thresholds[0]) return 0;
  else if (val > thresholds[1]) return 1;
  else if (val > thresholds[2]) return 2;
  else                          return 3;
}

// changes wait to false when timer reached
ISR(TIMER1_COMPA_vect) {
    wait = false;               // frees main code to continue
  if (receiving) {              // ensure we are in part of code that receives
    read_val = analogRead(PT);  // read ADC val
  }
}


