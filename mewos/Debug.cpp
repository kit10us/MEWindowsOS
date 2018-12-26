// Copyright (c) 2003 - 2011, Evil Quail LLC
// All Rights Reserved

 #include <mewos/Debug.h>
#include <unify/Exception.h>
#include <Windows.h>
#include <fstream>
#include <cassert>

using namespace mewos;

Debug::Debug( me::debug::DefaultDebug & defaultDebug )
	: DefaultDebug{ defaultDebug }
	, m_flushedLogLines{}
{
}

Debug::~Debug()	
{
}

void Debug::LogSectionLine( std::string section, std::string line )
{
	DefaultDebug::LogSectionLine( section, line );
	Flush();
}

void Debug::Flush()
{
	if (m_logFile.Empty()) return;

	using namespace std;

	for (m_flushedLogLines; m_flushedLogLines != m_logLines.size(); m_flushedLogLines++)
	{
		std::string text{ m_logLines[m_flushedLogLines] };
		ofstream out( m_logFile.ToString(), ios_base::out | ios_base::app );
		out << text << endl;

		OutputDebugStringA( text.c_str() );

		for (auto && listener : m_logListeners)
		{
			listener->Log( text );
		}
	}
}
