#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <string>

//----------------------------------------
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#define WIFI_SSID "TB1_ACTVN"
#define WIFI_PASSWORD "1234567@9"
#define API_KEY "AIzaSyBkDYB1ukNEMYRDYCzYNzKTx6EHPmDHz_M"
#define DATABASE_URL "https://sanbui-6b560-default-rtdb.firebaseio.com/"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "test@123.com"
#define USER_PASSWORD "123456"


// Define Firebase Data object
FirebaseData fbdo;


FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;
//----------------------------------------
int i;
void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h
#if defined(ESP8266)
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif
  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
}


void loop() {
  //xoavantay();
  nhanid();
  xoaidvantay();
}
void nhanid() {
  if (Serial.available()) {
    String idString = Serial.readStringUntil('\n');       // Đọc chuỗi ID từ cổng UART
    int index = idString.indexOf(',');                    // Tìm vị trí của dấu phẩy đầu tiên trong chuỗi
    if (index >= 0) {                                     // Nếu có dấu phẩy trong chuỗi ID
      String idSubString = idString.substring(0, index);  // Cắt chuỗi con từ đầu đến vị trí dấu phẩy
      int id = idSubString.toInt();                       // Chuyển đổi chuỗi ID thành giá trị số nguyên
      if (id > 0) {                                       // Kiểm tra xem giá trị ID có lớn hơn 0 hay không
        Serial.print("Received ID: ");
        Serial.println(id);
        // Xử lý giá trị ID đầu tiên ở đây
      }
    } else {                          // Nếu không có dấu phẩy trong chuỗi ID
      int lastId = idString.toInt();  // Xử lý giá trị cuối cùng
      if (lastId > 0) {               // Kiểm tra xem giá trị ID có lớn hơn 0 hay không
        //String message = Serial.readStringUntil('\n');
        Serial.print("Received ID: ");
        Serial.println(lastId);
        i = lastId;
        String numberString = String(i);
        if (Firebase.ready()) {
          String path = "/data/id" + numberString;
          Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, path, i) ? "ok" : fbdo.errorReason().c_str());
        }
      }
    }
    if (Serial.available()) {
      String message = Serial.readStringUntil('\n');
      if (message == "Stored!") {
        Serial.println("da luu id: ");
        Serial.println(i);
        // Vân tay đã được lưu thành công, thực hiện các thao tác tiếp theo
      }
    }
  }
}

void xoaidvantay() {
 if (Serial.available()) {
  String numberString = String(id);
  String path = "/data/id" + numberString;
  if (Firebase.RTDB.deleteNode(&fbdo, path)) {
    Serial.println("Remove success");
  } else {
    Serial.println("Remove failed");
  }
}
}

void xoavantay() {
  // Gửi chuỗi ký tự "delete,id" qua cổng Serial
  String command = "delete,";
  Serial.println(command);
  if (Serial.available()) {
    char id = Serial.read();
    // Xử lý ID ở đây
  }
  delay(1000);
}
