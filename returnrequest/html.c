
#include "http.h"
#include <string.h>
#include <stdio.h>



// delay_time can be up to 3 digits long.
int html_generate_redirect(void *destination_buffer, int delay_time, char *website_url){

	strncat(destination_buffer, html_start, strlen(html_start));
	strncat(destination_buffer, html_head_start, strlen(html_head_start));
	strncat(destination_buffer, html_redirect_start, strlen(html_redirect_start) );

	char *delay = "abc";
	sprintf(delay, "%d", delay_time);
	strncat(destination_buffer, delay, strlen(delay));
	strncat(destination_buffer, html_redirect_2, strlen(html_redirect_2));
	strncat(destination_buffer, website_url, strlen(website_url));
	strncat(destination_buffer, html_redirect_end, strlen(html_redirect_end));
	strncat(destination_buffer, html_head_end, strlen(html_head_end));
	strncat(destination_buffer, html_end, strlen(html_end));

	return 0;
}
