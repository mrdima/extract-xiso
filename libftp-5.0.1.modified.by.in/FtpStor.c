static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1  2004/05/04 19:31:35  in_the_mix
	Initial revision
	
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.1  1995/12/02  11:23:07  orel
 * *** empty log message ***
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 2.4  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.1  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 2.1  1995/02/04  09:02:53  orel
 * add rcsid
 *

*/
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

STATUS FtpStor (FTP * con , char * command ,
		       char *in , char * out)
{
  FILE *i;
  char buffer[FTPBUFSIZ];
  int size;

  con->seek=0;

  if ( FtpTestFlag(con,FTP_REST) && 
      (con->seek=FtpSize(con,out))<0 )
    con->seek=0;
  
  
  if ( FtpError(FtpData(con,command,out,"w")))
    {
      if (con->seek==0) return EXIT(con,con->err_no);

      con -> seek =0;
      if ( FtpError(FtpData(con,command,out,"w")) )
	return EXIT(con,con->err_no);
    }
  

  if ( (i=Ftpfopen(in,"rb")) == NULL )
    return EXIT(con,LQUIT);

  if (con->seek) fseek(i,con->seek,0);
  
  
  while ( (size=read ( fileno(i) , buffer, FTPBUFSIZ ))>0) 
    FtpWriteBlock(con,buffer,size);
  
  Ftpfclose(i);
  return FtpClose(con);
}







