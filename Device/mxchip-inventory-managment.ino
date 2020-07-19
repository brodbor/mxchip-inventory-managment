

#include "AZ3166WiFi.h"
#include "AzureIotHub.h"
#include "DevKitMQTTClient.h"
#include "OledDisplay.h"
#include "Sensor.h"
#include "SystemTickCounter.h"
#include "math.h"
#include "ArduinoJson.h"



const int anPin = 5;

static int sensor_run_count = 0;

const int local_sensor_max_runs = 10;
const int sensor_sampiling_time = 1000;
const int sensor_min_treshold =6;
const int sensor_max_treshold = 12;

//sensor readout
static int16_t in = 0;
static int16_t inSensor = 0;
static int16_t inHub = 0;
static int16_t isSpike = 0;
static int16_t sumArr[local_sensor_max_runs];


static int notification_sent;
static bool hasIoTHub = false;

static bool hasWifi = false;

static RGB_LED rgbLed;
static uint64_t send_interval_ms;

// Application  status
// 0 - idle
// 1 - work
static int app_status ;



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// sensor functions
void printRange()
{
  char msgBuf[100];

  long x = ((in - 6) * 100) / (12 - 6);


   Screen.print(0, "Sampling...");


  sprintf(msgBuf, "SEN CUR: %d", inSensor);
  Screen.print(1, msgBuf);

  sprintf(msgBuf, "SEN AVE: %d", in);
  Screen.print(2, msgBuf);

   if(inHub == 0){
    sprintf(msgBuf, "SEN HUB: PROC");
   }else{
     if(isSpike == 0){
          sprintf(msgBuf, "SEN HUB: %d", inHub);
     }else{
         sprintf(msgBuf, "SEN HUB:%d SPIKE", inHub);
     }
     
   }
   Screen.print(3, msgBuf);
   
}

void readSensor()
{
   //get sensor readouts
  inSensor = analogRead(anPin) / 2;

 //record sensor data based on sampling interval value. exlude sensor data outside of defined range.  
  if (((int)(SystemTickCounterRead() - send_interval_ms) >= sensor_sampiling_time) && (inSensor >=sensor_min_treshold && inSensor <=sensor_max_treshold ))
  {
    uint8_t i = 0;
    uint8_t j = 0;
    float mean = 0;
    sumArr[sensor_run_count] = inSensor;
    sensor_run_count += 1;
    //calculate num of elements in samplign array
    int num_elements = sizeof(sumArr) / sizeof(sumArr[0]);

    for (uint8_t i = 0; i < num_elements; i++)
    {
      if( sumArr[i] !=0){
        j++;
      }
      mean += sumArr[i];
    }

    in = round(mean / j);

    if (sensor_run_count == local_sensor_max_runs)
    {
        
      //send to IoT Hub every time sampling array is full
      callIoTHub();

      //reset app 
      //app_status = 0;
      //zero sampling array run count
      sensor_run_count = 0;
    }
  



     send_interval_ms = SystemTickCounterRead();
  }
 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


static void appCallBack(const char *payLoad, int size)
{

LogInfo("appCallBack:");
int pos1, pos2, pos3;


  const char* ptr_msg = &payLoad[0];
  int n = sscanf(ptr_msg, "%d,%d,%d", &pos1, &pos2, &pos3 );

    inHub = pos2;
    isSpike = pos3;
 

      if( pos1 == 1){
          Screen.clean();
          Screen.print(2, "Notificaiton Sent"); 
          delay(2000);
      }else if(pos1 == 2){
          Screen.clean();
          Screen.print(2, "Notificaiton Sent"); 
          delay(2000);
      }
}

static void callIoTHub()
{
  LogInfo("callIoTHub");
  //char msgBuf[100];

  DynamicJsonDocument doc(1024);
  doc["Volume"] = in;
  doc["Device"] = 1;
  String output;
  serializeJson(doc, output);
 // sprintf(msgBuf, "{\"Volume\": %d}", in);

  EVENT_INSTANCE *message = DevKitMQTTClient_Event_Generate(output.c_str(), MESSAGE);
  if (DevKitMQTTClient_SendEventInstance(message))
  {
    notification_sent = 2;
        Serial.printf("Sending data %s", "Data");  
  }
}


static void setInitMessage()
{

  Screen.print(0, "Azure IoT");
  Screen.print(1, "             ");
  Screen.print(2, "A - start App");
  Screen.print(3, "B - stop App ");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// az3166 functions


void setup()
{
  app_status = 0;
  notification_sent = 0;

  Screen.init();
  Serial.begin(115200);

  InitWiFi();
  InitHub();
  LogInfo("setup");
  rgbLed.setColor(0, 0, 0);

  DevKitMQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "az3166");
  DevKitMQTTClient_SetMessageCallback(appCallBack);


  send_interval_ms = SystemTickCounterRead();
}


static void appController()
{

  if (digitalRead(USER_BUTTON_A) == LOW)
  {
    app_status = 1;
  }


  if (digitalRead(USER_BUTTON_B) == LOW)
  {
    app_status = 0;
  }
}



static void InitWiFi()
{
  Screen.clean();
   Screen.print(0, "Connecting wifi...");
    
   if (WiFi.begin() == WL_CONNECTED)
  {
    Screen.print(2, "Running... \r\n");
  }
  else
  {
    hasWifi = false;
    Screen.print(2, "No Wi-Fi\r\n ");
  }
}

static void InitHub()
{
   Screen.clean();
   Screen.print(0, "Connecting IoT Hub..");
if (!DevKitMQTTClient_Init())
  {
    Screen.print(2, "No IoT Hub");
    delay(1000);

    hasIoTHub = false;
    return;
  }
}

void loop()
{
    

 
 appController();

    switch (app_status)
    {
    case 0:
             
               setInitMessage();
      break;

    case 1:
          readSensor();
          printRange();
          DevKitMQTTClient_Check();
      break;
    }
  
  
  delay(100);
}