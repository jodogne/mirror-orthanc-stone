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


#pragma once

#include "ISingleViewportApplicationCore.h"
#include "../../../OrthancStone/Sources/Loaders/OrthancMultiframeVolumeLoader.h"
#include "../../../OrthancStone/Sources/Loaders/OrthancSeriesVolumeProgressiveLoader.h"
#include "../../../OrthancStone/Sources/Volumes/VolumeSceneLayerSource.h"


namespace OrthancStone
{
  class VolumeViewerCore :
    public ISingleViewportApplicationCore,
    public boost::enable_shared_from_this<VolumeViewerCore>,
    public New::TypedObserver<DicomVolumeImage::GeometryReadyMessage>,
    public New::TypedObserver<DicomVolumeImage::ContentUpdatedMessage>
  {
  private:
    boost::shared_ptr<StoneApplication::Context>  context_;
    boost::shared_ptr<ViewportController>         controller_;

    VolumeProjection                 projection_;
    size_t                           currentPlane_;
    std::vector<CoordinateSystem3D>  planes_;

    boost::shared_ptr<DicomVolumeImage>                      volume_;
    boost::shared_ptr<OrthancSeriesVolumeProgressiveLoader>  seriesLoader_;
    boost::shared_ptr<OrthancMultiframeVolumeLoader>         instanceLoader_;
    boost::shared_ptr<IVolumeSlicer>                         slicer_;
    std::unique_ptr<VolumeSceneLayerSource>                  source_;

    void Invalidate();

    void UpdateProjection();

  protected:
    virtual void Handle(const OrthancStone::New::IObservable& source,
                        const DicomVolumeImage::GeometryReadyMessage& message) ORTHANC_OVERRIDE;

    virtual void Handle(const OrthancStone::New::IObservable& source,
                        const DicomVolumeImage::ContentUpdatedMessage& message) ORTHANC_OVERRIDE;

    virtual void CreateComponents(const boost::shared_ptr<StoneApplication::Context>& context,
                                  const boost::shared_ptr<ViewportController>& controller) ORTHANC_OVERRIDE;

    virtual void HandleKeyDown(const IEnvironment::ILock& environmentLock,
                               char key) ORTHANC_OVERRIDE;

    virtual void HandleMouseWheel(const IEnvironment::ILock& environmentLock,
                                  MouseWheelDirection direction,
                                  bool fast) ORTHANC_OVERRIDE;

  public:
    VolumeViewerCore();

    void LoadSeriesFromOrthanc(VolumeProjection projection,
                               const std::string& seriesId);

    void LoadInstanceFromOrthanc(VolumeProjection projection,
                                 const std::string& instanceId);
  };
}
