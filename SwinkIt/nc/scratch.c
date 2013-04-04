/*
* scratch.c
*
*  Created on: 21st-May-2010
*      Author: cds (chris.stones _AT_ gmail.com)
*/

#include "nc_intern.h"

#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(WIN32)
/*
**  STRREV.C - reverse a string in place
**
**  public domain by Bob Stout
*/
static char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}
#endif


	#if defined(WIN32)
		#define SEPCHAR '\\'
		#define strrev _strrev
	#else
		#define SEPCHAR '/'
	#endif

	static int require_scratch(const struct dir_ctx *curctx, int filescratchsz) {

		const struct dir_ctx *ctx;
		int sz = filescratchsz+1;

		/* longest directory name at this level (unless target is a file with known size (parameter2))*/
		if(!filescratchsz) {
			const struct path_list *dlist = curctx->folderlist;
			while(dlist) {
				int s = strlen(dlist->name);
				if(s>sz)
					sz=s;
				dlist = dlist->next;
			}
			++sz;
		}

		/* plus sum of all parent directory name lengths */
		for(ctx=curctx; ctx; ctx=ctx->parent)
			sz += strlen(ctx->name) + 1; 

		return sz;
	}

	char* full_path_with_scratch(const struct dir_ctx *tail,int filescratchsz) {

		char *scratch;
		int wrote = 0;
		int scratchsize = require_scratch(tail, filescratchsz);

		if(!(scratch = (char*)malloc(scratchsize)))
			return NULL;

		// write directory string ( working backwards )
		while(tail) {

			char *name = tail->name;
			int namelen = strlen(name);

			// inject separator if needed
			if(name[namelen-1]!='\\' && name[namelen-1]!='/')
				*(scratch+(wrote++)) = SEPCHAR;

			// write current name, then reverse it.
			sprintf(scratch+wrote, "%s", name);
			strrev(scratch+wrote);
			wrote+=namelen;
			tail = tail->parent;
		}

		// finally, reverse whole string.
		strrev(scratch);

		return scratch;
	}

#if defined(__cplusplus)
} /*extern "C" { */
#endif