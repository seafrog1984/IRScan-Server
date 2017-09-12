#include <stdafx.h>
#include <stdio.h>
#include <sstream>
#include <iterator>
#include <iomanip>
#include "string_utils.h"


int hexcharToInt(char c)
{ 
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}


void hexstringToBytes(const char* hexstring, int hexlength, char* bytes)
{         
    for (int i=0 ; i<hexlength; i+=2) {
        bytes[i/2] = (char) ((hexcharToInt(hexstring[i]) << 4)
                   | hexcharToInt(hexstring[i+1]));
    }
}


void bytesToHexstring(const char* bytes, int bytelength, char *hexstring, int hexstrlength)
{
    char str2[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    for (int i=0,j=0; i<bytelength && j<hexstrlength; i++,j++) 
    {
        //printf("%c\n", bytes[i]);
        int b;
        b = 0x0f&(bytes[i]>>4);
        char s1 = str2[b];
        hexstring[j] = s1;    
        b = 0x0f & bytes[i];
        char s2 = str2[b];
        j++;
        hexstring[j] = s2;    
    }
}


std::string bytesToHexstring(const char* bytes, int bytelength)
{
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');

	for (int i=0; i<bytelength; ++i) 
    {
		oss << std::setw(2) << int(bytes[i]);
	}
	return oss.str();
}


int split_vec(const char *psALine, std::vector<std::string> &vecItem, const char delimiter)
{
	char sInputLine[10240] = {0};
	strncpy_s(sInputLine, psALine, sizeof(sInputLine) - 1);

	char *pEnd = NULL;
	char *pHead = sInputLine;
	while ((pEnd = strchr(pHead, delimiter) ) != NULL)
	{
		*pEnd = '\0';
		vecItem.push_back(pHead);
		*pEnd = delimiter;
		pHead = pEnd + 1;
	}
	// 处理最后一个
	//if (pHead != NULL && strlen(pHead) > 0)
	if (pHead != NULL)
	{
		vecItem.push_back(pHead);
	}

	return vecItem.size();
}


std::string vec_join(const std::vector<std::string>& from, const char delimiter)
{
	//判断是否为空
	if (0 == from.size())
		return "";

	std::ostringstream result;
	//第一个
	result << from[0];
	//其他
	for (int i = 1; i < from.size(); ++i)
	{
		result << delimiter << from[i];
	}
	return result.str();
}

int split_map(const char *pszALine, std::map<std::string, std::string> &mapItem, char cSeparatorKey, char cSeparatorValue)
{
	char szInputLine[10240] = {0};
	strncpy_s(szInputLine, pszALine, sizeof(szInputLine) - 1);

	char *pEnd = NULL,*pMid = NULL;
	char *pHead = szInputLine;
	while ((pEnd = strchr(pHead, cSeparatorValue) ) != NULL)
	{
		*pEnd = '\0';
		if((pMid = strchr(pHead, cSeparatorKey) )!= NULL)
		{
			*pMid='\0';
			mapItem[pHead] = pMid+1;
			*pMid = cSeparatorKey;
		}
		*pEnd = cSeparatorValue;
		pHead = pEnd + 1;
	}

	//处理最后一个
	if (pHead != NULL && strlen(pHead) > 0)
	{
		if((pMid = strchr(pHead, cSeparatorKey) )!= NULL)
		{
			*pMid='\0';
			mapItem[pHead] = pMid+1;
			*pMid = cSeparatorKey;
		}
	}
	return mapItem.size();
}


std::string map_join(std::map<std::string, std::string> &from, const char delimiter1, const char delimiter2)
{
	// 判断是否为空
	if (0 == from.size())
		return "";

	std::ostringstream result;
	//第一个
	std::map<std::string, std::string>::iterator it = from.begin();
	result << it->first << delimiter2 << it->second;
	//其他
	for (++it; it != from.end(); ++it)
	{
		result << delimiter1 << it->first << delimiter2 << it->second;
	}
	return result.str();
}
