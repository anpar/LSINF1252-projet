#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <sys/types.h>
#include "util.h"

bool is_url(const char * filename) 
{
	regex_t regex;
	const char *pattern = "^(http)|(https)://w{3}?.[.-_[:alnum:]]+\\.[[:lower:]]{2,4}";
	int err = regcomp(&regex, pattern, REG_EXTENDED);
	if(err != 0)
		return(false); 

        bool result = !regexec(&regex, filename, 0, NULL, 0);
	regfree(&regex);

        return(result);		
}
