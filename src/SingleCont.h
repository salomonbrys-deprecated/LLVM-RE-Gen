/*
 * Final.h
 *
 *  Created on: 13 oct. 2010
 *      Author: So
 */

#ifndef SINGLE_CONT_H_
#define SINGLE_CONT_H_

#include "INode.h"

#include <iomanip>

class SingleCont : public INode
{
public:
	virtual ~SingleCont() { delete _r; }

protected:
	SingleCont(INode * r) : _r(r) {}
	SingleCont(const SingleCont &l) : _r(l._r->clone()) {}

	INode * _r;
};

class Repeat : public SingleCont
{
public:
	Repeat(INode * r) : SingleCont(r) {}
	Repeat(const Repeat &l) : SingleCont(l) {}
	virtual ~Repeat() {}
	virtual INode * clone() { return new Repeat(*this); };
	virtual IState * stateify(IState *start, IState *, bool, StateHelper & helper)
	{
		_r->stateify(start, start, false, helper);
		return start;
	};

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os	<< std::setw(nSpace) << std::setfill(' ') << "" << "Repeat:" << std::endl;
		_r->disp(os, nSpace + 2);

	}
};

class Optional : public SingleCont
{
public:
	Optional(INode * r) : SingleCont(r) {}
	Optional(const Optional &l) : SingleCont(l) {}
	virtual ~Optional() {}
	virtual INode * clone() { return new Optional(*this); };
	virtual IState * stateify(IState *start, IState *, bool replaceFinal, StateHelper & helper)
	{
		IState * success = new State(helper);
		success->Final(start->Final());
		_r->stateify(start, success, false, helper);
		return new StateReplicator(start, success);
	};

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os	<< std::setw(nSpace) << std::setfill(' ') << "" << "Optional:" << std::endl;
		_r->disp(os, nSpace + 2);

	}
};

#endif /* SINGLE_CONT_H_ */
