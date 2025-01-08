#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static SemaphoreHandle_t mutex;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C LCD

//Giroscopio mpu - Variables
MPU6050 mpu;
int pitch=0;
int roll=0;
float tiempoMPU;
int caraActual = 0;
int caraAnterior = 0;
bool ContarMPU = false;

//Seno con serie taylor
const char msgSenoTaylor[] = "Polinomio de Taylor de grado 20 para seno calculado";
int msgSenoTaylor_leng = strlen(msgSenoTaylor);
float anguloGrados = 0; // Ángulo en grados
float anguloRads = 0; // Ángulo en radianes
float senoResultado = 0;       // Resultado del polinomio
const int maxTerms = 20; // Grado del polinomio
bool calcular = false;
bool calculando = false;

// Configuración de pines
#define LED1_PIN 2       // LED para Tarea 1
#define LED2_PIN 4       // LED para Tarea 3
#define LED3_PIN 19       // LED para Tarea 5
#define BUTTON1_PIN 5    // Botón para Tarea 1
#define BUTTON2_PIN 18   // Botón para Tarea 3
#define BUTTON3_PIN 12   // Botón para Tarea 6
const int adcPin = 34; // Pin de entrada ADC tarea 2

// Variables para las tareas
TaskHandle_t task1Handle;
TaskHandle_t task2Handle;
TaskHandle_t task3Handle;
TaskHandle_t task4Handle; 
TaskHandle_t task5Handle;
TaskHandle_t task6Handle; 
TaskHandle_t task7Handle; 
TaskHandle_t task8Handle; 

// Prototipos de funciones
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void task4(void *pvParameters); 
void task5(void *pvParameters);
void task6(void *pvParameters); 
void task7(void *pvParameters); 
void task8(void *pvParameters); 

//Mensajes a imprimir por terminal
const char T1E1[] = "Tarea 1: Botón presionado";
const char T1E0[] = "Tarea 1: Botón no presionado";
const char T2E0[] = "Tarea 2: Mostrando datos del ADC en consola";
const char T3E1[] = "Tarea 3: Botón presionado";
const char T3E0[] = "Tarea 3: Botón no presionado";
const char T5E0[] = "Tarea 5: Tiempo de retardo para suavizar brillo en led = ";
const char ComandoError[] = "Comando no reconocido. Ejemplo: ET01, ET02, ET03";
//Longitud de mensajes
int T1E1_leng = strlen(T1E1);
int T1E0_leng = strlen(T1E0);
int T2E0_leng = strlen(T2E0);
int T3E1_leng = strlen(T3E1);
int T3E0_leng = strlen(T3E0);
int T5E0_leng = strlen(T5E0);
int ComandoError_leng = strlen(ComandoError);

//Variable para ajustar el tiempo de retardo en tarea 5 (PWM)
int smoothTime = 10;

//------------------- void setup ------------------------------
void setup() {

  lcd.init(); // Inicializar LCD
  lcd.setBacklight(true); // Activar la retroiluminación del LCD
  lcd.clear();  //Limpiar LCD
  lcd.print("Iniciando...");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  // Configuración de pines
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(BUTTON3_PIN, INPUT);
  pinMode(adcPin, INPUT);

  // Inicialización del puerto serial
  Serial.begin(115200);

  // Inicializar el sensor MPU6050
  mpu.initialize();
  // Configurar el rango de escala del giroscopio y acelerómetro
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

  //Crear mutex antes de comenzar las tareas
  mutex = xSemaphoreCreateMutex();

  // Inicialización de tareas
  xTaskCreatePinnedToCore(task1, "Task 1", 2048, NULL, 1, &task1Handle, 1); //Secuencia LED
  xTaskCreatePinnedToCore(task2, "Task 2", 2048, NULL, 1, &task2Handle, 0); //Muestrea ADC
  xTaskCreatePinnedToCore(task3, "Task 3", 2048, NULL, 1, &task3Handle, 1); //Togglea LED
  xTaskCreatePinnedToCore(task4, "Task 4", 2048, NULL, 1, &task4Handle, 0); //UART
  xTaskCreatePinnedToCore(task5, "Task 5", 2048, NULL, 1, &task5Handle, 0); //PWM LED
  xTaskCreatePinnedToCore(task6, "Task 6", 4096, NULL, 1, &task6Handle, 0); //MPU
  xTaskCreatePinnedToCore(task7, "Task 7", 1024, NULL, 1, &task7Handle, 0); //MPU
  xTaskCreatePinnedToCore(task8, "Task 8", 2048, NULL, 1, &task8Handle, 0); //Calculo taylor
}
//----------------------------- Void loop --------------------------------- 
void loop() {
  // Nada en loop principal
}

// ----------------------------- Tareas ----------------------------------

// Tarea 1: Secuencia de LED según el estado del botón
void task1(void *pvParameters) {
  int contador = 0;
  while (1) {
    if (digitalRead(BUTTON1_PIN) == HIGH) {
      // Mantiene el led encendido durante 3 segundos si el boton se mantiene presionado
      while ((digitalRead(BUTTON1_PIN) == HIGH) && (contador <= 30)){
        digitalWrite(LED1_PIN, HIGH); // Encender LED
        contador++;
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      // Cambia el estado del led cada 500 ms mientras el boton siga presionado
      while (digitalRead(BUTTON1_PIN) == HIGH) {
        digitalWrite(LED1_PIN, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED1_PIN, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    } else {
      digitalWrite(LED1_PIN, LOW); // Apagar LED si no está presionado
      contador = 0;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); // Pequeño retraso
  }
}

// Tarea 2: Leer ADC y mostrar en pantalla
void task2(void *pvParameters) {
  while (1) {
    int adcValue = analogRead(adcPin);
    float voltage = (adcValue / 4095.0) * 3.3; // Conversión a voltaje (0-3.3V)
    //Serial.printf("ADC: %.2f V\n", voltage);  // Imprimir valor en consola (simula pantalla)
    // Mostrar el valor en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ADC: ");
    lcd.print(voltage, 2); // Mostrar con 2 decimales
    lcd.print(" Volt");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Actualizar cada segundo
  }
}

// Tarea 3: Parpadeo infinito del LED mientras botón esté presionado
void task3(void *pvParameters) {
  while (1) {
    if (digitalRead(BUTTON2_PIN) == HIGH) {
      digitalWrite(LED2_PIN, HIGH); // Enciende el led
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(LED2_PIN, LOW);  // Apaga el led
      vTaskDelay(500 / portTICK_PERIOD_MS);
    } else {
      digitalWrite(LED2_PIN, LOW);  // Si no se presiona, el led se mantiene apagado
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); // Pequeño retraso
  }
}

// Tarea 4: Comunicación UART para consultar el estado de las tareas
void task4(void *pvParameters) {
  while (1) {
    if (Serial.available()) {
      String command = Serial.readStringUntil('\n'); // Leer comando
      // Se utiliza mutex para no interrumpir mensajes impresos por otras tareas
      if(xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) == pdTRUE){
        if (command == "ET01") {
          // Estado de la Tarea 1 (Botón 1)
          if (digitalRead(BUTTON1_PIN) == HIGH) {
            // Si el botón está presionado, imprime "Tarea 1: Botón presionado"
            Serial.println();
            for(int i = 0; i<T1E1_leng; i++){
              Serial.print(T1E1[i]);
              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
          } else {
            // Si el botón NO está presionado, imprime "Tarea 1: Botón no presionado"
            Serial.println();
            for(int i = 0; i<T1E0_leng; i++){
              Serial.print(T1E0[i]);
              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
          }
        } else if (command == "ET02") {
          // Estado de la Tarea 2 (ADC)
          // Imprime "Tarea 2: Mostrando datos del ADC en consola"
          Serial.println();
          for(int i = 0; i<T2E0_leng; i++){
            Serial.print(T2E0[i]);
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
        } else if (command == "ET03") {
          // Estado de la Tarea 3 (Botón 2)
          if (digitalRead(BUTTON2_PIN) == HIGH) {
            // Si el botón está presionado, imprime "Tarea 3: Botón presionado"
            Serial.println();
            for(int i = 0; i<T3E1_leng; i++){
              Serial.print(T3E1[i]);
              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
          } else {
            // Si el botón NO está presionado, imprime "Tarea 3: Botón no presionado"
            Serial.println();
            for(int i = 0; i<T3E0_leng; i++){
              Serial.print(T3E0[i]);
              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
          }
        } else if (command == "ET05") {
          // Estado de la Tarea 5 
          // Imprime "Tarea 5: Tiempo de retardo para suavizar brillo en led = (tiempo)"
            Serial.println();
            for(int i = 0; i<T5E0_leng; i++){
              Serial.print(T5E0[i]);
              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            Serial.print(smoothTime);
        } else if (command.indexOf("T05TIME=") != -1) {
          // Modificar tiempo de suavizado de la Tarea 5
          int posicion = command.indexOf('='); // Obtiene la posición del '='
          String numero = command.substring(posicion + 1); // Extrae todo lo que está después del '='
          smoothTime = numero.toInt(); // Convierte la subcadena a entero
          // Imprime "Tarea 5: Tiempo de retardo para suavizar brillo en led = (tiempo)"
          Serial.println();
          for(int i = 0; i<T5E0_leng; i++){
            Serial.print(T5E0[i]);
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
          Serial.print(smoothTime);
        }else if (command.indexOf("Seno:") != -1) {
          Serial.println();
          // Introducir valor para calcular su seno
          int posicion = command.indexOf(':'); // Obtiene la posición del ':'
          String senoX = command.substring(posicion + 1); // Extrae todo lo que está después del ':'
          anguloGrados = senoX.toFloat(); // Convierte la subcadena a flotante
          calcular = true;
          Serial.print("Calculando...");
          while(calcular == true){
            vTaskDelay(20 / portTICK_PERIOD_MS);
          }
          Serial.println();
          for (int i = 0; i < msgSenoTaylor_leng; i++) {
            Serial.print(msgSenoTaylor[i]);
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
          Serial.println();
          Serial.printf("Ángulo: %.2f°\nSeno aproximado: %.8f\n", anguloGrados, senoResultado);          
        }else{
          // Cuando se introduce un comando no reconocido
          // Imprimir "Comando no reconocido. Ejemplo: ET01, ET02, ET03"
          Serial.println();
          for(int i = 0; i<ComandoError_leng; i++){
            Serial.print(ComandoError[i]);
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
        }
      xSemaphoreGive(mutex); //Libera mutex
      }else{}
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); // Pequeño retraso 
  }
}

// Tarea 5: Cambio de intensidad en led
void task5(void *pvParameters) {
  while (1) {
    // Aumenta el ciclo de trabajo de la señal pwm desde 0 hasta el máximo
    for (int pwmVal = 0; pwmVal <= 255; pwmVal++){
      analogWrite(LED3_PIN, pwmVal);
      vTaskDelay(smoothTime / portTICK_PERIOD_MS); // Retardo entre iteraciones personalizado (10ms por defecto)
    }
    // Decrementa el ciclo de trabajo de la señal pwm desde el máximo hasta el mínimo
    for (int pwmVal = 255; pwmVal >= 0; pwmVal--){
      analogWrite(LED3_PIN, pwmVal);
      vTaskDelay(smoothTime / portTICK_PERIOD_MS); // Retardo entre iteraciones personalizado (10ms por defecto)
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Tarea 6: MPU
void task6(void *pvParameters) {
  // Mensajes que se imprimen en la tarea
  const char InicioMPU []= "Conteo del MPU6050 iniciado";
  int InicioMPU_leng = strlen(InicioMPU);
  const char TiempoTotal []= "Tiempo total en cara ";
  int TiempoTotal_leng = strlen(TiempoTotal);
  // Variables locales
  bool imprimirCuentas = false;
  float cuenta1 = 0;
  float cuenta2 = 0;
  float cuenta3 = 0;
  float cuenta4 = 0;
  float cuenta5 = 0;
  float cuenta6 = 0;
  while (1) {
    while(ContarMPU == true){ 
      if (imprimirCuentas == false){
        if(xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) == pdTRUE){
          // Imprime "Conteo del MPU6050 iniciado" cuando inicia el conteo (al presionar boton)
          Serial.println();
          for(int i = 0; i<InicioMPU_leng; i++){
            Serial.print(InicioMPU[i]);
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
          //Liberamos seccion critica
          xSemaphoreGive(mutex);
        }else{
        }  
      }
      imprimirCuentas = true; //Indica que hay datos que imprimir
      // Leer los datos del sensor
      int16_t ax, ay, az, gx, gy, gz;
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
      // Convertir los datos en unidades de medida
      float accelX = ax / 16384.0; // Sensibilidad del acelerómetro para el rango de ±2g
      float accelY = ay / 16384.0;
      float accelZ = az / 16384.0;
      
      float gyroX = gx / 131.0; // Sensibilidad del giroscopio para el rango de ±250°/s
      float gyroY = gy / 131.0;
      float gyroZ = gz / 131.0;
      
      //----- Angulos pitch and roll a partir de un acelerometro//
      pitch = -(atan2(accelX, sqrt(accelY*accelY + accelZ*accelZ))*180.0)/M_PI;
      roll = ((atan2(accelY, accelZ)*180.0)/M_PI);
      // Pasamos los grados a valores de 0 a 1023 con la finalidad de no tener negativos
      roll=map(roll,-180,180,0,1023);
      pitch=map(pitch,-180,180,0,1023);
      
      int value1 = pitch;
      int value2 = roll;

      // Evalúa la cara en que está orientado en base al roll y ppitch obtenidos
      caraAnterior = caraActual;
      if ((value1 > 220 && value1 < 300) && (value2 > 0 && value2 < 120)){
        caraActual = 1; 
        cuenta1 += 0.1; // Aumenta +.1s el tiempo total que permanece orientado en esta cara
      } else if ((value1 > 440 && value1 < 540) && (value2 > 510 && value2 < 550)) {
        caraActual = 2;
        cuenta2 += 0.1;
      } else if ((value1 > 450 && value1 < 550) && (value2 > 710 && value2 < 810)) {
        caraActual = 3;
        cuenta3 += 0.1;
      } else if ((value1 > 450 && value1 < 550) && (value2 > 200 && value2 < 300)) {
        caraActual = 4;
        cuenta4 += 0.1;
      } else if ((value1 > 470 && value1 < 570) && (value2 > 980 && value2 < 1040)) {
        caraActual = 5;
        cuenta5 += 0.1;
      } else if ((value1 > 680 && value1 < 800) && (value2 > 470 && value2 < 580)) {
        caraActual = 6;
        cuenta6 += 0.1;
      } else {
        caraActual = caraActual;
        // Si no ocurre un cambio total de orientación, aumenta el tiempo en la cara actual
        if (caraActual == 1){
          cuenta1 += 0.1;
        }else if(caraActual == 2){
          cuenta2 += 0.1;
        }else if(caraActual == 3){
          cuenta3 += 0.1;
        }else if(caraActual == 4){
          cuenta4 += 0.1;
        }else if(caraActual == 5){
          cuenta5 += 0.1;
        }else if(caraActual == 6){
          cuenta6 += 0.1;
        }else{}
      }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Retardo de 100ms entre ciclos
    }

    // Impresión de conteo total al presionar botón por segunda vez
    if (imprimirCuentas == true){
      if(xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) == pdTRUE){
        // Imprime el tiempo orientado en cara 1:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("1 = ");
        Serial.print(cuenta1);
        // Imprime el tiempo orientado en cara 2:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("2 = ");
        Serial.print(cuenta2);
        // Imprime el tiempo orientado en cara 3:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("3 = ");
        Serial.print(cuenta3);
        // Imprime el tiempo orientado en cara 4:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("4 = ");
        Serial.print(cuenta4);
         // Imprime el tiempo orientado en cara 5:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("5 = ");
        Serial.print(cuenta5);
        // Imprime el tiempo orientado en cara 6:
        Serial.println();
        for(int i = 0; i<TiempoTotal_leng; i++){
          Serial.print(TiempoTotal[i]);
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        Serial.print("6 = ");
        Serial.print(cuenta6);
        //Liberamos seccion critica
        xSemaphoreGive(mutex);
      }else{
      }
    //Limpia el valor acumulado en las variables:
    imprimirCuentas = false;
    cuenta1 = 0;
    cuenta2 = 0;
    cuenta3 = 0;
    cuenta4 = 0;
    cuenta5 = 0;
    cuenta6 = 0;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }  
}

// Tarea 7: Asistencia a la tarea 6 para leer el botón
void task7(void *pvParameters) {
  while (1) {
    if (digitalRead(BUTTON3_PIN) == HIGH){
      //Si el botón se presiona y el MPU no está contando, comienza a contar
      if (ContarMPU == false){
        ContarMPU = true; // Permite a la tarea 6 comenzar el conteo
        while (digitalRead(BUTTON3_PIN) == HIGH){
          vTaskDelay(10 / portTICK_PERIOD_MS);        
        }
      }else{
        //Si el botón se presiona y el MPU está contando, termina conteo
        ContarMPU = false; // Indica a la tarea 6 terminar el conteo
        while (digitalRead(BUTTON3_PIN) == HIGH){
          vTaskDelay(10 / portTICK_PERIOD_MS);        
        }
      }
    }else{
      // No hacer nada
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// Tarea 8: Polinomio de Taylor de grado 20 para seno
void task8(void *pvParameters) {
  while (1) {
    if (calcular == true){
      anguloRads = anguloGrados * (PI / 180.0); // Convertir a radianes
      // Calcular el polinomio de Taylor de grado 20 para seno
      senoResultado = 0;
      for (int n = 0; n <= maxTerms; n++) {
        float term = pow(-1, n) * pow(anguloRads, 2 * n + 1) / tgamma(2 * n + 2); // Término n de Taylor
        senoResultado += term; // Suma término n de la serie al resultado
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }    
      calcular = false;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  } 
}
