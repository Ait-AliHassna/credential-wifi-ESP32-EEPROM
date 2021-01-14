/*
 * ____ Create by Ait-Ali Hassna
 * ---------------/09/2020------------------
 * WIFI credentials stored in EEPROM  ESP32 --- with creation of wrb page
 */
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "****************"
#define FIREBASE_AUTH "*****************"
// Variables
int i=0;
int statuscode;
int dataval=2;
const char* ssid="wifi";// the username of  your WiFi
const char* pwd="******";// the password of your WIFI
String st;
String content;

// Declaration of Functions 
FirebaseData firebaseData;
bool wifitest(void);
void startweb(void);
void Setting(void);
//void launchwebpage();

BluetoothSerial myble;
// Establishing Server at port 80 to open a web page
WebServer  server(80); 
// create web page : Authentification 
String style =
"<style>#file-input,file-input_1{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"#file-input,file-input_1{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"form{background:#ebe834;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
"body { background-image: url(https://www.energeia-med.eu/wp-content/uploads/2019/11/%C3%89nergies-solaires.jpg); background-color: #cccccc}"
".btn{background:#3498db;color:#0ec4a3;cursor:pointer}</style>";

String stringHTML=
       "<!DOCTYPE HTML>\r\n<html>"
      " <h1 style:font-size:300%><strong><i>Configurer Votre WiFI ESP32:V1Solax </i></strong></h1> "
       "</p><form method='get' action='setting_submit'><p><label>SSID: </label><input name='ssid' id=file-input length=32 ></p> <p><label> Password </label><input name='pass' id=file-input length=32 type=Password minlength='8'> </p><input class=btn type='submit' value='Envoyer'></form>"
      
       "</html>"+style;
void setup() {
  // put your setup code here, to run once:WiFi
  // start to initialising the Serial Monitor 
  Serial.begin(115200);
   // disconnect to all wifi storted in eeprom
  Serial.println("Disconnect to all wifi storted in EEPROM");
  WiFi.disconnect();//disconnect to wifi 
  EEPROM.begin(512);// initialized my EEPROM
  delay(10);// waiting
  Serial.println("StartUP Now");
   //server.on("/", startweb);
    server.on("/",start_page_web);
   server.on("/setting_submit",connect_xifi); 
   server.onNotFound(handle_NotFound);

 
 
  //****************************------------------Start Reading in EEPROM-------------------***********
  Serial.println("Reading EEPROM SSID");
 
  String ressid="";
  for(int i=0;i<30;++i)
  {
  ressid+=char(EEPROM.read(i));
   
  }
 Serial.println("SSID");
  Serial.println(ressid);
 // read the password
  String password="";
  for(int i=30;i<94;++i)
  {
  password+=char(EEPROM.read(i));
  }
  Serial.println("password");
  Serial.println(password);
  // connect to wifi
 WiFi.begin(ressid.c_str(),password.c_str());
  //WiFi.begin(ssid,pwd);
  Serial.println(ressid.c_str());
 // Serial.print(WiFi.status());
   //WiFi.begin(ssid,pwd);
  if(wifitest())
  {
    Serial.println("Connected to your Device");
      startweb();
         //Setting();
    return;
  }
  else
  {
    Serial.println("Hotspot On");
    startweb();
    Setting();
  }
  Serial.println();
  Serial.print("Wait please *****************");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print("***");
    delay(200);
      server.handleClient();// listen to new Client (new wifi to wonnect with)
  }
   if(WiFi.status()==WL_CONNECTED)
   {
     Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
    startweb();
   }
}

void loop() {
  // put your main code here, to run repeatedly:
    server.handleClient();




}
// -------------------------------------------  WIFI TEST-----------------------

bool wifitest(void)
{
  int cnt=0;
  Serial.println("Waiting to connect to WiFi");
  while(cnt<20)
  {
    if(WiFi.status()==WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("***********");
    cnt++;
    }
    Serial.println("Time OUT ");// ESP32 is out of Time no Connected 
    return false;
  }

// launch page web
void startweb()
{
  if(WiFi.status()==WL_CONNECTED)
  {
    Serial.print("Wifi connected");
    Serial.println("LOCAL IP ADRESSS: ");
    Serial.println(WiFi.localIP());
    server.begin();
    Serial.println("Server Started ************************");
    
  }
}
//Get all wifi to connect with

void Setting(void)
{
  WiFi.mode(WIFI_STA);
 // WiFi.disconnect();
  delay(100);
  int nbSCan= WiFi.scanNetworks();
  Serial.println("Scan finished");
  if(nbSCan == 0)
  {
    Serial.println("No Network is Found");
  }
  else
  {Serial.print(nbSCan);
    for(int j=0;j<nbSCan;++j)
    {
      Serial.print(j+1);
      Serial.print("):");
      Serial.print(WiFi.SSID(j));
      Serial.print(")");
      delay(10);
      
    }
  }
  // try to connect the ESP32 to your wifi without password to get the IP address
  Serial.println("");
   WiFi.begin("wifi","");

   if(WiFi.status()==WL_CONNECTED)
   {
  startweb();
   }
    //Serial.println("over");
  
}
//Create Web page

//    Serial.println("Creation page");

 //server.on("/setting", []() {
 void start_page_web()
 {
   //server.on("/",[] () {
   IPAddress ip = WiFi.localIP();
      Serial.println("Connection to server ");
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
       server.send(200, "text/html", stringHTML);
       
 //});
 }
 void connect_xifi(){
      String ssidv = server.arg("ssid");
      String psd = server.arg("pass");
      if (ssidv.length() > 0 && psd.length() > 0) {// make sure that is not empty
        Serial.println("Start to clean EEPROM");
        for (int i = 0; i < 100; ++i) {
          EEPROM.write(i,0);// add all wifi 
        }
        Serial.println(ssidv);
        Serial.println("");
        Serial.println(psd);
        Serial.println("");

        Serial.println("Writing ssid in eeprom:");
        for (int i = 0; i < ssidv.length(); ++i)
        {
          EEPROM.write(i, ssidv[i]);
          Serial.print("Wrote: ");
          Serial.println(ssidv[i]);
        }
        Serial.println("add password too eeprom:");
        for (int i = 0; i < psd.length(); ++i)
        {
          EEPROM.write(30 + i, psd[i]);
          Serial.print("Wrote: ");
          Serial.println(psd[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statuscode = 200;
       ESP.restart();
      } /*else {
        content = "{\"Error\":\"404 not found\"}";
        statuscode = 404;
        Serial.println("Sending 404");
      }*/
    server.sendHeader("Access-Control-Allow-Origin", "*");
     server.send(statuscode, "application/json", content);

    }
 //  return content;
  void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}




