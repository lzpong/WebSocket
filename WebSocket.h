#pragma once
//#include <WinSock2.h>

#ifndef __WEBSOCKET__
#define __WEBSOCKET__

//WebSocket 数据包头
struct WSDATA_INFO
{
	bool bFrameOver;
//data[0]
	bool isEof;//是否是结束帧 data[0]>>7
	char dfExt;//是否有扩展定义 data[0] & 0x70
	/*帧类型 type 的定义data[0] & 0xF
	0x0表示附加数据帧
	0x1表示文本数据帧
	0x2表示二进制数据帧
	0x3-7暂时无定义，为以后的非控制帧保留
	0x8表示连接关闭
	0x9表示ping
	0xA表示pong
	0xB-F暂时无定义，为以后的控制帧保留
	*/
	char type;

//data[1]-->
	bool hasMask;//是否有掩码 data[1]>>7
	unsigned __int64 Len;//真实数据长度  (data[1] & 0x7f)计算出来的数据长度
	unsigned __int64 rcvLen;//已接收的数据长度(可能是多帧,未接收完?)
//Other-->
	char Mask[4];//掩码
	char pData[4096];//指向真实数据(动态分配,自动管理);其长度为 Len
	WSDATA_INFO(){memset(this,0,sizeof(WSDATA_INFO));};
	~WSDATA_INFO(){};
};

class WebSocket
{
//WebSocket Hand head
struct WsHead
{
	//char Method[10];
	//char Connection[20];//  [Upgrade] [keep-alive]
	//char Cookie[60];// [io=J4pEoCfkdoNycOCqAAAB]
	char Host[50];// [127.0.0.1:8080]

	//char Upgrade[20];// [websocket]
	char WebSocket_Key[100];//
	//char WebSocket_Extension[50];// [x-webkit-deflate-frame]
	char WebSocket_Ver[20];//[13][13,8,6]
};

union u2{
	unsigned short n;
	unsigned char c[2];
};
union u8{
	unsigned __int64 n;
	unsigned char c[8];
};
	static const char head[];

	//获取帧头部信息
	bool GetHead(string str,WsHead& heads);

public:
	WebSocket(void);
	~WebSocket(void);
	//接收消息,并判断是否是帧后续数据包,完成后解码,返回true;否则返沪false
	bool OnGetData(WSDATA_INFO* wsi,char* data,u_int64 len);

	//得到WebSocket握手加密串 Sec-WebSocket-Accept
	//参数1为 请求接收到的所有数据
	//参数2为 返回缓存数据
	//参数3为 返回缓存数据的最大可写长度
	//返回值为是否已经握手; 没握手不是WebSocket 请求(:false)
	bool Handshake(const char* data, char* buf, unsigned long bufLen);

	//从贞数据中取得结构信息,并解码掩码得到原始数据
	void GetDataInfo(WSDATA_INFO* dInfo, char* data, unsigned long dataLen);

	//webSocket协议 掩码解码/加码
	char* DoMask(WSDATA_INFO* dInfo);

	//转换为不分片WebSocket贞([newData]空间必须至少大于[data]16字节)
	char* SetDataInfo(char* newData,unsigned __int64& newDataSize,const char* data, unsigned __int64 dataLen,bool bMask=false,char* mask=NULL);
	char* SetDataInfo(char* newData,unsigned __int64& newDataSize,WSDATA_INFO* dInfo);//转换为不分片WebSocket贞

};

#endif
