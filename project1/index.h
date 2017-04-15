// CS130 Fall 2014: Computer Graphics
// index.h
//
// This file does not need to be modified
/////////////////////////////////////////
#ifndef __INDEX_H__
#define __INDES_H__

struct Index
{
	int first;
	int second;
	int third;

	Index() : first(0), second(0), third(0) {}
	Index(const int & n1 = 0, const int & n2 = 0, const int & n3 = 0) : first(n1), second(n2), third(n3) {}

};

#endif
