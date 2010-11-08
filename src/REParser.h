/*
 * REParser.h
 *
 *  Created on: Nov 5, 2010
 *      Author: salomon
 */

#ifndef REPARSER_H_
#define REPARSER_H_

#include "INode.h"
#include "Final.h"

class REParser
{
public:
	INode * parseRegExp(std::string::const_iterator c, const std::string::const_iterator & end);
	INode * operator () (std::string::const_iterator c, const std::string::const_iterator & end) { return parseRegExp(c, end); }

private:
	INode * getSequence(char start, char end);
	char getAChar(std::string::const_iterator & c, const std::string::const_iterator & end);
	INode * getAFinal(std::string::const_iterator & c, const std::string::const_iterator & end);
	void	addClassToSequence(std::string::const_iterator & c, const std::string::const_iterator & end, FinalSequence *s);
	INode * parseCharSequence(std::string::const_iterator & c, const std::string::const_iterator & end);
	INode * execRepeat(int min, int max, INode* ret, int n = 1);
	INode * parseRepeat(std::string::const_iterator & c, const std::string::const_iterator & end, INode* ret);
	INode * parseNodeUnit(std::string::const_iterator & c, const std::string::const_iterator & end);
	INode * parseNodeAnd(std::string::const_iterator & c, const std::string::const_iterator & end, bool isInBracket);
	INode * parseNodeOr(std::string::const_iterator & c, const std::string::const_iterator & end, bool isInBracket);
};

#endif /* REPARSER_H_ */
