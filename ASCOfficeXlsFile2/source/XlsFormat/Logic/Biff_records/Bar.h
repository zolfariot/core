#pragma once

#include "BiffRecord.h"

namespace XLS
{


// Logical representation of Bar record in BIFF8
class Bar: public BiffRecord
{
	BIFF_RECORD_DEFINE_TYPE_INFO(Bar)
	BASE_OBJECT_DEFINE_CLASS_NAME(Bar)
public:
	Bar();
	~Bar();

	BaseObjectPtr clone();

	void writeFields(CFRecord& record);
	void readFields(CFRecord& record);

	static const ElementType	type = typeBar;

	int		serialize(std::wostream & _stream);

	BIFF_SHORT			pcOverlap;
	BIFF_WORD			pcGap;

	BIFF_BOOL			fTranspose;
	BIFF_BOOL			fStacked;
	BIFF_BOOL			f100;
	BIFF_BOOL			fHasShadow;
//------------------------------------
	BaseObjectPtr		m_chart3D;
};

} // namespace XLS

