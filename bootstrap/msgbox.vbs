' Display MsgBox
' Author: Dmitri Rubinstein
' Version: 1.0

' Declare all global variables
Dim FSO, WshShell, WshEnv

' Create objects that will be shared by all following code
Set FSO = CreateObject("Scripting.FileSystemObject")
Set WshShell = Wscript.CreateObject("Wscript.Shell")
Set WshEnv = WshShell.Environment("PROCESS")

Sub PrintUsage
     Wscript.Echo "MsgBox" & vbCrLf & _
                  "Usage : " & Wscript.ScriptName & " [options] text info|error title" & vbCrLf & vbCrLf & _
                  "Options:" & vbCrLf & _
                  "/?                  print this" & vbCrLf
End Sub

' Process command-line arguments
Set args = Wscript.Arguments
If args.Count > 0 Then
   If args(0) = "-h" Or args(0) = "-?" Or _
      args(0) = "--help" Or args(0) = "/?" Or _
	  args.Count < 3 Then
      PrintUsage
      Wscript.Quit
   End If
Else
  PrintUsage
  Wscript.Quit
End If

Dim text, info, title
text = args(0)
If args(1) = "info" Or args(1) = "vbInformation" Then
  info = vbInformation
ElseIf args(1) = "error" Or args(1) = "vbExclamation" Then
  info = vbExclamation
Else
  info = 0
End If
title = args(2)

MsgBox text, info, title
