# Others

## Clean up Panda3D and Render Pipeline
```
// clean up remained tasks
AsyncTaskManager::get_global_ptr()->cleanup();

// release DLLs of plugin in Render Pipeline
rpcore::PluginManager::release_all_dll();

// reset ostream of Notify
Notify::ptr()->set_ostream_ptr(nullptr, false);
```
