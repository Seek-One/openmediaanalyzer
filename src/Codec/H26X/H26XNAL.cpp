//
// Created by ebeuque on 27/08/25.
//

#include "H26XNAL.h"

H26XNAL::H26XNAL(H26XNALHeader* pNALHeader)
{
	m_pNALHeader = pNALHeader;
	completelyParsed = true;
}

void H26XNAL::dump(H26XDumpObject& dumpObject) const
{

}

void H26XNAL::validate()
{

}