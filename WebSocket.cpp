//#include "../stdafx.h"

#include "Code.h"
#include "WebSocket.h"

const char WebSocket::head[]={"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n\0\0"};

WebSocket::WebSocket(void){}
WebSocket::~WebSocket(void){}

//取得握手头部信息,WebSocket-Key
bool WebSocket::GetHead(string str,WsHead& heads)
{
	//long len=strlen(data);
	if(str.size()<10) return false;
	//vector<string> arr=Split(str,"\r\n");
	//if(arr.size()<2) return false;
	long n,m;
	if(str.find("GET ")!=str.npos||str.find("POST ")!=str.npos)
	{
		n=str.find("Host: ");
		if(n!=str.npos)
		{
			m=str.find("\r\n",n+1);
			strncpy_s(heads.Host,str.substr(n+6,m-n-6).c_str(),50);
		}
		n=str.find("WebSocket-Version: ");
		if(n!=str.npos)
		{
			m=str.find("\r\n",n+1);
			strncpy_s(heads.WebSocket_Ver,str.substr(n+15,m-n-15).c_str(),100);
		}
		n=str.find("WebSocket-Key: ");
		if(n!=str.npos)
		{
			m=str.find("\r\n",n+1);
			strncpy_s(heads.WebSocket_Key,str.substr(n+15,m-n-15).c_str(),100);
			return true;
		}
	}
	return false;
}

//得到WebSocket握手加密串 Sec-WebSocket-Accept
//参数1为 请求接收到的所有数据
//参数2为 返回缓存数据
//参数3为 返回缓存数据的最大可写长度
//返回值为是否已经握手; 没握手不是WebSocket 请求(:false)
bool WebSocket::Handshake(const char* data,char* buf,unsigned long bufLen)
{
	WsHead heads;
	if(!GetHead(data,heads)) return false;
	Hash1 hash;
	Base64 base64;

	strcat_s(heads.WebSocket_Key,"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");//加上固定字符串 ->Hash1 ->Base64
	hash.Write( (unsigned char*)heads.WebSocket_Key, strlen(heads.WebSocket_Key) );
	hash.Final();
	string s1=base64.Encode(hash.GetHash1(),20);//sha1定长20字节,且中间可能有'\0'
	sprintf_s(buf,bufLen,head,s1.c_str());
	return true;
}
//webSocket协议 掩码解码/加码
char* WebSocket::DoMask(WSDATA_INFO* dInfo)
{
	if(dInfo->hasMask && dInfo->Mask!=0)
	{
		for (u_int64 i = 0; i < dInfo->Len; i++)
			dInfo->pData[i] = dInfo->pData[i] ^ dInfo->Mask[i % 4];
		dInfo->hasMask=false;
	}
	return dInfo->pData;
}

//接收消息,并判断是否是帧后续数据包,完成后解码,返回true;否则返沪false
bool WebSocket::OnGetData(WSDATA_INFO* dInfo,char* data,u_int64 len)
{
	if(!dInfo->bFrameOver && dInfo->Len >= dInfo->rcvLen)//帧头部
		GetDataInfo(dInfo,data,len);
	else //后续的数据包
	{
		memcpy(&dInfo->pData[dInfo->rcvLen],&data[6],len);
		dInfo->rcvLen+=len;
	}
	//若接收的是最后一个数据包
	if( dInfo->Len <= dInfo->rcvLen && dInfo->hasMask &&dInfo->Mask[0]!=0)
		for (u_int64 i = 0; i < dInfo->Len; i++)// && (wsi->type==1?wsi->pData[i]:true)
			dInfo->pData[i] = dInfo->pData[i] ^ dInfo->Mask[i % 4];
	return dInfo->isEof;
}

//从贞数据中取得结构信息,并解码掩码得到原始数据
void WebSocket::GetDataInfo(WSDATA_INFO* dInfo, char* data, unsigned long dataLen)
{
	dInfo->isEof = (char)(data[0]>>7) ;//是否结束
	dInfo->dfExt = (data[0] & 0x70);//扩展码
	dInfo->type = data[0] & 0xF;//OPCode
	dInfo->hasMask = (char)(data[1]>>7);
	//Payload长度是ExtensionData长度与ApplicationData长度之和。
	//ExtensionData长度可能是0，这种情况下，Payload长度即是ApplicationData长度(默认ExtensionData长度是0)
	unsigned long tLen=dInfo->Len;
	dInfo->Len=data[1] & 0x7f;

	dInfo->rcvLen=0;
    //这里的长度 暂固定为 4096
	if(dInfo->Len==tLen && dInfo->pData!=NULL)//上一帧的数据数据长度跟这一次相同
		memset(dInfo->pData,0,4096);
	else //长度不同
	{
		memset(dInfo->pData,0,4096);
	}
	//当前帧,第一截数据
	if(dInfo->Len < 126) //如果其值在0-125，则是payload的真实长度
	{
		if(dInfo->hasMask)
		{
			if((dataLen-6)>0)//防止结尾帧数据不够长度的错误
			{
				memcpy(dInfo->Mask,&data[2],4);
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[6],dataLen-6);
				dInfo->rcvLen+=dataLen-6;
			}
		}
		else
			if((dataLen-2)>0)
			{
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[2],dataLen-2);
				dInfo->rcvLen+=dataLen-2;
			}
	}
	else if(dInfo->Len == 126)//如果值是126，则后面2个字节形成的16位无符号整型数(unsigned short)的值是payload的真实长度，掩码就紧更着后面
	{
		u2 u;memcpy(u.c,&data[2],2);
		dInfo->Len=ntohs(u.n);//网络字节转换

		if(dInfo->hasMask)
		{
			if((dataLen-8)>0)
			{
				memcpy(dInfo->Mask,&data[4],4);//防止结尾帧数据不够长度的错误
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[8],dataLen-8);
				dInfo->rcvLen+=dataLen-8;
			}
		}
		else
			if((dataLen-4)>0)
			{
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[4],dataLen-4);
				dInfo->rcvLen+=dataLen-4;
			}
	}
	else if(dInfo->Len == 127)//如果值是127，则后面8个字节形成的64位无符号整型数(unsigned int64)的值是payload的真实长度，掩码就紧更着后面
	{
		u8 u;memcpy(u.c,&data[2],8);
		dInfo->Len=ntohll(u.n);//网络字节转换
		//u_int64 ui64 = data[2]*0x100000000000000ULL+data[3]*0x1000000000000ULL+data[4]*0x10000000000ULL+data[5]*0x100000000ULL
		//				+data[6]*0x1000000ULL+data[7]*0x10000ULL+data[8]*0x100ULL+data[9]*0x1ULL;//逐字节转换

		if(dInfo->hasMask)
		{
			if((dataLen-14)>0)
			{
				memcpy(dInfo->Mask,&data[10],4);//防止结尾帧数据不够长度的错误
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[14],dataLen-14);
				dInfo->rcvLen+=dataLen-14;
			}
		}
		else
			if((dataLen-10)>0)
			{
				memcpy(&dInfo->pData[dInfo->rcvLen],&data[10],dataLen-10);
				dInfo->rcvLen+=dataLen-10;
			}
	}
    DoMask(dInfo);
}

//转换为不分片WebSocket贞([newData]空间必须至少大于[data]14字节)
char* WebSocket::SetDataInfo(char* newData,unsigned __int64& newDataSize,const char* data, unsigned __int64 dataLen,bool bMask,char* mask)
{
	if(data==NULL)
		return newDataSize=0,NULL;
	//第一byte,10000000, fin = 1, rsv1 rsv2 rsv3均为0, opcode = 0x01,即数据为文本帧
	newData[0]=129;//0x81 最后一个包 |(无扩展协议)| 控制码(0x1表示文本帧)
	char len;
	unsigned __int64 pos=0;//newData使用了几位(newData[pos])
	if(dataLen<=125)
	{
		len=dataLen;
        //数据长度
		newData[1]=(bMask?0x80:0x00)|dataLen;
		pos=2;
	}
	else if (dataLen<=65535)
	{
		len=126;
		newData[1]=(bMask?0x80:0x00)|0x7E;
		//数据长度
		newData[2] = ( dataLen >> 8 ) & 255;
		newData[3] = ( dataLen ) & 255;
		pos=4;
	}
	else
	{
		len=127;
		newData[1]=(bMask?0x80:0x00)|0x7F;
		//数据长度
		newData[2] = ( dataLen >> 56 ) & 255;
		newData[3] = ( dataLen >> 48  ) & 255;
		newData[4] = ( dataLen >> 40 ) & 255;
		newData[5] = ( dataLen >> 32  ) & 255;
		newData[6] = ( dataLen >> 24 ) & 255;
		newData[7] = ( dataLen >> 16  ) & 255;
		newData[8] = ( dataLen >> 8 ) & 255;
		newData[9] = ( dataLen ) & 255;
		pos=10;
	}
	//是否有掩码
	if(bMask&&mask)
	{
		memcpy_s(newData+pos,5,mask,4);
		pos+=4;
		for(unsigned __int64 i=pos;i<dataLen;i++)
			newData[i]=data[i]^mask[i%4];
	}
	else
		memcpy_s(newData+pos,dataLen+1,data,dataLen);
	newDataSize=pos+dataLen;
	return newData;
}

char* WebSocket::SetDataInfo(char* newData,unsigned __int64& newDataSize,WSDATA_INFO* dInfo){
	if(dInfo->pData!=NULL&&dInfo->Len>0)
	{
		SetDataInfo(newData,newDataSize,dInfo->pData,dInfo->Len,dInfo->hasMask,dInfo->Mask);
		dInfo->hasMask=false;
	}
	return dInfo->pData;
}
