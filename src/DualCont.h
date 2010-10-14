/*
 * Final.h
 *
 *  Created on: 13 oct. 2010
 *      Author: So
 */

#ifndef DUAL_CONT_H_
#define DUAL_CONT_H_

#include "INode.h"

#include <iomanip>

class DualCont : public INode
{
public:
	virtual ~DualCont() { delete _r1; delete _r2; }

protected:
	DualCont(INode * r1, INode * r2) : _r1(r1), _r2(r2) {}
	DualCont(const DualCont &l) : _r1(l._r1->clone()), _r2(l._r2->clone()) {}

	INode * _r1;
	INode * _r2;
};

class And : public DualCont
{
public:
	And(INode * r1, INode * r2) : DualCont(r1, r2) {}
	And(const And &l) : DualCont(l) {}
	virtual ~And() {}
	virtual INode * clone() { return new And(*this); }
	virtual IState * stateify(IState * start, IState * success, bool replaceFinal)
	{
		IState * middle = _r1->stateify(start, 0, replaceFinal);
		success = _r2->stateify(middle, success, replaceFinal);
		return success;
	};

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		_r1->disp(os, nSpace + 2);
		os	<< std::setw(nSpace) << std::setfill(' ') << "" << "AND" << std::endl;
		_r2->disp(os, nSpace + 2);
	}
};

class Or : public DualCont
{
public:
	Or(INode * r1, INode * r2) : DualCont(r1, r2) {}
	Or(const Or &l) : DualCont(l) {}
	virtual ~Or() {}
	virtual INode * clone() { return new Or(*this); };
	virtual IState * stateify(IState *start, IState * success, bool replaceFinal)
	{
		IState * first = _r1->stateify(start, success, replaceFinal);
		IState * second = _r2->stateify(start, success, replaceFinal);
		return new StateReplicator(first, second);
	};

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		_r1->disp(os, nSpace + 2);
		os	<< std::setw(nSpace) << std::setfill(' ') << "" << "OR" << std::endl;
		_r2->disp(os, nSpace + 2);
	}
};

#endif /* DUAL_CONT_H_ */
