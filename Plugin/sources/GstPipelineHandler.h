

#ifndef GstPipelineHandler_h__
#define GstPipelineHandler_h__

#include <gst/gst.h>

#include <string>

#include "ListenerContainer.h"
#include "mTypes.h"

namespace mray {
namespace video {

class GstPipelineHandlerImpl;
class GstPipelineHandler;

class IPipelineListener {
   public:
    virtual void OnPipelineLink(GstPipelineHandler* p) {}
    virtual void OnPipelineReady(GstPipelineHandler* p) {}
    virtual void OnPipelinePlaying(GstPipelineHandler* p) {}
    virtual void OnPipelineStopped(GstPipelineHandler* p) {}
    virtual void OnPipelineClosed(GstPipelineHandler* p) {}
    virtual void OnPipelineError(GstPipelineHandler* p) {}
    virtual void OnPipelineEOS(GstPipelineHandler* p) {}
};

class GstPipelineHandler : public ListenerContainer<IPipelineListener*> {
   protected:
    GstPipelineHandlerImpl* m_data;

    DECLARE_FIRE_METHOD(OnPipelineLink, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelineReady, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelinePlaying, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelineStopped, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelineClosed, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelineError, (GstPipelineHandler * p), (p));
    DECLARE_FIRE_METHOD(OnPipelineEOS, (GstPipelineHandler * p), (p));

   public:
    GstPipelineHandler();
    virtual ~GstPipelineHandler();

    // set isMasterClock to true if
    virtual bool CreatePipeline(bool isMasterClock,
                                const std::string& clockIP = "",
                                uint clockPort = 7010);
    virtual void SetPaused(bool p);
    virtual void Stop();
    virtual bool IsLoaded();
    virtual bool IsPaused();
    virtual bool IsPlaying();
    virtual void Close();
    virtual bool HandleMessage(GstBus* bus, GstMessage* msg);
    virtual bool SetPosition(signed long pos);
    virtual signed long GetPosition();
    virtual signed long GetDuration();

    virtual bool QueryLatency(bool& isLive, ulong& minLatency,
                              ulong& maxLatency);

    virtual void SetClockBaseTime(ulong baseTime);
    virtual ulong GetClockBaseTime();

    void SetPipeline(GstElement* p);
    GstElement* GetPipeline();

    static bool busFunction(GstBus* bus, GstMessage* message,
                            GstPipelineHandler* player);
};

}  // namespace video
}  // namespace mray

#endif  // GstPipelineHandler_h__
