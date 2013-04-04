/*
* rewind.c
*
*  Created on: 4th-June-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/


#include "nc_intern.h"

#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif

	static void rewind_nodes(struct dir_ctx *ctx) {

		if(ctx) {
			int i;
			ctx->nc_cur = ctx->nc_list;
			for(i=0;i<ctx->children_sz;++i)
				rewind_nodes(ctx->children[i]);
		}
	}

	NC_API void cgt_rewindnc(cgt_nc_handle handle) {

		struct handle_struct *h = (struct handle_struct *)handle;

		if(!h || !h->curctx)
			return;

		// find root of context tree.
		while(h->curctx->parent)
			h->curctx = h->curctx->parent;

		// rewind each node recursively.
		rewind_nodes(h->curctx);
	}

#if defined(__cplusplus)
} /* extern "C" { */
#endif

