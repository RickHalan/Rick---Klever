/*
by: Rick Hálan 

Klever
*/
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/queue.h"

#define analogPin PA0
#define ledPin PA5

QueueHandle_t xFila; 
TaskHandle_t xReadADCHandle, xLedControlHandle, xReadSerialHandle; 
byte data[5]; 
char L, D, I, B, A; 
/*
L = LIGAR LED 
D = DESLIGAR LED 
I = INVERTER 
B = LOOP BACK 
A = ADC 
 */
void conigInit(void); 
void ReadADC (void *pvParameters);
void LedControl (void *pvParameters);
void ReadSerial (void *pvParameters);

void setup() {
  conigInit(); 
  xFila = xQueueCreate(5, sizeof(int)) ;
  if (xFila == NULL){
    Serial.println("Nao foi possivel criar a fila"); 
    while(1); 
  }

  BaseType_t xReturned = xTaskCreate(ReadADC, "Task1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xReadADCHandle); 
  if (xReturned == pdFAIL){
    Serial.println("Nao foi possivel criar a task ReadADC"); 
    while(1);   
  }

  xReturned = xTaskCreate(LedControl, "Task2", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xLedControlHandle); 
  if (xReturned == pdFAIL){
    Serial.println("Nao foi possivel criar a task LedControl"); 
    while(1);   
  }

  xReturned = xTaskCreate(ReadADC, "Task3", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xReadSerialHandle); 
  if (xReturned == pdFAIL){
    Serial.println("Nao foi possivel criar a task ReadADC"); 
    while(1);   
  }
}
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000)); 
}

void conigInit(void){
  Serial.begin(115200); 
  pinMode(ledPin, OUTPUT); 
}

void ReadADC (void *pvParameters){
  char ValueReceive;
  while (1)
  {
    if (xQueueReceive(xFila, &ValueReceive, pdMS_TO_TICKS(1000)) == pdTRUE){
      if (ValueReceive == 'L') {
        float ValueAnalog = analogRead(analogPin); 
        Serial.print(ValueAnalog); 
      }
      ValueReceive = NULL; 
    }
  }
  
}
void LedControl (void *pvParameters){
 char ValueReceive; 
  while (1)
  {
    if (xQueueReceive(xFila, &ValueReceive, pdMS_TO_TICKS(1000)) == pdTRUE)
    {
      if (ValueReceive == 'L'){
        Serial.print("OK"); 
        digitalWrite(ledPin, HIGH); 
      }
      else if (ValueReceive == 'D'){
        Serial.print("OK"); 
        digitalWrite(ledPin, LOW);  
      }
      else if (ValueReceive == 'I'){
        Serial.print("OK"); 
        digitalWrite(ledPin, !digitalRead(ledPin));  
      }
      ValueReceive = NULL; 
    }
  }
  
}
void ReadSerial (void *pvParameters){
  while (1)
  {
    byte data [5]; 
    if (Serial.available()){
      Serial.readBytes(data, 5); 
      if(data[0] == 0x01){
        if (data[1] != 'B')
          xQueueSend(xFila, &data[1], portMAX_DELAY); 
        else 
        {            
          for (int i = 0 ; i < 5 ; i++){
              Serial.print(data[i]); 
           }
        }
      }
    }
  }
}
