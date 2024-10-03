//Frecuencia inicial de los LEDs
volatile int frecLED_UNO = 250;
volatile int frecLED_DOS = 250;

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//Pines de salida/entrada
static const int led_UNO = 15; // Señal verde
static const int led_DOS = 2; // Segundo Amarilla
static const int boton = 5; // Boton

//Tarea01: Toggle LED 1 actualizable por pulsador
void toggleLED_UNO(void *parameter) {
  while (1) {
    digitalWrite(led_UNO, HIGH);  
    vTaskDelay(frecLED_UNO / portTICK_PERIOD_MS); 
    digitalWrite(led_UNO, LOW);   
    vTaskDelay(frecLED_UNO / portTICK_PERIOD_MS); 
  }
}

//Tarea02: Toggle LED 2 actualizable por pulsador 
void toggleLED_DOS(void *parameter) {
  while (1) {
    digitalWrite(led_DOS, HIGH); 
    vTaskDelay(frecLED_DOS / portTICK_PERIOD_MS); 
    digitalWrite(led_DOS, LOW);  
    vTaskDelay(frecLED_DOS / portTICK_PERIOD_MS);  
  }
}

//Tarea03: Leer pulsador
void readBoton(void *parameter) {
  while (1) {
    if (digitalRead(boton) == 1){ //Se mantiene el pulsador
      frecLED_UNO = 1000;
      frecLED_DOS = 500;
    } else {                      //No se oprime el pulsador 
      frecLED_UNO = 250;
      frecLED_DOS = 250;
    }
}
}
void setup() {
  Serial.begin(115200);
  
  pinMode(led_UNO, OUTPUT);
  pinMode(led_DOS, OUTPUT);
  pinMode(boton, INPUT);

  //Configuración de las tareas:
  xTaskCreatePinnedToCore(
      toggleLED_UNO,    
      "Toggle LED 1", 
      1024,           
      NULL,           
      2,              
      NULL,           
      app_cpu);       
  xTaskCreatePinnedToCore(
      toggleLED_DOS,  
      "Toggle LED 2", 
      1024,           
      NULL,           
      3,              
      NULL,           
      app_cpu);       
  xTaskCreatePinnedToCore(
      readBoton,     
      "Leer boton",  
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
