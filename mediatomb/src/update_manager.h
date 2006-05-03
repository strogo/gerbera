/*  update_manager.h - this file is part of MediaTomb.
                                                                                
    Copyright (C) 2005 Gena Batyan <bgeradz@deadlock.dhs.org>,
                       Sergey Bostandzhyan <jin@deadlock.dhs.org>
                                                                                
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
                                                                                
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                                
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __UPDATE_MANAGER_H__
#define __UPDATE_MANAGER_H__

#include <pthread.h>

#include "common.h"
#include "hash.h"

#define FLUSH_ASAP 2
#define FLUSH_SPEC 1

long getMillis();

class UpdateManager : public zmm::Object
{
public:
    UpdateManager();
    virtual ~UpdateManager();
    void init();
    void shutdown();

    void containerChanged(int objectID);
    void containersChanged(int *ids, int size);
    void flushUpdates(int flushFlag = FLUSH_SPEC);

    static zmm::Ref<UpdateManager> getInstance();
    
protected:
    bool lazyMode;
    int *objectIDs;
    int objectIDcount;
    zmm::Ref<DBBHash<int, int> > objectIDHash;
    
    pthread_t updateThread;
    pthread_mutex_t updateMutex;
    pthread_cond_t updateCond;

    bool shutdownFlag;
    int flushFlag;
    
    void lock();
    void unlock();
    static void *staticThreadProc(void *arg);
    void threadProc();
    
    void sendUpdates();
    bool haveUpdates();
};


#endif // __UPDATE_MANAGER_H__
