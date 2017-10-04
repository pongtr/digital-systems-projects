/*
    Proj1_Register.ino
    Pong Trairatvorakul
    2/8/17
    Hobbyist versin of project 1
    Measures reaction time of user by flashing the LED at a random delay
    between 1 and 2 seconds. Timer starts after light turns on and stops
    when button is depressed. Time interval is returned and sent to Matlab
    10 Readings are taken.
    With register commands
*/

#define LED  DDB5   // output LED is PB5 (pin 13)
#define BUT  DDD2   // input button is PD2 (pin 2)
#define FOURS   62500  // number of cycles in 4 seconds
#define SERIAL_SETUP Serial.begin(9600); Serial.setTimeout(10);

// set up the input and output
void io_setup() {
  // pinMode(LED, OUTPUT);     // set LED as output
  DDRB  |= (1 << DDB5);       // set LED as output
  // digitalWrite(LED, LOW);   // initialize LED to off
  PORTB |= (0 << DDB5);       // initialize LED to off
  // pinMode(BUT, INPUT);      // set button as input
  DDRD  &= 255 - (1 << DDD2); // clear DDD2
  // digitalWrite(BUT, HIGH);  // attach pullup resistor to button
  PORTD |= (1 << DDD2);       // attach pullup resistor
}

// set up timer/counter
void tc_setup() {
  TCCR1A = 0; // Timer/Counter1 Control Register A ->0
  TCCR1B = 5; // Timer/Counter1 Control Register B ->5 (prescaler = 1024)
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

void setup() {
  SERIAL_SETUP; // set up Serial baudrate (9600bps) & timeout (10ms)
  io_setup();   // set LED as output, BUT as input, then initialize values
  tc_setup();   // set up timer counter

  while (prompt());
  // digitalWrite(LED, LOW);         // turn LED off
  PORTB &= (0 << PORTB5);
  for (int i = 1; i <= 10; i++) { // 10 tries
    Serial.print((String) i + ",");  // print the experiment number
    int len = rand() % 1000 + 1000; // determine the random interval (in ms)
    delay(len);                   // wait for random interval
    //digitalWrite(LED, HIGH);      // turn LED on
    PORTB |= (1 << PORTB5);      // turn LED on
    TCNT1 = 0;                  // reset timer

    /* Hobbyist version to detect press when LED turns on
      if (!digitalRead(BUT)) {      // if button already pressed (or instantaneous)
      Serial.println(0);          // report zero
      digitalWrite(LED, LOW);     // turn off LED
      delay(500);                 // delay for 500ms
      continue;                   // got to next reading
      }
    */
    if (!(PIND & (1 << PIND2))) {    // if button already pressed (or instantaneous)
      Serial.println(0);          // report zero
      PORTB &= (0 << PORTB5);     // then reset
      delay(500);                 // delay for 500ms
      continue;                   // got to next reading
    }

    while (PIND & (1 << 2)        // wait for button press
           && TCNT1 <= FOURS);    // or 4sec
    unsigned long int c_end = TCNT1;    // get counter value
    unsigned long int rt;               // reaction time
    if (c_end > FOURS)                  // exceed 4 seconds
      rt = 0;                           // abort
    else {                              // otherwise
      rt = c_end * 64 / 1000;           // calculate reaction time
    }
    Serial.println(rt);  // print reaction time
    if (i < 10)       // if not last exp
      // digitalWrite(LED, LOW);         // turn LED off
      PORTB &= (0 << PORTB5);
    delay(500);
  }
  PORTB |= (1 << PORTB5);      // turn LED on / digitalWrite(LED, HIGH))
}

void loop() {}
