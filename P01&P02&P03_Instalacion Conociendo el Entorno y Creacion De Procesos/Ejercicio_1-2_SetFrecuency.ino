//Frecuencias de los LEDs:
volatile int frecLED_UNO = 500;
volatile int frecLED_DOS = 500;

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//Pines de salida:
static const int led_UNO = 15; // Primer LED
static const int led_DOS = 2; // Segundo LED

//Tarea Toggle Led1: LED con frecuencia actualizable
void toggleLED_UNO(void *parameter) {
  while (1) {
    digitalWrite(led_UNO, HIGH); 
    vTaskDelay(frecLED_UNO / portTICK_PERIOD_MS); 
    digitalWrite(led_UNO, LOW);   
    vTaskDelay(frecLED_UNO / portTICK_PERIOD_MS);  
  }
}

//Tarea Toggle Led2: LED con frecuencia NO actualizable
void toggleLED_DOS(void *parameter) {
  while (1) {
    digitalWrite(led_DOS, HIGH);  
    vTaskDelay(frecLED_DOS / portTICK_PERIOD_MS);  
    digitalWrite(led_DOS, LOW);   
    vTaskDelay(frecLED_DOS / portTICK_PERIOD_MS);  
  }
}

//Tarea readFrecuency: Actualizar frecuencia del LED 1
void readFrequency(void *parameter) {
  while (1) {
    if (Serial.available() > 0) {
      int lectura = Serial.parseInt();
      if (lectura > 0) {
        frecLED_UNO = lectura;
        Serial.println("Frecuencia actualizada");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  
  //Pines de salida
  pinMode(led_UNO, OUTPUT);
  pinMode(led_DOS, OUTPUT);

  //Configuración Tarea 1:
  xTaskCreatePinnedToCore(
      toggleLED_UNO,   
      "Toggle LED 1", 
      1024,           
      NULL,           
      1,              
      NULL,           
      app_cpu);       

  //Configuración Tarea 2:
  xTaskCreatePinnedToCore(
      toggleLED_DOS,  
      "Toggle LED 2", 
      1024,           
      NULL,           
      2,              
      NULL,           
      app_cpu);       

  //Configuración Tarea 3:
  xTaskCreatePinnedToCore(
      readFrequency,     
      "Read Frequency",  
      1024,              
      NULL,              
      1,                 
      NULL,              
      app_cpu);          
}

void loop() {
  // put your main code here, to run repeatedly:
  //delay(10); // this speeds up the simulation
}
