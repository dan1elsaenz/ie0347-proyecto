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

//Variables para la discretizacion 

float a=-1.924;
float b=0.9276;
float c=-1;
float d=-0.1353;
float g=111.11;
Scheduler runner;

void setHigh();
void setLow();
void controlador();

Task tHigh(halfPeriod, TASK_FOREVER, &setHigh, &runner, true);
Task tLow(halfPeriod, TASK_FOREVER, &setLow, &runner, true);
Task tcontrolador(10, TASK_FOREVER, &controlador, &runner, true);

void setHigh(){
  analogWrite(Vs, pwmMax);
  tLow.enableDelayed(halfPeriod); // Low en 3 segundos
}


void setLow(){
  analogWrite(Vs, pwmMin);
  tHigh.enableDelayed(halfPeriod); // Low en 3 segundos
}

void controlador(){

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

    u = -(d+c) * u_k1 - c*d * u_k2 + g * e - g*a * e_k1 + g*b * e_k2; 

  
    //Serial.println(MEntrada(A1)); //Para prueba 
   // Serial.println(MSalida(u));   //Para prueba

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

 runner.execute(); 
}






