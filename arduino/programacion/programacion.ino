#include <TaskScheduler.h>

// Resolucion de lectura y escritura en Arduino
#define RESOLUCION_LECTURA 1023
#define RESOLUCION_ESCRITURA 255

// Tiempos de tasks
#define TIEMPO_MUESTREO 100
#define TIEMPO_ALTO_CUADRADA 6000
#define TIEMPO_LECTURA_SWITCH 500

// Referencia de valores para senal cuadrada y pot
#define TENSION_ALTO 3.5
#define TENSION_BAJO 1
#define TENSION_MAX 5.0
#define TENSION_MIN 0.0

// Cero y ganancia (4 decimales)
#define Z 0.9594
#define K 1.88

// Entradas y salidas del Arduino
const int Vs = 6;     // Pin PWM
const int Vo = A2;    // Pin Vo
const int Ref = A1;   // Pin Pot
const int Switch = 7; // Pin Switch

// Variables del sistema de control
float referencia = 0.0;  // Referencia
float salida = 0.0;      // Salida actual
float e = 0.0;           // Error actual
float e_previo = 0.0;    // Error previo
float u;                 // Control actual
float u_previo = 0.0;    // Control previa

// Instanciar el objeto de Scheduler
Scheduler RealTimeCore;

// Senal cuadrada: para alternar entre 1 V y 3.5 V
bool tension_alta = true;

// Variable para elegir entre:
// potenciometro (true)
// senal cuadrada (false)
bool usar_pot = false;

// Definicion de la funcion del controlador
void controlador() {
  referencia = leer_referencia();
  salida = leer_salida();
  e = referencia - salida;

  // Ecuacion en diferencias del controlador
  u = u_previo + K*e - Z*K*e_previo;

  // Actualizacion de valores de instantes previos
  e_previo = e;
  u_previo = u;

  // Limites de senal de control
  u = constrain(u, TENSION_MIN, TENSION_MAX);

  // Escribir senal de control en la entrada de la planta
  float u_8b = (u * RESOLUCION_ESCRITURA) / TENSION_MAX;
  analogWrite(Vs, u_8b);
}

// Para determinar switch ON or OFF
void definir_entrada_switch(){
  int estado = digitalRead(Switch);
  if (estado == HIGH) {
    usar_pot = true; // Si el switch esta encendido
  }else if (estado == LOW) {
    usar_pot = false; // Si el switch esta apagado
  }
}

// Funcion para leer la referencia
float leer_referencia() {
  if (usar_pot) {
    // Realizar lectura del potenciometro
    int lectura = analogRead(Ref);

    return map(lectura, 0, RESOLUCION_LECTURA, TENSION_BAJO * 1000, TENSION_ALTO * 1000) / 1000.0;
  } else {
    // Alternar tension para la senal cuadrada
    if (tension_alta == true) {
      return TENSION_ALTO;
    }
    else {
      return TENSION_BAJO;
    }
  }
}

// Funcion para leer la salida de la planta
float leer_salida() {
  return (analogRead(Vo) * TENSION_MAX) / RESOLUCION_LECTURA;
}

// Alternar ref cuadrada
void cambiar_referencia() {
    if (!usar_pot) {
        // Se realiza el cambio periodicamente
        tension_alta = !tension_alta;
    }
}

// Funcion para imprimir los valores al monitor serial
void imprimir_datos() {
  Serial.print("r:");
  Serial.print(referencia);
  Serial.print("\t");
  Serial.print("y:");
  Serial.print(salida);
  Serial.print("\t");
  Serial.print("u:");
  Serial.println(u);
}

// Definir los tiempos de las tareas
Task taskControlador(TIEMPO_MUESTREO, TASK_FOREVER, &controlador, &RealTimeCore, true);

// Periodo de 12 s para la senal cuadrada
Task taskReferencia(TIEMPO_ALTO_CUADRADA, TASK_FOREVER, &cambiar_referencia, &RealTimeCore, true);

// Imprimir datos de r, y, u, cada lectura
Task taskImprimirDatos(TIEMPO_MUESTREO, TASK_FOREVER, &imprimir_datos, &RealTimeCore, true);

// Comprobar si hay cambio en el switch
Task taskEntradaSwitch(TIEMPO_LECTURA_SWITCH, TASK_FOREVER, &definir_entrada_switch, &RealTimeCore, true);

void setup() {
  Serial.begin(9600);

  // Configuracion de switch
  pinMode(Switch, INPUT);

  // Configuracion temporal
  RealTimeCore.startNow();
}

void loop() {
  RealTimeCore.execute();
}
