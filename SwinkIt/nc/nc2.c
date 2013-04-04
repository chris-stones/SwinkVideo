/*
* nc2.c
*
*  Created on: 18th-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*
*	find animations conforming to the naming convention 'identifier<FrameNo>.ext'
*/

#include "nc.h"


#if !defined(NC_API)
#define NC_API
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(WIN32)
#define strncasecmp _strnicmp
#else
#include<strings.h> // POSIX strncasecmp
#endif

#include<sys/types.h>
#include<sys/stat.h>

#define BIGGEST(x,y) x>y?x:y
#define SMALLEST(x,y) x<y?x:y

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "nc_intern.h"

#if defined(__cplusplus)
extern "C" {
#endif

	static int isnum(char c) {

		return c>='0' && c<='9';
	}
	int issame(const struct cgt_nc* p1, const struct cgt_nc* p2) {

			int skip1=0;
			int skip2=0;

			if((strncmp("./", p1->ident, 2) == 0) || (strncmp(".\\", p1->ident, 2) == 0))
				skip1 = 2;
			if((strncmp("./", p2->ident, 2) == 0) || (strncmp(".\\", p2->ident, 2) == 0))
				skip2 = 2;

			if (strcmp(p1->ident+skip1, p2->ident+skip2)) 
				return 0;

			if (!p1->lzflg && !p2->lzflg)
				return 1;

			if (p1->lzflg || p2->lzflg)
				return p1->numpad == p2->numpad;

			return 0;
	}

	void incorporate(struct cgt_nc* dst, struct cgt_nc* src) {

			dst->first = SMALLEST(dst->first, src->first);
			dst->last  = BIGGEST(dst->last, src->last);
			dst->lzflg = dst->lzflg || src->lzflg;
			dst->file_count += src->file_count;
			if(dst->numpad > src->numpad) {

				dst->numpad = src->numpad;
				strcpy(dst->nc, src->nc);
			}
	}

	struct cgt_nc* nc_create(const char* file) {

			struct cgt_nc* p=NULL;
			int len,dp,numbegin,nclen;

			if(!file)
				return NULL;

			len = strlen(file);

			dp=len;
			while((file[dp]!='.') && dp) {
				if(file[dp]=='\\' || file[dp]=='/')
					dp = 0;
				else
					--dp;
			}

			if(!dp)
				return NULL;

			numbegin=dp-1;
			if(!isnum(file[numbegin]))
				return NULL;

			while(isnum(file[numbegin]) && numbegin)
				--numbegin;

			if(!isnum(file[numbegin]))
				++numbegin;

			nclen=len +5 +(dp-numbegin); /* TODO: test this */

			if(!(p = (struct cgt_nc*)malloc(sizeof (struct cgt_nc))))
				return NULL;

			p->file_count = 1;
			p->lzflg = 0;
			p->current = p->last = p->first= atoi(file+numbegin);     
			p->numpad =  dp-numbegin;
			p->lzflg = ((file[numbegin]=='0') && ((dp-numbegin)>1));

			if(!(p->nc = (char*)malloc(nclen))) {

				nc_free(p);
				return NULL;
			}

			if(!(p->ident = (char*)malloc(nclen))) {

				nc_free(p);
				return NULL;
			}

			memset(p->nc,0,nclen);
			memset(p->ident,0,nclen);
			strncpy(p->nc, file, numbegin);
			sprintf(p->nc + numbegin, "%%0%dd.%s", p->numpad, file+dp+1);
			strncpy(p->ident, file, numbegin);
			sprintf(p->ident + numbegin, "#.%s",file+dp+1);

			return p;
	}

	int add_nc( struct dir_ctx *ctx, const char * d_name, const struct cgt_nc *match ) {

		int err = 0;
		char *path;
		int added = 1;
		struct nc_list* _new;
		struct nc_list* head;

		if((_new = (struct nc_list*)malloc( sizeof(struct nc_list)))==NULL)
			return -1;
		memset(_new,0,sizeof *_new);

		if(!(path = full_path_with_scratch(ctx,strlen(d_name)))) {
			free(_new);
			return -1;
		}
		strcat(path,d_name);
		if(!(_new->nc = nc_create(path))) {
			free(_new);
			free(path);
			return 0; /* todo: return -1 on memory errors, 0 on 'path not nc' errors. */
		}
		free(path);

		if(match && (!issame(_new->nc, match))) {

			nc_free(_new->nc);
			free(_new);
			return 0;
		}

		if((head = ctx->nc_list)) {

			while(head && _new) {

				if(issame(head->nc,_new->nc)) {

					incorporate(head->nc,_new->nc);
					nc_free(_new->nc);
					free(_new);
					_new = NULL;
					added = 0;
					break;
				} 
				if(head->next)
					head = head->next;
				else
					break;
			}
			if(_new)
				head->next = _new;
		} 
		else
			ctx->nc_list = _new;

		return added;
	}

#if defined(__cplusplus)
}; /* extern "C" { */
#endif



