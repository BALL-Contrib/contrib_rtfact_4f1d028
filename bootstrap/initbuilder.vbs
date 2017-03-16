' Startup for bootstrap script initbuilder.sh on Windows
' Author: Lukas Marsalek, marsalek@cs.uni-saarland.de
' Heavily based on install.vbs from URay Development Environment, see below.

' See accompanying LICENSE.txt file for terms of use

' Original copyright by:
' URay Development Environment Setup Script for Windows
' Author: Dmitri Rubinstein
' Version: 1.0
' 05.04.2009

' Declare all global variables
Dim FSO, WshShell, WshEnv, THIS_DIR
Dim VERBOSE, LAUNCHER, LAUNCHER_OPTIONS

' Create objects that will be shared by all following code
Set FSO = CreateObject("Scripting.FileSystemObject")
Set WshShell = Wscript.CreateObject("Wscript.Shell")
Set WshEnv = WshShell.Environment("PROCESS")

' Bootstrap
' Set THIS_DIR CONFIG_DIR
' We assume that this file is located in THIS_DIR directory
THIS_DIR = FSO.GetParentFolderName(Wscript.ScriptFullName)
CONFIG_DIR = FSO.BuildPath(THIS_DIR, "config")

' Check THIS_DIR for existence
Assert FSO.FolderExists(THIS_DIR), "Bootstrap: There is no directory " + THIS_DIR + ", something is wrong"

' Set VERBOSE to True if we Wscript.Echo will print to console
VERBOSE = InConsole()

' Process command-line arguments
Set args = Wscript.Arguments
If args.Count > 0 Then
   If args(0) = "-h" Or args(0) = "-?" Or _
      args(0) = "--help" Or args(0) = "/?" Then
     Wscript.Echo "URay Development Environment Setup Script for Windows" & vbCrLf & _
                  "Usage : " & Wscript.ScriptName & " [ /? ]" & vbCrLf & vbCrLf & _
                  "Options:" & vbCrLf & _
                  "/?                  print this" & vbCrLf & _
                  "DevEnv Scripts are in " & THIS_DIR & " directory"
     Wscript.Quit
   End If
End If

' Check for MsysGit
Dim GIT_DIR, GIT_SHELL
GIT_SHELL = ""
GIT_DIR = RegRead("HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1\InstallLocation", "")
If GIT_DIR <> "" Then
  Echo "Located Git in " + GIT_DIR + "."
  GIT_SHELL = FSO.BuildPath(GIT_DIR, "bin\sh.exe")
Else
  ' Check on 64-bit Windows
  GIT_DIR = RegRead("HKLM\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1\InstallLocation", "")
  If GIT_DIR <> "" Then
    Echo "Located Git in " + GIT_DIR + "."
    GIT_SHELL = FSO.BuildPath(GIT_DIR, "bin\sh.exe")
  End If
End If

Dim SHELL
SHELL = ""
If GIT_SHELL <> "" Then
  SHELL=GIT_SHELL
Else
  Error "Could not find Git, please install MsysGit.", 1
End If

' Construct command line for running initbuilder.sh
Dim COMSPEC, RUN_CMD
COMSPEC = WshShell.ExpandEnvironmentStrings("%COMSPEC%")

'C:\WINDOWS\system32\cmd.exe /c ""C:\Programme\Git\bin\sh.exe" --login -i"
RUN_CMD = COMSPEC & " /c """"" & SHELL & _
	  """ --login -i -c """ & WinToMsysPath(FSO.BuildPath(THIS_DIR, "..\\initbuilder.sh")) & """"""
Echo RUN_CMD
result = WshShell.Run(RUN_CMD, 1, True)
REM If result = 0 Then
REM	MsgBox "Successfully installed development environment", vbInformation, "DevEnv Installer"
REM End If

Wscript.Quit

' Help procedures and functions

' Checks whether the script is started in text console (through CSCRIPT.EXE)
' or not
Function InConsole()
  InConsole = (UCase(Left(FSO.GetFileName(Wscript.FullName),7))  = "CSCRIPT")
End Function

' Read registry key, return defaultValue if the key with specified name
' does not exists
Function RegRead(key, defaultValue)
  RegRead = defaultValue
  On Error Resume Next
  RegRead = WshShell.RegRead(key)
End Function

Function WinToMsysPath(path)
  WinToMsysPath = Replace(Replace(path, "\", "/"), " ", "\ ")
End Function

' Echoes message if the script is in VERBOSE mode
' VERBOSE mode will be set at script start through call to InConsole
Sub Echo(msg)
    If VERBOSE Then Wscript.Echo msg
End Sub

' Show error message in message box and exit
Sub Error(msg,exitCode)
  MsgBox msg, vbExclamation, "Error"
  Wscript.Quit exitCode
End Sub

Sub Assert(cond, msg)
  If Not cond Then
    Error msg, 1
  End If
End Sub

' If VBasic error was happened exit script
Sub CheckVBError(msg,exitCode)
  If Err.Number <> 0 Then
    MsgBox msg & vbCrLf & Err.Description,vbExclamation, "Error"
    Wscript.Quit exitCode
  End If
End Sub
