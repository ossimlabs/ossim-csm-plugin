//*****************************************************************************
// License:  See top level LICENSE.txt file.
//
// DESCRIPTION:
//   Contains implementation of class ossimCsmProjectionFactory
//
//*****************************************************************************
//  $Id: ossimCsmProjectionFactory.cpp 1577 2015-06-05 18:47:18Z cchuah $

#include "ossimCsmProjectionFactory.h"
#include "ossimCsmSensorModel.h"
#include <ossim/base/ossimKeywordNames.h>
#include <ossim/projection/ossimProjectionFactoryRegistry.h>
#include <ossim/base/ossimTrace.h>

static ossimTrace traceDebug("ossimCsmProjectionFactory:debug");

ossimCsmProjectionFactory* ossimCsmProjectionFactory::theInstance = 0;

ossimCsmProjectionFactory* ossimCsmProjectionFactory::instance()
{
    if(!theInstance)
    {
        theInstance = new ossimCsmProjectionFactory;
    }

    return (ossimCsmProjectionFactory*) theInstance;
}

ossimCsmProjectionFactory::ossimCsmProjectionFactory()
{
   // Instantiate once to load CSM model dynamic libs
   ossimCsmLoader csml;
}


ossimProjection* ossimCsmProjectionFactory::createProjection(const ossimFilename& filename,
                                                             ossim_uint32 entryIdx ) const
{
    string filenameStr = filename.string();
    ossimProjection* result = ossimCsmLoader::getSensorModel(filenameStr, entryIdx);
    return result;
}

ossimProjection* ossimCsmProjectionFactory::createProjection(const ossimKeywordlist &keywordList,
                                                             const char *prefix) const
{
    const char *lookup = keywordList.find(prefix, ossimKeywordNames::TYPE_KW);
    ossimProjection* result = 0;
    if(lookup)
    {
        result = createProjection(ossimString(lookup));
        if(result)
        {
            result->loadState(keywordList); 	

        }
    }
    return result;
}

ossimProjection* ossimCsmProjectionFactory::createProjection(const ossimString &name) const
{
    ossimProjection* result = 0;
   
    if(name == STATIC_TYPE_NAME(ossimCsmSensorModel))
    {
        result = new ossimCsmSensorModel();
    }
   
    return result;
}

ossimObject* ossimCsmProjectionFactory::createObject(const ossimString& typeName)const
{
    return createProjection(typeName);
}

ossimObject* ossimCsmProjectionFactory::createObject(const ossimKeywordlist& kwl,
                                                     const char* prefix)const
{
    return createProjection(kwl, prefix);
}

void ossimCsmProjectionFactory::getTypeNameList(std::vector<ossimString>& typeList)const
{
    typeList.push_back(STATIC_TYPE_NAME(ossimCsmSensorModel));
}

std::list<ossimString> ossimCsmProjectionFactory::getList()const
{
    std::list<ossimString> result;
    std::string pError;

	vector<string> pluginList;
	ossimCsmLoader::getAvailablePluginNames(pluginList);
    for (int i=0; i<pluginList.size(); i++)
    {
        vector<string> sensorList;
        ossimCsmLoader::getAvailableSensorModelNames( sensorList, pluginList[i] );
        std::copy (sensorList.begin (), sensorList.end (), std::back_inserter (result));
    }

    return result;
}
