#include "stdafx.h"
#include "MSharedCommandTable.h"
#include "MMatchConfig.h"
#include "MBMatchServer.h"

// ��������.
void MBMatchServer::OnScheduleAnnounce( const char* pszAnnounce )
{
	if( 0 == pszAnnounce )
		return;

	MCommand* pNewCmd = CreateCommand( MC_MATCH_SCHEDULE_ANNOUNCE_SEND, MUID(0, 0) );
	if( 0 == pNewCmd )
		return;

	MCommandParameterString* pCmdPrmStr = new MCommandParameterString( pszAnnounce );
	if( 0 == pCmdPrmStr ){
		delete pNewCmd;
		return;
	}

	if( !pNewCmd->AddParameter(pCmdPrmStr) ){
		delete pNewCmd;
		return;
	}

	RouteToAllClient( pNewCmd );
}

// Ŭ������ Disable.
void MBMatchServer::OnScheduleClanServerSwitchDown()
{
	#ifndef _QUESTCLAN
		if( MSM_CLAN != MGetServerConfig()->GetServerMode() )
			return;		// �ƴϸ� ����.
	#endif

	MGetServerConfig()->SetEnabledCreateLadderGame( false );

	if( !AddClanServerSwitchUpSchedule() )
	{
		mlog( "MBMatchServer::OnScheduleClanServerSwitchDown - Ŭ���� �ڵ� Ȱ��ȭ Ŀ�ǵ� ���� �۾� ����.\n" );
		return;
	}

	mlog( "MBMatchServer::OnScheduleClanServerSwitchDown - Ŭ������ Ŭ���� ��Ȱ��ȭ.\n" );
}


void MBMatchServer::OnScheduleClanServerSwitchUp()
{
	// Ŭ�� ������ ��츸 ����ɼ� �ְ�.
	#ifndef _QUESTCLAN
	if( MSM_CLAN != MGetServerConfig()->GetServerMode() )
		return;		// �ƴϸ� ����.
	#endif

	MGetServerConfig()->SetEnabledCreateLadderGame( true );

	// Ŭ���� ��Ȱ��ȭ Ŀ�ǵ� ����.
	if( AddClanServerAnnounceSchedule() )
	{
		if( !AddClanServerSwitchDownSchedule() )
			mlog( "MBMatchServer::OnScheduleClanServerSwitchUp - Ŭ���� ��Ȱ��ȭ �������۾� ����.\n" );
	}
	else
	{
		mlog( "MBMatchServer::OnScheduleClanServerSwitchUp - Ŭ���� ��Ȱ��ȭ ���� Ŀ�ǵ� ���� �������۾� ����.\n" );
		return;
	}	

	mlog( "MBMatchServer::OnScheduleClanServerSwitchDown - Ŭ������ Ŭ���� Ȱ��ȭ.\n" );
}