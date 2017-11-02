//----------------------------------------------------------------------------
//
// License:  See top level LICENSE.txt file
//
// Author:  cchuah
//
// DESCRIPTION:
//   Implementation of class ossimCsmPluginInit
//
//**************************************************************************************************
//  $Id: ossimCsmPluginInit.cpp 1579 2015-06-08 17:09:57Z cchuah $

#include "ossimCsmProjectionFactory.h"
#include "ossimCsmLoader.h"
#include <ossim/base/ossimRegExp.h>
#include <ossim/base/ossimPreferences.h>
#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/plugin/ossimSharedObjectBridge.h>
#include <ossim/projection/ossimProjectionFactoryRegistry.h>
#include <csm/Plugin.h>

#include <vector>
#include <sstream>
#if OSSIM_HAS_MSP
#include <SensorModel/SensorModelService.h>
#endif

extern "C"
{
static ossimSharedObjectInfo  myCsmInfo;
static ossimString theCsmDescription;
static std::vector<ossimString> theCsmObjList;

static const char* getCsmDescription()
{
   if (theCsmDescription.empty())
   {
      std::ostringstream out;
      out  << "Community Sensor Model Plugin. Written for CSM spec 3.0.1";
      ossimCsmLoader::List plugins;
      ossimCsmLoader::List sensors;

      ossimCsmLoader::getAvailablePluginNames(plugins);

      if(!plugins.empty())
      {
        out << "\nAvailable plugins are: \n\n";
        for(auto plugin:plugins)
        {
            out << plugin << "\n";
            sensors.clear();
            ossimCsmLoader::getAvailableSensorModelNames(sensors, plugin);
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
        out << "No plugins were available.\n\n";
      }
      out << "Keywords and values:\n"
          << "  ossim.plugins.csm.enable_plugins:  " << ossimString(ossimPreferences::instance()->findPreference("ossim.plugins.csm.enable_plugins")) << "\n"
          << "  ossim.plugins.csm.disable_plugins: " << ossimString(ossimPreferences::instance()->findPreference("ossim.plugins.csm.disable_plugins")) << "\n"
          << "  ossim.plugins.csm.plugin_path:     " << ossimString(ossimPreferences::instance()->findPreference("ossim.plugins.csm.plugin_path")) << "\n";

      theCsmDescription = out.str();
   }
   return theCsmDescription.c_str();
}

static int getCsmNumberOfClassNames()
{
   return (int)theCsmObjList.size();
}

static const char* getCsmClassName(int idx)
{
   if(idx < (int)theCsmObjList.size())
   {
      return theCsmObjList[idx].c_str();
   }
   return (const char*)0;
}

OSSIM_PLUGINS_DLL void ossimSharedLibraryInitialize(
      ossimSharedObjectInfo** info,  const char* /* options */ )
{
   theCsmObjList.push_back("ossimCsmSensorModel");

   myCsmInfo.getDescription = getCsmDescription;
   myCsmInfo.getNumberOfClassNames = getCsmNumberOfClassNames;
   myCsmInfo.getClassName = getCsmClassName;

   *info = &myCsmInfo;
  ossimCsmLoader::init();
   /* Register the ProjectionFactory */
  ossimProjectionFactoryRegistry::instance()->
         registerFactoryToFront(ossimCsmProjectionFactory::instance());

}

OSSIM_PLUGINS_DLL void ossimSharedLibraryFinalize()
{
   ossimProjectionFactoryRegistry::instance()->
         unregisterFactory(ossimCsmProjectionFactory::instance());
}
}
