// Copyright (c) 2014, Razvan Petru
// Copyright (c) 2014, Omar Carey
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "QsLogDestFunctor.h"
#include <cstddef>
#include <QtGlobal>

QsLogging::FunctorDestination::FunctorDestination(LogFunction f, ClearFunction c)
    : QObject(NULL)
    , mLogFunction(f)
    , mClearFunction(c)
{
}

QsLogging::FunctorDestination::FunctorDestination(QObject *receiver, const char *member, const char *clearMember)
    : QObject(NULL)
    , mLogFunction(NULL)
    , mClearFunction(NULL)
{
    connect(this, SIGNAL(logMessageReady(QString,int)), receiver, member, Qt::QueuedConnection);

    if(clearMember)
        connect(this, SIGNAL(logClear()), receiver, clearMember, Qt::QueuedConnection);
}

void QsLogging::FunctorDestination::clear()
{
    if (mClearFunction)
        mClearFunction();

    emit logClear();
}

void QsLogging::FunctorDestination::write(const Message& message)
{
    formatMessage(message);
    if (mLogFunction)
        mLogFunction(message);

    emit logMessageReady(message);
}

bool QsLogging::FunctorDestination::isValid()
{
    return true;
}
