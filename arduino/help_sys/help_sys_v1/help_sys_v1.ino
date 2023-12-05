#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

IPAddress server_addr(10,33,85,0);  // IP of the MySQL *server* here  10.33.88.21
char user[] = "esp8266";              // MySQL user login username
char sql_password[] = "123456";        // MySQL user login password

char INSERT_SQL[] = "INSERT INTO help_sys.users VALUES(NULL,'esp8266',0,20,NULL,NULL,119)";

String h ="76";
String y ="99";
String update_sql = "UPDATE help_sys.real_time_data SET heat="+h+",ox_yan ="+y+" WHERE id=1";
// const char* heat="76",ox_yan="99";
// char UPDATA_SQL[] = "UPDATE real_time_data SET heat="+heat+",ox_yan ="+ox_yan+" WHERE id=1";

const char* ssid     = "GUET-WiFi";
const char* password = "";

const char * host = "10.0.1.5";
const int httpPort = 801;

String ip;
WiFiClient client;                 // Use this for WFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

HTTPClient http;
String GetUrl;
String response;
String  data ,str;
int flag= 0,flag2=0;




void setup(){
  Serial.begin(9600);
  delay(10);
  // Serial.println();
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
 
  // Serial.println("");
  // Serial.println("WiFi connected");  
  // Serial.println("==========");
  // Serial.println("IP address: ");
  // Serial.println("==========");

  ip = WiFi.localIP().toString();
  // Serial.println(ip);
  // Serial.println(WiFi.localIP());

  //连接到网络后，打印ip
  // Serial.print("connecting to ");
  // Serial.println(host);
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    // Serial.println("connection failed");
    return;
  }
  delay(10);
  String v = String(random(1000,8000));
  // Serial.println("v--val: "+v);
  String getRequest =(String)("GET ") + "/eportal/portal/login?callback=dr1003&login_method=1&user_account=%2C0%2C23032307051&user_password=TlNpY2UyMDIzbGhs&wlan_user_ip="+
  ip+"&wlan_user_ipv6=&wlan_user_mac=1263963e0b3c&wlan_ac_ip="+ip+"&wlan_ac_name=HJ-BRAS-ME60-01&jsVersion=4.2&terminal_type=1&lang=zh-cn&v="+v+"&lang=zh"+ " HTTP/1.1\r\n"+
  "Host: " + host + "\r\n" +
  "User-Agent: arduino\r\n" +
  // "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n"+
  // "Accept-Encoding: gzip, deflate\r\n"+
  "Connection: close\r\n\r\n";
    
  // Serial.println(getRequest);
  client.print(getRequest);
  // Serial.println("request sent");
  delay(100);
  //获取响应
  String line = client.readStringUntil('\n');
  // Serial.println("reply was:");
  // Serial.println("==========");
  // Serial.println(line);   // 打印接受到的数据
  // Serial.println("==========");
  // if(line.substring(13,15)=="OK"){
  //   Serial.println("connect success!");
  // }
  // Serial.println("closing connection");
  client.stop();
  delay(100);
  // 连接苏宁网站的授时网页
  GetUrl = "http://quan.suning.com/getSysTime.do";
  http.setTimeout(5000);
  http.begin(client,GetUrl);

    delay(100);
  // Serial.println("");
  // Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, sql_password))
      delay(10);
    // Serial.println("mysql");
  // else
  //   Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);  
  if (conn.connected())
    cursor->execute(update_sql.c_str());
  // Serial.println("updata success.");
  // return ;
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
        // Serial.println("");
        Serial.print("time");
        Serial.print(response.substring(52, 60));
        Serial.println("end");
        //发送时间信息到串口
              }
  } 

  http.end();
  delay(1000);
}

void loop() {

  
  // delay(1000);
  //串口监视器可以辅助调试程序
  if (Serial.available() > 0)//串口接收到数据
  {
     
    size_t counti = Serial.available();
    byte sbuf[counti];
    Serial.readBytes(sbuf, counti);//client.println(data);
      int n = sizeof(sbuf);
      char chars[n + 1];
      memcpy(chars, sbuf, n);
      chars[n] = '\0';
    if(n<7){
      // int n = sizeof(sbuf);
      // char chars[n + 1];
      // memcpy(chars, sbuf, n);
      // chars[n] = '\0';
//      Serial.println(chars);
      if(memcmp(chars,"time",4)==0){
//        Serial.println("获取时间信息");
        get_time();
        }
      if(memcmp(chars,"emge",4)==0){
        flag2 = 1;
    }
    }else{
    // client.write(sbuf, counti);
    //对收到的串口数据进行处理

    if(memcmp(chars,"yan",3)==0){
        String data = chars;
        y = data.substring(3,5);
        // Serial.println(y);
        if(flag == 0)
        flag = 1;
    }
    if(memcmp(chars,"xin",3)==0){
        String data = chars;
        h = data.substring(3,5);
        // Serial.println(h);
        if(flag == 1)
        flag = 2;
    }





    }
    // delay(200);

  }

  if(flag==2){
    if (conn.connected())
      cursor->execute(String("UPDATE help_sys.real_time_data SET heat="+h+",ox_yan ="+y+" WHERE id=1").c_str());
    // Serial.println("updata success.");
    flag = 0;
  }
  if(flag2==1){
    if (conn.connected())
      cursor->execute(String("INSERT INTO help_sys.request_list VALUES(NULL,'emergency','esp8266','i need help !!!(sos)',NULL,NULL,NULL,2,NULL)").c_str());
    // Serial.println("insert success.");
    flag2 = 0;
  }

  //循环查询，是否有接收到，显示绿灯要
    if (conn.connected()){
      cursor->execute(String("SELECT accept FROM help_sys.real_time_data where id = 1").c_str());
      // Serial.println("SELECT success.");
      // Fetch the columns (required) but we don't use them.
      column_names *columns = cursor->get_columns();
      row_values *row = NULL;
      // row = cursor->get_next_row();
      int accept = 0;
      do {
            row = cursor->get_next_row();
            if (row != NULL) {
              accept = atol(row->values[0]);
              // Serial.println(accept);
            }
      } while (row != NULL);
      if(accept==1){
        Serial.println("accptend");
        cursor->execute(String("UPDATE help_sys.real_time_data SET accept=0 WHERE id=1").c_str());
      }
      }



}












