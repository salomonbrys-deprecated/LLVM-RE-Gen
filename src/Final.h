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
#include <set>

class Final : public INode
{
public:
	virtual ~Final() {}

	virtual void addTransitions(IState * start, IState * success) = 0;

	virtual IState * mkState(IState *start, IState * success, bool replaceFinal, StateHelper & helper)
	{
		if (!success)
		{
			success = new State(helper);
			if (replaceFinal)
				start->setFinal(false);
			else
				success->setFinal(false);
		}
		addTransitions(start, success);
		return success;
	}

protected:
	Final(INode::NeededGroups * & ng) : INode(ng) {}
};

class FinalChar : public Final
{
public:
	FinalChar(char c, INode::NeededGroups * & ng) : Final(ng), _c(c) {}
	FinalChar(const FinalChar &l) : Final(l._ng), _c(l._c) {}
	virtual ~FinalChar() {}
	virtual INode * clone() { return new FinalChar(*this); }
	virtual void addTransitions(IState * start, IState * success)
	{
		start->addTransition(_c, success);
	}

	virtual void dispContent(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << '\'' << _c << '\'' << std::endl;
	}
private:
	char _c;
};

class FinalAny : public Final
{
public:
	FinalAny(INode::NeededGroups * & ng) : Final(ng) {}
	FinalAny(const FinalAny & l) : Final(l._ng) {}
	virtual ~FinalAny() {}
	virtual INode * clone() { return new FinalAny(*this); }
	virtual void addTransitions(IState * start, IState * success)
	{
		start->addTransition(-1, success);
	}

	virtual void dispContent(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << "ANY" << std::endl;
	}
};

class FinalSequence : public Final
{
public:
	virtual ~FinalSequence() {}

protected:
	FinalSequence(INode::NeededGroups * & ng) : Final(ng) {}
	FinalSequence(const FinalSequence &l) : Final(l._ng), _s(l._s) {}

public:
	FinalSequence *	addChar(char c) { _s.insert(c); return this; }
	FinalSequence *	addRange(char cs, char ce) { for (; cs <= ce; ++cs) _s.insert(cs); return this; }

protected:
	typedef std::set<char> CharSet;
	CharSet _s;
};

class FinalOrSequence : public FinalSequence
{
public:
	FinalOrSequence(INode::NeededGroups * & ng) : FinalSequence(ng) {}
	FinalOrSequence(const FinalOrSequence &l) : FinalSequence(l) {}
	virtual ~FinalOrSequence() {}
	virtual INode * clone() { return new FinalOrSequence(*this); }
	virtual void addTransitions(IState * start, IState * success)
	{
		for (CharSet::const_iterator i = _s.begin(); i != _s.end(); ++i)
			start->addTransition(*i, success);
	}

	virtual void dispContent(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << '[';
		for (CharSet::const_iterator i = _s.begin(); i != _s.end(); ++i)
			os << *i;
		os << ']' << std::endl;
	}
};

class FinalNotSequence : public FinalSequence
{
public:
	FinalNotSequence(INode::NeededGroups * & ng) : FinalSequence(ng) {}
	FinalNotSequence(const FinalNotSequence &l) : FinalSequence(l) {}
	virtual ~FinalNotSequence() {}
	virtual INode * clone() { return new FinalNotSequence(*this); }
	virtual void addTransitions(IState * start, IState * success)
	{
		// NOTE: This behavior is currently the only one that would really
		//		prevent this library to be ported to Unicode. However, this is
		//		by far the easiest way to handle not-sequences.
		for (char i = 0; isascii(i); ++i)
			if (_s.find(i) == _s.end())
				start->addTransition(i, success);
	}

	virtual void dispContent(std::ostream & os, unsigned int nSpace) const
	{
		os << std::setw(nSpace) << std::setfill(' ') << "" << "NOT [";
		for (CharSet::const_iterator i = _s.begin(); i != _s.end(); ++i)
			os << *i;
		os << ']' << std::endl;
	}
};

#endif /* FINAL_H_ */
