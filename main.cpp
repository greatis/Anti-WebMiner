//---------------------------------------------------------------------------

#include <vcl.h>
#include <vcl\inifiles.hpp>
#pragma hdrstop

#include "main.h"
#include "SSLCon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#pragma link "wininet.lib"

TfrmMain *frmMain;
static String LogFileName;
static bool bDebugMode;

//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	// Init vars
	LogFileName=ExtractFilePath(ParamStr(0))+"debug.log";
	bDebugMode=true;
	strHosts=new TStringList;
	sLocalBlackListVersion="";

 if(bDebugMode)
 {
  try
  {
   DeleteFile(LogFileName);
  }
  catch(...)
  {
  }
 }
 else
 {
   LogFileName=""; // disable writing to log file
 }

	   WriteLogMessage(MAIN_LABEL+" "+MAIN_VERS+" "+Now().DateTimeString());

	   if(ReadMainStatus())
	   {
		timUpdate->Enabled=true;
	   }

}
__fastcall TfrmMain::~TfrmMain()
{
  delete strHosts;
}

bool __fastcall TfrmMain::ReadMainStatus()
{
  bool bResult=true;

  try
  {
	 String sLocalBlackList=sGetBlackListFilePath();
	 if(!FileExists(sLocalBlackList))
	 {
		WriteLogMessage("Local Black List not found: "+sLocalBlackList);
		 DisplayStatus(DISP_GET_BLACKLIST);
	 }
	 else
	 {
		sLocalBlackListVersion=sGetBlackListVersion(sGetBlackListFilePath());
		WriteLogMessage("Black List Version= "+sLocalBlackListVersion);
	 }

	sHostsPath=sGetHostsPath();


	   WriteLogMessage("Hosts Path= "+sHostsPath);
	 if(!sHostsPath.IsEmpty())
	 {
	   sHostsPath=sHostsPath+"hosts";
	   int iStart=-1;
	   int iEnd=-1;

	   if(!FileExists(sHostsPath))
	   {
		   bNeedUpdateHosts=true;
		   WriteLogMessage("Hosts file does not exists: "+sHostsPath);
		   DisplayStatus(DISP_INSTALL_TO_HOSTS);
	   }
	   else
	   {
			   if(ReadHostsFile(sHostsPath,strHosts))
			   {
					bNeedUpdateHosts=true;
				   WriteLogMessage("ReadHostsFile=success!");
					WriteLogMessage("Num rows in hosts= "+String(strHosts->Count));

					 iStart=sGetOurStartPosInHosts();
					WriteLogMessage(MAIN_LABEL+" Start Row= "+String(iStart));
					if(iStart>=0)
					{
					   iEnd=sGetOurFinalPosInHosts(iStart);
						WriteLogMessage(MAIN_LABEL+" End Row= "+String(iEnd));

						int iInstallDate=0;
					   sHostsBlackListVersion=	sGetOurVersionFromHosts( iStart, iInstallDate);

						WriteLogMessage("Found Version in Hosts= "+sHostsBlackListVersion);

						WriteLogMessage("Found Install Date in Hosts= "+String(iInstallDate));
						if(iInstallDate>0)
						{
						  TDateTime td= (TDateTime)iInstallDate;
						  WriteLogMessage(td.DateTimeString());
						}



						if(IsLocalVersUptodate( sLocalBlackListVersion,  sHostsBlackListVersion))
						{
						  bNeedUpdateHosts=false;
						   WriteLogMessage("Hosts is up-to-date: "+ sLocalBlackListVersion+" Remote version: "+sHostsBlackListVersion);

						}
						else
						{
						   WriteLogMessage("Hosts must be updated: "+ sLocalBlackListVersion+" Remote version: "+sHostsBlackListVersion);

						}


					}


			   }
			   else
			   {
				   sCriticalError =constHosstCannotRead->Caption+" "+constSystemError->Caption+" "+SysErrorMessage(GetLastError());
				   WriteLogMessage(sCriticalError);
				   DisplayStatus(DISP_FATAL_ERROR);
				   bResult= false;
			   }
	  } //else

	  if(bNeedUpdateHosts)
	  {
		DisplayStatus(DISP_UPDATE_HOSTS);
	  }
	  else
	  {
			DisplayStatus(DISP_ALL_DONE);
	  }


	} // endif!sHostsPath.IsEmpty())
	else
	{
	   sCriticalError=constHostsfilepathfoundpathFatalError->Caption;
	   WriteLogMessage(sCriticalError);

	   DisplayStatus(DISP_FATAL_ERROR);
				   bResult= false;
	}

 }
 catch(...)
 {
 }
  return bResult;
}

bool __fastcall TfrmMain::IsInstalledIntoHosts()
{
	   int iStart=-1;
	   bool bResult=false;
	   try
	   {
		   if(ReadHostsFile(sHostsPath,strHosts))
			   {
				   WriteLogMessage("ReadHostsFile=success!");
					WriteLogMessage("Num rows in hosts= "+String(strHosts->Count));

					 iStart=sGetOurStartPosInHosts();
					 WriteLogMessage(MAIN_LABEL+" Start Row= "+String(iStart));
					 if(iStart>=0)
					 {
					  bResult=true;
					 }
				  }
		 }
		 catch(...)
		 {
		 }

  return bResult;

}
bool __fastcall TfrmMain::UninstallFromHosts()
{
	   int iStart=-1;
	   int iEnd=-1;
	   bool bResult=false;
	   try
	   {
		 WriteLogMessage("Starting uninstall from hosts...");

			   if(ReadHostsFile(sHostsPath,strHosts))
			   {
				   WriteLogMessage("ReadHostsFile=success!");
					WriteLogMessage("Num rows in hosts= "+String(strHosts->Count));

					 iStart=sGetOurStartPosInHosts();
					WriteLogMessage(MAIN_LABEL+" Start Row= "+String(iStart));
					if(iStart>=0)
					{
					   iEnd=sGetOurFinalPosInHosts(iStart);
					  bResult= RemoveOurStringsFromHostsStrings(iStart, iEnd);
					}
					else
					{
					  WriteLogMessage(MAIN_LABEL + " is not installed into Hosts files");
					}
			  }
			  else
			  {
				 WriteLogMessage("ReadHostsFile=failure!");
			  }

	   }
	   catch(...)
	   {
	   }

	   return bResult;

}


bool __fastcall TfrmMain::WriteToHostsFile()
{

  bool bResult=false;
	UninstallFromHosts();

		TStringList *strServList=NULL;
		 try
		 {
			strServList=new TStringList;

			strServList->Sorted=true;
			strServList->Duplicates=dupIgnore;
			strServList->Add("coin-hive.com");
			strServList->Add("jsecoin.com");

			ReadBlackListFile(ExtractFilePath(ParamStr(0))+BLACKLIST_DB, strServList);

			AddServerListTopHostsStrings(  strServList);
			bResult=SaveHostsFile();
		 }
		 catch(...)
		 {
		 }

		 delete  strServList;

return bResult;

}
//---------------------------------------------------------------------------
String __fastcall TfrmMain::sGetHostsPath()
{
 String sDefPath="%SystemRoot%\\System32\\drivers\\etc";
   // Todo
   // Check the DatabasePath in registry
   // Does it work in Windows 7-10?

  return AddLastSlash(sExpandEnvStrings(sDefPath));

}

String __fastcall TfrmMain::sExpandEnvStrings(String sSource)
{

  TCHAR buff[MAX_PATH+1];
  String sResult=sSource;

  try
  {
		ExpandEnvironmentStrings(sSource.t_str(),buff,MAX_PATH);
		sResult=String(buff);
  }
  catch(...)
  {
  }
  return sResult;

}


void __fastcall WriteLogMessage(AnsiString sText)
{


TFileStream *fs=NULL;

 try
 {
   try
   {
	if(LogFileName.IsEmpty())
	 return;
   if(FileExists(LogFileName))
	fs=new TFileStream(LogFileName,fmOpenReadWrite |fmShareDenyNone	);
   else
	 fs=new TFileStream(LogFileName,fmCreate );
   fs->Seek(0,soFromEnd);
   fs->Write(sText.c_str(),sText.Length());
   fs->Write("\r\n",2);
   }

 catch(...)
 {
 }


 }
 catch(...)
 {
 }
  delete fs;

}


String __fastcall TfrmMain::AddLastSlash(String sSource)
{

  if(sSource.IsEmpty())
  {
   return "";
  }

  if(sSource.SubString(sSource.Length(),1)=="\\")
  {
	return sSource;
  }
  else
  {
	return sSource+"\\";
  }
}


bool __fastcall TfrmMain::ReadHostsFile(String sFilePath, TStringList *strOut)
{
   if(strOut==NULL)
   {
		  WriteLogMessage("ReadHostsFile: strOut=NULL");
	return false;
   }

   bool bResult=false;
   try
   {
	// Todo
	// Add DisableFileRedirection if DatabasePath parameter is valid
	   strOut->LoadFromFile(sFilePath);
	   bResult=true;
   }
   catch(...)
   {
		  WriteLogMessage("ReadHostsFile: Exception during LoadFromFile: "+sFilePath);
   }


   return bResult;
}



int __fastcall TfrmMain::sGetOurStartPosInHosts()
{
 int iResult=-1;
 try
 {
   for(int i=0;i<strHosts->Count;i++)
   {
	 if(strHosts->Strings[i].Pos(START_LABEL)>0)
	 {
	   iResult=i;
	   break;
	 }
   }
 }
 catch(...)
 {
 }

 return iResult;

}


int __fastcall TfrmMain::sGetOurFinalPosInHosts(int iStart)
{
 int iResult=-1;
 try
 {
   for(int i=iStart;i<strHosts->Count;i++)
   {
	 if(strHosts->Strings[i].Pos(FIN_LABEL)>0)
	 {
	   iResult=i;
	   break;
	 }
   }
 }
 catch(...)
 {
 }

 return iResult;

}


String __fastcall TfrmMain::sGetOurVersionFromHosts(int iStartRow, int &iOutInstallDate)
{
 String sResult;

  try
  {
   String sTemp=strHosts->Strings[iStartRow];
   String sLex=START_LABEL;
	int n_pos=sTemp.Pos(sLex);
	if(n_pos>0)
	{
	  sTemp=sTemp.SubString(n_pos+sLex.Length(), sTemp.Length());
	  sTemp=sTemp.Trim();

	   n_pos=sTemp.Pos(" ");
	   if(n_pos>0)
	   {
		 String sDate=sTemp.SubString(n_pos+1, sTemp.Length());
		 sDate=sDate.Trim();

		 sTemp=sTemp.SubString(1,n_pos-1);
		  sTemp=sTemp.Trim();

		  if(!sDate.IsEmpty())
		  {
			try
			{
			  iOutInstallDate=sDate.ToInt();
			}
			catch(...)
			{
			}
		  }
	   }
	   sResult=sTemp;
	}
  }
  catch(...)
  {
  }

  return sResult;
}


bool __fastcall TfrmMain::AddServerListTopHostsStrings(TStringList *strIn)
{
  if(strIn==NULL)
  {
		  WriteLogMessage("AddServerListTopHostsStrings: strIn=NULL");

  return false;
  }

  bool bResult=false;
  try
  {
	   strHosts->Add(String("# ")+String(START_LABEL)+" "+sLocalBlackListVersion+" "+String((int)Now()));
	   for(int i=0;i<strIn->Count;i++)
	   {
		 strHosts->Add("0.0.0.0 "+strIn->Strings[i]);
	   }
	   strHosts->Add(String("# ")+String(FIN_LABEL));
	   bResult=true;
  }
  catch(...)
  {
  }

  return bResult;

}


bool __fastcall TfrmMain::RemoveOurStringsFromHostsStrings(int iStart, int iFinish)
{
  if(iStart<0)
  {
   return false; // nothing to do
  }

  bool bResult=false;
  try
  {
	int iMax=strHosts->Count-1;
	if( iFinish>=0)
	{
		iMax=iFinish;
	}

	for(int i=iMax;i>=iStart;i--)
	{
	  strHosts->Delete(i);
	}
	bResult=true;
  }
  catch(...)
  {
  }


  return bResult;

}



bool __fastcall TfrmMain::SaveHostsFile()
{

  bool bResult=false;

  try
  {
   if(strHosts->Count==0) // something wrong!
   {
	return false;
   }

	 BackupHostsFile(false); // do not overwrite copy

	 strHosts->SaveToFile(sHostsPath);
	 bResult=true;
  }
  catch(...)
  {
  }

  return bResult;

}


bool __fastcall TfrmMain::BackupHostsFile(bool bOverWritePrevious)
{

	 String sBackupFilePath=ChangeFileExt(sHostsPath,".old");
	   bool bResult=false;

	 try
	 {
	   BOOL bFailIfExists=TRUE;

	   if(  bOverWritePrevious)
	   {
		 bFailIfExists=FALSE;
	   }
	   else
	   {
		  bFailIfExists=TRUE;
	   }

		bResult=CopyFile(sHostsPath.t_str(),  sBackupFilePath.t_str(), bFailIfExists);
	 }
	 catch(...)
	 {
	 }

	 return bResult;
}


String __fastcall TfrmMain::ReadBlackListFile(String sFile, TStringList *strOut)
{
 if(sFile.IsEmpty() || (strOut==NULL))
 {
  return "";
 }
  TIniFile *tif=NULL;

  String sResult;
  try
  {
	tif=new TIniFile(sFile);
	if(!tif->SectionExists(MAIN_LABEL))
	{
		  WriteLogMessage("ReadBlackListFile: File is not a black list: "+sFile);
	}
	else
	{
	  sResult=tif->ReadString( MAIN_LABEL,"Version","");



	}
  }
  catch(...)
  {
  }

  try
  {
	delete tif;
  }
  catch(...)
  {
  }



  try
  {
	  if(sResult.IsEmpty())
	  {
		  WriteLogMessage("ReadBlackListFile: Version is not found in black list: "+sFile);
	  }
	  else
	  {
		 ReadRawStringsFromIniFile(sFile,"Hosts",strOut);
	  }

  }
  catch(...)
  {
  }



  return sResult;
}


bool __fastcall TfrmMain::ReadRawStringsFromIniFile(String FileName, String Section, TStringList *strOut)
{
 if(Section.IsEmpty())
  return false;
 if(strOut==NULL)
  return false;
  if(FileName.IsEmpty())
   return false;
   if(!FileExists(FileName))
	return false;

	try
    {
     strOut->Clear();
    }
	catch(...)
    {
    }
 bool bResult=false;
 TStringList *str=NULL;
 int i;
 int iBeg=-1;
 String sSectString=  String("["+Section+"]").UpperCase();
  try
  {
   str=new  TStringList;
   str->LoadFromFile(FileName);
   for(i=0;i<str->Count;i++)
   {
    if(str->Strings[i].UpperCase()==sSectString)
    {
      iBeg=i+1;
      break;
    }
   }
   if(iBeg>=0)
   {
     for(i=iBeg;i<str->Count;i++)
     {
      if(str->Strings[i].SubString(1,1)=="[")
      {
       break;
      }
	  if(!str->Strings[i].IsEmpty())
	  {
	   strOut->Add(str->Strings[i].Trim());
	   bResult=true;
	  }
     }
   }
  }
  catch(...)
  {
  }

  delete str;
  return bResult;

}



bool __fastcall TfrmMain::IsLocalVersUptodate(String RemoteVers, String LocalVers)
{
   if(RemoteVers==LocalVers)
	return true;

 String sRemote, sLocal, str;
 String sTemp;
 bool bResult=true;
   int val_remote, val_local;
   int pos_remote, pos_local;
 bool bCompareStrings=false;
 try
 {
   sRemote=RemoteVers.Trim();
   sLocal=LocalVers.Trim();
   do
   {
     pos_remote= sRemote.Pos(".");
     if(pos_remote>0)
     {
       sTemp=sRemote.SubString(1,pos_remote-1);
       sRemote=sRemote.SubString(pos_remote+1,sRemote.Length());
     }
     else
     {
       pos_remote= sRemote.Pos(" ");
       if(pos_remote>0)
       {
        sTemp=sRemote.SubString(1,pos_remote-1);
        sRemote=sRemote.SubString(pos_remote+1,sRemote.Length());

       }
       else
       {
        sTemp=sRemote;
        sRemote="";
       }
     }
     try
     {
      val_remote=sTemp.ToInt();
     }
     catch(...)
     {
       bCompareStrings=true;
       sRemote=sTemp;
     }
     pos_local= sLocal.Pos(".");
     if(pos_local>0)
     {
       sTemp=sLocal.SubString(1,pos_local-1);
       sLocal=sLocal.SubString(pos_local+1,sLocal.Length());
     }
     else
     {
      pos_local= sLocal.Pos(" ");
      if(pos_local>0)
      {
       sTemp=sLocal.SubString(1,pos_local-1);
       sLocal=sLocal.SubString(pos_local+1,sLocal.Length());
      }
      else
      {
       sTemp=sLocal;
       sLocal="";
      }
     }
     try
     {
      val_local=sTemp.ToInt();
     }
     catch(...)
     {
       bCompareStrings=true;
       sLocal=sTemp;
     }
     if(  bCompareStrings)
     {
       if(sLocal.CompareIC(sRemote)<0)
        return false;
       else
        return true;
     }

     if( val_local<val_remote)
      return false;
    if( val_local>val_remote)
      return true;

   }while((!sRemote.IsEmpty()) && (!sLocal.IsEmpty()));

 // bad alg
 }
 catch(...)
 {
 }

  return bResult;
}


String __fastcall TfrmMain::sGetBlackListVersion(String sFile)
{
 if(sFile.IsEmpty())
 {
  return "";
 }
  TIniFile *tif=NULL;

  String sResult;
  try
  {
	tif=new TIniFile(sFile);
	if(!tif->SectionExists(MAIN_LABEL))
	{
		  WriteLogMessage("ReadBlackListFile: File is not a black list: "+sFile);
	}
	else
	{
	  sResult=tif->ReadString( MAIN_LABEL,"Version","");

		  WriteLogMessage("Version: "+sResult+" File: "+sFile);


	}
  }
  catch(...)
  {
  }

  try
  {
	delete tif;
  }
  catch(...)
  {
  }

  return sResult;
}

String __fastcall TfrmMain::sGetBlackListFilePath()
{

 return ExtractFilePath(ParamStr(0))+BLACKLIST_DB;

}

int __fastcall GetInternetRequest(String sURL, String &sOutResult)
{
  CSslConnection inetSec;

   AnsiString sSite;
   AnsiString sParam;
	  sSite=sURL;
	  if(sSite.IsEmpty())
	  {
		  WriteLogMessage("No sURL in GetInternetRequest");
			return -1;

	  }
	  int iResult=-1;
	  int iNumConnect=0;
	  bool bHTPPS=true;
  try
  {
	   WriteLogMessage("Starting GetInternetRequest to: "+sURL);
	   AnsiString sLex="http://";
	   if(sSite.SubString(1,sLex.Length())==sLex)
	   {
		sSite=sSite.SubString(sLex.Length()+1,sSite.Length());
		 bHTPPS=false;
	   }

		sLex="https://";
	   if(sSite.SubString(1,sLex.Length())==sLex)
	   {
		sSite=sSite.SubString(sLex.Length()+1,sSite.Length());
	   }
		int n_pos=sSite.Pos("/");
		if(n_pos>0)
		{
		  sParam=sSite.SubString(n_pos,sSite.Length());
		  sSite=sSite.SubString(1,n_pos-1);
		}

		WriteLogMessage("Server Name: "+sSite);
		WriteLogMessage("Param: "+sParam);

	String sAgentName("Mozilla/4.0");
	String sServerName(sSite.t_str()); //Can be any https server address
	String sUserName("");//if required
	String sPass(""); //if required
	String sObjectName(sParam.c_str());//there should be an object to send a verb

	//You may choose any field of a certificate to perform a context search,
	//i just implemented the OU field of the Issuer here
	String sOrganizationUnitName("");
	//end
	String strVerb = "GET";//My sample verb

	inetSec.SetAgentName(sAgentName);
	inetSec.SetCertStoreType(certStoreMY);
	inetSec.SetObjectName(sObjectName);
	String sHeaders="Accept-Language: en-us";
//	inetSec.SetHeaders(sHeaders);
	inetSec.SetServerName(sServerName);

		if(!bHTPPS)
		{
		 inetSec.SetPort(80);
		}
		else
		{
			inetSec.SetPort(443);//443 is the default HTTPS port

		}
	   /*	 int secureFlags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE;
		 inetSec.SetSecurityFlags(secureFlags);
		 */

CONNECT:
	inetSec.SetRequestID(0);

	   int iTry=0;
	  for(iTry=0; iTry<10; iTry++)
	  {
		if (!inetSec.ConnectToHttpsServer(strVerb))
		{
		  AnsiString sEr=inetSec.m_strLastError.c_str();
			WriteLogMessage( "ConnectToHttpsServer failed: "+sEr +AnsiString(" Code: ") +AnsiString(inetSec.GetLastErrorCode()));
		   Sleep(200);
		}
		else
		{
		 break;
		}
	  }

       bool bUseOnlyHTPP=false;

		if(!inetSec.SendHttpsRequest())
		{
		  AnsiString sEr=inetSec.m_strLastError.c_str();
		  int iError=inetSec.GetLastErrorCode();
			WriteLogMessage(sEr +AnsiString(" Code: ") +AnsiString(iError));
		   if(iError==12029) // SSL does not work
		   {
			if(!bUseOnlyHTPP)
			{
			  bUseOnlyHTPP=true;
			  //  WriteHTTPOnly(true);
			  iNumConnect++;
			  WriteLogMessage("HTPPS does not work. Switch to HTPP...");
				 inetSec.SetPort(80);//443 is the default HTTPS port
				 int secureFlags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE;
				 inetSec.SetSecurityFlags(secureFlags);
				 inetSec.ClearHandles();
				if(iNumConnect<2)
				{
				 goto CONNECT;
				}
			}

		   }
		  // error
		}
		else
		{
		  WriteLogMessage("Successful Connect!");
		  iResult=1;
		}

  sOutResult = inetSec.GetRequestResult();

 }
 catch(...)
 {
 }
FINISH:
   try
   {
	 inetSec.ClearHandles();
   }
   catch(...)
   {
   }

 //  Terminate();
 return iResult;
}


void __fastcall TfrmMain::CheckUpdateBlackList()
{

 MSG Msg;


	FThread =  new TSSLThread(true, URL_BLACKLIST);
	FThread->OnTerminate = ThreadDone;
	FThread->Resume();
   if(FThread)
   {
	 while( PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	 {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	 }
   }

}

void __fastcall TfrmMain::ThreadDone(TObject *Sender)
{
 try
 {
  if(FThread!=NULL)
  {
	WriteLogMessage("Update thread finished. Returned (bytes): "+String(FThread->FResult.Length()));

	 int iResult=FThread->iResult;

	  if( UpdateLocalBlackListFromRemote(FThread->FResult))
	  {
		bNeedUpdateHosts=true;
		DisplayStatus(DISP_UPDATE_HOSTS);
	  }
  }
 }
 catch(...)
 {
 }

	FThread=NULL;

}
void __fastcall TfrmMain::DisplayStatus(int iStatus)
{
  switch(iStatus)
  {
   case DISP_ALL_DONE:
				break;
   case DISP_INSTALL_TO_HOSTS:
				break;
   case DISP_UPDATE_HOSTS:
				break;
   case DISP_GET_BLACKLIST:
				 break;

  }
}

bool __fastcall TfrmMain::UpdateLocalBlackListFromRemote(String sRemoteText)
{
TStringList *str=NULL;
bool bResult=false;
 try
 {
	str=new TStringList;
	str->Text= sRemoteText;
	 sRemoteBlackListVersion= sGetDBVersionFromStrings(str);

	 WriteLogMessage("DB Version found in text: "+sRemoteBlackListVersion);

	 if(!IsLocalVersUptodate(sRemoteBlackListVersion,sLocalBlackListVersion))
	 {
	   WriteLogMessage("Remote version is newer than local. We must update local blacklist!");
	   String sFile=sGetBlackListFilePath();
	   try
	   {
		str->SaveToFile(sFile);
		bResult=true;
	   }
	   catch(...)
	   {
		  WriteLogMessage("Exception! Save to file: "+sFile);
	   }
	 }
	 else
	 {
	   WriteLogMessage("Local version is up-to-date: "+ sLocalBlackListVersion+" Remote version: "+sRemoteBlackListVersion);

	 }
 }
 catch(...)
 {
 }
 delete str;
return bResult;
}
String __fastcall TfrmMain::sGetDBVersionFromStrings(TStringList *str)
{
 if(str==NULL)
 {
   return "";
 }

  String sResult;
   try
   {
	 bool bFoundChapater=false;
	 for(int i=0;i<str->Count;i++)
	 {
	   String s=str->Strings[i].Trim();

	   if(s.IsEmpty())
	   {
		continue;
	   }

	   if( bFoundChapater)
	   {
		  int n_pos=s.Pos("=");
		  if(n_pos>0)
		  {
			String sName=s.SubString(1,n_pos-1);
			if(sName.CompareIC("Version")==0)
			{
			   sResult=s.SubString(n_pos+1,s.Length());
			   break;
			}
		  }
	   }
	   else
	   {
			if(s=="["+MAIN_LABEL+"]")
			{
			   bFoundChapater=true;
			}

	   }
	 }

   }
   catch(...)
   {
   }

   return sResult;
}


__fastcall TSSLThread::TSSLThread(bool CreateSuspended, String URL)
		: TThread(CreateSuspended)
{
  FreeOnTerminate=true;
  FURL=URL;

 if(!CreateSuspended)
  Resume();

}



 void __fastcall TSSLThread::Execute()
 {
   iResult= GetInternetRequest(FURL, FResult);
	 Terminate();

 }
void __fastcall TfrmMain::timUpdateTimer(TObject *Sender)
{
	timUpdate->Enabled=false;
	 CheckUpdateBlackList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnExitClick(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnUninstallClick(TObject *Sender)
{
  if(IsInstalledIntoHosts())
  {
     UninstallFromHosts();
  }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnCheckupdateClick(TObject *Sender)
{
  timUpdate->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnRefreshClick(TObject *Sender)
{
   ReadMainStatus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnDisplayLogClick(TObject *Sender)
{
  ShellExecute(NULL,L"open",LogFileName.t_str(),NULL,NULL,SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::mnVisitHomePageClick(TObject *Sender)
{

  ShellExecute(NULL,L"open",L"https://github.com/greatis/Anti-WebMiner",NULL,NULL,SW_SHOW);

}
//---------------------------------------------------------------------------

