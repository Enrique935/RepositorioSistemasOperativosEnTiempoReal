#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

// Definimos las longitudes de las Queues
static const uint8_t len_Queue_A = 5;
static const uint8_t len_Queue_B = 5;

// Declaramos los manejadores para las Queues
static QueueHandle_t Queue_A;
static QueueHandle_t Queue_B;

// Función para procesar datos de la Queue A
void ProcesarTareaA(void *parametros) {
    static int contadorA = 0; // Variable local estática para el conteo
    int dato;
    while (1) {
        // Recibimos datos de Queue A
        if (xQueueReceive(Queue_A, (void*)&dato, 0) == pdTRUE) {
            Serial.print("Dato recibido en Tarea A: ");
            Serial.println(dato);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS); // Esperamos 50 ms

        // Enviamos datos a Queue A
        if (xQueueSend(Queue_A, (void*)&contadorA, 10) != pdTRUE) {
            Serial.println("Queue A llena");
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Esperamos otros 100 ms
        contadorA++;
    }
}

// Función para procesar datos de la Queue B
void ProcesarTareaB(void *parametros) {
    int dato;
    static int contadorB = 0; // Variable local estática para el conteo
    while (1) {
        // Recibimos datos de Queue B
        if (xQueueReceive(Queue_B, (void*)&dato, 0) == pdTRUE) {
            Serial.print("Dato recibido en Tarea B: ");
            Serial.println(dato);
        }
        vTaskDelay(60 / portTICK_PERIOD_MS); // Esperamos 50 ms

        // Enviamos datos a Queue B
        if (xQueueSend(Queue_B, (void*)&contadorB, 10) != pdTRUE) {
            Serial.println("Queue B llena");
        }
        vTaskDelay(120 / portTICK_PERIOD_MS); // Esperamos otros 100 ms
        contadorB++;
    }
}

void setup() {
    Serial.begin(115200); // Inicializamos comunicación serial
    Serial.println("Inicio del programa"); 
    delay(400); // Pausa breve para permitir el inicio

    // Creamos las Queues con las longitudes definidas
    Queue_A = xQueueCreate(len_Queue_A, sizeof(int));
    Queue_B = xQueueCreate(len_Queue_B, sizeof(int));

    // Creamos las tareas y las asignamos al núcleo específico
    xTaskCreatePinnedToCore(ProcesarTareaA, "Proceso A", 1024, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(ProcesarTareaB, "Proceso B", 1024, NULL, 1, NULL, app_cpu);
}

void loop() {

}