//**************************************************************************************************
// OSSIM Open Source Geospatial Data Processing Library
// See top level LICENSE.txt file for license information
//
// Author:  cchuah
//
// This is the class responsible for loading all the sensor models found in the
// CSM plugin folder
//
//**************************************************************************************************
//  $Id: ossimCsmLoader.h 1577 2015-06-05 18:47:18Z cchuah $

#ifndef CsmLoader_HEADER
#define CsmLoader_HEADER

#include "ossimCsmConfig.h"
#include <ossim/base/ossimFilename.h>
#include <ossim/plugin/ossimPluginConstants.h>
#include <csm/RasterGM.h>
#include <string>
#include <vector>

class ossimCsmSensorModel;

/**
 * This is the class responsible for loading all the sensor models found in the CSM plugin folder
 */
class OSSIM_PLUGINS_DLL ossimCsmLoader 
{
public:
    typedef std::vector<std::string> List;
   /** Needs to be instantiated once to load all CSM plugins (unless MSP is used) */
   ossimCsmLoader();

   static void init();

   /**
   * If supported it will try to unload plugins based on the ossim.plugins.csm.enable_plugin and 
   * ossim.plugins.csm.disable_plugin keywords
   *
   *
   * This method is already called during the init to apply the enable and disable proeprties
   */ 
   static void unloadPlugins();

   /*!
    * Returns All plugins
    *
    * For the csm interface it will use the ossim.plugins.csm.plugin_path keyword
    */
   static void getAllPluginNames(List& plugins);   

   /*!
    * Returns available plugins found in plugin path
    * plguin path is specified in prefrence file, in keyword "ossim.plugins.csm.plugin_path"
    */
   static void getAvailablePluginNames(List& plugins);

   /*!
    * Returns a list of sensor model names contained in the specified plugin 
    */
   static void getAvailableSensorModelNames(List& models, const std::string& pluginName);
    
   /*!
    * Returns the sensor model for the specified image file name 
    */
	static ossimCsmSensorModel* getSensorModel(const ossimFilename& filename, ossim_uint32 index);

   /*!
    * Load the named sensor models from the named plugin for the input image file
    */
	static csm::RasterGM* loadModelFromFile(const std::string& pPluginName,
	                                        const std::string& pSensorModelName,
	                                        const std::string& pInputImage,
	                                         ossim_uint32 index=0);

    /*!
    * Load the named sensor models from the named plugin from the sensor state
    */
	static csm::RasterGM* loadModelFromState(const std::string& pPluginName,
	                                         const std::string& pSensorModelName,
	                                         const std::string& pSensorState);

};


#endif

