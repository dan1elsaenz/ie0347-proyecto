#include <TaskScheduler.h>

const int Vs  = 6;               // Pin PWM 
const int pwmMin = 51;           // 1V  (≈ 51/255 * 5V)
const int pwmMax = 179;          // 3.5V (≈ 179/255 * 5V)
const int halfPeriod = 3000;     // 3000 ms = 3 segundos

bool ModoSenial = true; // Modo de senial en potenciometro por default :) 
float r = 0.0;
float y = 0.0; 
float e = 0.0;
float u_k1 = 0.0; 
float u_k2 = 0.0;
float e_k1 = 0.0;
float e_k2 = 0.0;
float u = 0.0; 


Scheduler runner;

void setHigh();
void setLow();

Task tHigh(halfPeriod, TASK_FOREVER, &setHigh, &runner, true);
Task tLow(halfPeriod, TASK_FOREVER, &setLow, &runner, true);

void setHigh(){
  analogWrite(Vs, pwmMax);
  tLow.enableDelayed(halfPeriod); // Low en 3 segundos
}


void setLow(){
  analogWrite(Vs, pwmMin);
  tHigh.enableDelayed(halfPeriod); // Low en 3 segundos
}


// Agregar ifs de la entrada con el potenciometro 

float MEntrada(int entrada){
  return map(entrada, 0, 1023, 1000, 3500) / 1000.0; 
}

float MSalida(int salida){
  return map(salida, 0, 255, 0, 5000) / 1000.0; 
}

bool modo(){
  //No creo que sea necesario meter manejo de errores pq el programa se debería de utilizar a conciencia. 
  Serial.println("Selección de modo para la señal: \n 1 - Potenciómetro \n 2 - Señal cuadrada");
  String entrada = Serial.readStringUntil('\n');
  if (entrada == 1){
    return true;
  } else if (entrada == 2){
    return false;
  }
}

float CalculoU(){

    //Calculo valor actual de U 

    u = 1.1353 * u_k1 - 0.1353 * u_k2 + 277.78 * e - 1.924 * 277.78 * e_k1 + 277.78 * 0.9276 * e_k2; 

  
    //Serial.println(MEntrada(A1)); //Para prueba 
    //Serial.println(MSalida(u));   //Para prueba

    u_k1 = u; 
    e_k1 = e; 
    u_k2 = u_k1;
    e_k2 = e_k1; 

  return u; 

  }



void setup() {
  Serial.begin(9600);
  pinMode(Vs, OUTPUT); 
  ModoSenial = modo();
}




void loop() {

  if (ModoSenial == true){
    r = MEntrada(analogRead(A1));
    y = MEntrada(analogRead(A2)); 
    e = r - y; 
    digitalWrite(MSalida(u), PD6); 
    Serial.println(y);
    CalculoU();
  } else if (ModoSenial == false){
      setHigh(); // Dentro de la funcion se llama a setLow para ir creadon la senial cuadrada de periodo 6s
  }
}







