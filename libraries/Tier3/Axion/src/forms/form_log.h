#ifndef FORM_LOG_H
#define FORM_LOG_H

#define FORMLOG_LEVEL_TRACE 0
#define FORMLOG_LEVEL_DEBUG 1
#define FORMLOG_LEVEL_INFO 2
#define FORMLOG_LEVEL_WARNING 3
#define FORMLOG_LEVEL_CRITICAL 4

#define FORMLOG_MIN_LEVEL FORMLOG_LEVEL_INFO

#define FORMLOG_NOTICE \
QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1C""FORM").debug

#define FORMLOG_TRACE \
    if(FORMLOG_MIN_LEVEL>FORMLOG_LEVEL_TRACE) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1B""FORM").debug
#define FORMLOG_DEBUG \
    if(FORMLOG_MIN_LEVEL>FORMLOG_LEVEL_DEBUG) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"FORM").debug
#define FORMLOG_INFO \
    if(FORMLOG_MIN_LEVEL>FORMLOG_LEVEL_INFO) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"FORM").info
#define FORMLOG_WARNING \
    if(FORMLOG_MIN_LEVEL>FORMLOG_LEVEL_WARNING) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"FORM").warning
#define FORMLOG_CRITICAL \
    if(FORMLOG_MIN_LEVEL>FORMLOG_LEVEL_CRITICAL) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"FORM").critical
#define FORMLOG_FATAL \
    QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"FORM").fatal

#endif // FORM_LOG_H
