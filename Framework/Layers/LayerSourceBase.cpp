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


#include "LayerSourceBase.h"

#include <Core/OrthancException.h>

namespace OrthancStone
{
  namespace
  {
//    class LayerReadyFunctor : public boost::noncopyable
//    {
//    private:
//      std::auto_ptr<ILayerRenderer>  layer_;
//      const CoordinateSystem3D&      slice_;
//      bool                           isError_;
      
//    public:
//      LayerReadyFunctor(ILayerRenderer* layer,
//                        const CoordinateSystem3D& slice,
//                        bool isError) :
//        layer_(layer),
//        slice_(slice),
//        isError_(isError)
//      {
//      }

//      void operator() (ILayerSource::IObserver& observer,
//                       const ILayerSource& source)
//      {
//        observer.NotifyLayerReady(layer_, source, slice_, isError_);
//      }
//    };
  }

  void LayerSourceBase::NotifyGeometryReady()
  {
    SignalGeometryReady(*this);
  }
    
  void LayerSourceBase::NotifyGeometryError()
  {
    SignalGeometryError(*this);
  }  
    
  void LayerSourceBase::NotifyContentChange()
  {
    SignalContentChange(*this);
  }

  void LayerSourceBase::NotifySliceChange(const Slice& slice)
  {
    SignalSliceChange(*this, slice);
  }

  void LayerSourceBase::NotifyLayerReady(boost::shared_ptr<ILayerRenderer> renderer,
                                         const CoordinateSystem3D& slice,
                                         bool isError)
  {
    SignalLayerReady(renderer, *this, slice, isError);
  }

  void LayerSourceBase::Register(boost::shared_ptr<IObserver> observer)
  {
    SignalGeometryReady.connect(LayerSourceBase::SignalGeometryReadyType::slot_type(&IObserver::NotifyGeometryReady, observer.get(), _1).track(observer));
    SignalGeometryError.connect(LayerSourceBase::SignalGeometryErrorType::slot_type(&IObserver::NotifyGeometryError, observer.get(), _1).track(observer));
    SignalContentChange.connect(LayerSourceBase::SignalContentChangeType::slot_type(&IObserver::NotifyContentChange, observer.get(), _1).track(observer));
    SignalSliceChange.connect(LayerSourceBase::SignalSliceChangeType::slot_type(&IObserver::NotifySliceChange, observer.get(), _1, _2).track(observer));
    SignalLayerReady.connect(LayerSourceBase::SignalLayerReadyType::slot_type(&IObserver::NotifyLayerReady, observer.get(), _1, _2, _3, _4).track(observer));
  }
}
