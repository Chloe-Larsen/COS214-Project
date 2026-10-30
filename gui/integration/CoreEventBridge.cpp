#include "CoreEventBridge.h"
#include "observer/EventSource.h"
#include <algorithm>
#include <utility>

namespace tividy {
CoreEventBridge::CoreEventBridge(std::function<void()> handler) : callback(std::move(handler)) {}
CoreEventBridge::~CoreEventBridge() { disconnect(); }
void CoreEventBridge::attachTo(EventSource &source)
{
    if (std::find(sources.begin(), sources.end(), &source) != sources.end()) return;
    sources.push_back(&source);
    try { source.attach(this); } catch (...) { sources.pop_back(); throw; }
}
void CoreEventBridge::disconnect() { for (EventSource *source : sources) source->detach(this); sources.clear(); }
void CoreEventBridge::update(Event *) { try { if (callback) callback(); } catch (...) {} }
Observer *CoreEventBridge::clone() { return nullptr; }
}
