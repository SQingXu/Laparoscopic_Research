int analogPin_g = 8;
int analogPin_s = 12;
int raw = 0;
float Vin = 3.3;
float Vout_g = 0;
float Vout_s = 0;
float R1 = 10000;
float R_g = 0;
float R_s = 0;
float buffer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

}

void loop() {
  // put your main code here, to run repeatedly:
  raw = analogRead(analogPin_g);
  if(raw){
    buffer = raw * Vin;
    Vout_g = (buffer)/1024.0;
    buffer = (Vin/Vout_g) - 1;
    R_g = R1 * buffer;
  }
  raw = analogRead(analogPin_s);
  if(raw){
    buffer = raw * Vin;
    Vout_s = (buffer)/1024.0;
    buffer = (Vin/Vout_s) - 1;
    R_s = R1 * buffer;
  }
  if(R_g && R_s){
    Serial.print("{ \"R_g\": ");
    Serial.print(R_g);
    Serial.print(",\"R_s\": ");
    Serial.print(R_s);
    Serial.println("}");
  }
  delay(30);
  
}
