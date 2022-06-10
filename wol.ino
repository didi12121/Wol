#define BLINKER_WIFI

//#include <WiFiUDP.h>
#include <Blinker.h>
//#include <ESP8266Ping.h>

//对应客户端的秘钥
char auth[] = "点灯密钥";
//WIFI SSID
char ssid[] = "wifi名称";
//WIFI Password
char pswd[] = "wifi密码";

#define BUTTON_1 "OPEN"

#define BUTTON_2 "PC-Status"

WiFiUDP Udp;
//根据自己的路由器选择子网段的广播地址
#define BROADCAST_ADD "192.168.31.255"
//网卡唤醒端口(一般为9，可以在BIOS中查看)
#define REMOTEPORT 9
//PC端的IP地址（需要在路由器端进行MAC和IP绑定）
const IPAddress remote_ip(192, 168, 31, 174);
//本地UDP端口
#define LOCALPORT 9
//网卡的MAC地址 6个字节
char NETCARD_MAC[6] = {0xD8, 0xBB, 0xC1, 0xA2, 0xF8, 0x41};
//WOL包
char WOL_Buffer[102];

//WOL包发送完成标志
int complete_flag = 0;
//ping计数
int ping_num = 0;

//WOL魔术包初始化
void WOL_packet_init()
{
  int i, j;
  for (i = 0; i < 6; i++)
  {
    WOL_Buffer[i] = 0xFF;
  }
  for (i = 1; i < 17; i++)
  {
    for (j = 0; j < 6; j++)
    {
      WOL_Buffer[i * 6 + j] = NETCARD_MAC[j];
    }
  }
#ifdef WOL_DEBUG
  Serial.printf("The WOL packet is :\n");
  for (i = 0; i < 102; i++) {
    Serial.printf("%x ", WOL_Buffer[i]);
  }
#endif
}

void WOL_packet_transfer()
{
#ifdef WOL_DEBUG
  Serial.printf("NOW is send WOL packet!\n");
#endif
  Udp.beginPacket(BROADCAST_ADD, REMOTEPORT);
  Udp.write(WOL_Buffer);
  Udp.endPacket();
}

BlinkerButton Button1(BUTTON_1);

void button1_callback(const String & state)
{
  BLINKER_LOG("get button1 state: ", state);

  if (state == BLINKER_CMD_BUTTON_TAP) {
    BLINKER_LOG("Button1 tap!");
    WOL_packet_transfer();
    BLINKER_LOG("WOL Packet Transfer Completed!");
    complete_flag = 1;
    Blinker.notify("!Opening... Plase wait a moment");
  }
  else {
    BLINKER_LOG("Get user setting: ", state);
  }
}

void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  Blinker.vibrate();
  uint32_t BlinkerTime = millis();
  Blinker.print("millis", BlinkerTime);
}

void setup()
{
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  Button1.attach(button1_callback);
  Udp.begin(LOCALPORT);
  WOL_packet_init();
}

void loop()
{
  Blinker.run();
}
