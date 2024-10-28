#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//Configuración de la LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
static SemaphoreHandle_t mutex;

//Pines de lectura para sensores de temperatura e infrarrojo
static const int TempPin = 12;
static const int InfraPin = 14;
//Variables globales lectura de sensores
static int temperatura;
static char infrarrojo;
//Tarea de leer Temperatura LM35
void ReadTemp(void *parameters){
  while(1){
    temperatura = analogRead(TempPin)*330/4096;
    //Seccion critica impresion a LCD
    if(xSemaphoreTake(mutex,0) == pdTRUE){
      lcd.setCursor(0, 1); lcd.print("Temp: ");
      lcd.setCursor(8, 1); lcd.print(temperatura);
      //Liberamos seccion critica
      xSemaphoreGive(mutex);
    }else{
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//Tarea de leer presencia Infrarrojo
void ReadInfra(void *parameters){
  while(1){
    if(analogRead(InfraPin) < 300){
        //Presencia detectada
        infrarrojo = 'Y';
      }else{
        //No presencia detectada
        infrarrojo = 'N';
      }
    //Seccion critica impresion a LCD
    if(xSemaphoreTake(mutex,0) == pdTRUE){
      lcd.setCursor(0, 3); lcd.print("Infra: ");
      lcd.setCursor(8, 3); lcd.print(infrarrojo);
      //Liberamos seccion critica
      xSemaphoreGive(mutex);
    }else{
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
Serial.begin(9600);
//LCD
  Wire.begin(21,22);
  lcd.init();       // Inicializa la pantalla LCD
  lcd.backlight();  // Activa la retroiluminación
  lcd.setCursor(0, 0); lcd.print("Lectura sensores");

vTaskDelay(1000 / portTICK_PERIOD_MS);

//Crear mutex amtes de comenzar las tareas
mutex = xSemaphoreCreateMutex();

  xTaskCreate(ReadTemp,"ReadTemp",2048,NULL,1,NULL);

  xTaskCreate(ReadInfra,"ReadInfra",2048,NULL,1,NULL);
}

void loop() {}
