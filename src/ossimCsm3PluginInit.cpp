//----------------------------------------------------------------------------
//
// License:  See top level LICENSE.txt file
//
// Author:  cchuah
//
// DESCRIPTION:
//   Implementation of class ossimCsm3PluginInit
//
//**************************************************************************************************
//  $Id: ossimCsm3PluginInit.cpp 1579 2015-06-08 17:09:57Z cchuah $

#include "ossimCsm3ProjectionFactory.h"
#include "ossimCsm3Loader.h"
#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/plugin/ossimSharedObjectBridge.h>
#include <ossim/projection/ossimProjectionFactoryRegistry.h>
#include <csm/Plugin.h>

#include <vector>
#include <sstream>

extern "C"
{
static ossimSharedObjectInfo  myCsm3Info;
static ossimString theCsm3Description;
static std::vector<ossimString> theCsm3ObjList;

static const char* getCsm3Description()
{
   if (theCsm3Description.empty())
   {
      std::ostringstream out;
      out  << "Community Sensor Model 3.0.1 Plugin. ";
      ossimCsm3Loader::List plugins;
      ossimCsm3Loader::List sensors;

      ossimCsm3Loader::getAvailablePluginNames(plugins);

      if(!plugins.empty())
      {
        out << "\nAvailable plugins are: \n\n";
        for(auto plugin:plugins)
        {
            out << plugin << "\n";
            sensors.clear();
            ossimCsm3Loader::getAvailableSensorModelNames(sensors, plugin);
            if(sensors.size())
            {
                out << "     sensors: \n";
            }
            for(auto sensor:sensors)
            {
                out << "     " << sensor << "\n";
            }
        }  
      }
      else
      {
        out << "No plugins were found in directory.\n\n";
      }
    
      theCsm3Description = out.str();
   }
   return theCsm3Description.c_str();
}

static int getCsm3NumberOfClassNames()
{
   return (int)theCsm3ObjList.size();
}

static const char* getCsm3ClassName(int idx)
{
   if(idx < (int)theCsm3ObjList.size())
   {
      return theCsm3ObjList[idx].c_str();
   }
   return (const char*)0;
}

OSSIM_PLUGINS_DLL void ossimSharedLibraryInitialize(
      ossimSharedObjectInfo** info,  const char* /* options */ )
{
   theCsm3ObjList.push_back("ossimCsm3SensorModel");

   myCsm3Info.getDescription = getCsm3Description;
   myCsm3Info.getNumberOfClassNames = getCsm3NumberOfClassNames;
   myCsm3Info.getClassName = getCsm3ClassName;

   *info = &myCsm3Info;

   /* Register the ProjectionFactory */
   ossimProjectionFactoryRegistry::instance()->
         registerFactoryToFront(ossimCsm3ProjectionFactory::instance());
}

OSSIM_PLUGINS_DLL void ossimSharedLibraryFinalize()
{
   ossimProjectionFactoryRegistry::instance()->
         unregisterFactory(ossimCsm3ProjectionFactory::instance());
}
}
