// Copyright (c) 2003 - 2011, Kit10 Studios LLC
// All Rights Reserved

#include <mewos/WindowsOS.h>
#include <mewos/Debug.h>
#include <unify/Exception.h>
#include <Windows.h>
#include <fstream>
#include <cassert>

using namespace mewos;

#include <shellapi.h>

#define ID_EDIT_MESSAGE		100
#define ID_BUTTON_ABORT		101
#define ID_BUTTON_RETRY		102
#define ID_BUTTON_IGNORE	103

int s_failureWindowResult{};

extern "C"
LRESULT CALLBACK WndProcFailureWindow( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
	{
		HINSTANCE hInstace = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );

		const int padding = 5;
		RECT parentRect{};
		GetClientRect( hWnd, &parentRect );

		int childWidth = parentRect.right - padding * 2;
		int top = padding;
		int buttonHeight = 25;
		int buttonWidth = (childWidth / 3) - padding * 2;
		int editHeight = parentRect.bottom - parentRect.top - padding * 3 - buttonHeight;

		CreateWindowW(
			L"Edit",
			0,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
			padding, top, childWidth, editHeight,
			hWnd,
			(HMENU)ID_EDIT_MESSAGE,
			hInstace,
			0 );
		top += editHeight + padding;

		int buttonLeft = padding;
		CreateWindowW(
			L"Button",
			L"Abort",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			buttonLeft, top, buttonWidth, buttonHeight,
			hWnd,
			(HMENU)ID_BUTTON_ABORT,
			hInstace,
			0 );
		buttonLeft += buttonWidth + padding;

		CreateWindowW(
			L"Button",
			L"Retry",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			buttonLeft, top, buttonWidth, buttonHeight,
			hWnd,
			(HMENU)ID_BUTTON_RETRY,
			hInstace,
			0 );
		buttonLeft += buttonWidth + padding;

		CreateWindowW(
			L"Button",
			L"Ignore",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			buttonLeft, top, buttonWidth, buttonHeight,
			hWnd,
			(HMENU)ID_BUTTON_IGNORE,
			hInstace,
			0 );
		buttonLeft += buttonWidth + padding;
		top += buttonHeight;
		return 0;
	}

	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
		{
			//TesterBase::s_tester->RequestQuit();
			return 0;
		}

	case WM_COMMAND:
	{
		if( lParam )
		{
			int controlId = (int)LOWORD( wParam );
			int controlMessage = (int)HIWORD( wParam );
			switch( controlId )
			{
			case ID_BUTTON_ABORT:
			{
				s_failureWindowResult = controlId;
				PostQuitMessage( 0 );
				break;
			}

			case ID_BUTTON_RETRY:
			{
				s_failureWindowResult = controlId;
				PostQuitMessage( 0 );
				break;
			}

			case ID_BUTTON_IGNORE:
			{
				s_failureWindowResult = controlId;
				PostQuitMessage( 0 );
				break;
			}

			default:
				break;
			}
		}
		break;
	}

	case WM_CLOSE: // Fall through to WM_DESTROY...
	{
		PostQuitMessage( 0 );
		return 0;
	}

	case WM_DESTROY:
	{
		return 0;
	}

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

int ShowFailureWindow( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow, std::string failure, bool abort, bool retry, bool ignore )
{
	HWND activeWindow = GetActiveWindow();
	EnableWindow( activeWindow, false );

	const wchar_t * CLASS_NAME{ L"MercuryFailureWindowClass" };

	WNDCLASS wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW;

	wc.lpfnWndProc = (WNDPROC)WndProcFailureWindow;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MercuryFailureWindowClass";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	RegisterClass( &wc );

	HWND hWnd = CreateWindowW(
		CLASS_NAME,
		L"Mercury failure",
		WS_CAPTION | WS_MINIMIZEBOX | WS_BORDER | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		430, 300,
		0,
		0,
		hInstance,
		0 );
	if( !hWnd )
	{
		throw;
	}

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	HWND editMessage = GetDlgItem( hWnd, ID_EDIT_MESSAGE );
	SetWindowTextA( editMessage, failure.c_str() );

	HWND buttonAbort = GetDlgItem( hWnd, ID_BUTTON_ABORT );
	EnableWindow( buttonAbort, abort );

	HWND buttonRetry = GetDlgItem( hWnd, ID_BUTTON_RETRY );
	EnableWindow( buttonRetry, retry );

	HWND buttonIgnore = GetDlgItem( hWnd, ID_BUTTON_IGNORE );
	EnableWindow( buttonIgnore, ignore );

	MSG msg{};
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	DestroyWindow( hWnd );

	EnableWindow( activeWindow, true );
	SetActiveWindow( activeWindow );

	return s_failureWindowResult;
}

class ErrorHandler : public me::debug::IErrorHandler
{
	mewos::WindowsOS * m_os;

public:
	ErrorHandler( mewos::WindowsOS * os )
		: m_os{ os }
	{
	}

	me::ReportErrorResult ErrorHandler::ReportError( me::ErrorLevel level, std::string source, std::string error, bool canContinue, bool canRetry ) override
	{
		HINSTANCE hInstance = (HINSTANCE)m_os->GetOSParameters()->hInstance;
		HINSTANCE hPrevInstance = (HINSTANCE)m_os->GetOSParameters()->hPrevInstance;
		LPSTR lpszCmdLine = (LPSTR)m_os->GetOSParameters()->cmdLine.c_str();
		int nCmdShow = (int)m_os->GetOSParameters()->nCmdShow;

		std::string errorOutput{ m_os->Debug()->GetBlocks( "\r\n" ) + error };
		int result = ShowFailureWindow( hInstance, hPrevInstance, lpszCmdLine, nCmdShow, errorOutput, true, canRetry, canContinue );
		switch( result )
		{
		case ID_BUTTON_ABORT:
			m_os->Debug()->LogSectionLine( "", "   *User Aborted*" );
			throw unify::Exception( "Critical failure!" );

		case ID_BUTTON_RETRY:
			m_os->Debug()->LogSectionLine( "", "   *Retry*" );
			return me::ReportErrorResult::Retry;

		case ID_BUTTON_IGNORE:
		default:
			// Do nothing.
			m_os->Debug()->LogSectionLine( "", "   *Ignore*" );
			return me::ReportErrorResult::Continue;
		}

		return me::ReportErrorResult::Continue;
	}
};

Debug::Debug( me::os::IOS * os, me::debug::DefaultDebug & defaultDebug )
	: DefaultDebug{ defaultDebug }
	, m_os{ os }
	, m_flushedLogLines{}
{
	SetErrorHandler( me::debug::IErrorHandler::ptr{ new ErrorHandler( dynamic_cast< mewos::WindowsOS * >( os ) ) } );
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
