# EasyLog4C
Easy for get log by level setting

you can fond the logrc.ini file in the release folder, change the setting value you can get whatever log you want.

the following is the copy of th logrc.ini
[outputsetting]

; 1 is create a constant log name
; 2 is create a log file name once day
; 3 is create a log file name once second 
log_output_type=2 

; Prefix log file name
prefix_file_name="PURE.NET"

start_priority=1000
; setting ouput log level flag
enable_priority=11111111111

; 1 is output the log from some level
; 2 is output the specified level
select_priority_type=2


;level log
  ;/** fatal */	LOG_PRIORITY_FATAL	= 000, 
    ;/** alert */	LOG_PRIORITY_ALERT	= 100, 
   ; /** crit */	    LOG_PRIORITY_CRIT	= 200, 
    ;/** error */	LOG_PRIORITY_ERROR	= 300, 
    ;/** warn */	    LOG_PRIORITY_WARN	= 400, 
    ;/** notice */	LOG_PRIORITY_NOTICE	= 500, 
    ;/** info */	    LOG_PRIORITY_INFO	= 600, 
    ;/** debug */	LOG_PRIORITY_DEBUG	= 700,
    ;/** trace */	LOG_PRIORITY_TRACE	= 800,
    ;/** notset */	LOG_PRIORITY_NOTSET	= 900,
    ;/** unknown */	LOG_PRIORITY_UNKNOWN	= 1000
  

Also, you can use the MACRO which let you coding easy for output the log.
#define SSSKINKODAKE_MODEULE "StreamSocket"
#define INFINI_TRACE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, msg)
#define INFINI_ERROR_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_ERROR, "[%s()⇒%s:%d]【%s】",   __FILE__, __FUNC__, __LINE__, msg)
#define INFINI_NOTICE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_NOTICE,"[%s()⇒%s:%d]【%s】", __FILE__, __FUNCTION__, __LINE__, msg)
#define INFINI_TRACE_LOG_DETAIL(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, "[%s()⇒%s:%d]【%s】", __FILE__, __FUNC__, __LINE__, msg)
