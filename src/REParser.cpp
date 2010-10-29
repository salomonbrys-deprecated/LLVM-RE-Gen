//============================================================================
// Name        : Compile.cpp
// Author      : Salomon BRYS
// Copyright   : Salomon BRYS, Apache Lisence
//============================================================================

#include "SingleCont.h"
#include "Final.h"
#include "DualCont.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>
#include <cstdlib>

INode * parseNodeOr (std::string::const_iterator & c, const std::string::const_iterator & end, bool isInBracket);

INode * getSequence(char start, char end)
{
	INode * ret = new FinalChar(start);
	if (start != end)
		ret = new Or(ret, getSequence(start + 1, end));
	return ret;
}

char getAChar(std::string::const_iterator & c, const std::string::const_iterator & end)
{
	if (*c == '\\')
	{
		++c;
		if (c == end)
			throw std::string("Incomplete escape character");
		switch (*c)
		{
		case 'a':
			++c;
			return '\a';
		case 'b':
			++c;
			return '\b';
		case 'f':
			++c;
			return '\f';
		case 'n':
			++c;
			return '\n';
		case 'r':
			++c;
			return '\r';
		case 't':
			++c;
			return '\t';
		case 'x':
		{
			++c;
			char hex[3] = {0,};
			if (c == end || !isxdigit(*c))
				throw std::string("Incomplete hexadecimal character");
			hex[0] = *c;
			++c;
			if (c == end || !isxdigit(*c))
				throw std::string("Incomplete hexadecimal character");
			hex[1] = *c;
			++c;
			unsigned int n;
			sscanf(hex, "%x", &n);
			if (!isascii(n))
				throw std::string("Non-ascii hexadecimal character");
			return (char)n;
		}
		default:
			if (isalpha(*c))
				throw std::string("Unknown escape character");
		}
	}

	return *c++;
}

INode * getAFinal(std::string::const_iterator & c, const std::string::const_iterator & end)
{
	if (*c == '.')
	{
		++c;
		return new FinalAny();
	}
	return new FinalChar(*c++);
}

void	addClassToSequence(std::string::const_iterator & c, const std::string::const_iterator & end, FinalSequence *s)
{
	std::string className;
	for (;c != end; ++c)
	{
		if (*c == ':')
		{
			++c;
			break ;
		}
		className += *c;
	}
	if (c == end || *c != ']')
		throw std::string("Incorrect character class");
	++c;

	if		(className == "alnum")
		s->addRange('A', 'Z')->addRange('a', 'z')->addRange('0', '9');
	else if	(className == "word")
		s->addRange('A', 'Z')->addRange('a', 'z')->addRange('0', '9')->addChar('_');
	else if	(className == "alpha")
		s->addRange('A', 'Z')->addRange('a', 'z');
	else if	(className == "blank")
		s->addChar(' ')->addChar('\t');
	else if	(className == "cntrl")
		s->addRange(0x0, '\x1F')->addChar('\x7F');
	else if	(className == "digit")
		s->addRange('0', '9');
	else if	(className == "graph")
		s->addRange(0x21, 0x7E);
	else if	(className == "lower")
		s->addRange('a', 'z');
	else if	(className == "print")
		s->addRange(0x20, 0x7E);
	else if	(className == "punct")
		s->addRange('!', '/')->addRange(':', '@')->addRange('[', '`')->addRange('{', '~');
	else if	(className == "space")
		s->addChar(' ')->addChar('\t')->addChar('\r')->addChar('\n')->addChar('\v')->addChar('\f');
	else if	(className == "upper")
		s->addRange('A', 'Z');
	else if	(className == "xdigit")
		s->addRange('A', 'F')->addRange('a', 'f')->addRange('0', '9');
	else
		throw std::string("Unknown character class");
}

INode * parseCharSequence(std::string::const_iterator & c, const std::string::const_iterator & end)
{
	FinalSequence * ret = 0;
	if (*c == '^')
	{
		++c;
		if (c != end && *c == ']')
			throw std::string("Empty character sequence []");
		ret = new FinalNotSequence;
	}
	else
		ret = new FinalOrSequence;

	while (c != end && *c != ']')
	{
		if (*c == '[')
		{
			++c;
			if (c != end && *c == ':')
			{
				++c;
				addClassToSequence(c, end, ret);
				continue ;
			}
			else
				--c;
		}

		char rStart = getAChar(c, end);

		if (*c == '.')
		{
			++c;
			if (c != end && *c == '.')
			{
				++c;
				if (c == end)
					throw std::string("Incomplete character range");
				char rEnd = getAChar(c, end);
				if (rStart >= rEnd)
					throw std::string("Incorrect character range");
				ret->addRange(rStart, rEnd);
				continue ;
			}
			else
				--c;
		}

		ret->addChar(rStart);
	}

	if (c == end)
		throw std::string("Incomplete character sequence");

	return ret;
}

INode * execRepeat(int min, int max, INode* ret, int n = 1)
{
	if (n == min + 1)
		ret = new Optional(ret);

	if (n < max)
		return new And(ret->clone(), execRepeat(min, max, ret, n+1));
	return ret;
}

INode * parseRepeat(std::string::const_iterator & c, const std::string::const_iterator & end, INode* ret)
{
	std::string smin, smax;

	for (; c != end && isdigit(*c); ++c)
		smin += *c;
	if (c == end || *c != ',')
		throw std::string("Malformed repetitor {min,max} on min");
	++c;

	for (; c != end && isdigit(*c); ++c)
		smax += *c;
	if (c == end || *c != '}')
		throw std::string("Malformed repetitor {min,max} on max");

	int min = atoi(smin.c_str());
	int max = atoi(smax.c_str());

	if (min > max || min < 0)
		throw std::string("Error on repetitor {min,max}: bad numbers");

	return execRepeat(min, max, ret);
}
INode * parseNodeUnit(std::string::const_iterator & c, const std::string::const_iterator & end)
{
	INode * ret = 0;

	switch (*c)
	{
	case '(':
		++c;
		ret = parseNodeOr(c, end, true);
		if (ret == 0)
			throw std::string("empty brackets");
		if (c == end || *c != ')')
			throw std::string("Incomplete group, no matching ')'");
		++c;
		break;
	case '[':
		++c;
		if (c == end)
			throw std::string("Incomplete character sequence, no matching ']'");
		if (*c == ']')
			throw std::string("Empty character sequence []");
		ret = parseCharSequence(c, end);
		++c;
		break;
	default:
		ret = getAFinal(c, end);
	}

	for (; c != end && (*c == '?' || *c == '*' || *c == '+' || *c == '{'); ++c)
		switch (*c)
		{
		case '?':
			ret = new Optional(ret);
			break;
		case '*':
			ret = new Repeat(ret);
			break;
		case '+':
			ret = new And(ret->clone(), new Repeat(ret));
			break;
		case '{':
			++c;
			ret = parseRepeat(c, end, ret);
			break;
		}
	return ret;
}

INode * parseNodeAnd(std::string::const_iterator & c, const std::string::const_iterator & end, bool isInBracket)
{
	INode * ret = parseNodeUnit(c, end);

	if (c != end)
		switch (*c)
		{
		case '|':
			break;
		case ')':
//			if (!isInBracket)
//				throw std::string("Unexpected closing bracket");
			if (isInBracket)
				break;
			// Intentional fallthrough
		default:
			ret = new And(ret, parseNodeAnd(c, end, isInBracket));
		}

	return ret;
}

INode * parseNodeOr(std::string::const_iterator & c, const std::string::const_iterator & end, bool isInBracket)
{
	INode * ret = parseNodeAnd(c, end, isInBracket);
	if (c != end && *c == '|')
	{
		++c;
		if (c == end)
			throw std::string("No left operand of |");
		ret = new Or(ret, parseNodeOr(c, end, isInBracket));
	}
	return ret;
}

INode * parseRegExp(std::string::const_iterator c, const std::string::const_iterator & end)
{
	return parseNodeOr(c, end, false);
}
