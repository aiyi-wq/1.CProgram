/*************************************************************************
	> File Name: m_popen.h
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 02:48:38 PM CST
 ************************************************************************/

#ifndef _M_POPEN_H
#define _M_POPEN_H

FILE *m_popen(const char *cmd, const char *type);
int m_pclose(FILE *fp);

#endif
