
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "a_pink_pig";
const char* password = "88888888";

//创建TCP server对象,并将端口指定为8888
WiFiServer server(8888);

HTTPClient http;

String GetUrl;
WiFiClient client;
String response;
//char* str;
//char* data;
//char data[15];

String  data ,str;
 void setup() {
  // put your setup code here, to run once:
  //打开串口
  Serial.begin(9600);

    //初始化板载LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

    //连接WiFi
//  Serial.println();
//  Serial.print(F("Connecting to "));
//  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(F("."));
  }
//     Serial.println();
//     Serial.println(F("WiFi connected"));

    //启动TCP server服务器
  server.begin();
//  Serial.println(F("Server started"));
 
  //打印IP地址
//  Serial.println(WiFi.localIP());
//JSON 数据解析容量


  // 连接苏宁网站的授时网页
  GetUrl = "http://quan.suning.com/getSysTime.do";

  http.setTimeout(5000);

  http.begin(client,GetUrl);
}

void get_time(){
    // 从网站获得网页内容
  int httpCode = http.GET();
  if (httpCode > 0) {
//      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        //读取响应内容
        response = http.getString();        
//        Serial.println(response);
        Serial.print("time");
        Serial.print(response.substring(52, 60));
        Serial.println("end");
        //发送时间信息到串口
              }
  } 
        //json
          /* 3. 使用deserializeJson()函数来解析Json数据 */
//        const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
//        //要注意容量大小，猜测前面为固定容量，几个对象填几，后面为总体的json数据字符串长度
//        DynamicJsonDocument doc(capacity);
//        deserializeJson(doc, response);
//          String time_data = doc["sysTime1"].as<String>();
//          Serial.println(time_data);       
        //json
//        Serial.println(time_data);

//  else {
//
//      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//  }

  http.end();

  delay(3000);
  
  }
void loop() {






  
  // put your main code here, to run repeatedly:
  //  检查客户端是否已连接
  WiFiClient client = server.available();

  while(client){

    delay(1000);
  //串口监视器可以辅助调试程序
  if (Serial.available() > 0)//串口接收到数据
  {
     
    size_t counti = Serial.available();
    byte sbuf[counti];
    Serial.readBytes(sbuf, counti);//client.println(data);
    if(counti<7){
      int n = sizeof(sbuf);
      char chars[n + 1];
      memcpy(chars, sbuf, n);
      chars[n] = '\0';
//      Serial.println(chars);
      if(memcmp(chars,"time",4)==0){
//        Serial.println("获取时间信息");
        get_time();
        }
    }
    client.write(sbuf, counti);
    delay(200);

  }
 }











 
//  else{
////    Serial.print("h");
//  }
  


    
//  if (!client) {
//    return;
//    Serial.println(F("no client"));
//  }else
//  client.println("some thing to say");
////   client.setTimeout(5000); 
//delay(5000);
   

  



  

}
