static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1  2004/05/04 19:31:34  in_the_mix
	Initial revision
	
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
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
 * Revision 1.5  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 1.5  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 1.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
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

char * FtpPwd(FTP * con)
{
  FtpString tmp;
  static FtpString tmp1;
  int i;
  
  if ( FtpSendMessage(con,"PWD") == QUIT )
    return (char *) EXIT(con,QUIT);
  if ( (i=FtpGetMessage(con,tmp)) == QUIT )
    return (char *) EXIT(con,QUIT);
  
  if ( i != 257 )
    return (char *) EXIT(con,-i);

  strcpy(tmp1,word(tmp,2));
  if (*tmp1=='"')
    {
      strcpy(tmp,tmp1+1);
      strncpy(tmp1,tmp,strlen(tmp)-1);
    }
  con -> err_no = i;
  return tmp1;
}
