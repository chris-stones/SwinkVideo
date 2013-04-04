/*
* good_ext.c
*
*  Created on: 4th-June-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif

	int goodext(const char *fn, const char *const *ext) {

		int len,extlen;
		int i=0;

		if(!fn)
			return 0;

		if(!ext)
			return 1;

		len = strlen(fn);

		for(i=0;;++i) {

			if(*(ext[i])=='\0')
				return 0;

			extlen = strlen(ext[i]);

			if(extlen>len)
				continue;

			if(strncasecmp(fn+(len-extlen),ext[i], extlen)==0)
				return 1;
		}
	}

#if defined(__cplusplus)
} /* extern "C" { */
#endif

