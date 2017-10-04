/*  Learn
    EE348 Chameleon Project
    Prawat (Pong) Trairatvorakul
    3/30/17
    Acquires and stores M&M RGB feature values and their display color values in EEPROM database
*/

// setup serial
#define S_open  Serial.begin(9600);Serial.setTimeout(10);
// Define IO pins
#define R 6
#define G 9
#define B 10
#define PT 0
#define TRIES 16 // number of tries

// define colors in array
int n_colors = 7;
String colors[]   = {"Red", "Green", "Blue", "Orange", "Yellow", "Pink", "Fusia"};
String col_init[] = {"R", "G", "B", "O", "Y", "P", "F"};

int PT_val;   // potentiometer value
char mm_col;  // index of mm color

// color values for PWM
// fusia is darker than pink
int col_vals[7][3] = {
  {255,   0,   0}, // Red
  {  0, 255,   0}, // Green
  {  0,   0, 255}, // Blue
  {255,  50,   0}, // Orange
  {255, 150,   0}, // Yellow
  {255,  80, 150}, // Pink
  {255,  30, 150}  // Fusia
};

void setup() {
  S_open // not a fcn: complier inserts defined instruction
  IO_init(); // intilize the LEDs as output and off
  Serial.println("Send 1 when ready to learn that color");
}

void loop() {
  int uiAddress = 0; //address to store value in
  // iterate through each color then store readings
  for (int i = 0; i < n_colors; i++) {
    // prompt and wait for user
    Serial.print(col_init[i] + ": ");
    do {
      while (!Serial.available());
    } while (Serial.read() != '1');

    // make measurements for RGB
    int r_val = measure(R), g_val = measure(G), b_val = measure(B);

    // print values
    Serial.print(r_val); Serial.print(" ");
    Serial.print(g_val); Serial.print(" ");
    Serial.print(b_val); Serial.print("\t");

    // store values in EEPROM
    Serial.print("Store in: ");
    store(uiAddress++, r_val);
    store(uiAddress++, g_val);
    store(uiAddress++, b_val);

    Serial.println();
  }

  // Store PWM val portion
  Serial.println("Store PWM values");
  for (int i = 0; i < n_colors; i++) {
    Serial.print(col_init[i] + ": ");
    // print values to be stored
    for (int j = 0; j < 3; j++) {
      Serial.print(col_vals[i][j]); Serial.print(" ");
    }
    Serial.print("\tStore in: ");
    // store the values
    for (int j = 0; j < 3; j++) {
      store(uiAddress++, col_vals[i][j]);
    }
    Serial.println();
  }

  Serial.println("End of learn. Send '1' again to restart");
  do {
    while (!Serial.available());
  } while (Serial.read() != '1');
}

// initialize the LEDs. Set as output and output low
void IO_init() {
  pinMode(R, OUTPUT); digitalWrite(R, 0);
  pinMode(G, OUTPUT); digitalWrite(G, 0);
  pinMode(B, OUTPUT); digitalWrite(B, 0);
}

// measure value of PT for a given color 16 times then finds average and divide by 4
int measure(int color) {
  digitalWrite(color, HIGH); // turn that color on
  delay(10);                 // wait for value to stabilize
  int sum = 0;               // sum of all readings
  for (int i = 0; i < TRIES; i++)
    sum += analogRead(PT);   // read and add reading to sum
  delay(10);
  digitalWrite(color, LOW);  // turn that color off
  return (sum / TRIES) >> 2; // find average of 16 times and right shift two
}

void store(unsigned int uiAddress, unsigned char ucData) {
  while (EECR & (1 << EEPE)); // wait for completion of previous write
  EEAR = uiAddress;           // set address
  EEDR = ucData;              // set data
  EECR |= (1 << EEMPE);       // get ready to write EEPROM
  EECR |= (1 << EEPE);        // start EEPROM write
  Serial.print(uiAddress); Serial.print(" ");
}

