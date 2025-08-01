// Copyright (c) 2013, Razvan Petru
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

#ifndef QSLOGDEST_H
#define QSLOGDEST_H

#include "QsLogLevel.h"
#include "QsLogMessage.h"
#include <QSharedPointer>
#include <QtGlobal>

#ifdef QSLOG_IS_SHARED_LIBRARY
#define QSLOG_SHARED_OBJECT Q_DECL_EXPORT
#elif QSLOG_IS_SHARED_LIBRARY_IMPORT
#define QSLOG_SHARED_OBJECT Q_DECL_IMPORT
#else
#define QSLOG_SHARED_OBJECT
#endif

namespace QsLogging
{
class DestinationImpl; // d pointer

class QSLOG_SHARED_OBJECT Destination
{
public:
    typedef std::function<void(const Message& message)> LogFunction;
    typedef std::function<void()> ClearFunction;

public:
    Destination();
    virtual ~Destination();

    //! Logging at a level < 'newLevel' will be ignored
    void setLoggingLevel(Level newLevel);
    //! The default level is INFO
    Level loggingLevel() const;
    //! Set to false to disable timestamp inclusion in log messages
    void setIncludeTimestamp(bool e);
    //! Default value is true.
    bool includeTimestamp() const;
    //! Set to false to disable log level inclusion in log messages
    void setIncludeLogLevel(bool l);
    //! Default value is true.
    bool includeLogLevel() const;
    //! Set to false to disable line number inclusion in log messages
    void setIncludeLine(bool l);
    //! Default value is true.
    bool includeLine() const;
    //! Set to false to disable function inclusion in log messages
    void setIncludeFunction(bool f);
    //! Default value is true.
    bool includeFunction() const;
    //! Set to false to disable category inclusion in log messages
    void setIncludeCategory(bool c);
    //! Default value is true.
    bool includeCategory() const;
    //! Set to false to disable color inclusion in log messages
    void setIncludeColor(bool c);
    //! Default value is true.
    bool includeColor() const;

    //! For csv compatibility.
    void setSeparator(char s);
    char separator() const;

    virtual void clear() = 0;

    void writeToLog(const Message& message);
    virtual void write(const Message& message) = 0;
    virtual bool isValid() = 0; // returns whether the destination was created correctly

    void formatMessage(const Message& message);

private:
    DestinationImpl* d;
};
typedef QSharedPointer<Destination> DestinationPtr;


// a series of "named" paramaters, to make the file destination creation more readable
enum LogRotationOption
{
    DisableLogRotation = 0,
    EnableLogRotation  = 1
};

struct QSLOG_SHARED_OBJECT MaxSizeBytes
{
    MaxSizeBytes() : size(0) {}
    explicit MaxSizeBytes(qint64 size_) : size(size_) {}
    qint64 size;
};

struct QSLOG_SHARED_OBJECT MaxOldLogCount
{
    MaxOldLogCount() : count(0) {}
    explicit MaxOldLogCount(int count_) : count(count_) {}
    int count;
};


//! Creates logging destinations/sinks. The caller shares ownership of the destinations with the logger.
//! After being added to a logger, the caller can discard the pointers.
class QSLOG_SHARED_OBJECT DestinationFactory
{
public:
    static DestinationPtr MakeFileDestination(const QString& filePath,
        LogRotationOption rotation = DisableLogRotation,
        const MaxSizeBytes &sizeInBytesToRotateAfter = MaxSizeBytes(),
        const MaxOldLogCount &oldLogsToKeep = MaxOldLogCount());
    static DestinationPtr MakeDebugOutputDestination();
    // takes a pointer to a function
    static DestinationPtr MakeFunctorDestination(Destination::LogFunction f, Destination::ClearFunction c=nullptr);
    // takes a QObject + signal/slot
    static DestinationPtr MakeFunctorDestination(QObject *receiver, const char *member);
};

} // end namespace

#endif // QSLOGDEST_H
