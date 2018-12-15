// Copyright (c) 2003 - 2011, Evil Quail LLC
// All Rights Reserved

 #include <mewos/Debug.h>
#include <unify/Exception.h>
#include <Windows.h>
#include <fstream>
#include <cassert>


using namespace mewos;

Debug::Debug( me::os::DefaultDebug & defaultDebug )
	: m_failuresAsCritial{ true }
	, m_isDebug
#ifdef _DEBUG
		{ true }
#else
		{ false }
#endif
	, m_logLines{ defaultDebug.GetLogLines() }
	, m_flushedLogLines{ 0 }
{
}

Debug::~Debug()
{
}

void Debug::SetDebug( bool debug )
{
	m_isDebug = debug;
}

bool Debug::IsDebug()
{
	return m_isDebug;
}


void Debug::SetLogFile( unify::Path logFile )
{
	logFile.Rename( m_logFile );
	m_logFile = logFile;
	LogLine( "Debug", "Log file: " + m_logFile.ToString() );
}

void Debug::SetFailureAsCritical( bool faiureAsCritical )
{
	m_failuresAsCritial = faiureAsCritical;
}

bool Debug::Assert( bool assertion ) const
{
	bool debug =
#ifdef _DEBUG
		debug = true;
#else
		debug = false;
#endif

	assert( assertion );
	// TODO: Proper assertion based on debug or release.
	return false;
}

void Debug::LogLine( std::string section, std::string line )
{
	std::string text = "[" + section + "] " + line + "\n";
	m_logLines.push_back( text );
	Flush();
}

void Debug::AttachLogListener( me::ILogListener* listener )
{
	using namespace std;

	if( m_logFile.Empty() ) return;

	string line;
	ifstream in( m_logFile.ToString() );
	while( getline( in, line ) )
	{
		listener->Log( line + "\n" );
	}

	m_logListeners.push_back( listener );
}

void Debug::DetachLogListener( me::ILogListener* listener )
{
	m_logListeners.remove( listener );
}

void Debug::DetachAllLogListeners()
{
	m_logListeners.clear();
}

void Debug::ReportError( me::ErrorLevel level, std::string source, std::string error )
{
	switch( level )
	{
	case me::ErrorLevel::Critical:
		m_criticalErrors.push_back( "Critical Failure (" + source + "): " + error );
		LogLine( "Debug", "Critical Failure (" + source + "): " + error );
		throw unify::Exception( "Critical Failure (" + source + "): " + error );
	case me::ErrorLevel::Failure:
		LogLine( "Debug", "Failure (" + source + "): " + error );
		if( m_failuresAsCritial )
		{
			m_criticalErrors.push_back( "Critical Failure (" + source + "): " + error );
			throw unify::Exception( "Failure (" + source + "): " + error );
		}
		break;
	case me::ErrorLevel::Warning:
		LogLine( "Debug", "Warning (" + source + "): " + error );
		break;
	}
	return;
}

bool Debug::HadCriticalError() const
{
	return m_criticalErrors.size() != 0;
}

void Debug::DebugTimeStampBegin( std::string name )
{
	if( !IsDebug() ) return;

	using namespace std::chrono;

	auto now = high_resolution_clock::now();
	
	m_timeStamps[name].start = now;
	m_timeStamps[name].end = now;
}

void Debug::DebugTimeStampEnd( std::string name )
{
	if( !IsDebug() ) return;

	auto itr = m_timeStamps.find( name );
	if( itr == m_timeStamps.end() )
	{
		return;
	}

	using namespace std::chrono;

	auto now = high_resolution_clock::now();

	m_timeStamps[name].end = now;
}

float Debug::DebugGetTimeStamp( std::string name )
{
	auto itr = m_timeStamps.find( name );
	if( itr == m_timeStamps.end() )
	{
		return 0.0f;
	}
	
	using namespace std::chrono;

	auto durationS = duration_cast< duration< float > >( itr->second.end - itr->second.start );

	return durationS.count();
}

const std::vector< std::string > & Debug::GetLogLines() const
{
	return m_logLines;
}

void Debug::Flush()
{
	if( m_logFile.Empty() ) return;

	using namespace std;

	for( m_flushedLogLines; m_flushedLogLines != m_logLines.size(); m_flushedLogLines++ )
	{
		std::string text{ m_logLines[m_flushedLogLines] };
		ofstream out( m_logFile.ToString(), ios_base::out | ios_base::app );
		out << text;

		OutputDebugStringA( text.c_str() );

		for( auto && listener : m_logListeners )
		{
			listener->Log( text );
		}
	}
}