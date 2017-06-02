#pragma once

#include <genericAsyncTask.h>
#include <eventHandler.h>

#include <string>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

/** This is the class that all Direct/SAL classes should inherit from. */
class RENDER_PIPELINE_DECL DirectObject
{
public:
    bool accept(const std::string& ev_name, EventHandler::EventFunction* func);
    bool accept(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data);

    bool ignore(const std::string& ev_name);
    bool ignore(const std::string& ev_name, EventHandler::EventFunction* func);
    bool ignore(const std::string& ev_name, EventHandler::EventCallbackFunction* func, void* user_data);

    GenericAsyncTask* add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name);
    GenericAsyncTask* add_task(GenericAsyncTask::TaskFunc* func, void* user_data, const std::string& name, int sort);

    int remove_task(const std::string& task_name);

    GenericAsyncTask* do_method_later(float delay, GenericAsyncTask::TaskFunc* func, const std::string& name, void* user_data);
};

}
