/*
  Memory game, 5 digital inputs and 5 leds
*/

const int redButton = 8;
const int blueButton = 9;
const int yellowButton = 10;
const int greenButton = 11;
const int blackButton = 12; // black button goes to white LED

const int redLED = 6;
const int blueLED = 5;
const int yellowLED = 4;
const int greenLED = 3;
const int whiteLED = 2; //white LED goes to black button

int score = 0;
//const int scoreLED = 10;

// flashing interval
const unsigned long interval =                    0;
unsigned long previousMillis = 0;

//game state machine variable
int gameState = 1;
int debouncer[] = {0,0,0,0,0};
const int mapButton[] = {redButton, blueButton, yellowButton, greenButton, blackButton};
const int mapLED[] = {redLED, blueLED, yellowLED, greenLED, whiteLED};

//button handling vars
unsigned long mapPast[] = {0, 0, 0, 0, 0};
int mapLEDState[] =  {LOW, LOW, LOW, LOW, LOW};
int mapButtonState[] = {LOW, LOW, LOW, LOW, LOW};
const int debounceDelay = 50;

int getRInt(int Max){
  int n = random(Max);
  while (true) {
    if ( n > 0 && n < Max){
      return n;
    } else {
      n = random(Max);
    }
  }
}

//game vars
int sequence[100];
int sequenceLength = 4;
int input[100];
int inputLength = 4;
int inputNum = 0;
int pv = -1;
int v = -1;
int r = 0;
int inputDelay = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
//  pinMode(scoreLED, OUTPUT);
  
  // initialize the pushbutton pin as an input:
  pinMode(redButton, INPUT);
  pinMode(blueButton, INPUT);
  pinMode(yellowButton, INPUT);
  pinMode(greenButton, INPUT);
  pinMode(blackButton, INPUT);
  
  //start debugger
  Serial.begin(9600);
  Serial.println("Program Start");
}

void(* resetFunc) (void) = 0;

void loop() {
  switch (gameState) {
    case 1:
      Serial.println("startup");
      //startup sequence
      startup();
      break;
    case 2:
      v = checkButtons();
      // check input delay
      if ( v == -1 ){
        inputDelay++;
      } else {
        inputDelay = 0;
      }
      if (inputDelay > 10000) {
        gameState = 3;
      }

      //check edge done input
      if ( v == -1 && pv != -1 ) {
        input[inputNum] = pv;
        inputNum++;
      }
      // prepare for next state
      pv = v;

      Serial.print("l: ");
      Serial.print(inputLength);
      // debug
      Serial.print("; input: ");
      for ( int i = 0; i < inputLength; i++){
        Serial.print(input[i]);
      }
      Serial.println("");

      // check if input is over
      if (inputNum == sequenceLength){
        Serial.println("input is over");
        // the player has entered the neccessary amount of buttons for the sequence
        // check to see if they get to continue playing
        if (checkEqual(sequence, input, sequenceLength)){
          Serial.println("Sequences are Equal");
          //player continues, add one to the sequence
          sequence[sequenceLength] = getRInt(6) -1;
          Serial.println("Got new value for sequence");
          sequenceLength++;
          inputLength++;
          flash(1);
          delay(1000);
          showSequence(sequence, sequenceLength);
          Serial.println("resetting input");
          for (int i = 0; i < inputLength; i++){
              input[i] = -1;
          }
          Serial.print("l: ");
          Serial.print(inputLength);
          // debug
          Serial.print("; input: ");
          for ( int i = 0; i < inputLength; i++){
            Serial.print(input[i]);
          }
          inputNum = 0;
          gameState = 2;
        } else {
          // player stops, go to game over state
          gameState = 3;
        }
      }
      break;
    case 3:
      Serial.println("game over");
      for (int i = 0; i < 10; i++){
        r = getRInt(6) - 1; 
        digitalWrite(mapLED[r], HIGH);
        delay(50);
        turnAllOff();
        delay(50);
      }
      delay(1000);
      flash(sequenceLength - 4);
      resetFunc();
      break;
    default:
      Serial.println("error, ended up in default state");
      gameState = 1;
      break;
  }
  
}


int checkButtons() {
  int mapVal[] = { digitalRead(mapButton[0]), digitalRead(mapButton[1]), digitalRead(mapButton[2]), digitalRead(mapButton[3]), digitalRead(mapButton[4])};
  unsigned long t = millis();
  unsigned long mapDur[] = {t - mapPast[0], t - mapPast[1], t - mapPast[2], t - mapPast[3], t - mapPast[4]};

  if (mapButtonState[0] != mapVal[0]){
      mapPast[0] = t;
  }
  if (mapButtonState[1] != mapVal[1]){
      mapPast[1] = t;
  }
  if (mapButtonState[2] != mapVal[2]){
      mapPast[2] = t;
  }
  if (mapButtonState[3] != mapVal[3]){
      mapPast[3] = t;
  }
  if (mapButtonState[4] != mapVal[4]){
      mapPast[4] = t;
  }

  if ( mapDur[0] > debounceDelay ){
      if (mapButtonState[0] != mapVal[0]){
         mapButtonState[0] = mapVal[0];
         mapLEDState[0] = mapButtonState[0];
      }
  }
  if ( mapDur[1] > debounceDelay ){
      if (mapButtonState[1] != mapVal[1]){
         mapButtonState[1] = mapVal[1];
         mapLEDState[1] = mapButtonState[1];
      }
  }
  if ( mapDur[2] > debounceDelay ){
      if (mapButtonState[2] != mapVal[2]){
         mapButtonState[2] = mapVal[2];
         mapLEDState[2] = mapButtonState[2];
      }
  }
  if ( mapDur[3] > debounceDelay ){
      if (mapButtonState[3] != mapVal[3]){
         mapButtonState[3] = mapVal[3];
         mapLEDState[3] = mapButtonState[3];
      }
  }
  if ( mapDur[4] > debounceDelay ){
      if (mapButtonState[4] != mapVal[4]){
         mapButtonState[4] = mapVal[4];
         mapLEDState[4] = mapButtonState[4];
      }
  }

  digitalWrite(mapLED[0], mapLEDState[0]);
  digitalWrite(mapLED[1], mapLEDState[1]);
  digitalWrite(mapLED[2], mapLEDState[2]);
  digitalWrite(mapLED[3], mapLEDState[3]);
  digitalWrite(mapLED[4], mapLEDState[4]);

  for ( int i = 0; i < 5; i++){
    if (mapLEDState[i] == HIGH ) {
      return i;
    }
  }
  return -1;
}

bool checkEqual(int a[], int b[], int l){
  for ( int i = 0; i < l; i++){
    if (a[i] != b[i]){
      return false;
    }
  }
  return true;
}

void showSequence(int s[], int l){
  Serial.println("Show seq");
  for (int i = 0; i < l; i++){
    Serial.println(i);
    Serial.print("id: ");
    Serial.println(s[i]);
    digitalWrite(mapLED[s[i]], HIGH);
    delay(1000);
    turnAllOff();
    delay(500);
  }
  Serial.println("exit seq");
}

void newSeq(){
  for (int i = 0; i < 4; i ++){
    sequence[i] = getRInt(6) - 1;  
  }
}

void startup(){
  //test random numbers
  randomSeed(analogRead(11));
  
  // make initial sequence
  for (int i = 0; i < sequenceLength; i++){
    sequence[i] = getRInt(6) - 1 ;
  }
  //make initial input
  for (int i = 0; i < inputLength; i++){
    input[i] = -1;
  }
  flash(1);
  delay(1000);
  showSequence(sequence, sequenceLength);
  Serial.println("gameState = 2");
  gameState = 2;
}

void flash(int l){
    Serial.println("flash");
  for (int i = 0; i < l; i++){
    //turn off all
    turnAllOff();
    delay(200);
    //turn on all
    turnAllOn();
    //wait
    delay(400);
  }
  //turn off all
  turnAllOff();
}

void turnAllOff(){
  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(whiteLED, LOW);
}

void turnAllOn(){
  digitalWrite(redLED, HIGH);
  digitalWrite(blueLED, HIGH);
  digitalWrite(yellowLED, HIGH);
  digitalWrite(greenLED, HIGH);
  digitalWrite(whiteLED, HIGH);
}

//void showpattern(int[] pattern){
//  
//}
//
//int[] addOneToPattern(int[] pattern){
//  int newPattern[sizeof(pattern)+1];
//  for (int i = 0; i < sizeof(pattern); i++){
//    newPattern[i] = pattern[i];
//  }
//  newPattern[sizeof(pattern)] = random(5);
//  return newPattern
//}
//
//int[] getpattern(int len){
//  int pattern[];
//  unsigned long curTime = millis()
//  
//  
//}
