#include "StdAfx.h"
#include "SymbolManage.h"
#include "resource.h"
#include <climits>

using namespace symbollink;
using namespace std;

tagSymbol::tagSymbol()
	: nImageIndex(0)
	, nMinCrossing(UINT_MAX)
	, bValid(true)
{
}

CSymbolManage::CSymbolManage(void)
	: m_nSortOfImage(17)
	, m_nColum(8)
	, m_nRow(8)
	, m_nNumOfUnlinkedPair(m_nColum*m_nRow/2)
	, m_nCrossing(-1)
	//, m_symList(m_nColum*m_nRow, SYMBOL())
{	
	m_imageList.Create(IMAGE_RESOLUTION, IMAGE_RESOLUTION, ILC_COLOR24, m_nSortOfImage, 4);
	for (int i = 0; i < m_nSortOfImage; ++i)
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_BITMAP1+i);
		m_imageList.Add(&bmp, RGB(0, 0, 0));
		bmp.DeleteObject();
	}

	//
	srand(GetTickCount());
}

void CSymbolManage::Init(void)
{
	m_nNumOfUnlinkedPair = m_nColum*m_nRow/2;
	vector<int> statis(m_nSortOfImage, 0);
	m_symList.resize((m_nColum+2)*(m_nRow+2));
	int nAccum = 0;
	for (int i = 0; i < m_nRow; ++i)
	{
		for (int j = 0; j < m_nColum; ++j)
		{
			int nIndex = PosToIndex(j, i);
			m_symList[nIndex].bValid      = true;
			m_symList[nIndex].nMinCrossing= UINT_MAX;
			int tmp = rand()%m_nSortOfImage;
			m_symList[nIndex].nImageIndex = tmp;
			++statis[tmp];
		}
	}
	int nDelta = (m_nColum+2)*(m_nRow+1);
	for (int i = 0; i < m_nColum+2; ++i)
	{
		m_symList[i].bValid  = false;
		m_symList[i+nDelta].bValid = false;
	}
	
	nAccum = m_nColum+2;
	for (int i = 0; i < m_nRow; ++i)
	{
		m_symList[nAccum].bValid = false;
		m_symList[nAccum+m_nColum+1].bValid = false;
		nAccum += m_nColum+2;
	}

	//adjust images in odd numbers.
	int flag = 0;
	int tmp  = 0;
	for (int i = 0; i < m_nSortOfImage; ++i)
	{
		if (statis[i] % 2 == 0)
		{
			continue;
		}

		if (flag = 0)
		{
			flag = 1;
			tmp  = i;
		}
		else
		{
			flag = 0;
			int k = 0;
			int index = 0;
			for (int ii = 0; ii < m_nRow; ++ii)
			{
				for (int jj = 0; jj < m_nColum; ++jj)
				{
					index = PosToIndex(jj, ii);
					if (m_symList[index].nImageIndex == i)
					{
						goto SYM_FIND;
					}
				}
			}
SYM_FIND:
			m_symList[index].nImageIndex = tmp;
			statis[i]--;
			statis[tmp]++;
		}
	}
}


CSymbolManage::~CSymbolManage(void)
{
}


bool CSymbolManage::LoadImages(void)
{
	return true;
}



/*

----------------
|  |  |  |  |  | 
----------------
|  | A|  |  |  |
----------------
|  |  |  | B|  |
----------------
|  |  |  |  |  |
----------------
by BFS.
*/
bool CSymbolManage::PosLinked(const IMPOSITION& posSrc, const IMPOSITION& posDest)
{
	m_posCrossing[0] = posSrc;
	m_posCrossing[3] = posDest;

	IMPOSITION pos1(posSrc.first+1, posSrc.second+1);
	IMPOSITION pos2(posDest.first+1, posDest.second+1);
	int n = PosToIndex(pos1);
	if (m_symList[PosToIndex(pos1)].nImageIndex != m_symList[PosToIndex(pos2)].nImageIndex)
	{
		m_nCrossing = -1;
		return false;
	}
	
	ClearCrossing();
	queue<IMPOSITION> q;
	//find reachable position with crossing 0
	int iRet = FindPos(pos1, pos2, q, 0);
	if (1 == iRet)
	{
		m_nCrossing = 0;
		return true;
	}
	//find position with crossing 1
	while (!q.empty() && m_symList[PosToIndex(q.front())].nMinCrossing == 0)
	{
		iRet = FindPos(q.front(), pos2, q, 1);
		if (1 == iRet)
		{
			m_nCrossing = 1;
			m_posCrossing[1].first = q.front().first-1;
			m_posCrossing[1].second = q.front().second-1;
			return true;
		}
		q.pop();
	}

	//find position with crossing 2
	while (!q.empty() && m_symList[PosToIndex(q.front())].nMinCrossing == 1)
	{
		iRet = FindPos(q.front(), pos2, q, 2);
		if (1 == iRet)
		{
			m_nCrossing = 2;
			m_posCrossing[2].first = q.front().first-1;
			m_posCrossing[2].second = q.front().second-1;

			if (m_posCrossing[2].first == m_posCrossing[3].first)
			{
				m_posCrossing[1].second = m_posCrossing[2].second;
				m_posCrossing[1].first  = m_posCrossing[0].first;
			}
			else
			{
				m_posCrossing[1].second = m_posCrossing[0].second;
				m_posCrossing[1].first  = m_posCrossing[2].first;
			}
			return true;
		}
		q.pop();
	}

	m_nCrossing = -1;
	return false;
}

int CSymbolManage::NextPos(IMPOSITION &pos, enumDIRECTION dir)
{
	int x = pos.first;
	int y = pos.second;
	switch (dir)
	{
	case DIR_LEFT:
		x -= 1;
		break;
	case DIR_TOP:
		y -= 1;
		break;
	case DIR_RIGHT:
		x += 1;
		break;
	case DIR_BOTTOM:
		y += 1;
		break;
	}
	return 0;
}

int CSymbolManage::FindPos(const IMPOSITION& pos1, const IMPOSITION& pos2, std::queue<IMPOSITION>& q, unsigned int nCrossing)
{
	//left
	IMPOSITION posTmp;
	int nIndex =  0;
	posTmp.second = pos1.second;
	for (int ii = pos1.first - 1; ii >= 0; --ii)
	{
		posTmp.first = ii;
		
		if (posTmp == pos2)
		{	
			return 1;
		}
		nIndex = PosToIndex(posTmp);
		if (m_symList[nIndex].bValid || m_symList[nIndex].nMinCrossing <nCrossing)
		{
			break;
		}
		q.push(posTmp);
		m_symList[nIndex].nMinCrossing = nCrossing;
	}
	//right
	for (int ii = pos1.first+1; ii < m_nColum+2; ++ii)
	{
		posTmp.first = ii;
		if (posTmp == pos2)
		{
			return 1;
		}
		nIndex = PosToIndex(posTmp);
		if (m_symList[nIndex].bValid || m_symList[nIndex].nMinCrossing <nCrossing)
		{
			break;
		}
		q.push(posTmp);
		m_symList[nIndex].nMinCrossing = nCrossing;
	}
	//top
	posTmp.first = pos1.first;
	for (int ii = pos1.second - 1; ii >= 0; --ii)
	{
		posTmp.second = ii;
		if (posTmp == pos2)
		{
			return 1;
		}
		nIndex = PosToIndex(posTmp);
		if (m_symList[nIndex].bValid || m_symList[nIndex].nMinCrossing <nCrossing)
		{
			break;
		}
		q.push(posTmp);
		m_symList[nIndex].nMinCrossing = nCrossing;
	}
	//bottom
	for (int ii = pos1.second + 1; ii < m_nRow+2; ++ii)
	{
		posTmp.second = ii;
		if (posTmp == pos2)
		{
			return 1;
		}
		nIndex = PosToIndex(posTmp);
		if (m_symList[nIndex].bValid || m_symList[nIndex].nMinCrossing <nCrossing)
		{
			break;
		}
		q.push(posTmp);
		m_symList[nIndex].nMinCrossing = nCrossing;
	
	}
	return 0;
}


void CSymbolManage::ClearCrossing(void)
{
	for (vector<SYMBOL>::iterator iter = m_symList.begin();
		iter != m_symList.end();
		++iter)
	{
		iter->nMinCrossing = UINT_MAX;
	}
	m_nCrossing = -1;
}



/*
AB
BA
*/
bool CSymbolManage::IsDeadLock(void)
{
	if (m_nNumOfUnlinkedPair < 2)
	{
		return true;
	}

	//find unlinked image postion
	int nTotal = 2*m_nNumOfUnlinkedPair;
	int exColum = m_nColum + 2;
	vector<IMPOSITION> vPos(nTotal);
	
	int tmp = m_nColum+3;
	for (int i = 0; i < nTotal; ++i)
	{
		while (!m_symList[tmp++].bValid);
		vPos[i].first  = (tmp-1)%exColum - 1;
		vPos[i].second = (tmp-1)/exColum - 1;
	}
	
	for (int i = 0; i < nTotal-1; ++i)
	{
		for (int j = i+1; j < nTotal; ++j)
		{
			if (PosLinked(vPos[i], vPos[j]))
			{
				return false;
			}
		}
	}
	return true;
}