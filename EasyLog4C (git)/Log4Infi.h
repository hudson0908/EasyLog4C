// Log4Infi.h : Log4Infi.DLL �̃��C�� �w�b�_�[ �t�@�C��
//


/* $Id$
 *
 * Log4Infi.h
 *
 * Copyright 2010-2011, hudson.hu All rights reserved.
 * ���O�o�͐�p���C�u�����̃w�b�_�t�@�C��
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
�g���Ղ��ׂɁAALOCKY�Ŋ֐����}�N�������āA�g���Ă��܂��B
�e�v���W�F�N�g�ɂ���Ďg���₷���悤�ɍs���܂��傤�B
*/

#define SSSKINKODAKE_MODEULE "StreamSocket"
#define INFINI_TRACE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, msg)
#define INFINI_ERROR_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_ERROR, "[%s()��%s:%d]�y%s�z", __FILE__, __FUNC__, __LINE__, msg)
#define INFINI_NOTICE_LOG(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_NOTICE,"[%s()��%s:%d]�y%s�z", __FILE__, __FUNCTION__, __LINE__, msg)
#define INFINI_TRACE_LOG_DETAIL(msg) infini_log_output(SSSKINKODAKE_MODEULE, LOG_PRIORITY_TRACE, "[%s()��%s:%d]�y%s�z", __FILE__, __FUNC__, __LINE__, msg)



#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
Routine Description:
  ���O�o�͊֐��A
 ��{�I�ɏo�͏o�͓��e�͉��L�̒ʂ�ł��B
  [���O�̎��][PC���F���O�I�����[�U��]{[���t][���x��][�t�B����:�s��][���O�f�[�^]�p
 
  [���O�̎��][PC���F���O�I�����[�U��]{[���t]}�̓f�t�H���g�Ƃ��ďo��

Arguments:
   char *	a_category ���O�̎��
   int		a_priority ���O�̃��x��
   char*	a_format�o�̓f�[�^�̃t�H�[�}�b�g
   ...		�o�̓f�[�^
Return Value:

   None.

*******************************************************************************/
extern void __stdcall infini_log_output(char* a_category, int a_priority,  const char* a_format, ...);

/*******************************************************************************
Routine Description:
���O�o�͑O�ɐݒ肵���p�����[�^��ǂݍ���ŁA
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
