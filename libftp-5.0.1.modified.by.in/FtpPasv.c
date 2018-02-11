/*		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  exist
ing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"
#include <assert.h>


char * FtpPasv (FTP *ftp)
{
  char *msg;
  static FtpString PORT;
  char *p=PORT;
  
  if FtpError(FtpCommand(ftp,"PASV","",227,EOF)) 
    return "";
  
  msg = FtpMessage (227);

  msg+=3;
  
  while (!isdigit(*msg++));  
  msg--;

  while (isdigit(*msg)||*msg==',') *p++=*msg++;
  *p=0;
  
  return PORT;
}

void FtpPassiveGetAddressAndPort(char *portstrings, char* ipaddress, int *port)
{
  int a1, a2, a3, a4, p1, p2, dataPort;
  assert(ipaddress);
  assert(port);
  sscanf(portstrings, "%d,%d,%d,%d,%d,%d", &a1,&a2,&a3,&a4,&p1,&p2);
  *port = (p1 * 256) + p2;
  snprintf(ipaddress, 20, "%d.%d.%d.%d", a1, a2, a3, a4);
  return;
}

STATUS FtpLink(FTP *ftp1, FTP *ftp2)
{
  
  FtpString PORT;

  strcpy(PORT,FtpPasv(ftp1));

  return FtpCommand(ftp2,"PORT %s",PORT,200,EOF);
}

STATUS FtpPassiveTransfer(FTP *ftp1, FTP *ftp2, char *f1, char *f2)
{
  FtpString tmp;
  fd_set fds;

  FtpAssert(ftp1,FtpLink(ftp1,ftp2));


  if (!*f2) f2=f1;

  FtpAssert(ftp2,FtpCommand(ftp2,"STOR %s",f2, 200, 120 , 150 , 125 , 250 , EOF ));
  FtpAssert(ftp1,FtpCommand(ftp1,"RETR %s",f1, 200, 120 , 150 , 125 , 250 , EOF ));
  
  FD_ZERO(&fds);
  
  FD_SET(fileno(FTPCMD(ftp1)),&fds);
  FD_SET(fileno(FTPCMD(ftp2)),&fds);
  
  if (select(getdtablesize(),&fds,0,0,0)<0)
    {
#if ! defined( __DARWIN__ ) && ! defined( __LINUX__ ) && ! defined( __FREEBSD__ )
      if (ftp1->error!=NULL)
	return (*(ftp1->error))(ftp1,QUIT,sys_errlist[errno]);
      if (ftp2->error!=NULL)
	return (*(ftp2->error))(ftp1,QUIT,sys_errlist[errno]);
#else
      if (ftp1->error!=NULL)
	return (*(ftp1->error))(ftp1,QUIT,strerror( errno ) );
      if (ftp2->error!=NULL)
	return (*(ftp2->error))(ftp1,QUIT,strerror( errno ) );
#endif
      return QUIT;
    }
  
  if (FD_ISSET(fileno(FTPCMD(ftp1)),&fds))
    {
      FtpGetMessage(ftp1,tmp);
      FtpLog(ftp1->title,tmp);
      FtpGetMessage(ftp2,tmp);
      FtpLog(ftp2->title,tmp);
    }
  else
    {
      FtpGetMessage(ftp2,tmp);
      FtpLog(ftp2->title,tmp);
      FtpGetMessage(ftp1,tmp);
      FtpLog(ftp1->title,tmp);
    }

	return 0;
}

STATUS FtpPassiveData(FTP * con,int *pascon, char * command , char * file ,char * mode)
{
  struct sockaddr_in unit;
  struct hostent *host;
  int i,new_socket,port;
  char* pasvresponse;
  char *ipaddress = malloc(sizeof(char) * 20);

  pasvresponse = FtpPasv( con);
  FtpPassiveGetAddressAndPort(pasvresponse, ipaddress, &port);

  memset(&unit, '0', sizeof(unit));
  unit.sin_family = AF_INET;
  unit.sin_port = htons(port);
  if(inet_pton(AF_INET, ipaddress, &unit.sin_addr)<=0)
    {
       printf("inet_pton error occured\n");
       return EXIT(con,QUIT);
    } 
  if ( ( new_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) return EXIT(con,QUIT);
  if( connect(new_socket, (struct sockaddr *) &unit, sizeof(unit)) < 0)
    {
       printf("\n Error : Connect to %s:%i Failed\n",ipaddress,port);
       return EXIT(con,QUIT);
    }
  *pascon = new_socket;

  if ( con -> seek != 0) {
          if ((i = FtpCommand ( con, "REST %d" , con -> seek , 0, EOF)) != 350 ) {
                  return -i;
          }
  }

  FtpAssert(con, i=FtpCommand ( con , command , file , 200, 120 , 150 , 125 , 250 , EOF ));

  return i;
}
