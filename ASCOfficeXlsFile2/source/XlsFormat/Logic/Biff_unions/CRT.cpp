
#include "CRT.h"
#include <Logic/Biff_records/ChartFormat.h>
#include <Logic/Biff_records/Begin.h>
#include <Logic/Biff_records/Bar.h>
#include <Logic/Biff_records/Line.h>
#include <Logic/Biff_records/BopPop.h>
#include <Logic/Biff_records/BopPopCustom.h>
#include <Logic/Biff_records/Pie.h>
#include <Logic/Biff_records/Area.h>
#include <Logic/Biff_records/Scatter.h>
#include <Logic/Biff_records/Radar.h>
#include <Logic/Biff_records/RadarArea.h>
#include <Logic/Biff_records/Surf.h>
#include <Logic/Biff_records/CrtLink.h>
#include <Logic/Biff_records/SeriesList.h>
#include <Logic/Biff_records/Chart3d.h>
#include <Logic/Biff_unions/LD.h>
#include <Logic/Biff_unions/DROPBAR.h>
#include <Logic/Biff_records/CrtLine.h>
#include <Logic/Biff_records/LineFormat.h>
#include <Logic/Biff_unions/DFTTEXT.h>
#include <Logic/Biff_records/DataLabExtContents.h>
#include <Logic/Biff_unions/SS.h>
#include <Logic/Biff_unions/SHAPEPROPS.h>
#include <Logic/Biff_records/End.h>
#include <Logic/Biff_records/ValueRange.h>
#include <Logic/Biff_records/Tick.h>
#include <Logic/Biff_records/FontX.h>
#include <Logic/Biff_records/AxisLine.h>
#include <Logic/Biff_records/LineFormat.h>
#include <Logic/Biff_records/ShapePropsStream.h>
#include <Logic/Biff_records/TextPropsStream.h>
#include <Logic/Biff_records/PlotArea.h>
#include <Logic/Biff_records/DropBar.h>
#include <Logic/Biff_records/CrtMlFrt.h>


namespace XLS
{


CRT::CRT()
{
	m_indAXISPARENT = 0; 
}


CRT::~CRT()
{
}

//  (BopPop [BopPopCustom])
class Parenthesis_CRT_3: public ABNFParenthesis
{
	BASE_OBJECT_DEFINE_CLASS_NAME(Parenthesis_CRT_3)
public:
	BaseObjectPtr clone()
	{
		return BaseObjectPtr(new Parenthesis_CRT_3(*this));
	}

	const bool loadContent(BinProcessor& proc)
	{
		if(!proc.mandatory<BopPop>())
		{
			return false;
		}
		proc.optional<BopPopCustom>();
		return true;
	};
};



//  (Bar / Line / (BopPop [BopPopCustom]) / Pie / Area / Scatter / Radar / RadarArea / Surf)
class Parenthesis_CRT_1: public ABNFParenthesis
{
	BASE_OBJECT_DEFINE_CLASS_NAME(Parenthesis_CRT_1)
public:
	BaseObjectPtr clone()
	{
		return BaseObjectPtr(new Parenthesis_CRT_1(*this));
	}

	const bool loadContent(BinProcessor& proc)
	{
		return	proc.optional<Bar>() ||
				proc.optional<Line>() ||				
				proc.optional<Parenthesis_CRT_3>() ||
				proc.optional<Pie>() ||
				proc.optional<Area>() ||
				proc.optional<Scatter>() ||
				proc.optional<Radar>() ||
				proc.optional<RadarArea>() ||
				proc.optional<Surf>();
	};
};



//  (CrtLine LineFormat)
class Parenthesis_CRT_2: public ABNFParenthesis
{
	BASE_OBJECT_DEFINE_CLASS_NAME(Parenthesis_CRT_2)
public:
	BaseObjectPtr clone()
	{
		return BaseObjectPtr(new Parenthesis_CRT_2(*this));
	}

	const bool loadContent(BinProcessor& proc)
	{
		if(!proc.mandatory<CrtLine>())
		{
			return false;
		}
		proc.mandatory<LineFormat>();
		return true;
	};
};



BaseObjectPtr CRT::clone()
{
	return BaseObjectPtr(new CRT(*this));
}


/*
CRT = ChartFormat Begin 
	(Bar / Line / (BopPop [BopPopCustom]) / Pie / Area / Scatter / Radar / RadarArea / Surf)
	CrtLink [SeriesList] [Chart3d] [LD] [2DROPBAR] *4(CrtLine LineFormat) *2DFTTEXT 
	[DataLabExtContents] [SS] *4SHAPEPROPS End
*/
const bool CRT::loadContent(BinProcessor& proc)
{
	if(!proc.mandatory<ChartFormat>())
	{
		return false;
	}	

	m_ChartFormat = elements_.back();
	elements_.pop_back(); 

	proc.mandatory<Begin>();				elements_.pop_back();
	proc.mandatory<Parenthesis_CRT_1>();

	m_ChartType = elements_.front();
	elements_.pop_front();

	if (elements_.size() > 0 && "BopPop" == m_ChartType->getClassName())
	{
		BopPop *bp = dynamic_cast<BopPop*>(m_ChartType.get());
		bp->m_Custom = elements_.front();
		elements_.pop_front();
	}
	
	if (proc.optional<CrtLink>())
	{
		m_CrtLink = elements_.back();
		elements_.pop_back();
	}

	if (proc.optional<SeriesList>())
	{
		m_SeriesList = elements_.back();
		elements_.pop_back();
	}
	if (proc.optional<Chart3d>())
	{
		m_Chart3d = elements_.back();
		elements_.pop_back();
	}
	if (proc.optional<LD>())
	{
		m_LD = elements_.back();
		elements_.pop_back();
	}
	
	if(proc.optional<DROPBAR>())
	{
		m_DROPBAR[0] = elements_.back();
		elements_.pop_back();

		proc.mandatory<DROPBAR>();
		m_DROPBAR[1] = elements_.back();
		elements_.pop_back();
	}
	
	int count = proc.repeated<Parenthesis_CRT_2>(0, 4);
	while(count > 0)
	{
		if (typeCrtLine == elements_.front()->get_type())
		{	
			m_arCrtLine.push_back(elements_.front());
			elements_.pop_front(); 
		}
		if (typeLineFormat == elements_.front()->get_type())
		{
			CrtLine * crt_line = dynamic_cast<CrtLine *>(m_arCrtLine.back().get());
			crt_line->m_LineFormat = elements_.front();
		} 
		elements_.pop_front(); count--;
	}

	count = proc.repeated<DFTTEXT>(0, 2);
	while(count > 0)
	{
		m_arDFTTEXT.push_back(elements_.front());
		elements_.pop_front(); count--;
	}

	if (proc.optional<DataLabExtContents>())
	{
		m_DataLabExtContents = elements_.back();
		elements_.pop_back();
	}
	if (proc.optional<SS>())
	{
		m_SS = elements_.back();
		elements_.pop_back();
	}
	count = proc.repeated<SHAPEPROPS>(0, 4);
	while(count > 0)
	{
		m_arSHAPEPROPS.push_back(elements_.front());
		elements_.pop_front(); count--;
	}
	if (proc.optional<CrtMlFrt>()) // 
	{
		elements_.pop_back();
	}
	
	proc.mandatory<End>();					elements_.pop_back();
//-------------------------------------------------------------------------------------
	m_bIs3D = false;
	if (m_Chart3d)
	{
		m_bIs3D = true;
	}

	m_iChartType = 1;
	switch(m_ChartType->get_type())
	{
		case typeBar:		m_iChartType = 1; break;
		case typeLine:		m_iChartType = 2; break;
		case typePie:		m_iChartType = 3; break;
		case typeArea:		m_iChartType = 4; break;
		case typeSurf:		m_iChartType = 5; break;		
		case typeRadar:		m_iChartType = 6; break;
		case typeRadarArea:	m_iChartType = 7; break;
		case typeBopPop:	m_iChartType = 8; break;		
		case typeScatter:	m_iChartType = 9; break;
	}
	
	return true;
}

std::wstring CRT::getOoxChartType()
{
	switch(m_ChartType->get_type())
	{
	case typeBar:		return (m_bIs3D ? L"c:bar3DChart"		: L"c:barChart");
	case typeLine:
	{
		if (m_bIs3D) return  L"c:line3DChart";

		DROPBAR* Dr1 = dynamic_cast<DROPBAR*>(m_DROPBAR[0].get());
		DROPBAR* Dr2 = dynamic_cast<DROPBAR*>(m_DROPBAR[1].get());
		if (Dr1 || Dr2)
		{
			DropBar  *dropBar = NULL;
			if		(Dr1) dropBar = dynamic_cast<DropBar*>(Dr1->m_DropBar.get()); 
			else if (Dr2) dropBar = dynamic_cast<DropBar*>(Dr2->m_DropBar.get());

			if ((dropBar) && (dropBar->pcGap != (short)1)) 
			{
				m_iChartType = 11;
				return L"c:stockChart";
			}
		}
		return L"c:lineChart";
	}
	case typePie:	
	{
		Pie * p = dynamic_cast<Pie *>(m_ChartType.get());
		if (p->pcDonut.value() && p->pcDonut > 0)
		{
			m_iChartType = 12;
						return L"c:doughnutChart";
		}
		else 			return (m_bIs3D ? L"c:pie3DChart"		: L"c:pieChart");
	}break;
	case typeArea:		return (m_bIs3D ? L"c:area3DChart"		: L"c:areaChart");
	case typeSurf:		
	{
		ChartFormat *format	= dynamic_cast<ChartFormat*>(m_ChartFormat.get());
		format->fVaried = true;
		
		Surf* surf = dynamic_cast<Surf *>(m_ChartType.get());
		return ((m_bIs3D && surf->f3DPhongShade == true) ? L"c:surface3DChart"	: L"c:surfaceChart");
	}break;
	case typeScatter:
	{
		Scatter * sc = dynamic_cast<Scatter *>(m_ChartType.get());
		if ((sc->fBubbles.value()) && (sc->fBubbles))
		{
			m_iChartType = 10;
			return L"c:bubbleChart";
		}
		else											return L"c:scatterChart";
	}break;
	case typeRadar:		return L"c:radarChart";
	case typeRadarArea:	return L"c:radarChart";
	case typeBopPop:	return L"c:ofPieChart";		
	}
	return L"c:barChart";

}
} // namespace XLS

