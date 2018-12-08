/************************************************************************
* 	Water Level COntroller using ATMEGA328P
* 	File:   main.c
* 	Author:  Jithin Krishnan.K
*       Rev. 0.0.1 : 23/07/2015 :  11:09 AM
* 
*	This program is free software: you can redistribute it and/or modify
*  	it under the terms of the GNU General Public License as published by
*  	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*	Email: jithinkrishnan.k@gmail.com
*   
************************************************************************/

int led_green = 2;        // Auto Manual status LED
int led_red = 3;          // Error LED
int led_yellow = 4;       // Motor ON LED
int led_blue = 5;         // Auto Manual Switch LED
int buzzer  = 6;          // Buzzer
int relay   = 7;          // Relay 
int motor_on  = 8;        // Manual Motor Start
int auto_manual  = 9;     // Auto manual mode Select     
int tank_low = 10;        // Sensor Tank Low
int tank_full = 11;       // Sensor Tank Full 
int well_low = 12;        // Sensor Well Low
int led_blue_en = 0, 
    led_green_en = 0, 
    led_yellow_en = 0,
    led_red_en = 0, 
    buzzer_en = 0;
    
boolean toggle = 0;
int motor_on_en = 0;
int mode = -1;
int buzzer_flag = 1;

/* Switch Debounce control varables */
long lastDebounceTime = 0;
long debounceDelay = 1;
int buttonState, ledState = LOW; 
int lastButtonState = LOW;
int reading;     

void setup(){
  /* PIN Mode configuration */
  /* Configure below Pin as OUTPUT */
  pinMode(led_blue, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  /* Configure below Pin as INPUT */
  pinMode(motor_on, INPUT);
  pinMode(auto_manual, INPUT);
  pinMode(tank_low, INPUT);
  pinMode(tank_full, INPUT);
  pinMode(well_low, INPUT);
  
  cli(); // Stop interrupts
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;  // Set entire TCCR1A register to 0
  TCCR1B = 0;  // Same for TCCR1B
  TCNT1  = 0;  // Initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // Enable interrupts
  
  digitalWrite(relay, HIGH);
  buzzer_en = 2;
  for (int i = 0; i <= 5; i++) {
    digitalWrite(led_yellow, LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(led_red, LOW);
    digitalWrite(led_blue, LOW);
    delay(250);
    digitalWrite(led_yellow, HIGH);
    delay(250);
    digitalWrite(led_green, HIGH);
    delay(250);
    digitalWrite(led_red, HIGH);
    delay(250);
    digitalWrite(led_blue, HIGH);
    delay(250);
  }
  //delay(4000);
  buzzer_en = 0;
  //led_yellow_en = 0;
  //led_red_en = 0;
}//end setup

ISR(TIMER1_COMPA_vect) {
  toggle = !toggle;
  if(led_blue_en == 2) {
    digitalWrite(led_blue, toggle); 
  } else if (led_blue_en == 1) {
    digitalWrite(led_blue, HIGH);
  } else if (led_blue_en == 0) {
    digitalWrite(led_blue, LOW);
  }
 if(led_red_en == 2) {
    digitalWrite(led_red, toggle); 
  } else if (led_red_en == 1) {
    digitalWrite(led_red, HIGH);
  } else if (led_red_en == 0) {
    digitalWrite(led_red, LOW);
  }
 if(led_green_en == 2) {
    digitalWrite(led_green, toggle); 
  } else if (led_green_en == 1) {
    digitalWrite(led_green, HIGH);
  } else if (led_green_en == 0) {
    digitalWrite(led_green, LOW);
  }
 if(led_yellow_en == 2) {             // Toggle Yellow LED
    digitalWrite(led_yellow, toggle); 
  } else if(led_yellow_en == 1) {     // Yellow LED ON
    digitalWrite(led_yellow, HIGH);
  } else if ( led_yellow_en == 0) {   // Yellow LED OFF
    digitalWrite(led_yellow, LOW); 
  }
 if(buzzer_en == 2) {
    digitalWrite(buzzer, toggle);  // Buzzer Beep
  } else if(buzzer_en == 1) {      // Buzzer OFF           
    digitalWrite(buzzer, HIGH);       
  } else if(buzzer_en == 0) {
    digitalWrite(buzzer, LOW);
  }
  if (motor_on_en == 1) {
    digitalWrite(relay, LOW);
  } else {
   digitalWrite(relay, HIGH);
  }  
}
  

void loop() {
 
  if(digitalRead(auto_manual) != mode) {
    mode = digitalRead(auto_manual);
    if (mode == 1) {
       led_blue_en   = 0;
       led_green_en  = 2;
    } else if (mode == 0) {
       led_blue_en   = 2;
       led_green_en  = 1;
       buzzer_flag   = 1;
    }
    led_red_en  = 0;
    buzzer_en   = 0;
    led_yellow_en = 0;
    motor_on_en   = 0;
  } 
  if (digitalRead(well_low) == 1) {
      led_red_en  = 0;
      buzzer_en = 0;
      
  if (mode == 1) {       //Auto mode
      if (digitalRead(tank_low) == 0 && digitalRead(tank_full) == 0) {
          led_yellow_en = 1;
          motor_on_en = 1;
      } else if (digitalRead(tank_low) == 1 && digitalRead(tank_full) == 1) {
          led_yellow_en = 0;
          motor_on_en = 0;
      }
      
  } else if(mode == 0) { // Manual mode
  
      if (motor_on_en == 0) { 
        if (digitalRead(tank_low) == 0) {
            led_yellow_en = 2;
        } else if (digitalRead(tank_low) == 1 && digitalRead(tank_full) == 0) {
           led_yellow_en = 0;
           led_blue_en = 2;
        }  
      }

      // Manual Motor ON
      if (digitalRead(tank_full) == 0) {
          if (SwitchPress() == 1) {
               led_yellow_en = 1;
               motor_on_en = 1;
               led_blue_en = 0;
               if (buzzer_flag == 1) {
                 buzzer_en = 1;
                 delay(1000);
                 buzzer_en = 0;
                 buzzer_flag = 0;
               }     
           } 
        }
        
        if (digitalRead(tank_low) == 1 && digitalRead(tank_full) == 1) {
            led_yellow_en = 0;
            motor_on_en   = 0;
            buzzer_flag   = 1;
            led_blue_en   = 0;
      }
  }
 } else if (digitalRead(well_low) == 0) {
           if (mode == 1) {
             led_blue_en   = 0;
             led_green_en  = 2;
           } else if (mode == 0) {
             led_blue_en   = 2;
             led_green_en  = 1;
             buzzer_flag   = 1;
           }
           motor_on_en = 0;
           led_red_en  = 2;
           led_yellow_en = 0;
           buzzer_en = 2; 
       }
} 

int SwitchPress()
 {
           
     reading = digitalRead(motor_on);
     
     if (reading != lastButtonState) {
         lastDebounceTime = millis();
      } 
  
      if ((millis() - lastDebounceTime) > debounceDelay) {
  
        if (reading != buttonState) {
            buttonState = reading;
           if (buttonState == HIGH) {
                ledState = HIGH;
           } else {
                ledState = LOW;
          }
        }
     }
  
   lastButtonState = reading;

   return ledState;
}

