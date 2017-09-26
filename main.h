//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
private:	// User declarations

		  String LogFileName;
		  bool bDebugMode;
		  TStringList *strHosts;
		  String sDBVersion;
		  String sHostsPath;

		  String __fastcall TfrmMain::sExpandEnvStrings(String sSource);
		  String __fastcall TfrmMain::sGetHostsPath();
		  String __fastcall TfrmMain::AddLastSlash(String sSource);
		  bool __fastcall TfrmMain::ReadHostsFile(String sFilePath, TStringList *strOut);

		  int __fastcall TfrmMain::sGetOurStartPosInHosts();
		  int __fastcall TfrmMain::sGetOurFinalPosInHosts(int iStart);

		  String __fastcall TfrmMain::sGetOurVersionFromHosts(int iStart);
		  bool __fastcall TfrmMain::AddServerListTopHostsStrings(TStringList *strIn);
		  bool __fastcall TfrmMain::RemoveOurStringsFromHostsStrings(int iStart, int iFinish);
		  bool __fastcall TfrmMain::BackupHostsFile(bool bOverWritePrevious);
		  bool __fastcall TfrmMain::SaveHostsFile();



public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	__fastcall TfrmMain::~TfrmMain();

	void __fastcall TfrmMain::WriteLogMessage(AnsiString sText);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#define MAIN_LABEL  String("Anti-WebMiner")
#define START_LABEL MAIN_LABEL+String(" Start")
#define FIN_LABEL MAIN_LABEL+String(" End")
#endif
