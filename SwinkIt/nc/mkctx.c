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

	#if defined(WIN32)
		#define SEPCHAR '\\'
	#else
		#define SEPCHAR '/'
	#endif
	

	static void* zmalloc(int sz) {

		void *ret;

		if((ret = malloc(sz)))
			memset(ret,0,sz);

		return ret;
	}

	static struct dir_ctx* alloc_ctx(const char * path, struct dir_ctx *parent) {

		struct dir_ctx* ret = NULL;

		int plen = strlen(path);

		/*to save mem, we remove basename in all but the root node. 
			If we need a full path, we can construct one from parents */
		if(parent) {
			if(plen && (path[plen-1]=='/' || path[plen-1]=='\\'))
				--plen;
			while(plen && path[plen-1]!='/' && path[plen-1]!='\\')
				--plen;
			path += plen;
		}

		// re-determine length
		plen = strlen(path);

		if((ret = zmalloc( sizeof *ret + plen + 1))) {

			ret->name = (char*)(ret + 1);
			memcpy(ret->name, path, plen+1);
			ret->parent = parent;
		}

		return ret;
	}

	static int add_dir(struct path_list **list, char *path) {

		struct path_list *newp;
		struct path_list *tail = *list;
		int plen = strlen(path);
		
		// allocate new list item
		if(!(newp = (struct path_list *)zmalloc(sizeof (struct path_list) + plen + 1)))
			return -1;

		// set name
		newp->name = (char*)(newp + 1);
		memcpy(newp->name, path, plen+1);

		if(tail) {
			// append to list
			while(tail->next)
				tail = tail->next;
			tail->next = newp;
		}
		else {
			// start new list
			*list = newp; 
		}
	
		return 0;
	}

	static void mkparent(char * file) {

		int i = strlen(file);

		while(i--)
			switch(file[i])
			{
			default:
				file[i]='\0';
				break;
			case  '/':
			case '\\':
			case  ':':
				i=0;
			}

		if(*file == 0) {
			file[0] = '.';
			file[1] = SEPCHAR; 
		}
	}

	static struct dir_ctx* mkctx_file( const char * path, const char * const * ext, struct dir_ctx *parent_ctx) {

		DIR *dir;
		struct dir_ctx *ctx;
		struct dirent  *dirent;
		char *parent;
		struct cgt_nc * nc_template;
		int err;

		if(!(nc_template = nc_create(path)))
			return NULL;

		// get parent dir string
		if(!(parent = (char*)malloc(strlen(path)+1))) {
			nc_free(nc_template);
			return NULL;
		}
		strcpy(parent,path);
		mkparent(parent);

		// open parent dir
		if(!(dir = opendir(parent)))
		{
			free(parent);
			nc_free(nc_template);
			return NULL;
		}

		if(!(ctx = alloc_ctx(parent,parent_ctx))) {
			free(parent);
			nc_free(nc_template);
			closedir(dir);
			return NULL;
		}

		while((dirent = readdir(dir)))
			if(dirent->d_type == DT_REG)
				if((err = add_nc( ctx, dirent->d_name, nc_template ))<0)
					break;

		ctx->nc_cur = ctx->nc_list;

		free(parent);
		closedir(dir);
		nc_free(nc_template);

		if(err<0) {
			free_ctx(ctx);
			ctx = NULL;
		}
		return ctx;
	}

	struct dir_ctx* mkctx(const char * path, struct handle_struct *h, struct dir_ctx *parent) {

		DIR *dir;
		struct dir_ctx *ctx;
		struct dirent  *dirent;
		int err = 0;

		if(!(dir = opendir(path)))
			return mkctx_file(path, (const char * const *)h->ext, parent);

		if(!(ctx = alloc_ctx(path, parent))) {
			closedir(dir);
			return NULL;
		}

		while(!err && (dirent = readdir(dir))) {

			if(strcmp(dirent->d_name,".")==0)
				continue;
			if(strcmp(dirent->d_name,"..")==0)
				continue;

			if((dirent->d_type == DT_REG) && (goodext(dirent->d_name, (const char * const *)h->ext)))
				add_nc( ctx, dirent->d_name, NULL );	
			else if((dirent->d_type == DT_DIR) && (h->flags & CGT_NC_RECURSE))
				err = add_dir(&(ctx->folderlist),dirent->d_name);
		}

		ctx->nc_cur = ctx->nc_list;

		closedir(dir);

		if(err) {
			free_ctx(ctx);
			ctx = NULL;
		}

		return ctx;
	}

#if defined(__cplusplus)
} /***	extern "C" { ***/
#endif

