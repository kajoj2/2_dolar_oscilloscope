// Borland C++ Builder
// Copyright (c) 1995, 2005 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Gnugettext.pas' rev: 10.00

#ifndef GnugettextHPP
#define GnugettextHPP

#if 0

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Strutils.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Typinfo.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Gnugettext
{
//-- type declarations -------------------------------------------------------
typedef WideString UnicodeString;

typedef AnsiString RawUtf8String;

typedef AnsiString RawByteString;

typedef AnsiString DomainString;

typedef AnsiString LanguageString;

typedef AnsiString ComponentNameString;

typedef AnsiString FilenameString;

typedef WideString MsgIdString;

typedef WideString TranslatedUnicodeString;

typedef void __fastcall (__closure *TTranslator)(System::TObject* obj);

class DELPHICLASS EGnuGettext;
class PASCALIMPLEMENTATION EGnuGettext : public Sysutils::Exception 
{
	typedef Sysutils::Exception inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EGnuGettext(const AnsiString Msg) : Sysutils::Exception(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EGnuGettext(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EGnuGettext(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EGnuGettext(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EGnuGettext(const AnsiString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EGnuGettext(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EGnuGettext(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EGnuGettext(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EGnuGettext(void) { }
	#pragma option pop
	
};


class DELPHICLASS EGGProgrammingError;
class PASCALIMPLEMENTATION EGGProgrammingError : public EGnuGettext 
{
	typedef EGnuGettext inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EGGProgrammingError(const AnsiString Msg) : EGnuGettext(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EGGProgrammingError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EGGProgrammingError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EGGProgrammingError(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EGGProgrammingError(const AnsiString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EGGProgrammingError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EGGProgrammingError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGProgrammingError(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EGGProgrammingError(void) { }
	#pragma option pop
	
};


class DELPHICLASS EGGComponentError;
class PASCALIMPLEMENTATION EGGComponentError : public EGnuGettext 
{
	typedef EGnuGettext inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EGGComponentError(const AnsiString Msg) : EGnuGettext(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EGGComponentError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EGGComponentError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EGGComponentError(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EGGComponentError(const AnsiString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EGGComponentError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EGGComponentError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGComponentError(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EGGComponentError(void) { }
	#pragma option pop
	
};


class DELPHICLASS EGGIOError;
class PASCALIMPLEMENTATION EGGIOError : public EGnuGettext 
{
	typedef EGnuGettext inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EGGIOError(const AnsiString Msg) : EGnuGettext(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EGGIOError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EGGIOError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EGGIOError(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EGGIOError(const AnsiString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EGGIOError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EGGIOError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGIOError(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EGGIOError(void) { }
	#pragma option pop
	
};


class DELPHICLASS EGGAnsi2WideConvError;
class PASCALIMPLEMENTATION EGGAnsi2WideConvError : public EGnuGettext 
{
	typedef EGnuGettext inherited;
	
public:
	#pragma option push -w-inl
	/* Exception.Create */ inline __fastcall EGGAnsi2WideConvError(const AnsiString Msg) : EGnuGettext(Msg) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmt */ inline __fastcall EGGAnsi2WideConvError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size) : EGnuGettext(Msg, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateRes */ inline __fastcall EGGAnsi2WideConvError(int Ident)/* overload */ : EGnuGettext(Ident) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmt */ inline __fastcall EGGAnsi2WideConvError(int Ident, System::TVarRec const * Args, const int Args_Size)/* overload */ : EGnuGettext(Ident, Args, Args_Size) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateHelp */ inline __fastcall EGGAnsi2WideConvError(const AnsiString Msg, int AHelpContext) : EGnuGettext(Msg, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateFmtHelp */ inline __fastcall EGGAnsi2WideConvError(const AnsiString Msg, System::TVarRec const * Args, const int Args_Size, int AHelpContext) : EGnuGettext(Msg, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResHelp */ inline __fastcall EGGAnsi2WideConvError(int Ident, int AHelpContext)/* overload */ : EGnuGettext(Ident, AHelpContext) { }
	#pragma option pop
	#pragma option push -w-inl
	/* Exception.CreateResFmtHelp */ inline __fastcall EGGAnsi2WideConvError(System::PResStringRec ResStringRec, System::TVarRec const * Args, const int Args_Size, int AHelpContext)/* overload */ : EGnuGettext(ResStringRec, Args, Args_Size, AHelpContext) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~EGGAnsi2WideConvError(void) { }
	#pragma option pop
	
};


typedef void __fastcall (__closure *TOnDebugLine)(System::TObject* Sender, const AnsiString Line, bool &Discard);

typedef int __fastcall (*TGetPluralForm)(int Number);

typedef void __fastcall (__closure *TDebugLogger)(AnsiString line);

class DELPHICLASS TMoFile;
class PASCALIMPLEMENTATION TMoFile : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	bool doswap;
	
public:
	int Users;
	__fastcall TMoFile(const AnsiString filename, const __int64 Offset, __int64 Size, const bool xUseMemoryMappedFiles);
	__fastcall virtual ~TMoFile(void);
	AnsiString __fastcall gettext(const AnsiString msgid, bool &found);
	__property bool isSwappedArchitecture = {read=doswap, nodefault};
	
private:
	unsigned N;
	unsigned O;
	unsigned T;
	int startindex;
	int startstep;
	bool FUseMemoryMappedFiles;
	unsigned mo;
	unsigned momapping;
	char *momemoryHandle;
	char *momemory;
	unsigned __fastcall autoswap32(unsigned i);
	unsigned __fastcall CardinalInMem(char * baseptr, unsigned Offset);
};


class DELPHICLASS TDomain;
class PASCALIMPLEMENTATION TDomain : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	bool Enabled;
	AnsiString vDirectory;
	void __fastcall setDirectory(const AnsiString dir);
	
public:
	TDebugLogger DebugLogger;
	AnsiString Domain;
	__property AnsiString Directory = {read=vDirectory, write=setDirectory};
	__fastcall TDomain(void);
	__fastcall virtual ~TDomain(void);
	void __fastcall SetLanguageCode(const AnsiString langcode);
	void __fastcall SetFilename(const AnsiString filename);
	void __fastcall GetListOfLanguages(Classes::TStrings* list);
	WideString __fastcall GetTranslationProperty(AnsiString Propertyname);
	AnsiString __fastcall gettext(const AnsiString msgid);
	
private:
	TMoFile* mofile;
	AnsiString SpecificFilename;
	AnsiString curlang;
	bool OpenHasFailedBefore;
	void __fastcall OpenMoFile(void);
	void __fastcall CloseMoFile(void);
};


class DELPHICLASS TExecutable;
class PASCALIMPLEMENTATION TExecutable : public System::TObject 
{
	typedef System::TObject inherited;
	
public:
	virtual void __fastcall Execute(void) = 0 ;
public:
	#pragma option push -w-inl
	/* TObject.Create */ inline __fastcall TExecutable(void) : System::TObject() { }
	#pragma option pop
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TExecutable(void) { }
	#pragma option pop
	
};


class DELPHICLASS TGnuGettextInstance;
class PASCALIMPLEMENTATION TGnuGettextInstance : public System::TObject 
{
	typedef System::TObject inherited;
	
private:
	TOnDebugLine fOnDebugLine;
	unsigned CreatorThread;
	
public:
	bool Enabled;
	int DesignTimeCodePage;
	__fastcall TGnuGettextInstance(void);
	__fastcall virtual ~TGnuGettextInstance(void);
	void __fastcall UseLanguage(AnsiString LanguageCode);
	void __fastcall GetListOfLanguages(const AnsiString domain, Classes::TStrings* list);
	virtual WideString __fastcall gettext(const AnsiString szMsgId)/* overload */;
	virtual WideString __fastcall ngettext(const AnsiString singular, const AnsiString plural, int Number)/* overload */;
	virtual WideString __fastcall gettext(const WideString szMsgId)/* overload */;
	WideString __fastcall gettext_NoExtract(const WideString szMsgId);
	WideString __fastcall gettext_NoOp(const WideString szMsgId);
	virtual WideString __fastcall ngettext(const WideString singular, const WideString plural, int Number)/* overload */;
	WideString __fastcall ngettext_NoExtract(const WideString singular, const WideString plural, int Number);
	AnsiString __fastcall GetCurrentLanguage();
	WideString __fastcall GetTranslationProperty(const AnsiString Propertyname);
	WideString __fastcall GetTranslatorNameAndEmail();
	void __fastcall TP_Ignore(System::TObject* AnObject, const AnsiString name);
	void __fastcall TP_IgnoreClass(TMetaClass* IgnClass);
	void __fastcall TP_IgnoreClassProperty(TMetaClass* IgnClass, AnsiString propertyname);
	void __fastcall TP_GlobalIgnoreClass(TMetaClass* IgnClass);
	void __fastcall TP_GlobalIgnoreClassProperty(TMetaClass* IgnClass, AnsiString propertyname);
	void __fastcall TP_GlobalHandleClass(TMetaClass* HClass, TTranslator Handler);
	void __fastcall TranslateProperties(System::TObject* AnObject, AnsiString textdomain = "");
	void __fastcall TranslateComponent(Classes::TComponent* AnObject, const AnsiString TextDomain = "");
	void __fastcall RetranslateComponent(Classes::TComponent* AnObject, const AnsiString TextDomain = "");
	virtual WideString __fastcall dgettext(const AnsiString szDomain, const AnsiString szMsgId)/* overload */;
	virtual WideString __fastcall dngettext(const AnsiString szDomain, const AnsiString singular, const AnsiString plural, int Number)/* overload */;
	virtual WideString __fastcall dgettext(const AnsiString szDomain, const WideString szMsgId)/* overload */;
	WideString __fastcall dgettext_NoExtract(const AnsiString szDomain, const WideString szMsgId);
	virtual WideString __fastcall dngettext(const AnsiString szDomain, const WideString singular, const WideString plural, int Number)/* overload */;
	WideString __fastcall dngettext_NoExtract(const AnsiString szDomain, const WideString singular, const WideString plural, int Number);
	void __fastcall textdomain(const AnsiString szDomain);
	AnsiString __fastcall getcurrenttextdomain();
	void __fastcall bindtextdomain(const AnsiString szDomain, const AnsiString szDirectory);
	void __fastcall bindtextdomainToFile(const AnsiString szDomain, const AnsiString filename);
	WideString __fastcall LoadResString(System::PResStringRec ResStringRec);
	void __fastcall DebugLogToFile(const AnsiString filename, bool append = false);
	void __fastcall DebugLogPause(bool PauseEnabled);
	__property TOnDebugLine OnDebugLine = {read=fOnDebugLine, write=fOnDebugLine};
	WideString __fastcall ansi2wideDTCP(const AnsiString s);
	
protected:
	void __fastcall TranslateStrings(Classes::TStrings* sl, const AnsiString TextDomain);
	virtual void __fastcall WhenNewLanguage(const AnsiString LanguageID);
	virtual void __fastcall WhenNewDomain(const AnsiString TextDomain);
	virtual void __fastcall WhenNewDomainDirectory(const AnsiString TextDomain, const AnsiString Directory);
	
private:
	AnsiString curlang;
	TGetPluralForm curGetPluralForm;
	AnsiString curmsgdomain;
	Sysutils::TMultiReadExclusiveWriteSynchronizer* savefileCS;
	TextFile savefile;
	Classes::TStringList* savememory;
	AnsiString DefaultDomainDirectory;
	Classes::TStringList* domainlist;
	Classes::TStringList* TP_IgnoreList;
	Classes::TList* TP_ClassHandling;
	Classes::TList* TP_GlobalClassHandling;
	TExecutable* TP_Retranslator;
	TExecutable* __fastcall TP_CreateRetranslator(void);
	void __fastcall FreeTP_ClassHandlingItems(void);
	void __fastcall TranslateProperty(System::TObject* AnObject, Typinfo::PPropInfo PropInfo, Classes::TStrings* TodoList, const AnsiString TextDomain);
	TDomain* __fastcall Getdomain(const AnsiString domain, const AnsiString DefaultDomainDirectory, const AnsiString CurLang);
};


//-- var, const, procedure ---------------------------------------------------
#define DefaultTextDomain "default"
extern PACKAGE AnsiString ExecutableFilename;
static const bool PreferExternal = false;
static const bool UseMemoryMappedFiles = true;
static const bool ReReadMoFileOnSameLanguage = false;
#define VCSVersion "$LastChangedRevision: 220 $"
static const bool AutoCreateHooks = true;
//static const Shortint LOCALE_SISO639LANGNAME = 0x59;
//static const Shortint LOCALE_SISO3166CTRYNAME = 0x5a;
extern PACKAGE TGnuGettextInstance* DefaultInstance;
extern PACKAGE WideString __fastcall gettext(const WideString szMsgId);
extern PACKAGE WideString __fastcall gettext_NoExtract(const WideString szMsgId);
extern PACKAGE WideString __fastcall gettext_NoOp(const WideString szMsgId);
extern PACKAGE WideString __fastcall _(const WideString szMsgId);
extern PACKAGE WideString __fastcall dgettext(const AnsiString szDomain, const WideString szMsgId);
extern PACKAGE WideString __fastcall dgettext_NoExtract(const AnsiString szDomain, const WideString szMsgId);
extern PACKAGE WideString __fastcall dngettext(const AnsiString szDomain, const WideString singular, const WideString plural, int Number);
extern PACKAGE WideString __fastcall ngettext(const WideString singular, const WideString plural, int Number);
extern PACKAGE WideString __fastcall ngettext_NoExtract(const WideString singular, const WideString plural, int Number);
extern PACKAGE void __fastcall textdomain(const AnsiString szDomain);
extern PACKAGE AnsiString __fastcall getcurrenttextdomain();
extern PACKAGE void __fastcall bindtextdomain(const AnsiString szDomain, const AnsiString szDirectory);
extern PACKAGE void __fastcall TP_Ignore(System::TObject* AnObject, const AnsiString name);
extern PACKAGE void __fastcall TP_GlobalIgnoreClass(TMetaClass* IgnClass);
extern PACKAGE void __fastcall TP_IgnoreClass(TMetaClass* IgnClass);
extern PACKAGE void __fastcall TP_IgnoreClassProperty(TMetaClass* IgnClass, const AnsiString propertyname);
extern PACKAGE void __fastcall TP_GlobalIgnoreClassProperty(TMetaClass* IgnClass, const AnsiString propertyname);
extern PACKAGE void __fastcall TP_GlobalHandleClass(TMetaClass* HClass, TTranslator Handler);
extern PACKAGE void __fastcall TranslateComponent(Classes::TComponent* AnObject, const AnsiString TextDomain = "");
extern PACKAGE void __fastcall RetranslateComponent(Classes::TComponent* AnObject, const AnsiString TextDomain = "");
extern PACKAGE WideString __fastcall GetTranslatorNameAndEmail();
extern PACKAGE void __fastcall UseLanguage(AnsiString LanguageCode);
extern PACKAGE WideString __fastcall LoadResString(System::PResStringRec ResStringRec);
extern PACKAGE WideString __fastcall LoadResStringW(System::PResStringRec ResStringRec);
extern PACKAGE AnsiString __fastcall GetCurrentLanguage();
extern PACKAGE void __fastcall AddDomainForResourceString(const AnsiString domain);
extern PACKAGE void __fastcall RemoveDomainForResourceString(const AnsiString domain);
extern PACKAGE void __fastcall HookIntoResourceStrings(bool enabled = true, bool SupportPackages = false);

}	/* namespace Gnugettext */
using namespace Gnugettext;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------

#else
	#include <System.hpp>
	#define _(String) String
#endif

#endif	// Gnugettext
