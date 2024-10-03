//Pines de salida para cada tarea:
static const int tarea1 = 15;
static const int tarea2 = 2;
static const int tarea3 = 4;
static const int tarea4 = 16;
static const int tarea5 = 17;
static const int tarea6 = 5;
static const int tarea7 = 18;

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
//Mensaje a imprimir por terminal
const char msg[] = "UNIDAD PROFESIONAL INTERDISCIPLINARIA DE INGENIERIA CAMPUS ZACATECAS";
//Gestion de tareas 
static TaskHandle_t tarea_1 = NULL;
static TaskHandle_t tarea_2 = NULL;
static TaskHandle_t tarea_3 = NULL;
static TaskHandle_t tarea_4 = NULL;
static TaskHandle_t tarea_5 = NULL;
static TaskHandle_t tarea_6 = NULL;
static TaskHandle_t tarea_7 = NULL;

/*------------------Tareas-------------------------------*/
//Tarea 01: Imprimir mensaje caracter por caracter
void tarea01 (void *parameter){
  int msg_leng = strlen(msg);
  while(1){

    Serial.println();
    for(int i = 0; i<msg_leng; i++){
      digitalWrite(tarea1, HIGH);
      vTaskDelay(50 / portTICK_PERIOD_MS);
      Serial.print(msg[i]);
      digitalWrite(tarea1, LOW);
    }
    Serial.println();
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Tarea 02: Num 1
void tarea02 (void *parameter){
  while(1){
    digitalWrite(tarea2, HIGH);
    Serial.print('1');
    digitalWrite(tarea2, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

//Tarea 03: Num 2
void tarea03 (void *parameter){
  while(1){
    digitalWrite(tarea3, HIGH);
    Serial.print('2');
    digitalWrite(tarea3, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

//Tarea 04: Num 3
void tarea04 (void *parameter){
  while(1){
    digitalWrite(tarea4, HIGH);
    Serial.print('3');
    digitalWrite(tarea4, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

//Tarea 05: Num 4
void tarea05 (void *parameter){
  while(1){
    digitalWrite(tarea5, HIGH);
    Serial.print('4');
    digitalWrite(tarea5, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

//Tarea 06: Num 5
void tarea06 (void *parameter){
  while(1){
    digitalWrite(tarea6, HIGH);
    Serial.print('5');
    digitalWrite(tarea6, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

//Tarea 07: Num 6
void tarea07 (void *parameter){
  while(1){
    digitalWrite(tarea7, HIGH);
    Serial.print('6');
    digitalWrite(tarea7, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

void setup(){
  pinMode(tarea1, OUTPUT);
  pinMode(tarea2, OUTPUT);
  pinMode(tarea3, OUTPUT);
  pinMode(tarea4, OUTPUT);
  pinMode(tarea5, OUTPUT);
  pinMode(tarea6, OUTPUT);
  pinMode(tarea7, OUTPUT);

  Serial.begin(300);
  vTaskDelay(10 / portTICK_PERIOD_MS);
  Serial.println('\n');
  //Configuración tareas:
  xTaskCreatePinnedToCore(tarea01,
                          "Tarea 1",
                          1024,
                          NULL,
                          4,    
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
                          3,    
                          &tarea_3,
                          app_cpu);
  xTaskCreatePinnedToCore(tarea04,
                          "Tarea 4",
                          1024,
                          NULL,
                          1,    
                          &tarea_4,
                          app_cpu);
  xTaskCreatePinnedToCore(tarea05,
                          "Tarea 5",
                          1024,
                          NULL,
                          2,    
                          &tarea_5,
                          app_cpu);
  xTaskCreatePinnedToCore(tarea06,
                          "Tarea 6",
                          1024,
                          NULL,
                          1,    
                          &tarea_6,
                          app_cpu);
  xTaskCreatePinnedToCore(tarea07,
                          "Tarea 7",
                          1024,
                          NULL,
                          2,    
                          &tarea_7,
                          app_cpu);
}
void loop(){

}
