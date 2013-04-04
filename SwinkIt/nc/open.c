
/*
* open.c
*
*  Created on: 18th-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif

	NC_API
	cgt_nc_handle cgt_opennc(const char * path, const char *const *ext, int flags) {

		struct handle_struct *h;
		
		if((h = (struct handle_struct*)malloc(sizeof(struct handle_struct)))==NULL)
			return (cgt_nc_handle)0; /* out of memory! */

		memset(h,0,sizeof(*h));

		h->flags    = flags;

		// copy valid extension list.
		if(ext) {
			int extlen = 0;
			while(*(ext[extlen++]) != '\0');
			if((h->ext = (char**)malloc(sizeof (char*) * extlen))) {
				int i;
				int err=0;
				memset(h->ext,0,sizeof(char*)*extlen);
				for(i=0;i<extlen && !err;++i)
					if((h->ext[i] = (char*)malloc( strlen( ext[i] )+1 )))
						strcpy(h->ext[i],ext[i]);
					else
						err = 1;
				if(err) {
					for(i=0;i<extlen;i++)
						free(h->ext[i]);
					free(h->ext);
					h->ext = NULL;
					free(h);
				}
			}
		}

		// create root context
		h->curctx = mkctx(path, h, NULL);

		// cleanup on err.
		if(!h->curctx || (!h->ext && ext)) {
			free_ctx(h->curctx);
			free(h);
			h=NULL;
		}

		return (cgt_nc_handle)h;
	}

#if defined(__cplusplus)
} /* extern "C" { */
#endif

