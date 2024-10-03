static const int tarea1 = 15;
static const int tarea2 = 2;
static const int tarea3 = 4;

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//Gestion de tareas 
static TaskHandle_t tarea_1 = NULL;
static TaskHandle_t tarea_2 = NULL;
static TaskHandle_t tarea_3 = NULL;

/*------------------Tareas-------------------------------*/
//Tarea 01: imprime mensaje y se elimina
void tarea01 (void *parameter){
  int contador = 0;
  while(1){
    if (contador < 1){
      digitalWrite(tarea1, HIGH);
      Serial.print('1');
      digitalWrite(tarea1, LOW);
      contador ++;
    } else {
      vTaskDelete(tarea_1);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

//Tarea 02: Imprime mensaje 
void tarea02 (void *parameter){

  while(1){
    digitalWrite(tarea2, HIGH);
    Serial.print('2');
    vTaskDelay(500 / portTICK_PERIOD_MS); 
    digitalWrite(tarea2, LOW);  
  }
}

//Tarea 03: Imprime mensaje con alta priorirdad
void tarea03 (void *parameter){
  while(1){
    digitalWrite(tarea3, HIGH);
    Serial.print('3');
    vTaskDelay(500 / portTICK_PERIOD_MS);   
    digitalWrite(tarea3, LOW);
  }
}

void setup(){
  pinMode(tarea1, OUTPUT);
  pinMode(tarea2, OUTPUT);
  pinMode(tarea3, OUTPUT);
  Serial.begin(1200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  xTaskCreatePinnedToCore(tarea01,
                          "Tarea 1",
                          1024,
                          NULL,
                          1,
                          &tarea_1,
                          app_cpu);
  xTaskCreatePinnedToCore(tarea02,
                          "Tarea 2",
                          1024,
                          NULL,
                          1,
                          &tarea_2,
                          app_cpu);                          
  xTaskCreatePinnedToCore(tarea03,
                          "Tarea 3",
                          1024,
                          NULL,
                          2,
                          &tarea_3,
                          app_cpu);                          
}
void loop(){
  vTaskSuspend(tarea_2);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  vTaskResume(tarea_2);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
/*if (tarea_1 != NULL){
  vTaskDelete(tarea_1);
  tarea_1 = NULL;
}*/
}