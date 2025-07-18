#ifndef TABLE_LOG_H
#define TABLE_LOG_H

#define TABLELOG_LEVEL_TRACE 0
#define TABLELOG_LEVEL_DEBUG 1
#define TABLELOG_LEVEL_INFO 2
#define TABLELOG_LEVEL_WARNING 3
#define TABLELOG_LEVEL_CRITICAL 4

#define TABLELOG_MIN_LEVEL TABLELOG_LEVEL_INFO

#define TABLELOG_NOTICE \
QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1C""TABLE").debug

#define TABLELOG_TRACE \
    if(TABLELOG_MIN_LEVEL>TABLELOG_LEVEL_TRACE) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1B""TABLE").debug
#define TABLELOG_DEBUG \
    if(TABLELOG_MIN_LEVEL>TABLELOG_LEVEL_DEBUG) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TABLE").debug
#define TABLELOG_INFO \
    if(TABLELOG_MIN_LEVEL>TABLELOG_LEVEL_INFO) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TABLE").info
#define TABLELOG_WARNING \
    if(TABLELOG_MIN_LEVEL>TABLELOG_LEVEL_WARNING) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TABLE").warning
#define TABLELOG_CRITICAL \
    if(TABLELOG_MIN_LEVEL>TABLELOG_LEVEL_CRITICAL) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TABLE").critical
#define TABLELOG_FATAL \
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TABLE").fatal

#endif // TABLE_LOG_H
