/*
 * 
 * Framework for ESP-12
 * Start: July 08, 2018
 * 
 */


// TODO
/* 
 * - возможность подключения на несколько WI-FI
 * 
 * 1 - DHT11
 * 2 - BME280: температура, давление, влажность
 * 4 - освещенность
 * 8 - УФ-датчик
 * 16 - акустический датчик
 * 32 - датчик наличия осадков
 * 64 - внешний АЦП
 * - счетчик Гейгера
 * - запыленность
 * - концентрация CO, CO2
 * 
 */

// ----------------------------------------------------------------------------------------------
// Settings:
#define PIN_LED D4 // 2/D4 - адрес LED, распаяного на ESP8266
                   // 13(blue)/12(green) - для двухцветного LED в ESP-кубике
                   // 5-D1, 16-D0, 4-D2...

const char* network = "TABLET"; // SMART
const char* viewer = "WEBSERVER"; // JSON
const int sensors = 1; // в двоичном виде 1 - активен, 0 - отсутствует

// нога для кнопки выбора режимов

#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
//#include <ESP8266WebServer.h>
// SSID, password
//const char* ssid = "MDAhotspot";
//const char* password = "ec7b8a411598";
const char* ssid = "Samsung Galaxy Core 5614";
const char* password = "aczu4748";
WiFiServer server(80);                 // Указываем порт Web-сервера, запущенного на самой ESP8266

#define COUNT 64
unsigned long time_start, time_end;
float time_length;


// нога для ADC о питании(VCC)
// ноги для I2C шины
// ноги для ADC освещенности, звукового давления и пр.

#include <DHT11.h>
int pinDHT11 = 5;
DHT11 dht11(pinDHT11);

#include <BMP280.h>                    // убедись, что #define BMP280_ADDR в BMP280.h именно 0x76
                                       // иначе нужно подсадить SDO на VCC (на чипе, конечно)
//BMP280 bmp1;
//BMP280 bmp2;

#include <ArduinoJson.h>


//1.- The range of operating voltage of ESP8266 is 1.8V~3.6V
//2.- getVcc function (system_get_vdd33): is only available when TOUT pin17 is suspended (floating), this function measure the power voltage of VDD3P3 pin 3 and 4 (in the ESP8266 chip)
//3.- RF must be enabled.
// измерение напряжения питания
//ADC_MODE(ADC_VCC);
// -- loop
//float voltaje=0.00f;
//voltaje = ESP.getVcc();
//Serial.print(voltaje/1024.00f);
//Serial.println(" V");

// ----------------------------------------------------------------------------------------------
double Fahrenheit(double celsius) { return ((double)(9 / 5) * celsius) + 32; }
double Kelvin(double celsius) { return celsius + 273.15; }



void setup() {

  Serial.begin(115200);
  Serial.println("There is SETUP branch, once");

  Serial.print("LED PIN - ");
  Serial.println(PIN_LED);

  pinMode(PIN_LED, OUTPUT); 
  // ********** вспышка 500 мс
  digitalWrite(PIN_LED, HIGH);
  delay(500);
  digitalWrite(PIN_LED, LOW);
  delay(500);
  // подключиться к WiFi точке доступа
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("SSID=");
  Serial.println(ssid);
  Serial.print("PASS=");
  Serial.println(password);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  //
  server.begin();                        // возможно сервер нужно стартовать после подключения к WiFi
  Serial.println("Server started");
  // создать объект
  
  // --------------------------------------------------------------------------------------------
  // прочитать данные DHT11
  /*  
  int err;
  float tempDHT11, humiDHT11;
  if ((err = dht11.read(humiDHT11, tempDHT11)) == 0) 
  {
    Serial.print("temperature:");
    Serial.print(tempDHT11);
    Serial.print(" humidity:");
    Serial.print(humiDHT11);
    Serial.println();
  }
  else
  {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err);
    Serial.println();
  }
  */
  // --------------------------------------------------------------------------------------------
  // прочитать данные BMP
  /*
  double T1,P1,T2,P2,dP,dPinch,dPmm;
  if (!bmp1.begin(0,2)) 
  {
    while (1);
  }
  bmp1.setOversampling(4);
  char result1 = bmp1.startMeasurment();
  if (result1 != 0)
  {
    delay(result1);
    result1 = bmp1.getTemperatureAndPressure(T1,P1);
      if(result1 != 0)
      {
      }
  }

  if (!bmp2.begin(1,3)) 
  {
    while (1);
  }
  bmp2.setOversampling(4);
  char result2 = bmp2.startMeasurment();
  if (result2 != 0)
  {
    delay(result2);
    result2 = bmp2.getTemperatureAndPressure(T2,P2);
      if(result2 != 0)
      {
      }
  }
  dP = P2 - P1;
  dPinch = dP / 248.7562;
  dPmm = dP / 9.80665;
  */
  
  // --------------------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------------------
  // отправка данных GET-запросом
  /*  
  WiFiClient client_get; // client для отправки информации путем GET-запроса
  
  if (client_get.connect("192.168.0.17", 8080)) {
    client_get.print(" GET /collector.php?");
  //  client_remote.print("name=");
  //  client_remote.print("ESP8266#1234567890"); // Специальный код, например asREb25C
  //  client_remote.print("&");
    client_get.print("cicle=");
    client_get.print(cicle);
    client_get.print("&T1=");
    client_get.print(T1);
    client_get.print("&P1=");
    client_get.print(P1);
    client_get.print("&T2=");
    client_get.print(T2);
    client_get.print("&P2=");
    client_get.print(P2);
    client_get.print("&dPinch=");
    client_get.print(dPinch);
    client_get.print("&dPmm=");
    client_get.print(dPmm);
    client_get.println(" HTTP/1.1");
    client_get.print(" Host: " );
    client_get.println("192.168.0.15:8080");
    client_get.println(" Connection: close" );
    client_get.println();
    client_get.println();
    client_get.stop();
    client_get.flush();
  }
  */


  // --------------------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------------------
  // создать JSON и отправить
  /*
  // WiFiClient client_local = server.available();                // Получаем данные, посылаемые клиентом
  // плюс вся обвязка 
  while (client_local.connected()){                           // Пока есть соединение с клиентом 
    if (client_local.available()){                            // Если клиент активен 
     char c = client_local.read();                            // Считываем посылаемую информацию в переменную "с"
     if (c == '\n' && blank_line){                            // Вывод HTML страницы 

       client_local.println("HTTP/1.1 200 OK");               // Стандартный заголовок HTTP 
       client_local.println("Content-Type: text/plain"); 
       client_local.println();

       StaticJsonBuffer<200> jsonBuffer;
       JsonObject& root = jsonBuffer.createObject();
       root["cicle"] = cicle;
       root["T1"] = T1;
       root["P1"] = P1;
       root["T2"] = T2;
       root["P2"] = P2;
       root["dPinch"] = dPinch;
       root["dPmm"] = dPmm;
       int  Sec   = (millis() / 1000UL) % 60;
       int  Min   = ((millis() / 1000UL) / 60UL) % 60;
       int  Hours = ((millis() / 1000UL) / 3600UL) % 24;
       int  Day   = ((millis() / 1000UL) / 3600UL / 24UL);
       String s = "";
       s += Day;
       s += "d. ";
       s += Hours;
       s += ":";
       s += Min;
       s += ":";
       s += Sec;
       root["uptime"] = s;

       root.printTo(client_local);
       break;                                                 // Выход
       }
       if (c == '\n'){                                        // Если "с" равен символу новой строки                                             
         blank_line = true;                                   // Тогда начинаем новую строку
       } else if (c != '\r'){                                 // Если "с" не равен символу возврата курсора на начало строки                                        
         blank_line = false;                                  // Тогда получаем символ на текущей строке 
       }                                        
    }
  }  
    client_local.stop();                                      // Закрытие соединения
 }
  */
  
  // ----- если нога режима свободна
  // ----- ********** вспышка 150 мс
  // ----- запустить засыпание; 20e6 = 20,000,000 мкс или 20 с
  //ESP.deepSleep(20e6);
  // ----- иначе - ничего
  

}

// ----------------------------------------------------------------------------------------------
void loop() {

  Serial.println("There is LOOP");
  // ********** три вспышки, три паузы по 100 мс (или 150 мс)
  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(100);
  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(100);
  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);
  delay(500);


  // --------------------------------------------------------------------------------------------
  // прочитать данные DHT11
  
  int err;
  float tempDHT11, humiDHT11;
  if ((err = dht11.read(humiDHT11, tempDHT11)) == 0) 
  {
    Serial.print("temperature:");
    Serial.print(tempDHT11);
    Serial.print(" humidity:");
    Serial.print(humiDHT11);
    Serial.println();
  }
  else
  {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err);
    Serial.println();
  }

  // создать объект (?)
  String data_in[COUNT];
  String data_temp = "";

    // прочитать данные
    WiFiClient client = server.available();                // Получаем данные, посылаемые клиентом 

    // в объекте создать HTML-страницу с отображением данных
    if (client)
    {
      boolean blank_line = true;                            // Создаем переменную, чтобы определить конец HTTP-запроса 
      while (client.connected())
      {                           // Пока есть соединение с клиентом 
        if (client.available())
        {                            // Если клиент активен 
          char c = client.read();                            // Считываем посылаемую информацию в переменную "с"
          if (c == '\n' && blank_line)
          {                      // Вывод HTML страницы 
            time_start = micros();
            int i = 0;
            while (i < COUNT)
            {
              if (Serial.available())
              {
                char cs = Serial.read();
                if (cs == '\n')
                {
                  data_in[i] = data_temp;
                  data_temp = "";
                  i++;
                }
                else
                {
                  data_temp += cs;
                }
              }
            }
            time_end = micros();

            client.println("HTTP/1.1 200 OK");               // Стандартный заголовок HTTP 
            client.println("Content-Type: text/html"); 
            client.println("Connection: close");             // Соединение будет закрыто после завершения ответа
            client.println("Refresh: 10");                   // Автоматическое обновление каждые 10 сек 
            client.println();
            client.println("<!DOCTYPE HTML>");               // Веб-страница создается с использованием HTML
            client.println("<html>");                        // Открытие тега HTML 
            client.println("<head>");
            client.print("<title>BMP280 + HDC1080 = TEMP&PRES&HUM</title>");     // Название страницы
            client.println("</head>");
            client.println("<body>");
            client.println("<br/>");
            client.println("<br/>");
// -----------------------------------------------------------
            client.println("<h1>HDC1080 - Data</h1>"); 
            client.println("<h2>Manufacturer ID=0x");

            time_length = float(time_end - time_start) / 1000000;
            client.print("Result: ");
            client.print(COUNT);
            client.print(" readings were received for ");
            client.print(time_length,6);
            client.println(" seconds");

            client.println("</h2>");
            
            for (int j = 0; j < COUNT; j++)
            {
              client.print(" ");
              client.print(j);
              client.print("---");
//              /*
              client.print(data_in[j]);
//              client.print("8");
//              for (int k = 0; k < 4; k++) {
//                client.print(char_in[k]);
//              }
//              * /
              Serial.print(" ");
              Serial.print(j);
              Serial.print("---");
            }
            client.println("<H2>ESP8266 & DHT11 Sensor</H2>");
            client.println("<H3>Humidity / Temperature</H3>");
            client.println("<pre>");
            client.print("Humidity (%)         : ");
            client.println((float)humiDHT11, 2);
            client.print("Temperature (°C)  : ");
            client.println((float)tempDHT11, 2);
            client.print("Temperature (°F)  : ");
            client.println(Fahrenheit(tempDHT11), 2);
            client.print("Temperature (°K)  : ");
            client.println(Kelvin(tempDHT11), 2);

// -----------------------------------------------------------
            client.println("</body>");
            client.println("</html>");                       // Закрытие тега HTML 
            break;                                           // Выход
          }
          if (c == '\n')
          {                                 // Если "с" равен символу новой строки
            blank_line = true;                             // Тогда начинаем новую строку
          }
          else if (c != '\r')
          {                           // Если "с" не равен символу возврата курсора на начало строки
            blank_line = false;                           // Тогда получаем символ на текущей строке 
          }                                        
        }
      }  
      client.stop();                                      // Закрытие соединения
    }

//    delay(3000);
    
//    delete []data_in;

  // пауза на 30-60-180 с
  delay(30000);
  //delay(60000);
  //delay(180000);

}




/*
#include <HX711.h>        // Подключаем библиотеку HX711 для работы с АЦП hx711
//HX711 scale(A1, A0);            // Указываем в какие пины подключен АЦП hx711 (DT(DOUT)  - pin A1, SCK - pin A0)
HX711 scale(0, 2);                // для ESP-01: GPIO0 (0) и GPIO2 (2)

float myScale;                    // Инициализируем переменную в которой будем хранить текущий вес


  scale.set_scale(469.58);        // Устанавливаем калибровочный коэффициент 
  scale.tare();                   // Сбрасываем весы на 0
  myScale = round(scale.get_units(10));  // Запоминаем среднее значение без груза, но с учетом тары 

          
          scale.power_up();                     // Отключаем энергосберегающий режим hx711
          if (round(scale.get_units(10)) != myScale) {  // Условие выполняется, если округленный до граммов вес отличается от последнего
            myScale = scale.get_units(20);      // Запоминаем текущий вес
            if (myScale > 0.5)                  // Если вес не ничтожно мал
              Serial.println(myScale);          // то выводим его
            else                                // иначе
              Serial.println(0.00);             // выводим нули
            myScale = round(myScale);           // Запоминаем округленный текущий вес (для сравнения)
          }
          scale.power_down();                   // Включаем энергосберегающий режим hx711
          
          client.println("<h3>Results = ");
*/
