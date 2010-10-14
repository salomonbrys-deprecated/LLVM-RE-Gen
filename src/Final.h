/*
 * Final.h
 *
 *  Created on: 13 oct. 2010
 *      Author: So
 */

#ifndef FINAL_H_
#define FINAL_H_

#include "INode.h"

#include <iomanip>
#include <vector>

class Final : public INode
{
public:
	virtual ~Final() {}

	virtual void addTransitions(IState * start, IState * success) = 0;

	virtual IState * stateify(IState *start, IState * success, bool replaceFinal)
	{
		if (!success)
		{
			success = new State;
			if (replaceFinal)
				start->Final(false);
			else
				success->Final(false);
		}
		addTransitions(start, success);
		return success;
	};

protected:
	Final() {}
};

class FinalChar : public Final
{
public:
	FinalChar(char c) : _c(c) {}
	FinalChar(const FinalChar &l) : _c(l._c) {}
	virtual ~FinalChar() {}
	virtual INode * clone() { return new FinalChar(*this); };
	virtual void addTransitions(IState * start, IState * success)
	{
		start->addTransition(_c, success);
	};

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << '\'' << _c << '\'' << std::endl;
	}
private:
	char _c;
};

class FinalAny : public Final
{
public:
	FinalAny() {}
	FinalAny(const FinalAny &) {}
	virtual ~FinalAny() {}
	virtual INode * clone() { return new FinalAny(*this); };
	virtual void addTransitions(IState * start, IState * success)
	{
		start->addTransition(-1, success);
	}

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << "ANY" << std::endl;
	}
};

class FinalSequence : public Final
{
public:
	virtual ~FinalSequence() {}

protected:
	FinalSequence() {}
	FinalSequence(const FinalSequence &l) : _s(l._s) {}

public:
	FinalSequence *	addChar(char c) { _s.push_back(c); return this; }
	FinalSequence *	addRange(char cs, char ce) { for (; cs <= ce; ++cs) _s.push_back(cs); return this; }

protected:
	std::vector<char> _s;
};

class FinalOrSequence : public FinalSequence
{
public:
	FinalOrSequence() {}
	FinalOrSequence(const FinalOrSequence &l) : FinalSequence(l) {}
	virtual ~FinalOrSequence() {}
	virtual INode * clone() { return new FinalOrSequence(*this); };
	virtual void addTransitions(IState * start, IState * success)
	{
		for (std::vector<char>::const_iterator i = _s.begin(); i != _s.end(); ++i)
			start->addTransition(*i, success);
	}

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << '[';
		for (std::vector<char>::const_iterator i = _s.begin(); i != _s.end(); ++i)
			os << *i;
		os << ']' << std::endl;
	}
};

class FinalNotSequence : public FinalSequence
{
public:
	FinalNotSequence() {}
	FinalNotSequence(const FinalNotSequence &l) : FinalSequence(l) {}
	virtual ~FinalNotSequence() {}
	virtual INode * clone() { return new FinalNotSequence(*this); };
	virtual void addTransitions(IState * start, IState * success)
	{
		start->addTransition(-1, success);
		State * fail = new State;
		fail->Success(false);
		for (std::vector<char>::const_iterator i = _s.begin(); i != _s.end(); ++i)
			start->addTransition(*i, fail);
	}

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << "NOT [";
		for (std::vector<char>::const_iterator i = _s.begin(); i != _s.end(); ++i)
			os << *i;
		os << ']' << std::endl;
	}
};

#endif /* FINAL_H_ */
