#include <TaskScheduler.h>
// Entradas y salidas del Arduino
const int Vs = 6; // Pin PWM
const int Vo = A2; // Pin Vo
const int Ref = A1; // Pin Pot
const int Switch = 7; // Pin Switch
const int Boton = 8; // Pin Switch


// Definición e inicialización
float ePrv[2] = {0.0, 0.0};
float uPrv[2] = {0.0, 0.0};
float uk;

// Constantes para la función u(k)
const float a = -1.924, b = 0.9276, c = -1, d = -0.1353, g = 69.444;

// Crear el objeto de Scheduler
Scheduler RealTimeCore;

// Definir las funciones de las tareas
float leerReferencia();
float leerSalida();
void funcionControlador();
void toggleReferencia();
void printLog();
void definirEntradaBoton();
void definirEntradaSwitch();

// Definir los periodos de las tareas
Task taskControlador(75, TASK_FOREVER, &funcionControlador, &RealTimeCore, true);
Task taskReferencia(6000, TASK_FOREVER, &toggleReferencia, &RealTimeCore, true);
Task taskPrintLog(75, TASK_FOREVER, &printLog, &RealTimeCore, true);
//Task taskEntradaSwitch(500, TASK_FOREVER, &definirEntradaSwitch, &RealTimeCore, true);
Task taskEntradaBoton(500, TASK_FOREVER, &definirEntradaBoton, &RealTimeCore, true);

// Variable para alternar entre 1 V y 3.5 V
bool referenciaAlta = true;

// Variable para elegir Pot
bool usarPotenciometro = false;

//Funcion con boton 
void definirEntradaBoton(){
  int estado = digitalRead(Boton);
  if (estado == HIGH) {
    usarPotenciometro = !usarPotenciometro; 
  }
  Serial.print(estado);
  Serial.print(",");
  Serial.print(usarPotenciometro);
   Serial.println();
}



//Fucnión para deternimar Switch ON or OFF

void definirEntradaSwitch(){
  int estado = digitalRead(Switch);
  if (estado == HIGH) {
    usarPotenciometro = true; //Si el switch está encendido
  }else if (estado == LOW) {
    usarPotenciometro = false; // Si el switch está apagado
  }
  //Serial.print(estado);
}

// Función para alternar la referencia entre 1 V y 3.5 V
void toggleReferencia() {
    if (!usarPotenciometro) {
        referenciaAlta = !referenciaAlta;
    }
}

// Función para leer la referencia
float leerReferencia() {
  if (usarPotenciometro) {
    int lectura = analogRead(Ref);
    return map(lectura, 0, 1023, 1000, 3500) / 1000.0;
  } else {
      return referenciaAlta ? 3.5 : 1.0;
  }
}

// Función para leer la salida de la planta
float leerSalida() {
  return (analogRead(Vo) * 5.0) / 1023.0;
}

// Definición de la función del controlador
void funcionControlador() {
  float referenciaVolt = leerReferencia();
  float salidaVolt = leerSalida();
  float e = referenciaVolt - salidaVolt;

  // Ecuación en diferencias del controlador
  uk = uPrv[0] + 1.7361*e - 0.9714 * 1.7361 * ePrv[0];

  // Actualización de valores de instantes previos
  // ePrv[1] = ePrv[0];
  ePrv[0] = e;
  // uPrv[1] = uPrv[0];
  uPrv[0] = uk;

  // Normalización para PWM
  uk = constrain(uk, 0.0, 5.0);
  float u_normalizado = (uk * 255.0) / 5.0;

  analogWrite(Vs, u_normalizado);
}

// Función para imprimir los valores al monitor serial
void printLog() {
  float referenciaVolt = leerReferencia();
  float salidaVolt = leerSalida();
  float e = referenciaVolt - salidaVolt;

  // Serial.print("r: ");
  // Serial.print(referenciaVolt);
  // Serial.print(", y: ");
  // Serial.print(salidaVolt);
  // Serial.print(", u: ");
  // Serial.print(uk);
  //Serial.print(referenciaVolt);
  //Serial.print(", ");
  //Serial.print(salidaVolt);
  //Serial.print(", ");
  //Serial.print(uk);
  //Serial.print(", ");
  //Serial.print(uPrv[0]);
  //Serial.print(", ");
  //Serial.print(e);
  //Serial.print(", ");
  //Serial.println();
}

void setup() {
  pinMode(Boton,INPUT_PULLUP);
  pinMode(Switch, INPUT);
  Serial.begin(9600);
  RealTimeCore.startNow();
}

void loop() {
  //definirEntrada();
  RealTimeCore.execute();
  //Serial.println(leerReferencia());
}