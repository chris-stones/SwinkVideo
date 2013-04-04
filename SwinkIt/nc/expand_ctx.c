/*
* mkctx.c
*
*  Created on: 18th-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif

	static int listsize( const struct path_list * list ) {

		int size;

		for(size=0; list; list = list->next)
			++size;

		return size;
	}

	

	int expand_ctx( struct dir_ctx *ctx , struct handle_struct *h) {

		int i,j,size,scratch_sz;
		char *scratch;
		struct path_list *list;	

		if((size = listsize(ctx->folderlist))==0)
			return 0;

		if(!(ctx->children = (struct dir_ctx**)calloc(size, sizeof(struct dir_ctx*))))
			return -1;

		if(!(scratch = full_path_with_scratch(ctx,0))) {
			free(ctx->children);
			return -1;
		}

		scratch_sz = strlen(scratch);

		for(i=0, list=ctx->folderlist; i<size && list; ++i, list = list->next) {

			sprintf(scratch+scratch_sz, "%s", list->name);

			ctx->children[i] = mkctx( scratch, h, ctx );
		}

		// some children may be null ( access denied ), remove them.
		for(i=0, j=0;i<size;++i)
			if((ctx->children[j] = ctx->children[i]))
				++j;
		size = j;
			


		// after expending directory list, free it.
		free_list(ctx->folderlist);
		ctx->folderlist = NULL;

		// store size of ctx->children array.
		ctx->children_sz = size;

		if(ctx->children_sz==0) {
			free(ctx->children);
			ctx->children = NULL;
		}

		free(scratch);

		return 0;
	}



#if defined(__cplusplus)
} /* extern "C" { */
#endif

