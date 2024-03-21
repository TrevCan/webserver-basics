#ifndef HTTP_H
#define HTTP_H

char http_boilerplatehttp[] = ""
"HTTP/1.0 200 OK\r\n"
"Server: theserver\r\n"
"Content-type: text/html\r\n"
"Content-Length: "
;

char html_start [] = ""
"<html>\n" ;

char html_end [] = ""
"</html>\n" ;

char html_code_start [] = ""
"\t<code>\n" ;

char html_code_end [] = ""
"\t</code>\n" ;

char html_head_start [] = ""
"<head>\n" ;

char html_head_end [] = ""
"</head>\n" ;


char html_redirect_start [] = ""
"<meta http-equiv=\"refresh\" content=\"";

char html_redirect_2 [] = ""
"; URL=";

char html_redirect_end [] = ""
"\" />";


#endif
