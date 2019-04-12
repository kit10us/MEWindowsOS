// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/os/IOS.h>
#include <me/game/IGame.h>
#include <me/render/IRenderer.h>
#include <me/render/Display.h>
#include <list>
#include <vector>

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

// Undefine Microsoft clashing defines.
#ifdef CreateWindow
#undef CreateWindow
#endif

#ifdef GetCommandLine
#undef GetCommandLine
#endif

namespace mewos
{
	/// <summary>
	/// Interface for Windows OS.
	/// Used to allow interfacing with the Windows operating system.
	/// </summary>
	class IWindowsOS : public me::os::IOS
	{
	public:
		/// <summary>
		/// Create pending display. These are display added via AddDisplay.
		/// </summary>
		virtual void CreatePendingDisplays( std::string title ) = 0;
		
		/// <summary>
		/// Create a display. Used by CreatePendingDisplay.
		/// </summary>
		virtual void CreateDisplay( me::render::Display display, std::string title ) = 0;

		/// <summary>
		/// Get our HInstance.
		/// </summary>
		virtual HINSTANCE GetHInstance() const = 0;
		
		/// <summary>
		/// Get our handle. Used to interfacing with our WOS window.
		/// </summary>
		virtual HWND GetHandle() const = 0;

		/// <summary>
		/// Windows process handler. Handles Windows OS messaging/events.
		/// </summary>
		virtual LRESULT WndProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam ) = 0;
	};
}