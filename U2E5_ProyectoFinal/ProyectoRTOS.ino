#include <ESP32Servo.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Creación de servos 
Servo servoC1;
Servo servoC2;

// Configuración de pines
#define BTNSTOP_PIN 34   // Botón de paro
#define BTNSTART_PIN 35  // Botón de arranque
#define POT_PIN 15  // Pin para potenciómetro 
#define S0_PIN 2   // Sensor para medir S0
#define S1_PIN 4   // Sensor para medir S1
#define S2_PIN 5   // Sensor para medir S2
#define S3_PIN 18   // Sensor S3 para activar banda
#define S4_PIN 19   // Sensor S4 para activar banda
#define S5_PIN 23   // Sensor S5 para activar banda
#define M1_PIN 12    // PWM para de motor M1
#define M1_IN1_PIN 13 // Entrada 1 para puente H
#define M1_IN2_PIN 32 // Entrada 2 para puente H
#define M2_PIN 33    // Activación de motor M2
#define M3_PIN 25    // Activación de motor M3
#define M4_PIN 26   // Activación de motor M4
#define C1_PIN 27   // Servomotor C1
#define C2_PIN 14   // Servomotor C2

// Declaración de semáforos para mutex
static SemaphoreHandle_t Mutex_M2;
static SemaphoreHandle_t Mutex_M3;
static SemaphoreHandle_t Mutex_M4;

// Variables para las tareas
TaskHandle_t task1Handle;
TaskHandle_t task2Handle;
TaskHandle_t task3Handle;
TaskHandle_t task4Handle; 
TaskHandle_t task5Handle;
TaskHandle_t task6Handle; 
TaskHandle_t task7Handle; 
TaskHandle_t task8Handle;  
TaskHandle_t task9Handle; 
TaskHandle_t task10Handle;

// Prototipos de funciones
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void task4(void *pvParameters); 
void task5(void *pvParameters);
void task6(void *pvParameters); 
void task7(void *pvParameters); 
void task8(void *pvParameters); 
void task9(void *pvParameters); 
void task10(void *pvParameters); 

// Variables globales
bool chambear = false; // Habilita el funcionamiento del sistema
int VelPWM = 0; // Determina la velocidad de la banda principal
int PosArriba = 90; // Posición de paso de los servomotores
int PosAbajo = 180; // Posición de bloqueo de los servomotores
bool Ldetectado = false;  // Indica si se detectó un objeto grande
bool Mdetectado = false;  // Indica si se detectó un objeto mediano
int LenBanda = 0; // Indica si existen paquetes grandes en la banda principal
int MenBanda = 0; // Indica si existen paquetes medianos en la banda principal
int CuentaL = 0;  // Almacena la cantidad de paquetes grandes clasificados
int CuentaM = 0;  // Almacena la cantidad de paquetes medianos clasificados
int CuentaS = 0;  // Almacena la cantidad de paquetes chicos clasificados
int TiempoL = 0;  // Almacenan el tiempo que tiene que encender las bandas 1, 2 y 3
int TiempoS = 0;
int TiempoM = 0;

void setup() {
  // Inicialización del puerto serial
  Serial.begin(115200);

  // Configuración de pines
  pinMode(BTNSTOP_PIN, INPUT);
  pinMode(BTNSTART_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(S0_PIN, INPUT);
  pinMode(S1_PIN, INPUT);
  pinMode(S2_PIN, INPUT);
  pinMode(S3_PIN, INPUT);
  pinMode(S4_PIN, INPUT);
  pinMode(S5_PIN, INPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(M1_IN1_PIN, OUTPUT);
  pinMode(M1_IN2_PIN, OUTPUT);
  pinMode(M2_PIN, OUTPUT);
  pinMode(M3_PIN, OUTPUT);
  pinMode(M4_PIN, OUTPUT);
  servoC1.attach(C1_PIN);
  servoC2.attach(C2_PIN);

  // Inicializar salidas
  digitalWrite(M2_PIN, LOW);
  digitalWrite(M3_PIN, LOW);
  digitalWrite(M4_PIN, LOW);
  digitalWrite(M1_PIN, LOW);
  digitalWrite(M1_IN1_PIN, LOW);
  digitalWrite(M1_IN2_PIN, LOW);
  servoC1.write(PosArriba);
  servoC2.write(PosArriba);

  ledcAttach(M1_PIN, 100, 12);

  //Crear mutex antes de comenzar las tareas
  Mutex_M2 = xSemaphoreCreateMutex(); 
  Mutex_M3 = xSemaphoreCreateMutex(); 
  Mutex_M4 = xSemaphoreCreateMutex(); 
  // Inicialización de tareas
  xTaskCreatePinnedToCore(task1, "Task 1", 4096, NULL, 1, NULL, 0); 
  xTaskCreatePinnedToCore(task2, "Task 2", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task3, "Task 3", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task4, "Task 4", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task5, "Task 5", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task6, "Task 6", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task7, "Task 7", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task8, "Task 8", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task9, "Task 9", 4096, NULL, 1, NULL, 0); //
  xTaskCreatePinnedToCore(task10, "Task 10", 4096, NULL, 1, NULL, 0); //
}

// Lectura de botones de paro y arranque
void task1(void *pvParameters) {
  while (1) {
    if (digitalRead(BTNSTART_PIN) == HIGH){
      chambear = true; // Habilitar sistema si se presiona botón de arranque
    } 
    if (digitalRead(BTNSTOP_PIN) == HIGH){
      chambear = false; // Deshabilitar sistema si se presiona botón de paro
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Activación de banda principal con M1
void task2(void *pvParameters) {
  int analogVel = 0;
  while (1) {
    // Lectura analógica de pin de potenciómetro
    analogVel = analogRead(POT_PIN);
    VelPWM = map(analogVel, 0, 4095, 0, 4095);
    if (chambear == true){
      // Actualiza PWM y señales que controlan velocidad de banda principal
      ledcWrite(M1_PIN, VelPWM);
      digitalWrite(M1_IN1_PIN, HIGH);
      digitalWrite(M1_IN2_PIN, LOW);
    } else {
      // Deshabilita señales que controlan banda principal
      analogWrite(M1_PIN, 0);
      digitalWrite(M1_IN1_PIN, LOW);
      digitalWrite(M1_IN2_PIN, LOW);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Medición de cajas
void task3(void *pvParameters) {
  while (1) {
    if (chambear == true){ 
      if ((analogRead(S0_PIN) < 50)&&(digitalRead(S1_PIN) == LOW)){
        Ldetectado = true; // Detección de paquete grande
        LenBanda ++; // Cantidad de paquetes grandes actualmente en banda aumenta 1
        while((analogRead(S0_PIN) < 50)&&(digitalRead(S1_PIN) == LOW)){
          vTaskDelay(10 / portTICK_PERIOD_MS);
          // Espera a que el paquete se retire del sensor
        }
      } else if((analogRead(S0_PIN) > 1000)&&(digitalRead(S1_PIN) == LOW)&&(LenBanda==0)){
        Mdetectado = true; // Detección de paquete mediano
        MenBanda ++;
        while((analogRead(S0_PIN) > 1000)&&(digitalRead(S1_PIN) == LOW)){
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// Redireccionamiento y conteo de cajas grandes
void task4(void *pvParameters) {
  while (1) {
    if(Ldetectado){
      // Si se detectó paquete grande
      while (digitalRead(S3_PIN) == HIGH){
        // C1 a posición baja para desviar paquete a banda 1
        servoC1.write(PosAbajo);
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
      if(xSemaphoreTake(Mutex_M2, pdMS_TO_TICKS(100)) == pdTRUE){
        // Establecer el tiempo de encendido de banda 1
        TiempoL = 10000;
        xSemaphoreGive(Mutex_M2);
      }
      LenBanda --; // Al detectar caja en banda 1, disminuye la cantidad en banda 0
      CuentaL ++; // Aumenta el total de cajas clasificadas
      while (digitalRead(S3_PIN) == LOW) {
        // Espera a despejar sensor S3
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
      Ldetectado = false;
    } else {
      // Quita barrera levantando C1
      servoC1.write(PosArriba);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


// Redireccionamiento y conteo de cajas medianas
void task5(void *pvParameters) {
  while (1) {
    if(MenBanda > 0){
      servoC2.write(PosAbajo); // Si hay paquetes medianos en banda, baja barrera de C2 para desviarlos
      if (digitalRead(S4_PIN) == LOW){
        if(xSemaphoreTake(Mutex_M3, pdMS_TO_TICKS(100)) == pdTRUE){
          // Establecer el tiempo de encendido de banda 2
          TiempoM = 10000;
          xSemaphoreGive(Mutex_M3);
        }
        MenBanda --; // Al detectar caja en banda 1, disminuye la cantidad en banda 0
        CuentaM ++; // Aumenta el total de cajas clasificadas
        while (digitalRead(S4_PIN) == LOW) {
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
      }
    } else {
      servoC2.write(PosArriba); // Levanta C2
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


// Conteo de cajas pequeñas
void task6(void *pvParameters) {
  while (1) {
    if (digitalRead(S5_PIN) == LOW){
      // Si se bloquea sensor S5
      if(xSemaphoreTake(Mutex_M4, pdMS_TO_TICKS(100)) == pdTRUE){
          TiempoS = 10000; // Establecer el tiempo de encendido de banda 3
          xSemaphoreGive(Mutex_M4);
      }
      CuentaS ++; // Aumenta el total de cajas clasificadas
      while (digitalRead(S5_PIN) == LOW) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


// Activación de banda para paquetes grandes 
void task7(void *pvParameters) {
  while (1) {
    if(chambear == true){ // Si el sistema está activo
      if(TiempoL>0){  
        if(xSemaphoreTake(Mutex_M2, pdMS_TO_TICKS(30)) == pdTRUE){
          TiempoL = TiempoL - 100; // Reducir tiempo restante cada 100ms
          vTaskDelay(100 / portTICK_PERIOD_MS);
          xSemaphoreGive(Mutex_M2);
        }
        // Mantener banda encendida mientras el conteo de tiempo no llegue a 0
        digitalWrite(M2_PIN, HIGH);
      }else{
        digitalWrite(M2_PIN, LOW); // Apagar banda
      }
    }else{
      digitalWrite(M2_PIN, LOW);  // Si el sistema no está activo, apaga banda
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


// Activación de banda para paquetes medianos 
void task8(void *pvParameters) {
  while (1) {
    if(chambear == true){ // Si el sistema está activo
      if(TiempoM>0){
        if(xSemaphoreTake(Mutex_M3, pdMS_TO_TICKS(30)) == pdTRUE){
          TiempoM = TiempoM - 100; // Reducir tiempo restante cada 100ms
          vTaskDelay(100 / portTICK_PERIOD_MS);
          xSemaphoreGive(Mutex_M3);
        }
        // Mantener banda encendida mientras el conteo de tiempo no llegue a 0
        digitalWrite(M3_PIN, HIGH);
      }else{
        digitalWrite(M3_PIN, LOW); // Apagar banda
      }
    }else{
      digitalWrite(M3_PIN, LOW); // Si el sistema no está activo, apaga banda
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Activación de banda para paquetes pequeños
void task9(void *pvParameters) {
  while (1) {
    if(chambear == true){ // Si el sistema está activo
      if(TiempoS>0){
        if(xSemaphoreTake(Mutex_M4, pdMS_TO_TICKS(30)) == pdTRUE){
          TiempoS = TiempoS - 100; // Reducir tiempo restante cada 100ms
          vTaskDelay(100 / portTICK_PERIOD_MS);
          xSemaphoreGive(Mutex_M4);
        }
        // Mantener banda encendida mientras el conteo de tiempo no llegue a 0
        digitalWrite(M4_PIN, HIGH);
      }else{
        digitalWrite(M4_PIN, LOW); // Apagar banda
      }
    }else{
      digitalWrite(M4_PIN, LOW); // Si el sistema no está activo, apaga banda
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}

// Petición de cuentas 
void task10(void *pvParameters) {
  while (1) {
    if (Serial.available()){
      String command = Serial.readStringUntil('\n'); // Leer comando
      if (command == "CUENTA"){
        // Imprime cuentas si recibe comando "CUENTA"
        Serial.print("\nPaquetes chicos: ");Serial.print(CuentaS);
        Serial.print("\nPaquetes medianos: ");Serial.print(CuentaM);
        Serial.print("\nPaquetes Grandes: ");Serial.print(CuentaL);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Uwu
}
