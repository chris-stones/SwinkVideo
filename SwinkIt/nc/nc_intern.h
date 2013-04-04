
/*
* nc_intern.h
*
*  Created on: 18th-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#pragma once

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

	

	#if defined(WIN32)
	#define strncasecmp _strnicmp
	#else
	#include<strings.h> // POSIX strncasecmp
	#endif

	#include "nc.h"

	struct path_list {

		struct path_list  *next;
		char              *name;
	};

	struct nc_list {

		struct cgt_nc  *nc;
		struct nc_list *next;
	};

	struct dir_ctx {

		char			 *name;			// folder name
		
		struct path_list *folderlist;	// folders in this context.
		struct nc_list	 *nc_list;		// list of naming convetions in this context.
		struct nc_list	 *nc_cur;		// current position in above list

		struct dir_ctx	*parent;		// parent context (null for root)

		struct dir_ctx	**children;		// array of child contexts
		int				children_sz;	// child context array size
	};

	struct handle_struct {

		struct dir_ctx *curctx;
		char ** ext;
		int flags;
	};

	void free_ctx(struct dir_ctx *ctx);
	struct dir_ctx* mkctx(const char * path, struct handle_struct *h, struct dir_ctx *parent);
	void free_list(struct path_list *head);
	char* full_path_with_scratch(const struct dir_ctx *tail,int filescratchsz);
	void nc_free(struct cgt_nc* p);
	int add_nc( struct dir_ctx *ctx, const char * d_name, const struct cgt_nc *match ) ;
	struct cgt_nc* nc_create(const char* file);
	void incorporate(struct cgt_nc* dst, struct cgt_nc* src);
	int goodext(const char *fn, const char *const *ext);
	int expand_ctx( struct dir_ctx *ctx , struct handle_struct *h);

#if defined(__cplusplus)
} /*	extern "C" { */
#endif


