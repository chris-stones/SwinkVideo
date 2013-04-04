/*
* frctx.h
*
*  Created on: 19th-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#include<string.h>
#include<stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif


	void nc_free(struct cgt_nc* p) {

		if(p) {
			free(p->nc);
			free(p->ident);
			free(p);
		}
	}

	static void free_nc_list(struct nc_list *head) {

		struct nc_list *pop;

		while((pop = head)) {
			head = head->next;
			nc_free(pop->nc);
			free(pop);
		}
	}

	void free_list(struct path_list *head) {

		struct path_list *pop;

		while((pop = head)) {
			head = head->next;
			free(pop);
		}
	}

	void free_ctx(struct dir_ctx *ctx) {

		int i;

		if(!ctx)
			return;

		free_nc_list(ctx->nc_list);

		free_list(ctx->folderlist);
		
		for(i=0;i<ctx->children_sz; ++i)
			free_ctx( ctx->children[i] );
		free(ctx->children);

		free(ctx);
	}

#if defined(__cplusplus)
} /*** extern "C" { ***/
#endif


