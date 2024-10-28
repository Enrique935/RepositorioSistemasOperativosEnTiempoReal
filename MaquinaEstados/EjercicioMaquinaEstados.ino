//Entradas
static const int Inicio = 12;
static const int SP1 = 2;
static const int SP2 = 4;
static const int HL = 5;

//Salidas
static const int VA = 18;
static const int VB = 19;
static const int V1 = 21;
static const int V2 = 22;
static const int M = 23;
static const int D = 13;

//Variables de control de estados
bool Edo1 = true;
bool Edo2 = false;
bool Edo3 = false;
bool Edo4 = false;
bool Edo5 = false;
bool Edo6 = false;

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//Gestion de tareas 
static TaskHandle_t E1 = NULL;
static TaskHandle_t E2 = NULL;
static TaskHandle_t E3 = NULL;
static TaskHandle_t E4 = NULL;
static TaskHandle_t E5 = NULL;
static TaskHandle_t E6 = NULL;

/*------------------Tareas-------------------------------*/
//Maquina de estados
//Estado 1
void Estado01 (void *parameter){
  while(1){
    if (Edo1 == true){
      if (digitalRead(Inicio) == 0){
            digitalWrite(VA, LOW);
            digitalWrite(VB, LOW);
            digitalWrite(V1, LOW);
            digitalWrite(V2, LOW);
            digitalWrite(M, LOW);
      }else{
        Edo2 = true;
        Edo1 = false;
      }
    }else{
    	//ugu
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Estado 2
void Estado02 (void *parameter){
  while(1){
    if (Edo2 == true){
      if (digitalRead(SP1) == 0){
            digitalWrite(VA, HIGH);
            digitalWrite(VB, LOW);
            digitalWrite(V1, LOW);
            digitalWrite(V2, LOW);
            digitalWrite(M, LOW);
      }else{
        Edo3 = true;
        Edo2 = false;
      }
    }else{
    	//ugu
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Estado 3
void Estado03 (void *parameter){
  while(1){
    if (Edo3 == true){
      if (digitalRead(SP2) == 0){
            digitalWrite(VA, LOW);
            digitalWrite(VB, HIGH);
            digitalWrite(V1, LOW);
            digitalWrite(V2, LOW);
            digitalWrite(M, LOW);
      }else{
        Edo4 = true;
        Edo3 = false;
      }
    }else{
    	//ugu
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Estado 4
void Estado04 (void *parameter){
  while(1){
    if (Edo4 == true){
      if (digitalRead(HL) == 0){
            digitalWrite(VA, LOW);
            digitalWrite(VB, LOW);
            digitalWrite(V1, HIGH);
            digitalWrite(V2, LOW);
            digitalWrite(M, LOW);
      }else{
        Edo5 = true;
        Edo4 = false;
      }
    }else{
    	//owo
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Estado 5
void Estado05 (void *parameter){
  while(1){
    if (Edo5 == true){
    	  digitalWrite(VA, LOW);
    	  digitalWrite(VB, LOW);
    	  digitalWrite(V1, LOW);
    	  digitalWrite(V2, HIGH);
    	  digitalWrite(M, HIGH);
    	  vTaskDelay(2000 / portTICK_PERIOD_MS);
	      Edo6 = true;
        Edo5 = false;
	
    }else{
    	//UwUr
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//Estado 6
void Estado06 (void *parameter){
  while(1){
    if (Edo6 == true){
    	  digitalWrite(VA, LOW);
    	  digitalWrite(VB, LOW);
    	  digitalWrite(V1, LOW);
    	  digitalWrite(V2, LOW);
    	  digitalWrite(M, LOW);
    	  digitalWrite(D, HIGH);
	  vTaskDelay(1000 / portTICK_PERIOD_MS);
	  Edo1 = true;
    Edo6 = false;
	
    }else{
    	digitalWrite(D, LOW);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup(){
  //Entradas
  pinMode(Inicio, INPUT);
  pinMode(SP1, INPUT);
  pinMode(SP2, INPUT);
  pinMode(HL, INPUT);

  //Salidas
  pinMode(VA, OUTPUT);
  pinMode(VB, OUTPUT);
  pinMode(V1, OUTPUT);
  pinMode(V2, OUTPUT);
  pinMode(M, OUTPUT);
  pinMode(D, OUTPUT);

  Serial.begin(1200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  xTaskCreatePinnedToCore(Estado01,
                          "Estado 1",
                          1024,
                          NULL,
                          2,
                          NULL,
                          app_cpu);
  xTaskCreatePinnedToCore(Estado02,
                          "Estado 2",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  xTaskCreatePinnedToCore(Estado03,
                          "Estado 3",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  xTaskCreatePinnedToCore(Estado04,
                          "Estado 4",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  xTaskCreatePinnedToCore(Estado05,
                          "Estado 5",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
  xTaskCreatePinnedToCore(Estado06,
                          "Estado 6",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
                       
}

void loop(){

}
