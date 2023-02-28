#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include <DHT.h>

#define FIREBASE_HOST "********************.firebaseio.com"  // رابط قاعده البيانات
#define FIREBASE_AUTH "*****************"                    // الرمز السري لقاعده البيانات
#define WIFI_SSID "*****************"                       // اسم الشبكه
#define WIFI_PASSWORD "***************"                     // كلمه السر

#define DHTPIN 18      // منفذ الاردوينو المتصل بالحساس
#define DHTTYPE DHT11  // اسم موديل الحساس المستخدم

DHT dht(DHTPIN, DHTTYPE);  // تغعيل الحساس


FirebaseData firebaseData; // تفعيل اوبجكت من المكتبه حق الفايبر بيس
FirebaseJson json;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);  // بدا الاتصال مع الكمبيورت بسرعه 9600
  dht.begin();         //بدا تشغيل الحساس
  pinMode(2, OUTPUT);

  lcd.init();       // initialize LCD
  lcd.backlight();  // turn on LCD backlight

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // الاتصال بالانترنت
  Serial.print("Connecting to Wi-Fi");   // ارسال رساله للكمبيوتر باننا بدانا محاوله الاتصال

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Starting.");

  delay(2000);

  lcd.clear();

  digitalWrite(2, LOW);
  
  while (WiFi.status() != WL_CONNECTED) {  // اذا لما يتم الاتصال بالانترنت اكتب نقطه ( . ) وانتظر
    Serial.print(".");
    delay(300);
    lcd.setCursor(0, 0);
    lcd.print("  Connecting to ");
    lcd.setCursor(0, 1);
    lcd.print("    Your WiFi   ");
  }
  digitalWrite(2, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("      Done      ");
  delay(1500);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  // الاتصال مع قاعده البيانات
  Firebase.reconnectWiFi(true);                  // تفعيل اعاده الاتصال مع قاعده البيانات
}

void loop() {
  // put your main code here, to run repeatedly:


  while (WiFi.status() != WL_CONNECTED) {  // تاكد من الاتصال من الانترنت او اعد محاوله الاتصال
    Serial.print(".");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(300);

    lcd.setCursor(0, 0);
    lcd.print("  Connecting to ");
    lcd.setCursor(0, 1);
    lcd.print("    Your WiFi   ");
    digitalWrite(2, LOW);
  }

  digitalWrite(2, HIGH);

  float humidity = dht.readHumidity();  // اقرا درجه الرطوبه من الحساس
  float temp = dht.readTemperature();   // اقرا درجه الحراره من الحساس

  lcd.setCursor(0, 0);
  lcd.print("Temp:  ");
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);


  if (isnan(humidity) || isnan(temp))  // اذا كان الحساس لا يعمل اعطيني ملاحظه
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Firebase.setFloat(firebaseData, "/Weather/Temperature", temp);   //ارسل درجه الحراره الى قاعده البيانات
  Firebase.setFloat(firebaseData, "/Weather/Humidity", humidity);  // ارسل الرطوبه الى قاعده البيانات


  delay(2000);  // انتظر ثانتين قبل قراءه الحساس من جديد
}
