const int rele = 11;

/***   Setup  ***/
void setup() {
  Serial.begin(9600);
    pinMode(rele,OUTPUT);
}

/***   Loop  ***/
void loop() {
    int data = Serial.parseInt();
    if (data == 1){
      digitalWrite(rele, HIGH);
      delay(2000);
    }
    else{
      digitalWrite(rele, LOW);
    }
  
}
