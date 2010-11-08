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
	virtual ~INode() {}
	virtual INode * clone(void) = 0;

	virtual void disp(std::ostream & os, unsigned int nSpace) const = 0;

	virtual IState * stateify(IState * start, IState * success, bool replaceFinal, StateHelper & helper) = 0;

protected:

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
