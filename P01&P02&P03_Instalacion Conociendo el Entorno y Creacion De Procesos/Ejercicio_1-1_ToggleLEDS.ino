#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//Pines de salida:
static const int led_UNO = 15;
static const int led_DOS = 2;

void toggleLED_UNO(void *parameter){
  while(1){
    digitalWrite(led_UNO, HIGH);
    vTaskDelay(250 / portTICK_PERIOD_MS);
    digitalWrite(led_UNO, LOW);
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

void toggleLED_DOS(void *parameter){
  while(1){
    digitalWrite(led_DOS, HIGH);
    vTaskDelay(250 / portTICK_PERIOD_MS);
    digitalWrite(led_DOS, LOW);
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}


void setup(){
  //Configuracion de GPIO como salida
  pinMode(led_UNO, OUTPUT);
  pinMode(led_DOS, OUTPUT);
  
  //Task to run forever
  xTaskCreatePinnedToCore(      //Use xTaskCreate() in vanilla FreeRTOS
                  toggleLED_UNO,    //Funtion to be called
                  "Toggle L1", //Name of task
                  1024,         //Stack size (bytes in ESP32, words)
                  NULL,         //Paremeter to pass to function
                  1,            //Task priority (0 to configMAX_PRIORITY)
                  NULL,         //Task HAndle
                  app_cpu);     //Run on one core for demo purposes

 xTaskCreatePinnedToCore(      //Use xTaskCreate() in vanilla FreeRTOS
                  toggleLED_DOS,    //Funtion to be called
                  "Toggle L2", //Name of task
                  1024,         //Stack size (bytes in ESP32, words)
                  NULL,         //Paremeter to pass to function
                  1,            //Task priority (0 to configMAX_PRIORITY)
                  NULL,         //Task HAndle
                  app_cpu);     //Run on one core for demo purposes
}
void loop(){
  // put your main code here, to run repeatedly:
  //delay(10); // this speeds up the simulation
}
