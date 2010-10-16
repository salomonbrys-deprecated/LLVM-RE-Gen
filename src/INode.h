/*
 * INode.h
 *
 *  Created on: 13 oct. 2010
 *      Author: So
 */

#ifndef INODE_H_
#define INODE_H_

#include "State.h"

#include <iostream>

class INode
{
public:
	virtual ~INode() {}
	virtual void disp(std::ostream & os, unsigned int nSpace) const = 0;
	virtual INode * clone(void) = 0;
	virtual IState * stateify(IState * start, IState * success, bool replaceFinal, StateHelper & helper) = 0;

private:
	INode & operator = (const INode &);
};

inline std::ostream & operator << (std::ostream & os, const INode & node)
{
	node.disp(os, 0);
	return os;
}

INode * parseRegExp(std::string::const_iterator c, std::string::const_iterator end);

#endif /* INODE_H_ */
