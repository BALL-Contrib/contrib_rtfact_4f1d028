' Read Registry Value
' Author: Dmitri Rubinstein
' Version: 1.0

' Declare all global variables
Dim FSO, WshShell, WshEnv

' Create objects that will be shared by all following code
Set FSO = CreateObject("Scripting.FileSystemObject")
Set WshShell = Wscript.CreateObject("Wscript.Shell")
Set WshEnv = WshShell.Environment("PROCESS")

Sub PrintUsage
     Wscript.Echo "RegRead" & vbCrLf & _
                  "Usage : " & Wscript.ScriptName & " [options] regName default" & vbCrLf & vbCrLf & _
                  "Options:" & vbCrLf & _
                  "/?                  print this" & vbCrLf
End Sub

' Read registry key, return defaultValue if the key with specified name
' does not exists
Function RegRead(key, defaultValue)
	RegRead = defaultValue
	On Error Resume Next
	RegRead = WshShell.RegRead(key)
End Function

' Process command-line arguments
Set args = Wscript.Arguments
If args.Count > 0 Then
   If args(0) = "-h" Or args(0) = "-?" Or _
      args(0) = "--help" Or args(0) = "/?" Or _
	  args.Count < 2 Then
      PrintUsage
      Wscript.Quit
   End If
Else
  PrintUsage
  Wscript.Quit
End If

Dim regName, defaultValue, value
regName = args(0)
defaultValue = args(1)

'Wscript.Echo regName
value = RegRead(regName, defaultValue)
Wscript.Echo value
