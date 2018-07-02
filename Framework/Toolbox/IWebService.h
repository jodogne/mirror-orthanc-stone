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

#include <Core/IDynamicObject.h>

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

namespace OrthancStone
{
  class IWebService : public boost::noncopyable
  {
  public:
    class IWebServiceObserver : public boost::noncopyable
    {
    public:
        typedef boost::signals2::signal<void (const std::string& uri,
                                    Orthanc::IDynamicObject* payload)> SignalErrorType;
        typedef boost::signals2::signal<void (const std::string& uri,
                                      const void* answer,
                                      size_t answerSize,
                                      Orthanc::IDynamicObject* payload)> SignalSuccessType;

    public:
      virtual ~IWebServiceObserver()
      {
      }

      virtual void OnRequestError(const std::string& uri,
                               Orthanc::IDynamicObject* payload) = 0;

      virtual void OnRequestSuccess(const std::string& uri,
                                 const void* answer,
                                 size_t answerSize,
                                 Orthanc::IDynamicObject* payload) = 0;
    };
    
    virtual ~IWebService()
    {
    }

    virtual void ScheduleGetRequest(IWebServiceObserver* observer,
                                    boost::shared_ptr<boost::noncopyable> tracker,
                                    const std::string& uri,
                                    Orthanc::IDynamicObject* payload) = 0;

    virtual void SchedulePostRequest(IWebServiceObserver* observer,
                                     boost::shared_ptr<boost::noncopyable> tracker,
                                     const std::string& uri,
                                     const std::string& body,
                                     Orthanc::IDynamicObject* payload) = 0;
  };
}
