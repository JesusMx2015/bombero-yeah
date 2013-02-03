#include "stdafx.h"
#include "ZTimer.h"
#include <Windows.h>

typedef unsigned __int8  UI8;
typedef   signed __int8  SI8;
typedef unsigned __int16 UI16;
typedef   signed __int16 SI16;
typedef unsigned __int32 UI32;
typedef   signed __int32 SI32;
typedef unsigned __int64 UI64;
typedef   signed __int64 SI64;


typedef SI32 (NTAPI *PFN_NT_QUERY_PERFORMANCE_COUNTER)(OUT PLARGE_INTEGER PerformanceCounter, OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL);
PFN_NT_QUERY_PERFORMANCE_COUNTER NtQueryPerformanceCounter;

class ZTimerEvent
{
private:
	unsigned long int			m_nUpdatedTime;				///< �ð�����ϱ� ���� ���ο����� ����ϴ� ����
	unsigned long int			m_nElapse;					///< ����ڰ� ������ �ð�(1000 - 1��)
	bool						m_bOnce;					///< true�� �����Ǹ� �ѹ��� Ÿ�̸� �̺�Ʈ�� �߻��Ѵ�.
	void*						m_pParam;					///< Event Callback�� �Ķ����
public:
	ZTimerEvent() { m_nUpdatedTime = 0; m_nElapse = 0; m_bOnce = false; m_fnTimerEventCallBack = NULL; m_pParam=NULL; }
	bool UpdateTick(unsigned long int nDelta)
	{
		if (m_nElapse<0) return false;

		m_nUpdatedTime += nDelta;

		if (m_nUpdatedTime >= m_nElapse)
		{
			if (m_fnTimerEventCallBack)
			{
				m_fnTimerEventCallBack(m_pParam);
			}

			if (m_bOnce) return true;
		}

		return false;
	}
	void SetTimer(unsigned long int nElapse, ZGameTimerEventCallback* fnTimerEventCallback, void* pParam, bool bTimerOnce)
	{
		m_nElapse = nElapse;
		m_fnTimerEventCallBack = fnTimerEventCallback;
		m_pParam = pParam;
		m_bOnce = bTimerOnce;
	}

	ZGameTimerEventCallback*	m_fnTimerEventCallBack;		///< Event Callback ������
};


/////////////////////////////////////////////////////////////////////////////////////////////////////

ZTimer::ZTimer()
{
	m_bInitialized = false;
	m_nNowTime = 0;
	m_nLastTime = 0;

	m_pbUsingQPF = new BOOL(FALSE);
	m_pllQPFTicksPerSec = new LONGLONG(0);
	m_pllLastElapsedTime = new LONGLONG(0);
	m_pThistime = new DWORD;
	m_pLasttime = new DWORD;
	m_pElapsed = new DWORD;
	HMODULE Ntdll;

	Ntdll = GetModuleHandleW(L"ntdll.dll");
	NtQueryPerformanceCounter = (PFN_NT_QUERY_PERFORMANCE_COUNTER) GetProcAddress(Ntdll, "NtQueryPerformanceCounter");

}

ZTimer::~ZTimer()
{
	for (list<ZTimerEvent*>::iterator itor = m_EventList.begin(); itor != m_EventList.end(); ++itor)
	{
		ZTimerEvent* pEvent = (*itor);
		delete pEvent;
	}

	m_EventList.clear();

	delete m_pbUsingQPF;
	delete m_pllQPFTicksPerSec;
	delete m_pllLastElapsedTime;
	delete m_pThistime;
	delete m_pLasttime;
	delete m_pElapsed;
}

void ZTimer::ResetFrame()
{
	m_bInitialized=false;
}

float ZTimer::UpdateFrame()
{
	LARGE_INTEGER qwTime;
	LARGE_INTEGER PerformanceCounter;

	if(!m_bInitialized)
	{
		m_bInitialized = true;
		LARGE_INTEGER qwTicksPerSec;
		(*m_pbUsingQPF) = NtQueryPerformanceCounter( &PerformanceCounter, &qwTicksPerSec );
		(*m_pllQPFTicksPerSec) = qwTicksPerSec.QuadPart;

		NtQueryPerformanceCounter( &qwTime, &PerformanceCounter );

		(*m_pllLastElapsedTime) = qwTime.QuadPart;
	}

	float fElapsed;

	NtQueryPerformanceCounter( &qwTime, &PerformanceCounter );

	fElapsed = (float)((double) ( qwTime.QuadPart - (*m_pllLastElapsedTime) ) / (double) (*m_pllQPFTicksPerSec));
	(*m_pllLastElapsedTime) = qwTime.QuadPart;

	
	UpdateEvents();			// Ÿ�̸� �̺�Ʈ�� ������Ʈ

	ShiftFugitiveValues();
	return fElapsed;
}

void ZTimer::ShiftFugitiveValues()
{
	// �޸����� ���ϱ� ���� ���� ������ �� ��ġ�� �ű��

	BOOL* pBool1 = m_pbUsingQPF;
	m_pbUsingQPF = new BOOL(*pBool1);

	LONGLONG* pll1 = m_pllQPFTicksPerSec;
	m_pllQPFTicksPerSec = new LONGLONG(*pll1);

	LONGLONG* pll2 = m_pllLastElapsedTime;
	m_pllLastElapsedTime = new LONGLONG(*pll2);

	DWORD* pDword1 = m_pThistime;
	m_pThistime = new DWORD(*pDword1);

	DWORD* pDword2 = m_pLasttime;
	m_pLasttime = new DWORD(*pDword2);

	DWORD* pDword3 = m_pElapsed;
	m_pElapsed = new DWORD(*pDword3);

	delete pBool1;
	delete pll1;
	delete pll2;
	delete pDword1;
	delete pDword2;
	delete pDword3;
}

void ZTimer::UpdateEvents()
{
	m_nNowTime = timeGetTime();
	unsigned long int nDeltaTime = m_nNowTime - m_nLastTime;
	m_nLastTime = m_nNowTime;

	if (m_EventList.empty()) return;

	for (list<ZTimerEvent*>::iterator itor = m_EventList.begin(); itor != m_EventList.end(); )
	{
		ZTimerEvent* pEvent = (*itor);
		bool bDone = pEvent->UpdateTick(nDeltaTime);
		if (bDone)
		{
			delete pEvent;
			itor = m_EventList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void ZTimer::SetTimerEvent(unsigned long int nElapsedTime, ZGameTimerEventCallback* fnTimerEventCallback, void* pParam, bool bTimerOnce)
{
	ZTimerEvent* pNewTimerEvent = new ZTimerEvent;
	pNewTimerEvent->SetTimer(nElapsedTime, fnTimerEventCallback, pParam, bTimerOnce);
	m_EventList.push_back(pNewTimerEvent);
}

void ZTimer::ClearTimerEvent(ZGameTimerEventCallback* fnTimerEventCallback)
{
	if (fnTimerEventCallback == NULL) return;

	for (list<ZTimerEvent*>::iterator itor = m_EventList.begin(); itor != m_EventList.end(); )
	{
		ZTimerEvent* pEvent = (*itor);

		if (pEvent->m_fnTimerEventCallBack == fnTimerEventCallback)
		{
			delete pEvent;
			itor = m_EventList.erase(itor);
		}
		else
		{
			++itor;
		}
	}

}