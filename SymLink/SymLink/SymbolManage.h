/********************************************\
* SymbolManage.h                             *
* Function:                                  *
*	simulate the link up game using BFS      *
* algorithm.                                 *
*    _______________                         *
*	| ___assist___ |                         *
*	| |          | |                         *
*	| |  valid   | |                         *
*	| |__________| |                         *
*	|______________|                         *
* Copyright(c)2012, Ben                      *
* No rights reserved.                        *
* mail: benyuan620@gmail.com                 *
\********************************************/

#pragma once

#include <utility>
#include <vector>
#include <queue>

namespace symbollink{
struct tagSymbol
{
	tagSymbol();
	int nImageIndex;
	unsigned int  nMinCrossing;
	bool bValid;
};
typedef tagSymbol SYMBOL;

class CSymbolManage
{
	enum enumDIRECTION
	{
		DIR_LEFT   = 1,
		DIR_TOP    = 2,
		DIR_RIGHT  = 3,
		DIR_BOTTOM = 4
	};
public:
	typedef std::pair<int, int> IMPOSITION;
	typedef std::vector<tagSymbol>  SYMLIST;
	enum {IMAGE_RESOLUTION = 48};
public:
	CSymbolManage(void);
	~CSymbolManage(void);
public:
	bool LoadImages(void);
	void Init(void);
public:
	bool PosLinked(const IMPOSITION& pos1, const IMPOSITION& pos2);
	void RemovePos(const IMPOSITION& pos){m_symList[PosToIndex(&pos)].bValid = false;}
public:
	SYMLIST&      GetSymList(void){return m_symList;}
	CImageList&   GetImageList(void){return m_imageList;}
	int           GetColum(void) const{return m_nColum;}
	int           GetRow(void) const{return m_nRow;}
	void          SetColum(int nCol){m_nColum = nCol;}
	void          SetRow(int nRow){m_nRow = nRow;}
	int           GetCrossing(void)const{return m_nCrossing;}
	void          SetCrossing(int nCrossing){m_nCrossing = nCrossing;}
	IMPOSITION *  GetCrossingPos(void){return m_posCrossing;}
	int           GetSortOfImage(void) const{return m_nSortOfImage;}
	void          SetSortOfImage(int nCount){m_nSortOfImage = nCount;}
	int           GetNumOfUnlinkedPair(void)const{return m_nNumOfUnlinkedPair;}
	void          SetNumOfUnLinkedPair(int nCount){m_nNumOfUnlinkedPair = nCount;}
	void          DecressUnlinkedPair(void){--m_nNumOfUnlinkedPair;}
	bool          IsPosValid(const IMPOSITION &pos)const{return m_symList[pos.first+1+(m_nColum+2)*(pos.second+1)].bValid;}
	bool          IsGameFinished(void)const{return 0 == m_nNumOfUnlinkedPair;}
	inline int    PosToIndex(const IMPOSITION* pPos){return pPos->first+1+(pPos->second+1)*(m_nColum+2);}
	inline int    PosToIndex(int x, int y){return x+1+(y+1)*(m_nColum+2);}
	bool          IsDeadLock(void);
	
private:
	inline int    NextPos(IMPOSITION &pos, enumDIRECTION dir);
	inline bool   PosInRange(const IMPOSITION &pos){return pos.first>=0 && pos.first<m_nColum && pos.second >= 0 && pos.second < m_nRow;}
	inline bool   PosInValidRange(const IMPOSITION &pos){return pos.first>=1 && pos.first<=m_nColum && pos.second >= 1 && pos.second <= m_nRow;}
	inline int    FindPos(const IMPOSITION& pos1, const IMPOSITION& pos2,std::queue<IMPOSITION>& q, unsigned int nCrossing);
	inline void   ClearCrossing(void);
	inline int    PosToIndex(const IMPOSITION& pos){return pos.first+pos.second*(m_nColum+2);}
private:
	int               m_nColum;
	int               m_nRow;
	int               m_nSortOfImage;
	int               m_nNumOfUnlinkedPair;
	SYMLIST           m_symList;
	CImageList        m_imageList;
	IMPOSITION        m_posCrossing[4];
	int               m_nCrossing;
};

}//endof namespace symbollink