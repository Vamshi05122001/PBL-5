#include <SoftwareSerial.h>
#include <dht11.h>
int ldr_data= A0; // pin number of arduino connected to the ldr.
int mq_data=A1;  // pin number of arduino connected to the Gas Sensor.
int soilMt_data=A2;  // pin number of arduino connected to the Soil Moisture.
int dht_data=6;  // pin number of arduino connected to the dht11 Sensor.
int relay_motor=8;  // pin number of arduino connected to the motor relay pin.
int relay_buzzer=9;  // pin number of arduino connected to the buzzer relay pin.
int relay_light=10;// pin number of arduino connected to the light relay pin.
int ldr_value;  //Values getting from ldr_data pin.
int mq_value;  //Values getting from mq_data pin.
int soilMt_value;  //Value getting from soilMt_data pin.
int motor_value;   //Value getting from motor_data pin.
int ldr_percentage; //percentage of the mq_value.
int mq_percentage; //percentage of the mq_value.
int soilMt_percentage; //percentage of the mq_value
String AP = "Free";       // AP NAME
String PASS = ""; // AP PASSWORD
String API = "K02W96D5EC981UH5";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

  
SoftwareSerial esp8266( 2 ,3 ); 
dht11 dhtObject; 
void setup() {
  pinMode(ldr_data,INPUT);
  pinMode(mq_data,INPUT);
  pinMode(soilMt_data,INPUT);
  pinMode(relay_light,OUTPUT);
  pinMode(relay_buzzer,OUTPUT);
  pinMode(relay_motor,OUTPUT);
  Serial.begin(115200);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  
 String getData = "GET /update?api_key="+ API +"&field1="+ldrpercent()+"&field2="+mqpercent()+"&field3="+temperaturevalue()+"&field4="+humidityvalue()+"&field5="+soilMtpercent();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 delay(10000);
}

String ldrpercent(){
  Serial.println("=================================");
  ldr_value=analogRead(ldr_data);
  ldr_percentage=(ldr_value)/10;
  Serial.print("the percentage of light is ");Serial.println(ldr_percentage);
  if(ldr_percentage < 40)
  {
    digitalWrite(relay_light,LOW);
    Serial.println("Alert!, The Sunlight is low");
    Serial.println("Artificial light is ON");
  }
  else{
    digitalWrite(relay_light,HIGH);
    Serial.println("The Sunlight is Normal");
  }
  delay(10);
  return String(ldr_percentage);
}


String mqpercent(){
  Serial.println("=================================");
  mq_value=analogRead(mq_data);
  mq_percentage=(mq_value-24)/10;
  Serial.print("the percentage of air contamination is ");Serial.println(mq_percentage);
  if(mq_percentage > 27)
  {
    digitalWrite(relay_buzzer,LOW);
    Serial.println("Alert!, The air contamination is High");
  }
  else{
    digitalWrite(relay_buzzer,HIGH);
  }
  delay(10);
  return String(mq_percentage);
}



String temperaturevalue(){
  Serial.println("=================================");
   dhtObject.read(dht_data);
   int temp = dhtObject.temperature;
   Serial.print(" Temperature(C)= ");Serial.print(31*(int)temp);Serial.println(" *C, "); 
   delay(10);
   return String(31*temp); 
  
}


String humidityvalue(){
  Serial.println("=================================");
   dhtObject.read(dht_data);
   int humidity = dhtObject.humidity;
   Serial.print(" Humidity in %= ");Serial.print(24*(int)humidity);Serial.println(" H");
   delay(10);
   return String(24*humidity); 
  
}


String soilMtpercent(){
  Serial.println("=================================");
  soilMt_value=analogRead(soilMt_data);
  soilMt_percentage=(1000-soilMt_value)/2.65;
  Serial.print("The percentage of water in soil is ");Serial.println(soilMt_percentage);

  if(soilMt_percentage > 65)
  {
    digitalWrite(relay_motor,HIGH);
    motor_value=1;
  }
  else if(soilMt_percentage < 35)
  {
    digitalWrite(relay_motor,LOW);
   motor_value=0; 
  }
  if(motor_value==0)
  {
    Serial.println("The Water Pump is ON");
  }
  else
  {
   Serial.println("The Water Pump is OFF"); 
  }
  delay(10);
  return String(soilMt_percentage);
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
 



  
  
  
