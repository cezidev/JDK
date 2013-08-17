/*
 * @(#)monitor_cache.h	1.21 97/10/07
 * 
 * Copyright (c) 1995, 1996 Sun Microsystems, Inc. All Rights Reserved.
 * 
 * This software is the confidential and proprietary information of Sun
 * Microsystems, Inc. ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with Sun.
 * 
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
 * SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR ANY DAMAGES
 * SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
 * THIS SOFTWARE OR ITS DERIVATIVES.
 * 
 * CopyrightVersion 1.1_beta
 * 
 */

/*
 * Monitor cache definitions
 */

#ifndef _MONITOR_CACHE_H_
#define _MONITOR_CACHE_H_

#include "monitor.h"		/* For TIMEOUT_INFINITY */

/*
 * There is a distinct recursive lock (not necessarily a monitor) for
 * the monitor cache.
 */
#define CACHE_LOCK_INIT() sysCacheLockInit()
#define CACHE_LOCK()	  sysCacheLock()
#define CACHE_LOCKED()	  sysCacheLocked()
#define CACHE_UNLOCK()	  sysCacheUnlock()

/*
 * External routines.
 */
monitor_t *lookupMonitor(unsigned int);
monitor_t *createMonitor(unsigned int);
monitor_t *findMonitor(sys_thread_t *tid, unsigned int key, bool_t create);
void monitorEnumerate(void (*)(monitor_t *, void *), void *);
void monitorEnumerate_unlocked(void (*)(monitor_t *, void *), void *);

/*
 * This should probably just be defined as a function but it seems that
 * most of the x86 compilers aren't very agressive inliners and I'd
 * really like this to be inlined.
 */

#define checkCache(mon, tid, key) \
{ \
    extern int systemIsMP; \
    sys_thread_t * _tid = (tid); \
    unsigned _key = (key); \
    monitor_t * _mon; \
    if (tid) { \
        /* \
         * Stash the key currently being looked up; the monitor in the \
         * global cache for it, even if otherwise unused, is never collected. \
         */ \
        sysCurrentCacheKey(tid) = key; \
        if (systemIsMP) { \
            sysMemoryFlush(); \
        } \
        _mon = sysLocalMonCache(tid, (((key) >> 3) & (SYS_TLS_MONCACHE-1))); \
        if ((_mon != 0) && (_mon->key == key)) { \
            mon = _mon; \
        } else { \
            mon = NULL; \
        } \
    } else \
	mon = NULL; \
}



#endif /* !_MONITOR_CACHE_H_ */
