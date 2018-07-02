/**
 * Stone of Orthanc
 * Copyright (C) 2012-2016 Sebastien Jodogne, Medical Physics
 * Department, University Hospital of Liege, Belgium
 * Copyright (C) 2017-2018 Osimis S.A., Belgium
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/


#pragma once

#include "IWebService.h"
#include "SlicesSorter.h"
#include "../StoneEnumerations.h"

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

namespace OrthancStone
{
  class OrthancSlicesLoader : public IWebService::IWebServiceObserver
  {
  public:
    class IObserver : public boost::noncopyable
    {
    public:
      typedef boost::signals2::signal<void (const OrthancSlicesLoader& loader)> SignalGeometryReadyType;
      typedef boost::signals2::signal<void (const OrthancSlicesLoader& loader)> SignalGeometryErrorType;
      typedef boost::signals2::signal<void (const OrthancSlicesLoader& loader,
                                            unsigned int sliceIndex,
                                            const Slice& slice,
                                            std::auto_ptr<Orthanc::ImageAccessor>& image,
                                            SliceImageQuality effectiveQuality)> SignalSliceImageReadyType;
      typedef boost::signals2::signal<void (const OrthancSlicesLoader& loader,
                                            unsigned int sliceIndex,
                                            const Slice& slice,
                                            SliceImageQuality quality)> SignalSliceImageErrorType;


    public:
      virtual ~IObserver()
      {
      }

      virtual void NotifyGeometryReady(const OrthancSlicesLoader& loader) = 0;

      virtual void NotifyGeometryError(const OrthancSlicesLoader& loader) = 0;

      virtual void NotifySliceImageReady(const OrthancSlicesLoader& loader,
                                         unsigned int sliceIndex,
                                         const Slice& slice,
                                         std::auto_ptr<Orthanc::ImageAccessor>& image,
                                         SliceImageQuality effectiveQuality) = 0;

      virtual void NotifySliceImageError(const OrthancSlicesLoader& loader,
                                         unsigned int sliceIndex,
                                         const Slice& slice,
                                         SliceImageQuality quality) = 0;
    };

  private:
    IObserver::SignalGeometryReadyType SignalGeometryReady;
    IObserver::SignalGeometryErrorType SignalGeometryError;
    IObserver::SignalSliceImageReadyType SignalSliceImageReady;
    IObserver::SignalSliceImageErrorType SignalSliceImageError;

  private:
    enum State
    {
      State_Error,
      State_Initialization,
      State_LoadingGeometry,
      State_GeometryReady
    };
    
    enum Mode
    {
      Mode_SeriesGeometry,
      Mode_InstanceGeometry,
      Mode_FrameGeometry,
      Mode_LoadImage,
      Mode_LoadRawImage
    };

    class Operation;
    //class WebCallback;

    //boost::shared_ptr<WebCallback>  webCallback_;  // This is a PImpl pattern

    //ICallback&    userCallback_;
    boost::shared_ptr<OrthancSlicesLoader::IObserver> observer_;
    IWebService&  orthanc_;
    State         state_;
    SlicesSorter  slices_;

    void NotifySliceImageSuccess(const Operation& operation,
                                 std::auto_ptr<Orthanc::ImageAccessor>& image) const;
  
    void NotifySliceImageError(const Operation& operation) const;
    
    void ParseSeriesGeometry(const void* answer,
                             size_t size);

    void ParseInstanceGeometry(const std::string& instanceId,
                               const void* answer,
                               size_t size);

    void ParseFrameGeometry(const std::string& instanceId,
                            unsigned int frame,
                            const void* answer,
                            size_t size);

    void ParseSliceImagePng(const Operation& operation,
                            const void* answer,
                            size_t size);

    void ParseSliceImageJpeg(const Operation& operation,
                             const void* answer,
                             size_t size);

    void ParseSliceRawImage(const Operation& operation,
                            const void* answer,
                            size_t size);

    void ScheduleSliceImagePng(boost::shared_ptr<boost::noncopyable> tracker, const Slice& slice,
                               size_t index);
    
    void ScheduleSliceImageJpeg(boost::shared_ptr<boost::noncopyable> tracker, const Slice& slice,
                                size_t index,
                                SliceImageQuality quality);

    void SortAndFinalizeSlices();
    
  public:
    OrthancSlicesLoader(boost::shared_ptr<OrthancSlicesLoader::IObserver> observer,
                        IWebService& orthanc);

    void ScheduleLoadSeries(boost::shared_ptr<boost::noncopyable> tracker, const std::string& seriesId);

    void ScheduleLoadInstance(boost::shared_ptr<boost::noncopyable> tracker, const std::string& instanceId);

    void ScheduleLoadFrame(boost::shared_ptr<boost::noncopyable> tracker, const std::string& instanceId,
                           unsigned int frame);

    bool IsGeometryReady() const;

    size_t GetSliceCount() const;

    const Slice& GetSlice(size_t index) const;

    bool LookupSlice(size_t& index,
                     const CoordinateSystem3D& plane) const;

    void ScheduleLoadSliceImage(boost::shared_ptr<boost::noncopyable> tracker, size_t index,
                                SliceImageQuality requestedQuality);


    void OnRequestSuccess(const std::string& uri,
                   const void* answer,
                               size_t answerSize,
                               Orthanc::IDynamicObject* payload);

    void OnRequestError(const std::string& uri,
                             Orthanc::IDynamicObject* payload);

  };
}
