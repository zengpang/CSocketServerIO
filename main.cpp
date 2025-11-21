#include <winsock2.h> //包含Windows Sockets API的头文件
#include <windows.h> //包含Windows 操作系统的基本功能头文件 
#include <iostream> //用于输入输出流的头文件
#pragma comment(lib, "ws2_32.lib") // 指定链接 ws2_32.lib 库，该库包含 Windows Sockets API 的实现。
#define DEFAULT_PORT 8080 //指定监听的端口号
#define DEFAULT_BUFLEN 512 //指定接收缓冲区的大小

int main()
{
    WSADATA wsaData;//初始化Winsock库，MAKEWORD(2,2)表示使用Winsock2.2版本
    // 初始化 winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // 初始化Winsock库，MAKEWORD(2,2)表示使用Winsock2.2版本。
    if (iResult != 0)
    {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // 创建监听套接字
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 绑定套接字到指定地址和端口
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    iResult = bind(ListenSocket, (SOCKADDR *)&service, sizeof(service));
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 开始监听
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "bind failed:" << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 创建一个事件对象
    WSAEVENT event = WSACreateEvent();
    if (event == WSA_INVALID_EVENT)
    {
        std::cerr << "WSACreateEvent failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 将监听套接字与事件对象关联，并指定要监听的网络事件(这里建通FD_ACCEP事件，即有新连接到来)
    iResult = WSAEventSelect(ListenSocket, event, FD_ACCEPT);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "WSAEventSelect failed: " << WSAGetLastError() << std::endl;
        WSACloseEvent(event);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 等待事件发生
    while (true)
    {
        iResult = WSAWaitForMultipleEvents(1, &event, FALSE, WSA_INFINITE, FALSE);
        if (iResult == WSA_WAIT_FAILED)
        {
            std::cerr << "WSAWaitForMultipleEvents  failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // 重置事件对象
        iResult = WSAResetEvent(event);
        if (iResult == FALSE)
        {
            std::cerr << "WSAResetEvent failed" << WSAGetLastError() << std::endl;
            break;
        }

        // 获取发生的网络事件
        WSANETWORKEVENTS networkEvents;
        iResult = WSAEnumNetworkEvents(ListenSocket, event, &networkEvents);
        if (iResult == SOCKET_ERROR)
        {
            std::cerr << "WSAEnumNetworkEvents failed:" << WSAGetLastError() << std::endl;
            break;
        }

        // 检查是否有新连接到来
        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                std::cerr << "FD_ACCEPT failed: " << networkEvents.iErrorCode[FD_ACCEPT_BIT] << std::endl;
            }
            else
            {
                // 接收新连接
                SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
                if (ClientSocket == INVALID_SOCKET)
                {
                    std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
                }
                else
                {
                    std::cout << "New connection accepted." << std::endl;
                    // 这里可以处理客户端连接，例如接收和发送数据
                    char recvBuf[DEFAULT_BUFLEN];
                    iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
                    if (iResult > 0)
                    {
                        std::cout << "Received: " << std::string(recvBuf, iResult) << std::endl;
                    }
                    else if (iResult == 0)
                    {
                        std::cout << "Connection closed." << std::endl;
                    }
                    else
                    {
                        std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
                    }
                    closesocket(ClientSocket);
                }
            }
        }
        // 清理资源
        WSACloseEvent(event);
        closesocket(ListenSocket);
        WSACleanup();
        return 0;
    }
}
