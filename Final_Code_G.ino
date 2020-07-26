#include<SoftwareSerial.h>
int sensor;   //to store the sensor value read from the Pin A0
int data;     //to store the data from LDR sensor
int light_status = 10;  
int threshold = 12;  //minimum light_intensity below which bulb gets triggerd


SoftwareSerial esp8266(3,4);
#define SSID "xxxx"   // replace xxx with name of the wifi
#define PASS "xxxx"  // replace xxxx with the wifi password


String sendAT(String command , const int timeout)  //user-defined function to send AT command from Arduino to ESP8266
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while((time+timeout)>millis())   //millis() is a delay function used when arduino connects with ThingSpeak
  {
    while(esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  Serial.print(response);
  return response;
}


void connectwifi()    //user-defined function to connect the arduino to wifi via ESP8266 with the g
{
  sendAT("AT\r\n",1000);
  sendAT("AT+CWMODE=1\r\n",1000);     //enables ESP8266 to remain in the station mode
  sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n",2000);    //connects ESP8266 to the desired wifi network
  while(!esp8266.find("OK"))
  {
    //wait for the connection
  }
  sendAT("AT+CIFSR\r\n",1000);   //derives the IP address of the ESP8266 connected to the internet
  sendAT("AT+CIPMUX=0\r\n",1000);   //enable the esp8266 to connect onlu one device for Wifi coonnectivity
}

void setup() 
{
  Serial.begin(9600);    
  esp8266.begin(9600);
  sendAT("AT+RST\r\n",2000);     //resets the esp8266 to 0 incase of the presence of any garbage value
  connectwifi();
  pinMode(8,OUTPUT);      // sets Pin 8 as the output to which the relay is connected
}

void loop() 
{
  sensor = analogRead(A0);
  data = map(sensor,0,1023,0,100);    //maps the sensor value read from A0pin within range of 0-100
  if(data<threshold)
  {
    digitalWrite(8,HIGH);       //triggers the relay ON as a result the Light bulb glows
    light_status = 100;
    
  }
  else
  {
    digitalWrite(8,LOW);      //triggers the relay OFF as a result the Light bulb stops glowing
    light_status = 10;
  }
  String sensor_value = String(data);
  Serial.print("Light Intensity = ");
  Serial.println(data);


  String light_status1 = String(light_status);
  Serial.print("Light Status = ");
  Serial.println(light_status);


  
  String threshold1 = String(threshold);
  Serial.print("Threshold");
  Serial.println(threshold);


  updateTS(sensor_value , light_status1 , threshold1);   //this function is used to send the collected parameters like sensor_value , light_status ,thhreshold  to THINGSPEAK for graphical representation 
  delay(1000);
  
}

void updateTS(String S , String L , String T)
{
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",1000);
  delay(2000);
  String cmdlen;
  String cmd = "GET /update?key=xxxxxxxxxxxxx&field1="+L+"&field2="+S+"&field3="+T+"\r\n";   //replace xxxxxx with the write api key available on the thingspeak account
  cmdlen = cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd);
  Serial.print("");
  sendAT("AT+CIPCLOSE\r\n",2000);
  Serial.println("");
  delay(1000);
  
}
