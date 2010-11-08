/*
 * INode.h
 *
 *  Created on: 13 oct. 2010
 *      Author: So
 */

#ifndef INODE_H_
#define INODE_H_

#include "State.h"

#include <ostream>
#include <iomanip>
#include <set>

class INode
{
public:
	typedef std::set<unsigned int> NeededGroups;

	INode(NeededGroups * & ng) : _group(0), _ng(ng) {}
	virtual ~INode() { if (_ng) delete _ng; _ng = 0; }
	virtual INode * clone(void) = 0;

	void disp(std::ostream & os, unsigned int nSpace) const
	{
		if (_group != 0 && _ng->count(_group))
			os << std::setw(nSpace) << std::setfill(' ') << "" << _group << '(' << std::endl;
		this->dispContent(os, nSpace);
		if (_group != 0 && _ng->count(_group))
			os << std::setw(nSpace) << std::setfill(' ') << "" << ')' << _group << std::endl;
	}

	IState * stateify(IState * start, IState * success, bool replaceFinal, StateHelper & helper)
	{
//		if (_group != 0 && _ng->count(_group))
//		{
//			State * tmpStart = new State;
//			State * tmpSuccess = new State;
//			tmpStart->addTransition(-2, start);
//			if (success)
//				tmpSuccess->addTransition(-2, success);
//			start = tmp;
//		}
		return this->mkState(start, success, replaceFinal, helper);
	}

	void setGroup(unsigned int g) { _group = g; }

	void addNeededGroup(unsigned int g) { _ng->insert(g); }
	const NeededGroups & getNeededGroups() { return *_ng; }

protected:
	virtual void dispContent(std::ostream & os, unsigned int nSpace) const = 0;
	virtual IState * mkState(IState * start, IState * success, bool replaceFinal, StateHelper & helper) = 0;

	unsigned int _group;
	NeededGroups * & _ng;

	friend class SingleCont;
	friend class DualCont;

private:
	INode & operator = (const INode &);
};

inline std::ostream & operator << (std::ostream & os, const INode & node)
{
	node.disp(os, 0);
	return os;
}

#endif /* INODE_H_ */
