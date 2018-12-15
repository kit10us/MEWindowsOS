// Copyright (c) 2003 - 2011, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <mewos/MEWindowsOS.h>
#include <me/os/DefaultDebug.h>
#include <unify/Path.h>
#include <list>
#include <string>
#include <chrono>
#include <map>

namespace mewos
{
	class Debug : public me::os::IDebug
	{
	public:
		Debug( me::os::DefaultDebug & defaulDebug );
		virtual ~Debug();

		void SetDebug( bool debug ) override;
		bool IsDebug() override;

		void SetLogFile( unify::Path logFile ) override;
		void SetFailureAsCritical( bool faiureAsCritical ) override;

		bool Assert( bool assertion ) const override;

		void LogLine( std::string section, std::string line ) override;
		void AttachLogListener( me::ILogListener * listener ) override;
		void DetachLogListener( me::ILogListener * litener ) override;
		void DetachAllLogListeners() override;

		void ReportError( me::ErrorLevel level, std::string source, std::string error ) override;

		bool HadCriticalError() const override;

		void DebugTimeStampBegin( std::string name ) override;
		void DebugTimeStampEnd( std::string name )	 override;
		float DebugGetTimeStamp( std::string name )	 override;

		const std::vector< std::string > & GetLogLines() const override;

		/// <summary>
		/// Flush log lines by outputting lines logged, and ensuring our logged lines are consumed.
		/// </summary>
		void Flush();

	private:
		bool m_isDebug;
		bool m_failuresAsCritial;
		unify::Path m_logFile;
		std::list< me::ILogListener* > m_logListeners;
		std::list< std::string > m_criticalErrors;
		
		struct TimeStartEnd
		{
			std::chrono::high_resolution_clock::time_point start;
			std::chrono::high_resolution_clock::time_point end;
		};
		
		std::map< std::string, TimeStartEnd > m_timeStamps;
		std::vector< std::string > m_logLines;
		size_t m_flushedLogLines;
	};
}