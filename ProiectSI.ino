#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

// FUNCTIE PT DELAY-uri

// FUNCTIE PENTRU AFISAREA DE MESAJE PE ECRANUL LCD


// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Keypad 
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {12, 11, 10, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// PIR sensors
const int PIR_SENSOR1 = 2; // the pin that the sensor is atteched to
int state_sensor1 = LOW; // by default, no motion detected
int val_sensor1 = 0; // variable to store the sensor status (value)

const int PIR_SENSOR2 = 3;              
int state_sensor2 = LOW;             
int val_sensor2 = 0;                 

// BUZZER
const int BUZZER_1 = 4;
const int BUZZER_2 = 5;
unsigned long previous_millis = 0UL;

// SERVO
const int SERVO_PIN = 13;
Servo Servo;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(PIR_SENSOR1 , INPUT);    // initialize sensor as an input 
  pinMode(PIR_SENSOR2, INPUT);
  Servo.attach(SERVO_PIN);
  
  Serial.begin(9600);        // initialize serial
}

// FUNCTIE PENTRU ARMAREA ALARMEI
// cod introdus de la tastatura
// armarea alarmei va actiona servomotorul

int alarm_status = 0; // 0 - alarm off/1 - alarm on
int menu = 1;
char code[] = "12";
char cod_activare[10];
char cod_d[10];
int i = 0;
int clear_menu4 = 0;
void alarm() {
  
  char key;
  
  //if(alarm_status == 0) {
  // mesaj initial
  switch(menu) {
    case 1:
      lcd.setCursor(0, 0);        
      lcd.print("Alarm disabled"); 
      lcd.setCursor(0,1);
      lcd.print("Enter the Code..");
      key = keypad.getKey();
      if(key) {
       menu = 2; 
      }
      break;
    case 2:
      key = keypad.getKey();
      Serial.println(cod_activare);
      if (key){
        cod_activare[i]=key;
        i++;
      }
    
      if((strlen(cod_activare)==0 || strcmp(cod_activare, "") == 0) && i == 0) {
        lcd.clear();
      }
    
      if(strlen(cod_activare) != 0 && i != 0) {
        lcd.setCursor(0, 0);
        for(int j = 0; j < i; j++) {
          lcd.print(cod_activare[j]);
        }
        
      }
      
    	
      if(strcmp(cod_activare, code) == 0 && i == 2) {
        lcd.clear();
        strcpy(cod_activare, "\0");
        cod_activare[0] = '\0';
		    menu = 3;
        i = 0;
        break; //  daca codul e iese din case-ul asta
      }
    
      if(strlen(cod_activare) == strlen(code) && i == 2) {
     	  strcpy(cod_activare, "\0");
        cod_activare[0] = '\0';
        i = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong Code");
        delay(500);
        lcd.setCursor(0,1);
        lcd.print("Try Again..");
        delay(1000);
        
      }
      break;
     case 3:
      clear_menu4 = 0;
    	lcd.setCursor(0, 0);
        lcd.print("Alarma e armata");
    	lcd.setCursor(0, 1);
        lcd.print("Zone libere");
        // Make servo go to 0 degrees 
        Servo.write(0); 
        delay(250); 
        // Make servo go to 90 degrees 
        Servo.write(90); 
        delay(250); 
        // Make servo go to 180 degrees 
        Servo.write(180); 
        delay(250);
    	menu = 4;
    	break;
   	  case 4:
        if(clear_menu4 == 0) {
          lcd.clear();
          clear_menu4 = 1;
        }
        pir_state();
        break;
        case 5:
        pir_state();
        key = keypad.getKey();
        if(strlen(cod_d)==0 && key && i == 0) {
            lcd.clear();
        }
        
        if (key){
          cod_d[i]=key;
          i++;
        }

        if(strlen(cod_d) != 0 && i != 0) {
          lcd.setCursor(0, 0);
          for(int j = 0; j < i; j++) {
            lcd.print(cod_d[j]);
          }
          
        }
  
        if(strcmp(cod_d, code) == 0 && i == 2) {
          lcd.clear();
          menu = 1;
          strcpy(cod_d, "\0");
          cod_d[0] = '\0';
          i = 0;
          break; //  daca codul e iese din case-ul asta
        }
      
        if(strlen(cod_d) == strlen(code) && i == 2) {
          strcpy(cod_d, "\0");
          cod_activare[0] = '\0';
          i = 0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Wrong Code");
          delay(500);
          lcd.setCursor(0,1);
          lcd.print("Try Again..");
          delay(1000);
          
        }
    	break;
  }
  
}

// FUNCTIE CE VERIFICA STAREA SENZORILOR PIR
int c1 = 0;
int c2 = 0;
void pir_state() {

  
  val_sensor1 = digitalRead(PIR_SENSOR1);   // read sensor value
  if (val_sensor1 == HIGH) {           // check if the sensor is HIGH
    
    if (state_sensor1 == LOW) {
 	  c1 = 1;
      lcd.clear();
      Serial.println("Motion detected sensor 1!");
      state_sensor1 = HIGH;       // update variable state to HIGH
    }
  } 
  else {
      if (state_sensor1 == HIGH){
        Serial.println("Motion stopped Sensor 1!");
        state_sensor1 = LOW;       // update variable state to LOW
    }
  }
  
  val_sensor2 = digitalRead(PIR_SENSOR2);   // read sensor value
  if (val_sensor2 == HIGH) {           // check if the sensor is HIGH
    
    if (state_sensor2 == LOW) {
      c2 = 1;
      lcd.clear();
      Serial.println("Motion detected sensor 2!"); 
      state_sensor2 = HIGH;       // update variable state to HIGH
    }
  } 
  else {  
      if (state_sensor2 == HIGH){
        Serial.println("Motion stopped Sensor 2!");
        state_sensor2 = LOW;       // update variable state to LOW
    }
  } 
  
  if(c1 == 1 && c2 == 0) {
    if(i == 0) {
      lcd.setCursor(0,0);
      lcd.print("Alerta Camera 1"); 
    }
    start_buzzer(1000, 1000, BUZZER_1);
    noTone(BUZZER_1);
    menu = 5;
  }
  if(c1 == 0 && c2 == 1) {
    if(i == 0) {
      lcd.setCursor(0,0);
      lcd.print("Alerta Camera 2");      
    }
	
    start_buzzer(1000, 1500, BUZZER_2);
    noTone(BUZZER_2);
    menu = 5;
  }
  if(c1 == 1 && c2 == 1) {
    if(i == 0) {
      lcd.setCursor(0,0);
      lcd.print("Alerta Ambele");
      lcd.setCursor(0,1);
      lcd.print("Camere");
    }
    start_buzzers(1000, 2000, BUZZER_1, BUZZER_2);
    //start_buzzer(1200, 2000, BUZZER_2);
    menu = 5;
  }
}

// millis 
void start_buzzer(int time, int tone_hz, int BUZZER) {
  unsigned long current_millis = millis();
  if(current_millis - previous_millis >  2 * time) {
    tone(BUZZER, tone_hz, time);
    previous_millis = current_millis;
  }

}

void start_buzzers(int time, int tone_hz, int BUZZER1, int BUZZER2) {
  unsigned long current_millis = millis();
  if(current_millis - previous_millis >  2 * time) {
    tone(BUZZER1, tone_hz, time);
    delay(1000);
    noTone(BUZZER1);
    tone(BUZZER2, tone_hz, time);
    previous_millis = current_millis;
  }

}
/*
void start_buzzer(int interval, int tone_hz) {
  unsigned long current_millis = millis();
  if(current_millis - previous_millis > 2 * interval) {
    tone(BUZZER, tone_hz, interval);
    previous_millis = current_millis;
  }        // ...for 0.1sec
}
*/

void loop(){
  
  // SERVOMOTOR CODE
  /*
  // Make servo go to 0 degrees 
  Servo.write(0); 
  delay(100); 
  // Make servo go to 90 degrees 
  Servo.write(90); 
  delay(100); 
  // Make servo go to 180 degrees 
  Servo.write(180); 
  delay(100);
  */
  
  // BUZZER CODE
  /*
  tone(BUZZER, 1000); // Send 1KHz sound signal...
  delay(100);         // ...for 0.1 sec
  noTone(BUZZER);     // Stop sound...
  delay(100);         // ...for 0.1sec
  */
  
  alarm();
  
  
}