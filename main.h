//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

class TSSLThread : public TThread
{
private:
   String FURL;

protected:
		void __fastcall TSSLThread::Execute();
public:
  __fastcall TSSLThread::TSSLThread(bool CreateSuspended, String URL);

   String FResult;
   int iResult;

};

class TfrmMain : public TForm
{
__published:	// IDE-managed Components
private:	// User declarations

		  TStringList *strHosts;
		  String sDBVersion;
		  String sHostsPath;

		  TSSLThread *FThread;

		  String __fastcall TfrmMain::sExpandEnvStrings(String sSource);
		  String __fastcall TfrmMain::sGetHostsPath();
		  String __fastcall TfrmMain::AddLastSlash(String sSource);
		  bool __fastcall TfrmMain::ReadHostsFile(String sFilePath, TStringList *strOut);

		  int __fastcall TfrmMain::sGetOurStartPosInHosts();
		  int __fastcall TfrmMain::sGetOurFinalPosInHosts(int iStart);

		  String __fastcall TfrmMain::sGetOurVersionFromHosts(int iStartRow, int &iOutInstallDate);

		  bool __fastcall TfrmMain::AddServerListTopHostsStrings(TStringList *strIn);
		  bool __fastcall TfrmMain::RemoveOurStringsFromHostsStrings(int iStart, int iFinish);
		  bool __fastcall TfrmMain::BackupHostsFile(bool bOverWritePrevious);
		  bool __fastcall TfrmMain::SaveHostsFile();
		  bool __fastcall TfrmMain::ReadRawStringsFromIniFile(String FileName, String Section, TStringList *strOut);

		  String __fastcall TfrmMain::ReadBlackListFile(String sFile, TStringList *strOut);
		  bool __fastcall TfrmMain::IsRemoteVersLarger(String RemoteVers, String LocalVers);
		  String __fastcall TfrmMain::sGetBlackListVersion(String sFile);
		  String __fastcall TfrmMain::sGetBlackListFilePath();
		  void __fastcall TfrmMain::CheckUpdate();
		  void __fastcall TfrmMain::ThreadDone(TObject *Sender);
		  String __fastcall TfrmMain::sGetDBVersionFromStrings(TStringList *str);


public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	__fastcall TfrmMain::~TfrmMain();

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------

int __fastcall GetInternetRequest(String sURL, String &sOutResult);
void __fastcall WriteLogMessage(AnsiString sText);

#define MAIN_LABEL  String("Anti-WebMiner")
#define START_LABEL MAIN_LABEL+String(" Start")
#define FIN_LABEL MAIN_LABEL+String(" End")

#define BLACKLIST_DB String("blacklist.txt")

#define URL_BLACKLIST String("https://raw.githubusercontent.com/greatis/Anti-WebMiner/master/blacklist.txt")



#endif
