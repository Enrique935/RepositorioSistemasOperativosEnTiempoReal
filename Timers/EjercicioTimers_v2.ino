#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
 #else
 static const BaseType_t app_cpu = 1;
 #endif
//Timer con periodo de 1 segundo
 static TimerHandle_t auto_reload = NULL;
//Contador para numeros primos
 static int maxCount = 0;
 static int count = 0;
 static bool newInput = false;
//LED de salida para numeros primos
static const int ledPin = 2;
//Prototipo funcion detectar numero primo
 bool esPrimo(int num);

 void myTimer (TimerHandle_t xTimer){
  if (newInput && (count <= maxCount)) {
    //Conteo desde 0 en incremento hasta numero ingresado cada 1 segundo
    count++;
      Serial.println(count);
      //Verificacion numero primo
      if (esPrimo(count)) {
        digitalWrite(ledPin, HIGH);  // Encender el LED
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Mantener el LED encendido 500 ms
        digitalWrite(ledPin, LOW);   // Apagar el LED
      }

      if (count == maxCount) {
        Serial.println("Conteo finalizado.");
        newInput = false;  // Detener la cuenta hasta recibir un nuevo valor
        count = 0;
        maxCount = 0;
      }
  }
 }

//Lectura por monitor serial
 void readSerialTask(void *parameters) {
  while (1) {
    if (Serial.available() > 0) {
      int input = Serial.parseInt();
      if (input > 0) {
        maxCount = input;
        count = 0;
        newInput = true;
      } else {
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Espera 100 ms entre lecturas
  }
}


bool esPrimo(int num) {
  //Deteccion de numero primo por criterio de raiz cuadrada
  if (num <= 1) return false;
  for (int i = 2; i <= sqrt(num); i++) {
    if (num % i == 0) return false;
  }
  return true;
}

 void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  //Creamos Timer auto reload
  auto_reload = xTimerCreate(
          "Timer 01",//Nombre del Timer
           1000/ portTICK_PERIOD_MS,//Periodo en (ticks)
          pdTRUE,//Configuración
          (void *)1,//Timer ID
          myTimer);//Función
    xTimerStart(auto_reload,portMAX_DELAY);

  xTaskCreatePinnedToCore(readSerialTask, "readSerialTask", 2048, NULL, 1, NULL, 1);
 }
 
 void loop(){}