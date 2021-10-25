
String palabra1;          //variable en la q ira la palabra
String palabra2;          //variable en la q ira la palabra
byte cuenta = 0;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  Serial3.write('h');
  while (Serial3.available() && (cuenta < 3)) {   //Confirmando que la comunicación Serial esté disponible para hacer la lectura del mensaje
    delay(10);                    //para dar estabilidad al programa
    char c = Serial3.read();
    palabra1 += c;
    cuenta++;
  }
  cuenta = 0;
  Serial.println("Dato1: " + palabra1);
  delay(50);

  Serial3.write('t');
  while (Serial3.available() && (cuenta < 3)) {  //Confirmando que la comunicación Serial esté disponible para hacer la lectura del mensaje
    delay(10);                    //para dar estabilidad al programa
    char c = Serial3.read();
    palabra2 += c;
    cuenta++;
  }
  Serial.println("Dato2: " + palabra2);
  cuenta = 0; 
  palabra1 = "";
  palabra2 = "";
}
