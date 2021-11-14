char palabra;
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  if (Serial2.available()) {
    palabra = Serial2.read();
  }
  if (palabra != ' ') {  //para ver si no esta vacia la variable
    Serial.println(palabra);
  }
  palabra = ' ';
  delay(10);
}
