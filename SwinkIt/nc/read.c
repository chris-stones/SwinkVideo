
/*
* read.c
*
*  Created on: 4th-June-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif


	/*** 
		Have at-least one more nc in the current context ? 
		Return it, and advance the current nc marker.
	***/
	static struct cgt_nc* one_ready(struct handle_struct *h) {

		struct cgt_nc *ret;

		if(!h || !h->curctx || !h->curctx->nc_cur)
			return NULL;

		ret = h->curctx->nc_cur->nc;

		h->curctx->nc_cur = h->curctx->nc_cur->next;

		return ret;
	}

	/*** 
		Scan sub-directories for a context with 'one_ready'
	***/
	static struct cgt_nc* find_nc_in_subdir(struct handle_struct *h) {

		struct cgt_nc *ret = NULL;

		// while we have children.
		while(!ret && (h->curctx->folderlist || h->curctx->children_sz)) {

			// if children are not yet expanded, expand them.
			if(!h->curctx->children_sz)
				expand_ctx(h->curctx, h);

			// successfully expanded ?
			if(h->curctx->children_sz) {
				// move to first child.
				h->curctx = *(h->curctx->children);
				// naming convention ready? return it.
				ret = one_ready(h);
			}
		}
		return ret;
	}

	/***
	  find next naming convention in the context.
	***/
	NC_API struct cgt_nc* cgt_readnc(cgt_nc_handle handle) {

		struct cgt_nc *ret;
		struct handle_struct *h = (struct handle_struct *)handle;

		if(!h || !h->curctx)
			return NULL;

		if((ret = one_ready(h)))
			return ret;

		if((ret = find_nc_in_subdir(h)))
			return ret;

		//end of the road, back up and try a sibling!
		while(h->curctx->parent)
		{
			int i;
			struct dir_ctx *old_context = h->curctx;
			h->curctx = h->curctx->parent;
			for(i=0;i<h->curctx->children_sz;++i)
				if(old_context == h->curctx->children[i])
					break;

			// has more un-scanned siblings ?
			if(i<(h->curctx->children_sz-1)) {
				h->curctx = h->curctx->children[i+1]; // drop to old_ccontext's sibling.

				// does this context have any not yet returned naming conventions?
				if((ret = one_ready(h)))
					return ret;

				// does this child have a naming convention ready ?
				if((ret = find_nc_in_subdir(h)))
					return ret;
			}
		}

		return NULL; // got all of them.
	}

#if defined(__cplusplus)
} /* extern "C" { */
#endif

