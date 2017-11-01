//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include "ossimCsmLoader.h"
#include "ossimCsmSensorModel.h"
#include <ossim/plugin/ossimDynamicLibrary.h>
#include <ossim/base/ossimTrace.h>
#include <ossim/base/ossimPreferences.h>
#include <ossim/base/ossimDirectory.h>
#include <ossim/base/ossimNotify.h>
#include <csm/Plugin.h>
#include <csm/NitfIsd.h>
#include "VTSMisc.h"
#include <ossim/base/ossimRegExp.h>
#include <mutex>
#include <memory>

#if OSSIM_HAS_MSP
#include <SensorModel/SensorModelService.h>
#endif

#ifdef _WIN32
#include <windows.h>
# include <io.h>
#else
# include <cstdio>  // for sprintf
# include <cstring> // for strstr
# include <dirent.h>
# include <dlfcn.h> // for dlopen, dlclose
# include <errno.h> // for errno
#endif

static ossimTrace traceDebug("ossimCsmLoader:debug");

using namespace csm;
using namespace std;

#ifdef _WIN32
typedef HINSTANCE DllHandle;
static const string dylibExt = ".dll";
#else
typedef void* DllHandle;
static const string dylibExt = ".so";
#endif

#if OSSIM_HAS_MSP
MSP::SMS::SensorModelService ossimCsmLoader::m_sensorModelService;
#endif

ossimCsmLoader::ossimCsmLoader()
{
   static const char* MODULE = "ossimCsmLoader Constructor -- ";

   init();
   
   return;
}

void ossimCsmLoader::init()
{
   static const char* MODULE = "ossimCsmLoader init -- ";
   static bool initialized=false;
   static std::mutex m_initMutex;
   std::lock_guard<std::mutex> lock(m_initMutex);

   if(!initialized)
   {

#if OSSIM_HAS_MSP
   try
   {
      m_sensorModelService = std::make_shared<MSP::SMS::SensorModelService>();
      m_sensorModelService->setPluginPreferencesRigorousBeforeRpc();
      //cout<<"In ossimCsmLoader Ctor"<<endl;//#### TODO REMOVE
      MSP::SMS::NameList pluginList;
      m_sensorModelService->getAllRegisteredPlugins(pluginList);
   }
   catch(exception &mspError)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Exception: " << mspError.what() << "\n";
      }
   }
   catch(...)
   {

   }

#else
      ossimString dataDir = ossimPreferences::instance()->findPreference("ossim.plugins.csm.data_dir");
      if(!dataDir.empty())
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_INFO) << "Adding data dir to the csm: " << dataDir << "\n";
         }

         csm::Plugin::setDataDirectory(dataDir.c_str());
      }
      // get plugin path from the preferences file and verify it
      ossimFilename pluginPath (ossimPreferences::instance()->findPreference("csm3_plugin_path"));
      if(pluginPath.empty())
      {
         pluginPath = ossimPreferences::instance()->findPreference("ossim.plugins.csm.plugin_path");
      }
      if(pluginPath.empty())
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"No CSM plugin path was specified. Make sure that"
                  " the ossim preferences file contains \"Csm_plugin_path\" entry."<<endl;
         }
         return;
      }

      // Load all of the dynamic libraries in the plugin path
      // first get the list of all the dynamic libraries found
      std::vector<ossimFilename> dllfiles;
      ossimDirectory pluginDir(pluginPath);
      pluginDir.findAllFilesThatMatch(dllfiles, dylibExt );

      for (int i=0; i<dllfiles.size(); i++)
      {
         // when loaded, each dynamic libraries found will register itself in the Plugin object
         // the list is accessible using  PluginList pluginList = Plugin::getList( )

         ossimDynamicLibrary *lib = new ossimDynamicLibrary;
         if (!lib->load(dllfiles[i]))
         {
            if(traceDebug())
            {
               ossimNotify(ossimNotifyLevel_WARN)
                     << "loadPlugins: " + dllfiles[i] + "\" file failed to load." << std::endl;
            }
         }
      }
#endif
      unloadPlugins();
      initialized = true;
   }
}

void ossimCsmLoader::unloadPlugins()
{
   ossimString enablePlugins = ossimPreferences::instance()->findPreference("ossim.plugins.csm.enable_plugins");
   ossimString disablePlugins = ossimPreferences::instance()->findPreference("ossim.plugins.csm.disable_plugins");
#if OSSIM_HAS_MSP
   try{
     //MSP::SMS::SensorModelService sms;
     MSP::SMS::NameList pluginList;
     m_sensorModelService->getAllRegisteredPlugins(pluginList);

     if(!enablePlugins.empty())
     {
       ossimRegExp regExp(enablePlugins);
       for(MSP::SMS::NameList::iterator iter = pluginList.begin();
           iter != pluginList.end();++iter)
       {
           if(!regExp.find((*iter).c_str()))
           {
               bool expel=false;
               m_sensorModelService->canPluginBeSafelyExpelled(*iter, expel);
               if(expel)
               {
                   m_sensorModelService->expelPlugin(*iter, false);
               }
           }
       }
     }
     else if(!disablePlugins.empty())
     {
       ossimRegExp regExp(enablePlugins);
       for(MSP::SMS::NameList::iterator iter = pluginList.begin();
           iter != pluginList.end();++iter)
       {
           if(regExp.find((*iter).c_str()))
           {
               bool expel=false;
               m_sensorModelService->canPluginBeSafelyExpelled(*iter, expel);
               if(expel)
               {
                   m_sensorModelService->expelPlugin(*iter, false);
               }
           }
       }
     }
   }
   catch(...)
   {

   }

#else

#endif

}

void ossimCsmLoader::getAllPluginNames(List& plugins)
{
   plugins.clear();

#if OSSIM_HAS_MSP
   MSP::SMS::NameList pluginList;
   try
   {
      m_sensorModelService->getAllRegisteredPlugins(pluginList);
   }
   catch(exception &mspError)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN) <<"Exception: "<<mspError.what()<<"\n";;

      }
   }
   catch(...)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN) <<"Exception: getAvailablePluginNames\n";
      }
   }

   MSP::SMS::NameList::iterator plugin = pluginList.begin();
   while (plugin != pluginList.end())
   {
      plugins.push_back(*plugin);
      ++plugin;
   }
#else

   // now get the PluginList to get the PluginName
   PluginList pluginList = Plugin::getList( );
   for( PluginList::const_iterator i = pluginList.begin(); i != pluginList.end(); i++ )
      plugins.push_back( (*i)->getPluginName() );
#endif

}

void ossimCsmLoader::getAvailablePluginNames(List& plugins)
{
   ossimString enablePlugins  = ossimPreferences::instance()->findPreference("ossim.plugins.csm.enable_plugins");
   ossimString disablePlugins = ossimPreferences::instance()->findPreference("ossim.plugins.csm.disable_plugins");
   plugins.clear();

   getAllPluginNames(plugins);

   if(!plugins.empty())
   {
      if(!enablePlugins.empty())
      {
         ossimRegExp regExp(enablePlugins);
         for(ossimCsmLoader::List::iterator iter = plugins.begin();
            iter != plugins.end();)
         {
            if(!regExp.find((*iter).c_str()))
            {
                iter = plugins.erase(iter);
            }
            else
            {
                ++iter;
            }
         } 

      }
      else if(!disablePlugins.empty())
      {
         ossimRegExp regExp(disablePlugins);
         for(ossimCsmLoader::List::iterator iter = plugins.begin();
            iter != plugins.end();)
         {
            if(regExp.find((*iter).c_str()))
            {
                iter = plugins.erase(iter);
            }
            else
            {
                ++iter;
            }
         } 
      }
   }
} 


void ossimCsmLoader::getAvailableSensorModelNames(List& models, const string& pluginName)
{
   models.clear();

#if OSSIM_HAS_MSP
   MSP::SMS::NameList modelList;
   try
   {
      m_sensorModelService->listModels(pluginName, modelList);
   }
   catch(exception &mspError)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN) <<"Exception: "<<mspError.what()<<"\n";
      }
   }
   catch(...)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN) <<"Unknown Exception in getAvailableSensorModelNames\n";
      }
   }

   MSP::SMS::NameList::iterator model = modelList.begin();
   while (model != modelList.end())
   {
      models.push_back(*model);
      ++model;
   }
#else

   // now get the PluginList to get the PluginName
   PluginList pluginList = Plugin::getList( );
   for( PluginList::const_iterator i = pluginList.begin(); i != pluginList.end(); i++ )
   {
      if( (*i)->getPluginName() != string(pluginName) )
         continue;

      for( size_t j = 0; j < (*i)->getNumModels(); j++ )
         models.push_back( (*i)->getModelName( j ) );
   }
#endif

   if (models.empty())
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN)
                                   << "getAvailableSensorModelNames: No plugins were found with the name \"" + pluginName + "\""
                                   << endl;
      }
   }
} 

RasterGM* ossimCsmLoader::loadModelFromState(const string& pPluginName,
                                              const string& pSensorModelName,
                                              const string& pSensorState )
{
   static const char* MODULE = "ossimCsmLoader::loadModelFromState() -- ";
   Model* sensorModel = 0;
   RasterGM* rgm = 0;

   // Make sure the input data is not NULL. For MSP, the plugin name and model name are not used:
   if( pSensorState.empty() )
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Sensor state is empty.\n";
      }
      return NULL;
   }

   try
   {

#if OSSIM_HAS_MSP
//      MSP::SMS::SensorModelService sms;
      sensorModel = m_sensorModelService->createModelFromState(pSensorState.c_str());
#else
      // Make sure the input data is not NULL
      if( pPluginName.empty() || pSensorModelName.empty())
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Plugin Name and"
                  " Sensor Model Name must be specified.\n";
         }
         return NULL;
      }
      const Plugin* plugin = Plugin::findPlugin( pPluginName );
      if( plugin == NULL )
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"No plugin "
                  " with the name \"" << pPluginName << "\" was found.\n";
         }
         return NULL;
      }

      // Load the sensor model using the appropriate information
      bool constructible = false;

      // See if it's possible to construct the sensor model from the state
      constructible = plugin->canModelBeConstructedFromState(pSensorModelName, pSensorState);
      if(plugin->canModelBeConstructedFromState(pSensorModelName, pSensorState))
      {
         plugin->canModelBeConstructedFromState(pSensorModelName, pSensorState);
      }
      else
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"The specified state cannot be "
                  "used to construct a sensor model.\n";
         }
         return NULL;
      }

      // Create the sensor model from the plugin and sensor state
      sensorModel = plugin->constructModelFromState( pSensorState );
#endif

      if(sensorModel)
      {
         rgm = dynamic_cast<RasterGM*>(sensorModel);
      }
      else
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Failed to create sensor model "
                  "from sensor state.\n";
         }
      }

   }
   catch(exception &e)
   {
      ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Hit exception: "<<e.what()<<endl;
   }
   catch(...)
   {

   }
   return rgm;
}


RasterGM* ossimCsmLoader::loadModelFromFile(const string& pPluginName,
                                             const string& pSensorModelName,
                                             const string& pInputImage,
                                             ossim_uint32 index )
{
   static const char* MODULE = "ossimCsmLoader::loadModelFromFile() -- ";
   Model* sensorModel = NULL;
   RasterGM* rgm = 0;

   // Make sure the input data is not NULL. For MSP, the plugin name is not used:
   if( pSensorModelName.empty() || pInputImage.empty())
   {
      ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Sensor model name and/or image filename is "
            "empty." << std::endl;
      return NULL;
   }

   do {
      try {

#if OSSIM_HAS_MSP
         // MSP::SMS::SensorModelService sms;
         const char* modelName = 0;
         if (pSensorModelName.size())
            modelName = pSensorModelName.c_str();
         MSP::ImageIdentifier entry ("IMAGE_INDEX", ossimString::toString(index).string());
         sensorModel = m_sensorModelService->createModelFromFile(pInputImage.c_str(), modelName, &entry);
#else

         // Make sure the input data is not NULL
         if( pPluginName.empty())
         {
            if(traceDebug())
            {
               ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Plugin Name must be specified.\n";
            }
            return NULL;
         }

         const Plugin* plugin = Plugin::findPlugin( pPluginName );
         if( plugin == NULL )
         {
            if(traceDebug())
            {
               ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"No plugin with the name \"" << pPluginName
                     << "\" was found.\n";
            }
            return NULL;
         }

         // Load the sensor model using the appropriate information
         WarningList warnings;
         bool constructible = false;

         // First try to construct the sensor model from the filename ISD
         Isd* fnameIsd = new Isd( pInputImage );
         try
         {
            constructible = plugin->canModelBeConstructedFromISD( *fnameIsd,
                                                                  pSensorModelName, &warnings);
         }
         catch(...)
         {
            delete fnameIsd;
            constructible = false;
         }

         if (constructible)
         {
            //ossimNotify(ossimNotifyLevel_INFO)<<MODULE<< "Constructing sensor model via filename"<<endl;
            sensorModel = plugin->constructModelFromISD( *fnameIsd, pSensorModelName );
            delete fnameIsd;
            break;
         }

         // Next try a NITF 2.1 ISD
         Nitf21Isd* nitf21Isd = new Nitf21Isd( pInputImage );
         try
         {
            initNitf21ISD( nitf21Isd, pInputImage, index, &warnings );
            constructible = plugin->canModelBeConstructedFromISD( *nitf21Isd,
                                                                  pSensorModelName, &warnings);
         }
         catch(...)
         {
            delete nitf21Isd;
            constructible = false;
         }

         if (constructible)
         {
            //ossimNotify(ossimNotifyLevel_INFO) << "Constructing sensor model from NITF 2.1 file" << endl;
            sensorModel = plugin->constructModelFromISD( *nitf21Isd, pSensorModelName );
            delete nitf21Isd;
            break;
         }

         // finally try a NITF 2.0 ISD
         Nitf20Isd* nitf20Isd = new Nitf20Isd( pInputImage );

         try {
            initNitf20ISD( nitf20Isd, pInputImage, index, &warnings );
            constructible = plugin->canModelBeConstructedFromISD( *nitf20Isd,
                                                                  pSensorModelName, &warnings);
         }
         catch(...)
         {
            delete nitf20Isd;
            constructible = false;
         }

         if (constructible)
         {
            //ossimNotify(ossimNotifyLevel_INFO) << "Constructing sensor model from NITF 2.0 file" << endl;
            sensorModel = plugin->constructModelFromISD( *nitf20Isd, pSensorModelName );
            delete nitf20Isd;
            break;
         }
#endif

      }
      catch(exception& e)
      {
         // This is noisy. Leave out for production:
         // ossimNotify(ossimNotifyLevel_WARN)<< "Exception: "<< e.what() << endl;
      }
   } while (false);

   if( sensorModel )
      return dynamic_cast<RasterGM*>(sensorModel);

//   ossimNotify(ossimNotifyLevel_WARN)<<MODULE<<"Unable to create sensor model \""
//         << pSensorModelName<< "\" using the image \"" + pInputImage + "\""  << endl;
   return NULL;
}


ossimCsmSensorModel* ossimCsmLoader::getSensorModel(const ossimFilename& filename,
                                                      ossim_uint32 index)
{
   ostringstream xmsg;
   xmsg<<__FILE__<<": getCsmSensorModel() -- ";

   ossimCsmSensorModel* model = 0;
   string pluginName = "";
   string sensorName = "";
   string fname = filename;

   csm::RasterGM* csmModel = 0;
   try
   {
#if OSSIM_HAS_MSP
      // MSP::SMS::SensorModelService sms;
      const char* modelName = 0;
      MSP::ImageIdentifier entry ("IMAGE_INDEX", ossimString::toString(index).string());
      csm::Model* base = m_sensorModelService->createModelFromFile(filename.c_str(), modelName, &entry);
      csmModel = dynamic_cast<csm::RasterGM*>(base);
#else
    ossimCsmLoader::List pluginNames;
    getAvailablePluginNames(pluginNames);

    for(int i = 0; i < pluginNames.size(); ++i)
    {
     ossimCsmLoader::List sensorModelNames;
     getAvailableSensorModelNames( sensorModelNames, pluginNames[i] );
     for(int j = 0; j<sensorModelNames.size() && !csmModel; ++j)
        csmModel = loadModelFromFile( pluginNames[i], sensorModelNames[j], filename, index);
    }
#endif
      if (csmModel)
      {
         if(traceDebug())
         {
            ossimNotify(ossimNotifyLevel_INFO) <<"ossimCsmLoader::getSensorModel()  modelName: "<<csmModel->getModelName()<<"\n";
         }
         model = new ossimCsmSensorModel(csmModel);
      }
   }
   catch (exception& e)
   {
      xmsg<<"Caught exception: "<<e.what();
      throw ossimException(xmsg.str());
   }
   return model;
}

