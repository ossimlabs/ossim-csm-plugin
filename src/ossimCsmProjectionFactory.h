//*******************************************************************
//
// License:  See top level LICENSE.txt file.
// 
// Author:  cchuah
//
//*******************************************************************
//  $Id: ossimCsmProjectionFactory.h 1579 2015-06-08 17:09:57Z cchuah $

#ifndef ossimCsmProjectionFactory_HEADER
#define ossimCsmProjectionFactory_HEADER

#include "ossimCsmLoader.h"
#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/projection/ossimProjectionFactoryBase.h>


class ossimProjection;
class ossimString;

class OSSIM_PLUGINS_DLL ossimCsmProjectionFactory : public ossimProjectionFactoryBase
{
public:
   /*!
    * METHOD: instance()
    * Instantiates singleton instance of this class:
    */
    static ossimCsmProjectionFactory* instance();

   /*!
    * METHOD: createProjection()
    * Attempts to create an instance of the projection from the filename for
    * image index entryIdx.
    * Returns successfully constructed projection or NULL.
    */
    virtual ossimProjection* createProjection(const ossimFilename& filename,
                                             ossim_uint32 entryIdx)const;
   /*!
    * METHOD: createProjection()
    * Attempts to create an instance of the projection specified by name.
    * Returns successfully constructed projection or NULL.
    */
    virtual ossimProjection* createProjection(const ossimString& name)const;

   /*!
    * METHOD: createProjection()
    * Attempts to create an instance of the projection from the kwl
    * Returns successfully constructed projection or NULL.
    */
    virtual ossimProjection* createProjection(const ossimKeywordlist& kwl,
                                                const char* prefix = 0)const;

    virtual ossimObject* createObject(const ossimString& typeName)const;

   /*!
    * Creates and object given a keyword list.
    */
    virtual ossimObject* createObject(const ossimKeywordlist& kwl,
                                     const char* prefix=0)const;
   
   /*!
    * This should return the type name of all objects in all factories.
    * This is the name used to construct the objects dynamiCally and this
    * name must be unique.
    */
    virtual void getTypeNameList(std::vector<ossimString>& typeList)const;

   /*!
    * METHOD: getList()
    * Returns list of all projections represented by this factory:
    */
    virtual std::list<ossimString> getList()const;

protected:

    ossimCsmProjectionFactory();
    static ossimCsmProjectionFactory* theInstance;
};

#endif
