#include <iostream>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //指定链接 ws2_32.lib 库，该库包含 Windows Sockets API 的实现。
#define DEFAULT_PORT 8080
#define DEFAULT_BUFLEN 512
int main()
{
    WSADATA wsaData;
    //初始化 winsock
    int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);//初始化Winsock库，MAKEWORD(2,2)表示使用Winsock2.2版本。
    if(iResult!=0)
    {
       std::cerr<<"WSAStartup failed: "<<iResult<<std::endl;
       return 1;
    }

}
