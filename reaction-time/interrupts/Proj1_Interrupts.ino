/*
    Proj1_Register.ino
    Pong Trairatvorakul
    2/8/17
    Interrupts version of project 1
    Measures reaction time of user by flashing the LED at a random delay
    between 1 and 2 seconds. Timer starts after light turns on and stops
    when button is depressed. Time interval is returned and sent to Matlab
    10 Readings are taken.
    With interrupts
*/

#define LED  DDB5   // output LED is PB5 (pin 13)
#define BUT  DDD2   // input button is PD2 (pin 2)
#define FOURS   62500  // number of cycles in 4 seconds
#define SERIAL_SETUP Serial.begin(9600); Serial.setTimeout(10);

// set up the input and output
void io_setup() {
  // pinMode(LED, OUTPUT);      // set LED as output
  DDRB  |= (1 << DDB5);         // set LED as output
  // digitalWrite(LED, LOW);    // initialize LED to off
  PORTB |= (0 << DDB5);         // initialize LED to off
  // pinMode(BUT, INPUT);       // set button as input
  DDRD  &= 255 - (1 << DDD2);   // clear DDD2
  // digitalWrite(BUT, HIGH);   // attach pullup resistor to button
  PORTD |= (1 << DDD2);         // attach pullup resistor
}

// set up timer counter
void tc_setup() {
  TCCR1A = 0; // Timer/Counter1 Control Register A ->0
  TCCR1B = 5; // Timer/Counter1 Control Register B ->5 (prescaler = 1024)
}

// set up interrupts
void ir_setup() {
  EICRA = (1 << ISC01);     // Trigger INT0 on falling edge
  EIMSK = (1 << INT0);      // Enable external interrupt INT0
  SREG |= (1 << 7);         // Enable global interrupts
}

// prompt for s. return 0 if s sent, 1 otherwise
int prompt() {
  Serial.println("Send 's' to start");
  while (!Serial.available());    // waits to start
  char c = Serial.read();         // get input
  if (c != 's')                   // if not s
    return 1;                     // trigger reprompt
  else
    return 0;                     // continue
}

volatile bool pressed = false;
volatile unsigned long int rt = 0;

void setup() {
  SERIAL_SETUP; // set up Serial baudrate (9600bps) & timeout (10ms)
  io_setup();   // set LED as output, BUT as input, then initialize values
  tc_setup();   // set up timer counter
  ir_setup();   // set up interrupt

  while (prompt());
  // digitalWrite(LED, LOW);         // turn LED off
  PORTB &= (0 << PORTB5);
  for (int i = 1; i <= 10; i++) {         // 10 tries
    pressed = false;    // initialize to false since button has not been pressed
    rt = 0;             // initialize reaction time to 0
    Serial.print((String) i + ",");       // print the experiment number
    int len = rand() % 1000 + 1000;       // determine the random interval (in ms)
    delay(len);                           // wait for random interval
    //digitalWrite(LED, HIGH);            // turn LED on
    PORTB |= (1 << PORTB5);               // turn LED on
    TCNT1 = 0;                            // reset timer
    while ((TCNT1 < (unsigned long int) FOURS)  // wait for 4 seconds
           && (!pressed));                      // or until button press
    Serial.println(rt);   // print reaction time
    if (i < 10)           // if not last exp
      // digitalWrite(LED, LOW);         // turn LED off
      PORTB &= (0 << PORTB5);
    delay(500);
  }
  PORTB |= (1 << PORTB5);      // turn LED on / digitalWrite(LED, HIGH))
}

void loop() {}

ISR(INT0_vect) {// Interrupt Service Routine attached to INT0 vector
  unsigned long int c_end = TCNT1;          // capture value in TCNT1 asap
  if (!pressed) {                           // ignore bounce
    bool led_on = PORTB &= (1 << PORTB5);   // true if LED is on
    rt = (led_on) ? c_end * 64 / 1000 : 0;  // set rt to calculated reaction time, 0 if early
    pressed = true;                         // turn button pressed flag to true
  }
}

