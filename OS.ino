//unsigned long time;
 /*
void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.print("Time: ");
  time = micros();
 
  Serial.println(time); //prints time since program started
  delayMicroseconds(100);          // wait a second so as not to send massive amounts of data
}
*/
//unsigned long oldtime;
//unsigned long resultTime;

// do 2 1   : set D2 output on
// do 2 0   : set D2 output off
// di 1     : get D1 input
// gio 1    : get IO
// settime 시간 : 솔벨브 on 시간 setting
/*
통신 [Command, value, 0, 0]
Command
1 = Get IO
2 = Set Output
3 = Set Parameter


*/
#include <Time.h>
int PressureInputNum = 2;
int PumpOutputNum = 3;
int SolOutputNum = 4;
int reValveOutputNum = 5;                                   //revalve 나노단위까지 가능하다함.
//int SolMaxtime = 100; // micro sec  10000ms== 1마이크로 초 
unsigned long SolMaxtime = 200;

//int SolMaxtime;

// #define SERIAL_MONITOR
#define BUF_COUNT 50
String buffer;
const int LED_PIN = LED_BUILTIN;
const int MS_BLINK_INTERVAL = 500;

void setup()
{
  Serial.begin(9600);   //시리얼 모니터
  initializeBlinkLED(LED_PIN);
  pinMode(PressureInputNum, INPUT_PULLUP);
  pinMode(PumpOutputNum, OUTPUT);
  pinMode(SolOutputNum, OUTPUT);
  pinMode(reValveOutputNum, OUTPUT);                //pinmode 특정핀 동작에 사용하려고 할때...
  initializeSerial();
  //Serial.println("setup done.");
  digitalWrite(PumpOutputNum, 1);
  digitalWrite(SolOutputNum, 1);
}
//int buf[4] = {0x25,0x62,0x3F,0x52};
int buf[4] = {1,2,3,4};
void loop()
{
    // Serial.println(CheckSum((unsigned char*)buf,4));
    blinkLED(LED_PIN, MS_BLINK_INTERVAL);
    processCommand();
    delay(100); 
}



int CheckSum(unsigned char *data, int len)
{
    int total = 0, sum = 0;
    for(int x=0; x<len; x++, data++)
    {
        sum += *data; // 모든 바이트 값 저장.
    }
    total = sum;
    total = total&0xFF; // 캐리 니블 버림
    total = ~total + 1; // 2의보수, 이때 total에 체크섬 바이트 저장되어있음.
    total = total + sum; // total에 체크섬 바이트와 처음 바이트 그룹 저장.
    total = total&0xFF; // 캐리 니블 버림
    return total;

}

void initializeBlinkLED(int ledPin)
{
    pinMode(ledPin, OUTPUT);
}
void blinkLED(int ledPin, int msBlinkInterval)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= msBlinkInterval)
    {
        previousMillis = currentMillis;
        (digitalRead(ledPin) == LOW) ? digitalWrite(ledPin, HIGH) : digitalWrite(ledPin, LOW);
    }
}
void initializeSerial()
{
    Serial.begin(9600);
    while(!Serial);
    Serial.setTimeout(0); // remove Serial.readString() delay (default : 1 sec)
}
void processCommand()
{
    if (Serial.available() <= 0) return;
    buffer += Serial.readString();

    while (true)
    {
        int newLineIndex = buffer.indexOf("\n");
        if (newLineIndex < 0) return;
            
        String command = buffer.substring(0, newLineIndex);
        buffer = buffer.substring(newLineIndex + 1);
      
        if (command.substring(0, 3) == "do ")                     //-----------------------------------------------------------------------------------------------------
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int doIndex;
            int value;
            sscanf(ch, "do %d %d", &doIndex, &value);
            
            #ifdef SERIAL_MONITOR
            Serial.print("doIndex="); Serial.print(doIndex);
            Serial.print(" | value="); Serial.println(value);
            #endif

            pinMode(doIndex, OUTPUT);
            if(value == 0)
            {
              value = 1;
                 digitalWrite(reValveOutputNum,LOW);
            }
            else
            {
              value = 0;
               
               
                
            }
            digitalWrite(doIndex, value);
            Serial.println(value);
            if(doIndex == SolOutputNum && value == 0)                            //수정구간
            {
              value = 1;
              if(value == 1)
              {
                unsigned long past = micros();
                delayMicroseconds(SolMaxtime);
                 unsigned long now = micros();
                 unsigned long test = now - past;
                 Serial.println(test);
                digitalWrite(reValveOutputNum,HIGH);
                delayMicroseconds(SolMaxtime);
                digitalWrite(reValveOutputNum,LOW);
              Serial.println("past"); Serial.println(past);
                int flag = 0;
               
                 Serial.println("now"); Serial.println(now);
                if(now - past <= 250){ 
                past = now;
                flag = 1;
                }
                 if(flag == 1){
                  digitalWrite(reValveOutputNum,HIGH);
                Serial.print("go home");
                flag = 0;
                 }
                
              }
             /*   Serial.println("Time: ");
                time_t oldtime,time;
                oldtime = time();
                delayMicroseconds(SolMaxtime);            //요기가 문제인데
                time = time();
                time_t resultTime = time - oldtime;
               */
               
              digitalWrite(doIndex, value);
            }
            
            //else{
              //  value = 0;
               // delayMicroscconds(Solmaxtime);      //0으로 켜줌.
               // digitalWrite(doIndex, value);
           // }
        }
        else if (command.substring(0, 3) == "di ")
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int diIndex;
            sscanf(ch, "di %d", &diIndex);
            pinMode(diIndex, INPUT_PULLUP);
            int value = digitalRead(diIndex);
            Serial.println(String(diIndex));
            #ifdef SERIAL_MONITOR
            Serial.print("diIndex="); Serial.print(diIndex);
            Serial.print(" | value="); Serial.println(value);
            #endif
        }
        else if (command.substring(0, 4) == "gio ")
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int iIndex;
            sscanf(ch, "gio %d", &iIndex);
            //pinMode(aiIndex, INPUT);
            int value = digitalRead(iIndex);
            
            Serial.println(value);
            #ifdef SERIAL_MONITOR
            Serial.print("diIndex="); Serial.print(iIndex);
            Serial.print(" | value="); Serial.println(value);
            #endif
        }
      /*  
        else if (command.substring(0, 8) == "settime ")               
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int imaxtime;
            sscanf(ch, "settime %d", &imaxtime);
            SolMaxtime = imaxtime;                      //solmaxtime이 마이크로세컨으로 가줄려면 초기화 한번 해야하는데 그걸 지금 imaxtime으로 잡은거
            Serial.print("시간(us): ");
            imaxtime = micros();                    //micros(); 로 바꿔야하는데 안바뀜,,,에대한 이해가 필요함 micors()
            Serial.println(imaxtime);                   
        }
        */
        else if (command.substring(0, 8) == "settime ")
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int imaxtime;
            sscanf(ch, "settime %d", &imaxtime);
            SolMaxtime = imaxtime;                      //solmaxtime이 마이크로세컨으로 가줄려면 초기화 한번 해야하는데 그걸 지금 imaxtime으로 잡은거
            Serial.println(imaxtime);                   //
        }
        
        else
        {
            Serial.print("invalid command=\""); Serial.print(command); Serial.println("\"");
            Serial.print("buffer=\""); Serial.print(buffer); Serial.println("\"");
            return;
        }
    }
}
