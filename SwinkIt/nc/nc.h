
/*
* nc.h
*
*  Created on: 16th-October-2009
*      Author: cds (chris.stones _AT_ gmail.com)
*
*	find animations conforming to the naming convention 'identifier<FrameNo>.ext'
*/

#ifndef __CGT_NC_H
#define __CGT_NC_H


#if defined(WIN32)
#if defined(LIBNC_EXPORTS)
#define NC_API __declspec(dllexport)
#else
#define NC_API __declspec(dllimport)
#endif
#endif

#if !defined(NC_API)
	#define NC_API
#endif

#if defined(__cplusplus)
extern "C" {
#endif

	enum CGT_ERR { 

		CGT_EOKAY=0,
		CGT_EERROR,
		CGT_ENOMEM,
		CGT_ENINVNC,
		CGT_EINVAL,
	};

	enum CGT_NC_FLAGS {

		CGT_NC_RECURSE	=(1<<0),
	};

	typedef void* cgt_nc_handle;

	struct cgt_nc {

		char *nc;
		char *ident;
		char numpad;
		int first;
		int last;
		int current;
		char lzflg;
		int file_count;
	};

	NC_API	cgt_nc_handle	cgt_opennc		(const char * path, const char *const * ext, int flags);
	NC_API	void		cgt_rewindnc		(cgt_nc_handle handle);
	NC_API	struct cgt_nc*	cgt_readnc		(cgt_nc_handle handle);
	NC_API	void		cgt_closenc		(cgt_nc_handle handle);

#if defined(__cplusplus)
} /*** extern "C" { ***/
#endif

#endif /*** __CGT_NC_H ***/

