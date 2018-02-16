/*
		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  existing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"

STATUS FtpData(FTP * con,char * command , char * file ,char * mode)
{
  struct sockaddr_in data,from, my_addr;
  register struct hostent *host;
  FtpString hostname;
  int NewSocket,Accepted_Socket,len=sizeof(data),one=1,fromlen=sizeof(from),i;
  char *a,*b;
  char local_ip[16];
  unsigned int local_port;
  int fd = fileno( FTPCMD( con ) );

// Get my ip address and port for existing cmd channel socket
  bzero(&my_addr, sizeof(my_addr));
  int len2 = sizeof(my_addr);
  getsockname(fd, (struct sockaddr *) &my_addr, &len2);
  inet_ntop(AF_INET, &my_addr.sin_addr, local_ip, sizeof(local_ip));
  local_port = ntohs(my_addr.sin_port);

  FREE(data);
  FREE(from);
  
  if ( gethostname( hostname , sizeof hostname ) == -1 )
    return EXIT(con,QUIT);
  
  if ((host=(struct hostent *)gethostbyname(hostname))==0)
    return EXIT(con,QUIT);
  
  data.sin_family = host -> h_addrtype;
  
  if(inet_pton(AF_INET, local_ip, &data.sin_addr)<=0)
  {
    printf("inet_pton error occured\n");
    return EXIT(con,QUIT);
  }
		
  if ((NewSocket = socket ( AF_INET  , SOCK_STREAM , 0 ))<0) {
	fprintf( stderr, "socket() failed in FtpData: %s\n", strerror( errno ) );
    return EXIT(con,QUIT);
  }


	if ( setsockopt ( NewSocket , SOL_SOCKET , SO_REUSEADDR , (char *)&one , sizeof(one) ) < 0 ) {
		fprintf( stderr, "setsockopt() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT ( con,QUIT );
	}

	data.sin_port = 0 ;

  if (con -> active_ftp) { // Active FTP
	if ( bind ( NewSocket , (struct sockaddr*) &data , sizeof data ) < 0 ) {
		fprintf( stderr, "bind() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}

	if ( getsockname ( NewSocket , (struct sockaddr*) &data , (socklen_t *) &len ) < 0 ) {
		fprintf( stderr, "bind() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}
  
	if ( listen ( NewSocket , 1 ) < 0 ) {
		fprintf( stderr, "listen() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}


	a = ( char * ) & data.sin_addr;
	b = ( char * ) & data.sin_port;

	FtpAssert(con,FtpPort(con,CUT(a[0]),CUT(a[1]),CUT(a[2]),CUT(a[3]),CUT(b[0]),CUT(b[1])));

	if ( con -> seek != 0) {
		if ((i = FtpCommand ( con, "REST %d" , con -> seek , 0, EOF)) != 350 ) {
			close(NewSocket);
			return -i;
		}
	}

  
	FtpAssert(con, i=FtpCommand ( con , command , file , 200, 120 , 150 , 125 , 250 , EOF ));
  
	if (( Accepted_Socket = accept (NewSocket , (struct sockaddr *)&from , (socklen_t *) &fromlen )) < 0) {
		fprintf( stderr, "accept() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}

	close(NewSocket);
  } else { // Passive FTP
        int port;
        char* pasvresponse;
        char *ipaddress = malloc(sizeof(char) * 20);

        pasvresponse = FtpPasv( con);
        FtpPassiveGetAddressAndPort(pasvresponse, ipaddress, &port);

        memset(&data, '0', sizeof(data));
        data.sin_family = AF_INET;
        data.sin_port = htons(port);
        if(inet_pton(AF_INET, ipaddress, &data.sin_addr)<=0)
          {
             printf("inet_pton error occured\n");
             return EXIT(con,QUIT);
        }
        if( connect(NewSocket, (struct sockaddr *) &data, sizeof(data)) < 0)
          {
             printf("\n Error : Connect to %s:%i Failed\n",ipaddress,port);
             close(NewSocket);
             return EXIT(con,QUIT);
          }
        Accepted_Socket = NewSocket;

        if ( con -> seek != 0) {
                if ((i = FtpCommand ( con, "REST %d" , con -> seek , 0, EOF)) != 350 ) {
                        return -i;
                }
        }
	FtpAssert(con, i=FtpCommand ( con , command , file , 200, 120 , 150 , 125 , 250 , EOF ));

  }

	FTPDATA(con) = winsock_fdopen(Accepted_Socket, "r+");

	return i;
}
