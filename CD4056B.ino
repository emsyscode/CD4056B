#define BUTTON_PIN2 2
volatile byte buttonReleased2 = false;

#define BUTTON_PIN3 3
volatile byte buttonReleased3 = false;

uint8_t hexNumber = 0;
uint8_t grid = 0x07;
uint8_t wordUnits = 0;
uint8_t wordTens = 0;
uint8_t wordHundreds = 0;
uint8_t wordThousands = 0;
unsigned int wordCycleValue = 0;
unsigned int wordCycleCompare = 60; //This is the initial value of compare!
uint8_t secs = 0x00;

bool flag = false;

char list[17]= {0b00000000, //0
                0b00000001, //1
                0b00000010, //2
                0b00000011, //3
                0b00000100, //4
                0b00000101, //5
                0b00000110, //6
                0b00000111, //7
                0b00001000, //8
                0b00001001, //9
                0b00001010, //A
                0b00001011, //B
                0b00001100, //C
                0b00001101, //D
                0b00001110, //E
                0b00001111  //F
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

 /*CS12  CS11 CS10 DESCRIPTION
    0        0     0  Timer/Counter1 Disabled 
    0        0     1  No Prescaling
    0        1     0  Clock / 8
    0        1     1  Clock / 64
    1        0     0  Clock / 256
    1        0     1  Clock / 1024
    1        1     0  External clock source on T1 pin, Clock on Falling edge
    1        1     1  External clock source on T1 pin, Clock on rising edge
  */
  // initialize timer1 
  cli();           // disable all interrupts
  //initialize timer1 
  //noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
  TCNT1  = 0;
  // Use 62499 to generate a cycle of 1 sex 2 X 0.5 Secs (16MHz / (2*256*(1+62449) = 0.5
  OCR1A = 62499;            // compare match register 16MHz/256/2Hz
  //OCR1A = 4000; //Change to low value to accelarate the clock to debug!  // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
 
  
  // Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
  // a little the value 62499 upper or lower if the clock have a delay or advance on hours.
    
  //  a=0x33;
  //  b=0x01;

  CLKPR=(0x80);
  //Set PORT
  DDRD = 0xFF;  // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
  PORTD=0x00;
  DDRB =0xFF;   // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
  PORTB =0x00;

  // pinMode(8, OUTPUT);  //This already is definde by lines above!
  // pinMode(9, OUTPUT);  //This already is definde by lines above!

  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2),
                  buttonReleasedInterrupt2,
                  FALLING);

  pinMode(BUTTON_PIN3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3),
                  buttonReleasedInterrupt3,
                  FALLING);                

  //only here I active the enable of interrupts to allow run the test of VFD
  //interrupts();             // enable all interrupts
  sei();

  DDRA = 0xFF;
  DDRC = 0xFF;

  PORTA = 0xFF;
  PORTC = 0xFF;
}
void swapLED(){
  //This is to swap LED's connected to the port 8 & 9.
  digitalWrite(8, !digitalRead(8));
  digitalWrite(9, !digitalRead(9));
}
void showVal(){
    hexNumber = (list[wordUnits]);
    PORTA = hexNumber;
    //PORTC = 0xFE; //The Cathode 1 is placed at left!
    PORTC = ~0x01; //The Cathode 1 is placed at left!
    delay(3);

    hexNumber = (list[wordTens]);
    PORTA = hexNumber;
    //PORTC = 0xFD; //This is the meadle position!
    PORTC = ~0x02; //This is the meadle position!
    delay(3);

    hexNumber = (list[wordHundreds]);
    PORTA = hexNumber;
    //PORTC = 0xFB; //This is the Cathode placed at right!
    PORTC = ~0x04; //This is the Cathode placed at right!
    delay(3);

    hexNumber = (list[wordThousands]);
    PORTA = hexNumber;
    //PORTC = 0xF7; //This is the Cathode placed at right!
    PORTC = ~0x08; //This is the Cathode placed at right!
    delay(3);

    //Uncomment one of lines bellow to check the values on the console output of IDE Arduino.
    // Serial.println(wordThousands, DEC);
    // Serial.print(wordHundreds, DEC);
    // Serial.print(wordTens, DEC);
    // Serial.print(wordUnits, DEC);
    //Serial.println(wordCycleValue, DEC);
    //Serial.println(wordCycleCompare, DEC);
}
void actionOverValues(){
  if(wordUnits != secs){
          wordUnits = secs;
          wordCycleValue++;
          //Point of compare
          if(wordUnits >= 10){
            wordUnits = 0;
            wordTens++;
            secs=0;
            if(wordTens >= 10){
              wordTens = 0;
              wordHundreds++;
              if(wordHundreds >= 10){
                secs = 0;
                wordUnits = 0;
                wordTens = 0;
                wordHundreds = 0;
                wordThousands++;
                  if(wordThousands >= 10){
                  secs = 0;
                  wordUnits = 0;
                  wordTens = 0;
                  wordHundreds = 0;
                  wordThousands = 0;
                }
              }
            }
          }
        }
}
void compareValues(){
  if(wordCycleValue >= wordCycleCompare){ //I use the equal or bigger to avoid any wrong cycle values...
            secs = 0;
            wordUnits = 0;
            wordTens = 0;
            wordHundreds = 0;
            wordThousands = 0;
            wordCycleValue = 0;
            swapLED();
    }
}
void resetAllValues(){
            secs = 0;
            wordUnits = 0;
            wordTens = 0;
            wordHundreds = 0;
            wordThousands = 0;
            wordCycleValue = 0;
            swapLED();
}
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(8, LOW);   //This will let the pin 8 at start with LOW value at output!
  digitalWrite(9, HIGH);  //This will let the pin 9 at start with HIGH value at output!
 
  secs=0;
  wordUnits = 0;
  wordUnits = secs;

//
  while(1){  //This will keep the internal execution inside the while !
//
        while(flag){ // This While loop need the change of status flag to go out!
          secs++;
          actionOverValues();
          showVal();
          delay(75);
        }   
//       
          if (buttonReleased2) {
          buttonReleased2= false;
          // do something here, for example print on Serial
          //Serial.println("Button pin 2 released");
          }
          if (buttonReleased3) {
          buttonReleased3= false;
          // do something here, for example print on Serial
          //Serial.println("Button pin 3 released");
          }
          actionOverValues();
          compareValues();
      showVal();   
  }//END of loop while
}//END of main Loop

//ISR Zone
ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
      secs++;
} 
void buttonReleasedInterrupt2() {
  //buttonReleased2 = true;
  flag = true;
}
void buttonReleasedInterrupt3() {
  //buttonReleased3 = true;
  wordCycleCompare = wordCycleValue;
  resetAllValues();
  flag = false;
}
