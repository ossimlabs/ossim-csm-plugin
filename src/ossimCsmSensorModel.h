//----------------------------------------------------------------------------
//
// License:  See top level LICENSE.txt file
//
// Author:  cchuah
//
// DESCRIPTION: Contains sensor model class declaration for CSM 3.0.1
//				the sensor model works in OSSIM through plugin.
//              It provides a ossimSensorModel wrapper around an actual CSM model 
//              obtained from the CSM plugin.
//              So this is a plugin within a plugin architecture.
//
//----------------------------------------------------------------------------
// $Id: ossimCsmSensorModel.h 1577 2015-06-05 18:47:18Z cchuah $

#ifndef ossimCsmSensorModel_HEADER
#define ossimCsmSensorModel_HEADER


#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/projection/ossimSensorModel.h>
#include <ossim/base/ossimFilename.h>
#include <csm/RasterGM.h>
#include <memory>

class OSSIM_PLUGINS_DLL ossimCsmSensorModel : public ossimSensorModel
{
public:
   /*!
    * Constructor
    */
   ossimCsmSensorModel();
   ossimCsmSensorModel( const ossimCsmSensorModel& src);
   ossimCsmSensorModel( csm::RasterGM* model);

   virtual ~ossimCsmSensorModel();

   ossimObject* dup() const {return new ossimCsmSensorModel(*this);}

   inline virtual bool useForward()const {return false;} //!image to ground faster

   //! it uses straight forward imageToGround() method from csm model
   virtual void lineSampleHeightToWorld(const ossimDpt& image_point,
                                        const double&  height,  ossimGpt&   world_point) const;

   // virtual void  lineSampleToWorld(const ossimDpt& image_point,
                                   // ossimGpt&       world_point) const;
   /*!
    * CSM API only has imageToGround() method at a specific height
    * so we use base class lineSampleToWorld() which depends on imagingRay() which in turns
    * depends on imageToGround() to establish the ray.
    */
   virtual void worldToLineSample(const ossimGpt& worldPoint, ossimDpt& ip) const;

   /*!
    * Uses imageToGround() method at a max valid height and 0 height to establish the ray
    */
   virtual void imagingRay(const ossimDpt& image_point,
                           ossimEcefRay&   image_ray) const;

   /*!
    * This method returns the partial derivatives of line and sample
    * (in pixels per the applicable model parameter units), respectively,
    * with respect to the model parameter given by index at the given
    * groundPt (x,y,z in ECEF meters).
    */
   virtual ossimDpt computeSensorPartials(int index, const ossimEcefPoint& ecefPt) const;

   /*!
    * This method returns the partial derivatives of line and sample  (a six elements vector)
    * (in pixels per meter) with respect to the given ecefPt
    */
   virtual std::vector<double> computeGroundPartials(const ossimEcefPoint& ecefPt) const;

   //! Initializes the adjustable parameter-related base-class data members to defaults.
   virtual void initAdjustableParameters();

   //! Assigns initial default values to adjustable parameters and related members.
   virtual void initializeModel();

   //! Following a change to the adjustable parameter set, this virtual is called to permit
   //! instances to compute derived quantities after parameter change.
   virtual void updateModel();

   virtual bool saveState(ossimKeywordlist& kwl, const char* prefix=0) const;

   virtual bool loadState(const ossimKeywordlist& kwl, const char* prefix=0);

   virtual bool isInternalModelAdjustable() { return m_modelIsAdjustable; }   ;

   virtual ossimString getPluginName() { return m_pluginName;  };
   virtual ossimString getSensorName() { return m_sensorName;  };

   //! Perhaps consumers will want direct access to CSM.
   std::shared_ptr<csm::RasterGM> getCsmModel() { return m_model; }
   // virtual bool isAffectedByElevation() const { return m_useImagingRay; }

protected:
   enum AdjustParamIndex
   {
      INTRACK_OFFSET = 0,
      CRTRACK_OFFSET,
      NUM_ADJUSTABLE_PARAMS // not an index
   };

   // the default adjustable parameters:
   double theIntrackOffset;
   double theCrtrackOffset;

   std::shared_ptr<csm::RasterGM> m_model;
   ossimString m_pluginName;
   ossimString m_sensorName;
   ossimFilename m_imageFile;
   bool m_modelIsAdjustable;
   // bool m_useImagingRay;

   TYPE_DATA
};

#endif
