/*
    Proj1_Hobbyist.ino
    Pong Trairatvorakul
    2/21/17
    Hobbyist version of project 1
    Measures reaction time of user by flashing the LED at a random delay
    between 1 and 2 seconds. Timer starts after light turns on and stops
    when button is depressed. Time interval is returned and sent to Matlab
    10 Readings are taken.
*/

#define LED 13  // output LED is pin 13
#define BUT  2  // input button is pin 2
#define SERIAL_SETUP Serial.begin(9600); Serial.setTimeout(10);

// set up the input and output
void io_setup() {
  pinMode(LED, OUTPUT);     // set LED as output
  digitalWrite(LED, LOW);   // initialize LED to off
  pinMode(BUT, INPUT);      // set button as input
  digitalWrite(BUT, HIGH);  // attach pullup resistor to button
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

  while (prompt());               // prompt until 's' is received
  digitalWrite(LED, LOW);         // turn LED off
  for (int i = 1; i <= 10; i++) { // 10 experiments
    Serial.print((String) i + ",");              // print the experiment number
    int len = random(1000, 2000); // determine the random interval
    delay(len);                   // wait for random interval
    digitalWrite(LED, HIGH);      // turn LED on
    int t_start = millis();       // get start time
    if (!digitalRead(BUT)) {      // if button already pressed (or instantaneous)
      Serial.println(0);          // report zero
      digitalWrite(LED, LOW);     // turn off LED
      delay(500);                 // delay for 500ms
      continue;                   // got to next reading
    }
    while (digitalRead(BUT)                   // wait for button press
           && millis() - t_start <= 4000);     // or time to be above 4s
    int t_end = millis();         // get end time
    int rt = t_end - t_start;     // get reaction time
    if (rt > 4000)                // for time that exceeds 4s
      rt = 0;                     // set rt to 0
    Serial.println(rt);           // print reaction time
    if (i < 10)                   // if not last experiment
      digitalWrite(LED, LOW);     // turn off LED
    delay(500);                   // delay 500ms
  }
  digitalWrite(LED, HIGH);     // ensures LED stays on after last exp
}

void loop() {}
