/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __EVENTPROCESSOR_H
#define __EVENTPROCESSOR_H

#include "ObjectHandler.h"
#include "LockedVector.h"
#include "ace/Null_Mutex.h"
#include "Log.h"
#include "Timer.h"

// Note. All times are in milliseconds here.

class BasicEvent
{
    public:

        BasicEvent()
            : to_Abort(false), finished(false)
        {
        }

        virtual ~BasicEvent()                               // override destructor to perform some actions on event removal
        {
        };

        // this method executes when the event is triggered
        // return false if event does not want to be deleted
        // e_time is execution time, p_time is update interval
        virtual bool Execute(uint64 /*e_time*/, uint32 /*p_time*/) { return true; }

        virtual bool IsDeletable() const { return true; }   // this event can be safely deleted

        virtual void Abort(uint32 /*e_time*/) {}            // this method executes when the event is aborted

        virtual void Finish() { finished = true;}           // this method executes when the event is aborted

        // these can be used for time offset control
        void ModExecTime(uint32 e_time = 0);
        bool IsFinished() const     { return finished;}     // check if event finished

        uint32 m_addTime;                                   // time when the event was added to queue, filled by event handler
        uint32 m_execTime;                                  // planned time of next execution, filled by event handler
        bool   to_Abort;                                    // set by externals when the event is aborted, aborted events don't execute

    private:
        bool   finished;                                    // set external where event completely executed (for speedup cleanup)
};

OBJECT_HANDLER(BasicEvent,BasicEventPtr);
typedef ACE_Based::LockedVector<BasicEventPtr> EventList;

class EventProcessor
{
    public:

        EventProcessor();
        ~EventProcessor();

        void Update(uint32 update_diff, uint32 p_time);
        void KillAllEvents(bool force);
        void AddEvent(BasicEvent* Event, uint32 e_time, bool set_addtime = true);
        uint32 CalculateTime(uint32 t_offset);
        void AddEvent(BasicEventPtr Event);

    protected:
        EventList m_events;
        bool m_aborting;
};

#endif
