#ifndef STRING_UTILS__H__
#define STRING_UTILS__H__
#pragma once


#include <string>
#include <vector>
#include <map>


void bytesToHexstring(const char* bytes, int bytelength, char *hexstring, int hexstrlength);

std::string bytesToHexstring(const char* bytes, int bytelength);

void hexstringToBytes(const char* hexstring, int hexlength, char* bytes);

int split_vec(const char *psALine, std::vector<std::string> &vecItem, const char delimiter);

std::string vec_join(const std::vector<std::string>& from, const char delimiter);

int split_map(const char *pszALine, std::map<std::string, std::string> &mapItem, char cSeparatorKey, char cSeparatorValue);

std::string map_join(std::map<std::string, std::string> &from, const char delimiter1, const char delimiter2);


#endif