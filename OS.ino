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

//에이블랩스 인턴
//1일차 할 일 : 파이펫 테스트 모듈 만들기
//2일차 할 일 : 출력단에 오실로스코프 찍어서 나오는 파형으로 마이크로세컨드 나오는지 확인

int PressureInputNum = 2;                 //분사 digitalpin 2번
int PumpOutputNum = 3;                   //펌프 digitalpin 3번
int SolOutputNum = 4;                     //솔레노이드 출력 4번
int SolMaxtime = 1000; // micro sec
// #define SERIAL_MONITOR
#define BUF_COUNT 50
String buffer = "";                              //StringBuffer는 문자열을 추가하거나 변경 할 때 주로 사용하는 자료형
                                                //초기화
const int LED_PIN = LED_BUILTIN;            //led 내장 핀
const int MS_BLINK_INTERVAL = 500;          //밀리세컨 마이크로세컨으로 변환할것


void setup()
{
  initializeBlinkLED(LED_PIN);              
  pinMode(PressureInputNum, INPUT_PULLUP);
  pinMode(PumpOutputNum, OUTPUT);
  pinMode(SolOutputNum, OUTPUT);
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

void initializeBlinkLED(int ledPin)         //내장 led
{
    pinMode(ledPin, OUTPUT);                //출력
}
void blinkLED(int ledPin, int msBlinkInterval)                  //수식정리 깜박이는 led만들기 msBlinkInterval
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= msBlinkInterval)              //만약에 현재시간 - 이전시간 보다 msblink 500 이상이면
    {
        previousMillis = currentMillis;                                 // 이전시간은 현재시간이 된다.
        (digitalRead(ledPin) == LOW) ? digitalWrite(ledPin, HIGH) : digitalWrite(ledPin, LOW);      //조건??>>>>>
    }
}
void initializeSerial()
{
    Serial.begin(9600);         // 시리얼모니터 값
    while(!Serial);             //시리얼포트가 돌아가면 TRUE
    Serial.setTimeout(0); // remove Serial.readString() delay (default : 1 sec)
}
void processCommand()
{
    if (Serial.available() <= 0) return;        //만약  시리얼 버퍼가 없다면 return
    buffer += Serial.readString();              //버퍼 =  버퍼 + 문자열로 읽은 버퍼 

    while (true)                                //참일때
    {
        int newLineIndex = buffer.indexOf("\n");    //buffer = 스트링형 변수명이고 , indexof 문자 또는 문자열을 찾는다. 
                                                    //()안의 ("\n")찾을값 - char 또는 문자 그니까 "" 되어있으니 문자열로 받겠지...
        if (newLineIndex < 0) return;               //만약 버퍼문자열 인덱스가 0보다 작다면 return
                                                    //스트링 안의 값을 못찾으면 이건 -1 이된다고 함.
            
        String command = buffer.substring(0, newLineIndex);     //스트링형 변수(from, to)
        buffer = buffer.substring(newLineIndex + 1);            //버퍼문자열 인덱스에 1을 더해주는 이유는 ???

        if (command.substring(0, 3) == "do ")                   //만약 스트링형 변수가 0에서 3일 경우 "do" 인데 여기서 스트링 커맨드 = 버터 섭스트링(0에서 newLineindex로) 설정 박았음
        {
            char ch[BUF_COUNT];                                 //배열 시작
            //Zeromemory(ch,BUF_COUNT);                         //초기화.... (추가)
            command.toCharArray(ch, command.length() + 1);      //커맨드가 들어온 길이보단 길어야함.
            int doIndex;                                        //인트형으로 받아야해서 선언
            int value;                                          //얘도
            sscanf(ch, "do %d %d", &doIndex, &value);           //
            #ifdef SERIAL_MONITOR                               
            Serial.print("doIndex="); Serial.print(doIndex);    
            Serial.print(" | value="); Serial.println(value);   
            #endif

            pinMode(doIndex, OUTPUT);       //출력 코드
            if(value == 0)                  //4,0 입력이되면
            {
              value = 1;                    //여기서 1로 바뀜
            }
            else                            //근데 value 1이면
            {
              value = 0;                    //value가 0으로 바뀜
            }
            digitalWrite(doIndex, value);                           //예시문) 만약에 4,0 을 입력했으면  4.1로 끝나서 전류 흘러
                                                                    //4,1이면 4.0으로 바뀌고 
            Serial.println(value);
            if(doIndex == SolOutputNum && value == 0)               //만약 doindex 가 4 였어 == Soloutputnum이 4이고 value가 0임
            {       
              value = 1;                                            //value가 0에서 1로 바뀌어서
              delay(SolMaxtime);
              digitalWrite(doIndex, value);
            }                                                       //그래서 결국 4.0이랑 4.1이 결과값이 같아서 오류다.
        }
        else if (command.substring(0, 3) == "di ")                  //di(스페이스바임) 여기서 공백까지 있기에 from=열림구간 0, to =닫힌구간 3
        {
            char ch[BUF_COUNT]; 
            command.toCharArray(ch, command.length() + 1);          //위와 같고     //toCharArray 스트링개체함수
            int diIndex;                                            //스트링형변수.스트링형개체 함수(buff, len)     //buff : 문자 복사해 넣을 버퍼 char[]
            sscanf(ch, "di %d", &diIndex);                          // len 버퍼크기(unsigned int)       반환 없음
            pinMode(diIndex, INPUT_PULLUP);                         //pressure 인풋
            int value = digitalRead(diIndex);                       
            Serial.println(String(diIndex));
            #ifdef SERIAL_MONITOR
            Serial.print("diIndex="); Serial.print(diIndex);
            Serial.print(" | value="); Serial.println(value);       //확인용
            #endif
        }
        else if (command.substring(0, 4) == "gio ")                 //di 랑 동일
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
        else if (command.substring(0, 8) == "settime ")             //닫힘구간 8인 이유는 settime 7글자에 공백포함해서 8개라 8개 미만으로 잡음
        {
            char ch[BUF_COUNT];
            command.toCharArray(ch, command.length() + 1);
            int imaxtime;
            sscanf(ch, "settime %d", &imaxtime);
            SolMaxtime = imaxtime;
            Serial.println(imaxtime);
        }
        else
        {
            Serial.print("invalid command=\""); Serial.print(command); Serial.println("\"");
            Serial.print("buffer=\""); Serial.print(buffer); Serial.println("\"");
            return;
        }
    }
}
    