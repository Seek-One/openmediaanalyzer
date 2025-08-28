//
// Created by ebeuque on 27/08/25.
//

#include "H26XNALUnit.h"

H26XNALUnit::H26XNALUnit(H26XNALHeader* pNALHeader)
{
	m_pNALHeader = pNALHeader;
	completelyParsed = true;
}

void H26XNALUnit::dump(H26XDumpObject& dumpObject) const
{

}

void H26XNALUnit::validate()
{

}