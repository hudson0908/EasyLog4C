// Log4Infi.h : Log4Infi.DLL のメイン ヘッダー ファイル
//


/* $Id$
 *
 * Log4Infi.h
 *
 * Copyright 2010-2011, hudson.hu All rights reserved.
 * ログ出力専用ライブラリのヘッダファイル
 */

#ifndef __LOG_4_INFI_H__
#define __LOG_4_INFI_H__

 typedef enum {
    /** fatal */	LOG_PRIORITY_FATAL	= 000, 
    /** alert */	LOG_PRIORITY_ALERT	= 100, 
    /** crit */	    LOG_PRIORITY_CRIT	= 200, 
    /** error */	LOG_PRIORITY_ERROR	= 300, 
    /** warn */	    LOG_PRIORITY_WARN	= 400, 
    /** notice */	LOG_PRIORITY_NOTICE	= 500, 
    /** info */	    LOG_PRIORITY_INFO	= 600, 
    /** debug */	LOG_PRIORITY_DEBUG	= 700,
    /** trace */	LOG_PRIORITY_TRACE	= 800,
    /** notset */	LOG_PRIORITY_NOTSET	= 900,
    /** unknown */	LOG_PRIORITY_UNKNOWN	= 1000
} infini_priority_level;

/*
使い易い為に、ALOCKYで関数をマクロ化して、使っています。
各プロジェクトによって使いやすいように行きましょう。
*/

#define SSSKINKODAKE_MODEULE "StreamSocket"
#define INFINI_TRACE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, msg)
#define INFINI_ERROR_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_ERROR, "[%s()⇒%s:%d]【%s】", __FILE__, __FUNC__, __LINE__, msg)
#define INFINI_NOTICE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_NOTICE,"[%s()⇒%s:%d]【%s】", __FILE__, __FUNCTION__, __LINE__, msg)
#define INFINI_TRACE_LOG_DETAIL(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, "[%s()⇒%s:%d]【%s】", __FILE__, __FUNC__, __LINE__, msg)



#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Routine Description:
  ログ出力関数、
 基本的に出力出力内容は下記の通りです。
  [ログの種類][PC名：ログオンユーザ名]{[日付][レベル][フィル名:行数][ログデータ]｝
 
  [ログの種類][PC名：ログオンユーザ名]{[日付]}はデフォルトとして出力

Arguments:
   char *	a_category ログの種類
   int		a_priority ログのレベル
   char*	a_format出力データのフォーマット
   ...		出力データ
Return Value:

   None.

*******************************************************************************/
extern void __stdcall infini_log_output(char* a_category, int a_priority,  const char* a_format, ...);

/*******************************************************************************
Routine Description:
ログ出力前に設定したパラメータを読み込んで、
a
Arguments:

   Code - 
Return Value:

   None.
*******************************************************************************/
extern void  __stdcall infini_log_init(void);

#ifdef __cplusplus
}
#endif

#endif __LOG_4_INFI_H__
