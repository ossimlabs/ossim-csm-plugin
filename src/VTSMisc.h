//#############################################################################
//
//    FILENAME:   VTSMisc.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the constants and other definitions used by the CSM VTS.
//
//    LIMITATIONS:       None
// 
//    SOFTWARE HISTORY:
//       Date       Author Comment    
//      13-Jan-2004  LMT   Initial version.
//      22 Jul 2010  DSL   CCB Change add writeStateFile and readStateFile
//      08 Feb 2011  DSL   Add capability to process large files (2G - 4G)
//      29 Jun 2011  DSL   Add thread testing 
//      29 Aug 2011  DSL   Handle large pointers to DES records at the end of large files 
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
//
//#############################################################################
#ifndef __VTSMISC_H
#define __VTSMISC_H


#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include <csm/csm.h>
#include <csm/Error.h>
#include <csm/Warning.h>
#include <csm/BytestreamIsd.h>
#include <csm/NitfIsd.h>
#include "constants.h"

#include <ossim/plugin/ossimPluginConstants.h>

#include <vector>
#include <list>
#include <string>

using namespace csm; 
//class Plugin;

#define MAXINPUTPARAMETERS      30
#define MAXOUTPUTPARAMETERS     200
#define TEMP_STRING_LENGTH 2048

#ifdef _WIN32
#define STATTYPE stat
#else
#define STATTYPE stat64
#endif

extern std::list<std::string> commandQueue;
extern std::vector<std::string> menulist;
extern std::vector<MENU_TYPE> menutype;
extern bool debugFlag;
extern bool compareDebugFlag;
extern bool nitfDebugFlag;
extern WarningList warnings;

class SegmentlengthInfo
{
 public:
  int seghdrlength;
  int segdatalength;
};
    
enum ISDType    
  {  NO_FILE_TYPE,
     NITF_20_TYPE,
     NITF_21_TYPE,
     BYTESTREAM_TYPE,
     FILENAME_TYPE
  }; 

std::string ISDType2str(ISDType type);

typedef std::list < std::string >      NameList;

void initMenuList();
void trim(std::string& str);

void printList(std::string logFile);
// pre: None.
// post: The std::list of all currently-registered factories has been printed.
void printModelList(std::string pluginName);

void makeModelListFromISD(const std::string isd_name, const Isd * isd, WarningList* warning = NULL);

// pre: None.
// post: A subset of aList has been printed; the subset contains all
//    factories that can support the given isd.

void listSupportingModels(const std::string isd_name,
				  const Isd* isd,
				  NameList& names,
				  WarningList* warnings);

void compareParam(std::vector<std::string> param_array,
		  std::vector<std::string> act_val,
		  std::vector<bool> &pass_fail);

void compareParam(std::vector<std::string> param_array,
		  std::map<int, double> act_val,
		  std::map<int, double> &comp_answer,
		  std::map<int, bool> &pass_fail);

void compareParam(std::vector<std::string> param_array,
		  double *act_val,
		  double *comp_answer,
		  bool *pass_fail);

void compareParam(std::vector<std::string> param_array,
		  std::vector<std::string> act_val,
		  std::map<int, bool> &pass_fail);

void compareParam(std::vector<std::string> param_array,
		  std::map<int, double> act_val,
		  std::map<int, double> &comp_answer,
		  std::map<int, bool> &pass_fail);

// recordLog I
void recordLog(std::string datafile,
               std::string comment,
               std::string command,         
               std::vector<std::string> param_array, 
               std::vector<std::string> out_param_array);

// recordLog II
void recordLog(std::string datafile,
			   std::string serialNum,
               std::string timedata,
               std::string comment,
               std::string command,         
               std::vector<std::string> param_array,
               std::vector<std::string> out_param_array);

// recordLog III
void recordLog(std::string datafile,
               std::string comment,
               std::string command,
               std::string text);

// recordLog IV
void recordLog(std::string datafile,
               std::string serialNum,
               std::string timedata,
               std::string comment,
               std::string command,
               std::vector<std::string> param_array,
               std::string text);

// recordLog V
void recordLog(std::string datafile,
               std::string comment,
               std::string text);

// recordLog VI
void recordLog(std::string datafile,
               const std::string text);

void echo2Term(std::string comment,
               std::string command,
               std::string text);

void getLogfileName(const std::string dirName, std::string *name);

void reportWarning(WarningList warnings, const char* logFile);
void reportError(Error* err, const char* logFile);

// prototypes for ISD "constructors"
//void initFilenameISD (Isd *isdfilename, 
//		      std::string isd);

void initBytestreamISD(BytestreamIsd *bytestream, std::string filename)  
     throw (Error);

OSSIM_PLUGINS_DLL void initNitf20ISD(Nitf20Isd *isd,
                   std::string fname,
                   const int imageIndex = -1,
				   WarningList* warnings = NULL);

OSSIM_PLUGINS_DLL void initNitf21ISD(Nitf21Isd *isd,
                   std::string fname,
                   const int imageIndex = -1,
				   WarningList* warnings = NULL);

FILE * fillBuff (std::string fname,
#ifdef _WIN32
                    struct stat &statbuf, 
#else
                    struct stat64 &statbuf,
#endif
                    char **buff) throw (Error);

std::string readStateFile( std::string fname) throw (Error);
void writeStateFile(std::string fname, std::string state) throw (Error);

void parseFile(Nitf20Isd *isd,
               FILE *ifile,
#ifdef _WIN32
               struct stat &statbuf,
#else
               struct stat64 &statbuf,
#endif
               char* buff,
               const int imageIndex,
		       WarningList* warnings);

void parseFile(Nitf21Isd *isd,
               FILE *ifile,
#ifdef _WIN32
               struct stat &statbuf,
#else
               struct stat64 &statbuf,
#endif
               char* buff,
               const int imageIndex,
		       WarningList* warnings);

void parseImages (Nitf20Isd *isd, 
                  FILE *ifile,
                  const int hl, 
                  const std::vector <size_t> imagehdrlengths, 
                  const std::vector <size_t>imagelengths,
                  const int imageIndex,
				  const int NUMI);

void parseImages (Nitf21Isd *isd, 
                  FILE *ifile,
                  const int hl, 
                  const std::vector <size_t> imagehdrlengths, 
                  const std::vector <size_t>imagelengths,
                  const int imageIndex,
				  const int NUMI);

std::vector<Tre> parseTre(int tlen, // length of tre data
	      std::string treData);  //  input tre data

void parseDes(Nitf20Isd *isd, 
              FILE *ifile,
              size_t des_offset, 
              const std::vector <size_t> hdrlengths, 
              const std::vector <size_t>datalengths,
			  const int NUMD);

void parseDes(Nitf21Isd *isd, 
              FILE *ifile,
              size_t des_offset, 
              const std::vector <size_t> hdrlengths, 
              const std::vector <size_t>datalengths,
			  const int NUMD);

char* getSegment( FILE *ifile,
                  const size_t offset,
		  const size_t bufferSize)  throw (Error);

void dumpHdr(Nitf20Isd *isd);

void dumpHdr(Nitf21Isd *isd);

int getCommandIndex(std::string command);
std::vector<std::string> loadHelpFile();
void set_keypress(void);
void reset_keypress(void);
std::string readDataLine();
std::vector<std::string> getCommands(std::string params);
#endif // VTSMISC_H

