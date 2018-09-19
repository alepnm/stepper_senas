'[Begin versionfile-gen.vbs]
Dim Args
Set Args = WScript.Arguments


   proj_full_name = "Stepper Controller"
   proj_code = "RXH"
   proj_ver = "1.0"

if (Args.Count < 1) then
   MsgBox "Error generate version file." + vbCrLf + "Usage:" + vbCrLf + "version-gen.vbs <filename>"
else
   verfilename = Args(0)
   vYear = CStr(Year(Date))
   vMonth = CStr(Month(Date))
   if Len(vMonth) < 2 then
      vMonth = "0" + vMonth
   end if
   vDay    = CStr(Day(Date))
   if Len(vDay) < 2 then
      vDay = "0" + vDay
   end if
   vHour  = CStr(Hour(Time))
   if Len(vHour) < 2 then
      vHour = "0" + vHour
   end if
   vMinute = CStr(Minute(Time))
   if Len(vMinute) < 2 then
      vMinute = "0" + vMinute
   end if
   vSecond = CStr(Second(Time))
   if Len(vSecond) < 2 then
      vSecond = "0" + vSecond
   end if


	info = "/*     "+proj_full_name+" "+proj_ver+"    Kompiliuota "+vDay+"."+vMonth+"."+vYear+" "+vHour+":"+vMinute+":"+vSecond+"     */"
   	version_full = "__root const UCHAR ucID_buf[] = """+proj_code+" "+vDay+"."+vMonth+"."+vYear+" "+vHour+":"+vMinute+":"+vSecond+""";"
	
   	Set FSO = CreateObject("Scripting.FileSystemObject")
   	Set FileOutStream = FSO.OpenTextFile(verfilename, 2, true, 0)   
   	FileOutStream.Write info + vbCrLf 
   	'FileOutStream.Write "#pragma constseg=INFOB" + vbCrLf    
   	FileOutStream.Write version_full + vbCrLf   
   	'FileOutStream.Write "#pragma constseg=default" + vbCrLf
   
end if
 '[End versionfile-gen.vbs]