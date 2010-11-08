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
	virtual INode * clone() { return new Repeat(*this); }
	virtual IState * stateify(IState *start, IState * /*ignored*/, bool /*ignored*/, StateHelper & helper)
	{
		State * success = new State(helper);
		success->setFinal(start->Final());
		start->addReplicatedFinal(success);

		_r->stateify(start, success, false, helper);
		_r->stateify(success, success, false, helper);
		return new StateReplicator(start, success, helper);
	}

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
	virtual INode * clone() { return new Optional(*this); }
	virtual IState * stateify(IState *start, IState * success, bool replaceFinal, StateHelper & helper)
	{
		if (!success)
		{
			success = new State(helper);
			success->setFinal(start->Final());
			start->addReplicatedFinal(success);
		}
		IState * nSuccess = _r->stateify(start, success, false, helper);
		return new StateReplicator(start, nSuccess, helper);
	}

	virtual void disp(std::ostream & os, unsigned int nSpace) const
	{
		os	<< std::setw(nSpace) << std::setfill(' ') << "" << "Optional:" << std::endl;
		_r->disp(os, nSpace + 2);

	}
};

#endif /* SINGLE_CONT_H_ */
