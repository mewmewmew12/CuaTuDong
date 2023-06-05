#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(2, 3); // Khai báo phần cứng Serial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
#include <Keypad.h>
#include<EEPROM.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <SoftwareSerial.h>
#include <Servo.h>
Servo myservo;
unsigned char a, j;
#define led A2
#define buzzer A3
int berr = 0;
int ber = 0;
char password[4];
char pass[4], pass1[4];
int i = 0;
int trangthaicua;
char customKey = 0;
char customKey1 = 0;
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 9, 10}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
void setup()
{
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  myservo.attach(A0);
  myservo.write(180);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.clear();
  lcd.print("Nhap mat khau:");
  lcd.setCursor(0, 1);
  for (int j = 0; j < 4; j++)
    EEPROM.write(j, j + 49); //j + 48 sẽ chuyển đổi giá trị số của j thành ký tự tương đương của nó
  for (int j = 0; j < 4; j++) //miễn là giá trị của j bị giới hạn trong khoảng từ 0 đến 9. Các ký tự được lưu trữ trong EEPROM.
    pass[j] = EEPROM.read(j); //Thay vào đó, việc sử dụng 49 đảm bảo rằng số không không bao giờ được ghi vào EEPROM, điều này có thể quan trọng vì số không gắn cờ cuối của chuỗi kiểu C.
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  finger.begin(57600);
  delay(5);
  if (!finger.verifyPassword()) {
    while (1) {
      delay(1);
      Serial.print(".");
    }
  }
}
void loop()
{
  appBlynk();
  chuyentrangthai();
  customKey = customKeypad.getKey();
  // Nếu người dùng nhấn phím #, thực hiện hàm đổi mật khẩu
  if (customKey == '#') {
    beep();
    doimk();
  }

  // Nếu người dùng nhấn phím *, đóng cửa
  if (customKey == '*') {
    beep();
    chucnang();
  }

  //  // Nếu người dùng nhấn bất kỳ phím nào trên bàn phím, thêm ký tự vào mật khẩu
  if (customKey) {
    password[i++] = customKey;
    lcd.print("*");
    beep();
  }

  // Nếu đã nhập đủ 4 ký tự mật khẩu, kiểm tra mật khẩu và mở khóa nếu đúng
  if (i == 4) {
    delay(200);
    for (int j = 0; j < 4; j++)
      pass[j] = EEPROM.read(j);
    if (!(strncmp(password, pass, 4))) // so sánh 2 chuỗi mật khẩu
    {
      //digitalWrite(led, HIGH);
      //lcd.clear();
      //lcd.print("Chinh xac");
      delay(1000);
      beep();
      nhandienvantay();
      return;
    }
    else
    {
      lcd.clear();
      lcd.print("Mat khau sai");
      ber++ ;
      delay(2000);
      lcd.setCursor(0, 1);
      lcd.clear();
      lcd.print("Nhap mat khau:");
      lcd.setCursor(0, 1);
      i = 0;

    }
  }
  if (ber > 2) {
    lcd.clear();
    lcd.print("Bao dong!");
    delay(2000);
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Nhap mat khau:");
    lcd.setCursor(0, 1);
    i = 0;
    ber = 0;
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
  }

  // Đóng cửa nếu đã mở sau 5 giây
  if (myservo.read() == 90) {
    delay(5000);
    dongcua();
  }
}

void dongcua()
{
  myservo.write(180);
  lcd.clear();
  lcd.print("Da dong cua");
  lcd.setCursor(0, 1);
  delay(1000);
  lcd.clear();
  lcd.print("Nhap mat khau:");
  lcd.setCursor(0, 1);
  customKey = 0;

}

void doimk() {
  int j = 0;
  lcd.clear();
  lcd.print("Pass hien tai:");
  lcd.setCursor(0, 1);
  while (j < 4)
  {
    char key = customKeypad.getKey();
    if (key)
    {
      pass1[j++] = key;
      lcd.print("*");
      beep();
    }
    key = 0;
  }
  delay(500);
  if ((strncmp(pass1, pass, 4)))
  {
    lcd.clear();
    lcd.print("Sai");
    lcd.setCursor(0, 1);
    lcd.print("Xin thu lai!");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("Nhap mk moi:");
    lcd.setCursor(0, 1);

    // Nhập mật khẩu mới
    for (int j = 0; j < 4; j++) {
      char customKey = 0;
      while (!customKey) {
        customKey = customKeypad.getKey();
      }
      pass[j] = customKey;
      lcd.print("*");
      beep();
      delay(200);
    }

    // Xác nhận mật khẩu mới
    lcd.clear();
    lcd.print("Nhap lai mk:");
    lcd.setCursor(0, 1);
    for (int j = 0; j < 4; j++) {
      char customKey = 0;
      while (!customKey) {
        customKey = customKeypad.getKey();
      }
      pass1[j] = customKey;
      lcd.print("*");
      beep();
      delay(200);
    }

    // Kiểm tra xem hai mật khẩu mới có giống nhau không
    if (strncmp(pass, pass1, 4) == 0) {
      lcd.clear();
      lcd.print("Doi mk thanh cong");
      delay(1000);

      // Lưu mật khẩu mới vào EEPROM
      for (int j = 0; j < 4; j++) {
        EEPROM.write(j, pass[j]);
      }
      delay(1000);
      lcd.clear();
      lcd.print("Nhap mat khau:");
      lcd.setCursor(0, 1);
    } else {
      lcd.clear();
      lcd.print("Mat khau khong khop");
      delay(1000);
      lcd.clear();
      lcd.print("Nhap mat khau:");
      lcd.setCursor(0, 1);
    }
  }
  lcd.clear();
  lcd.print("Nhap mat khau:");
  lcd.setCursor(0, 1);
  customKey = 0;
}

void beep()
{
  digitalWrite(buzzer, HIGH);
  delay(20);
  digitalWrite(buzzer, LOW);
}
void chucnang() {
  int j = 0;
  lcd.clear();
  lcd.print("Pass hien tai:");
  lcd.setCursor(0, 1);
  while (j < 4)
  {
    char key = customKeypad.getKey();
    if (key)
    {
      pass1[j++] = key;
      lcd.print("*");
      beep();
    }
    key = 0;
  }
  delay(500);
  if ((strncmp(pass1, pass, 4)))
  {
    lcd.clear();
    lcd.print("Sai");
    lcd.setCursor(0, 1);
    lcd.print("Xin thu lai!");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("1: Them van tay");
    lcd.setCursor(0, 1);
    lcd.print("2: Xoa van tay");
    lcd.setCursor(0, 2);
    while (true) {
      char customKey = customKeypad.getKey();
      if (customKey == '1') { // Thêm vân tay
        beep();
        lcd.clear();
        lcd.print("Them van tay...");
        lcd.setCursor(0, 1);
        delay(1000);
        lcd.clear();
        lcd.print("Nhap ID: ");
        char fingerprint_id[4];
        uint8_t i = 0;
        while (true) {
          char key = customKeypad.getKey();
          if (key == '*') { // Nếu ấn nút # thì gửi ID lên serial và thoát vòng lặp
            beep();
            Serial.println(fingerprint_id);
            break;
          }
          if (key) { // Nếu có phím được nhấn thì lưu vào mảng fingerprint_id và hiển thị lên LCD
            beep();
            lcd.print(key);
            fingerprint_id[i++] = key;
          }
        }
        uint8_t fingerprintId;
        if (strlen(fingerprint_id) > 0) {
          fingerprintId = atoi(fingerprint_id);
          Serial.println(fingerprintId);
        } else {
          fingerprintId = 0;
          Serial.println("Invalid input!");
        }

        // Xóa mảng fingerprint_id để bắt đầu lại quá trình nhập giá trị từ đầu
        memset(fingerprint_id, 0, sizeof(fingerprint_id));
        i = 0; // Chuyển đổi ID từ chuỗi sang số // Chuyển đổi ID từ chuỗi sang số
        getFingerprintEnroll(fingerprintId);;
        delay(2000);
        lcd.clear();
        lcd.print("1: Them van tay");
        lcd.setCursor(0, 1);
        lcd.print("2: Xoa van tay");
        lcd.setCursor(0, 2);
      }
      else if (customKey == '2') { // Xóa vân tay
        beep();
        lcd.clear();
        lcd.print("Xoa van tay...");
        // Đoạn code xóa vân tay được thực hiện ở đây
        lcd.clear();
        lcd.print("Nhap ID: ");
        char fingerprint_id[4];
        uint8_t i = 0;
        while (true) {
          char key = customKeypad.getKey();
          if (key == '*') { // Nếu ấn nút # thì gửi ID lên serial và thoát vòng lặp
            beep();
            Serial.println(fingerprint_id);
            break;
          }
          if (key) { // Nếu có phím được nhấn thì lưu vào mảng fingerprint_id và hiển thị lên LCD
            beep();
            lcd.print(key);
            fingerprint_id[i++] = key;
          }
        }
        uint8_t fingerprintId;
        if (strlen(fingerprint_id) > 0) {
          fingerprintId = atoi(fingerprint_id);
          Serial.println(fingerprintId);
        } else {
          fingerprintId = 0;
          Serial.println("Invalid input!");
        }

        // Xóa mảng fingerprint_id để bắt đầu lại quá trình nhập giá trị từ đầu
        memset(fingerprint_id, 0, sizeof(fingerprint_id));
        i = 0; // Chuyển đổi ID từ chuỗi sang số
        deleteFingerprint(fingerprintId);
        delay(2000);
        lcd.clear();
        lcd.print("1: Them van tay");
        lcd.setCursor(0, 1);
        lcd.print("2: Xoa van tay");
        lcd.setCursor(0, 2);
        delay(1000);
        lcd.clear();
        lcd.print("1: Them van tay");
        lcd.setCursor(0, 1);
        lcd.print("2: Xoa van tay");
        lcd.setCursor(0, 2);
      }
      else if (customKey == '#') { // Quay lại
        beep();
        lcd.clear();
        lcd.print("Nhap mat khau: ");
        lcd.setCursor(0, 1);
        break;
      }
    }
  }
  lcd.clear();
  lcd.print("Nhap mat khau:");
  lcd.setCursor(0, 1);
  customKey = 0;
}
// phan them van tay
uint8_t getFingerprintEnroll(uint8_t id) {
  lcd.clear();
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.print("dat ngon tay...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        lcd.clear();
        lcd.print("error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        lcd.clear();
        lcd.print("error");
        break;
      default:
        Serial.println("Unknown error");
        lcd.clear();
        lcd.print("error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("error");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("error");
      return p;
  }
  lcd.clear();
  Serial.println("Remove finger");
  beep();
  lcd.print("nhac ngon tay...");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("them lan 2...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        lcd.clear();
        lcd.print("error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        lcd.clear();
        lcd.print("error");
        break;
      default:
        Serial.println("Unknown error");
        lcd.clear();
        lcd.print("error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("error");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("error");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    lcd.clear();
    beep();
    lcd.print("Thanh cong...");
    Serial.print("ID:");
    Serial.println(id);
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.clear();
    lcd.print("Error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.print("Error");
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    Serial.write("Stored!");
    lcd.clear();
    lcd.print("Da luu...");
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.clear();
    lcd.print("Error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    lcd.clear();
    lcd.print("Error");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    lcd.clear();
    lcd.print("Error");
    return p;
  } else {
    Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Error");
    return p;
  }

  return true;
}
//xoa van tay
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  Serial.print("IDX:");
  Serial.println(id, DEC);
  Serial.flush();
  p = finger.deleteModel(id);
  lcd.clear();
  lcd.print("Dang xoa...");
  delay(1000);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }
  lcd.clear();
  lcd.print("xong...");
  return p;
}
//nhan dien van tay
void nhandienvantay() {
  lcd.clear();
  lcd.print("Nhap van tay: ");
  while (true) {
    if (finger.getImage() == FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Dang nhan...   ");
      delay(1000);
      if (finger.image2Tz() == FINGERPRINT_OK) {
        if (finger.fingerFastSearch() == FINGERPRINT_OK) {
          lcd.clear();
          beep();
          lcd.print("thanh cong... ");
          myservo.write(90);
          digitalWrite(led, HIGH);
          delay(1000);
          digitalWrite(led, LOW);
          lcd.clear();
          lcd.print("Nhap mat khau: ");
          i = 0;
          break;
        } else {
          lcd.clear();
          lcd.print("khong khop... ");
          delay(2000);
          lcd.clear();
          lcd.print("Nhap van tay: ");
          ber++;
        }
      } else {
        lcd.clear();
        lcd.print("Nhap lai van tay   ");
        delay(2000);
        lcd.clear();
        lcd.print("Nhap van tay: ");
        ber++;
      }
    }
  }
}

void chuyentrangthai()
{
  int c = myservo.read();
  if (c == 180)
  {
    int dc = 2;
    Serial.write(dc);
  }
  if (c == 90)
  {
    int mc = 3;
    Serial.write(mc);
  }
  if (ber > 2)
  {
    digitalWrite(buzzer, HIGH);
  }
  else if (ber < 3)
  {

  }
}
void appBlynk()
{
  int data = Serial.read(); //Read the serial data and store it
  Serial.print(data);
  if (data == 0)
  {
    digitalWrite(1, HIGH);
    digitalWrite(led, HIGH);
    lcd.clear();
    lcd.print("Da mo cua");
    lcd.setCursor(0, 1);
    myservo.write(90);
    delay(1000);
    lcd.clear();
    lcd.print("Nhap mat khau:");
    lcd.setCursor(0, 1);
    digitalWrite(led, LOW);
    ber = 0;
  }
  if (data == 90)
  {
    lcd.clear();
    lcd.print("Da dong cua");
    lcd.setCursor(0, 1);
    myservo.write(180);
    delay(1000);
    lcd.clear();
    lcd.print("Nhap mat khau:");
    lcd.setCursor(0, 1);
    ber = 0;
  }
}
