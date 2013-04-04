
/*
* close.c
*
*  Created on: 4th-June-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif

	NC_API void cgt_closenc(cgt_nc_handle handle) {

		struct handle_struct *h = (struct handle_struct *)handle;

		if(h) {

			// re-wind and free context tree
			struct dir_ctx *ctx = h->curctx;
			while(ctx->parent)
				ctx = ctx->parent;
			free_ctx(ctx);

			// free our copy of the file extension array
			if(h->ext)
			{
				char exitflag;
				int i=0;
				do  {
					exitflag = *(h->ext[i]);
					free(h->ext[i]);
					++i;
				} while(exitflag);
				free(h->ext);
			}

			// free struct
			free(h);
		}

	}

#if defined(__cplusplus)
} /* extern "C" { */
#endif

