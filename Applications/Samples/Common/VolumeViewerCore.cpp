/**
 * Stone of Orthanc
 * Copyright (C) 2012-2016 Sebastien Jodogne, Medical Physics
 * Department, University Hospital of Liege, Belgium
 * Copyright (C) 2017-2023 Osimis S.A., Belgium
 * Copyright (C) 2021-2026 Sebastien Jodogne, ICTEAM UCLouvain, Belgium
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


#include "VolumeViewerCore.h"

#include "../../../OrthancStone/Sources/Scene2D/GrayscaleStyleConfigurator.h"
#include "../../../OrthancStone/Sources/Viewport/DefaultViewportInteractor.h"
#include "../../../OrthancStone/Sources/Volumes/DicomVolumeImageMPRSlicer.h"


namespace OrthancStone
{
  void VolumeViewerCore::Invalidate()
  {
    if (currentPlane_ < planes_.size())
    {
      source_->Update(controller_->GetScene(), planes_[currentPlane_]);
    }

    controller_->InvalidateViewport();
  }


  void VolumeViewerCore::UpdateProjection()
  {
    if (volume_->HasGeometry())
    {
      VolumeImageGeometry geometry = volume_->GetGeometry();

      const unsigned int depth = geometry.GetProjectionDepth(projection_);
      currentPlane_ = depth / 2;

      LOG(WARNING) << "Number of cutting planes along the projection: " << depth;

      planes_.resize(depth);

      for (unsigned int z = 0; z < depth; z++)
      {
        planes_[z] = geometry.GetProjectionSlice(projection_, z);
      }
    }
    else
    {
      planes_.clear();
      currentPlane_ = 0;
    }

    Invalidate();
  }


  void VolumeViewerCore::Handle(const OrthancStone::New::IObservable& source,
                                const DicomVolumeImage::GeometryReadyMessage& message)
  {
    LOG(WARNING) << "Dimension of the volume: "
                 << volume_->GetGeometry().GetWidth() << "x"
                 << volume_->GetGeometry().GetHeight() << "x"
                 << volume_->GetGeometry().GetDepth();
    UpdateProjection();
  }


  void VolumeViewerCore::Handle(const OrthancStone::New::IObservable& source,
                                const DicomVolumeImage::ContentUpdatedMessage& message)
  {
    Invalidate();
  }


  void VolumeViewerCore::CreateComponents(const boost::shared_ptr<StoneApplication::Context>& context,
                                          const boost::shared_ptr<ViewportController>& controller)
  {
    context_ = context;
    controller_ = controller;

    volume_.reset(new DicomVolumeImage);
    slicer_.reset(new DicomVolumeImageMPRSlicer(volume_));
    source_.reset(new VolumeSceneLayerSource(0, slicer_));

    std::unique_ptr<GrayscaleStyleConfigurator> style(new GrayscaleStyleConfigurator);
    style->SetLinearInterpolation(true);
    source_->SetConfigurator(style.release());

    seriesLoader_ = OrthancSeriesVolumeProgressiveLoader::Create(*context, volume_, true /* progressive */);
    instanceLoader_ = OrthancMultiframeVolumeLoader::Create(*context, volume_);

    New::IObservable::Bind<DicomVolumeImage::GeometryReadyMessage>(*seriesLoader_, shared_from_this());
    New::IObservable::Bind<DicomVolumeImage::ContentUpdatedMessage>(*seriesLoader_, shared_from_this());

    New::IObservable::Bind<DicomVolumeImage::GeometryReadyMessage>(*instanceLoader_, shared_from_this());
    New::IObservable::Bind<DicomVolumeImage::ContentUpdatedMessage>(*instanceLoader_, shared_from_this());

    controller_->AcquireInteractor(new DefaultViewportInteractor);
  }


  void VolumeViewerCore::HandleKeyDown(const IEnvironment::ILock& environmentLock,
                                       char key)
  {
    switch (key)
    {
      case 'a':
        projection_ = VolumeProjection_Axial;
        UpdateProjection();
        break;

      case 'c':
        projection_ = VolumeProjection_Coronal;
        UpdateProjection();
        break;

      case 's':
        projection_ = VolumeProjection_Sagittal;
        UpdateProjection();
        break;
    }
  }


  void VolumeViewerCore::HandleMouseWheel(const IEnvironment::ILock& environmentLock,
                                          MouseWheelDirection direction,
                                          bool fast)
  {
    int delta;

    switch (direction)
    {
      case MouseWheelDirection_Down:
        delta = 1;
        break;

      case MouseWheelDirection_Up:
        delta = -1;
        break;

      default:
        break;
    }

    if (fast)
    {
      delta *= static_cast<int>(planes_.size()) / 10;
    }

    int tmp = static_cast<int>(currentPlane_) + delta;

    size_t nextPlane;

    if (tmp < 0)
    {
      nextPlane = 0;
    }
    else if (tmp >= static_cast<int>(planes_.size()))
    {
      nextPlane = planes_.size() - 1;
    }
    else
    {
      nextPlane = tmp;
    }

    if (nextPlane != currentPlane_)
    {
      currentPlane_ = nextPlane;
      Invalidate();
    }
  }


  VolumeViewerCore::VolumeViewerCore() :
    // Dummy initialization
    projection_(VolumeProjection_Axial),
    currentPlane_(0)
  {
  }


  void VolumeViewerCore::LoadSeriesFromOrthanc(VolumeProjection projection,
                                               const std::string& seriesId)
  {
    projection_ = projection;
    seriesLoader_->LoadSeries(seriesId);
  }


  void VolumeViewerCore::LoadInstanceFromOrthanc(VolumeProjection projection,
                                                 const std::string& instanceId)
  {
    projection_ = projection;
    instanceLoader_->LoadInstance(instanceId);
  }
}
