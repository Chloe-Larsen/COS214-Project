#ifndef TIVIDY_GUI_COREEVENTBRIDGE_H
#define TIVIDY_GUI_COREEVENTBRIDGE_H
#include "observer/Observer.h"
#include <functional>
#include <vector>
class EventSource;

namespace tividy {
/**
 * Actual Observer adapter for the GUI's coalesced invalidation callback.
 * Sources are borrowed: disconnect BEFORE deleting/replacing any source.
 * Own this concrete type, since the supplied Observer destructor is nonvirtual.
 */
class CoreEventBridge final : public Observer {
public:
    /** Construct a bridge whose callback must not touch widgets directly. */
    explicit CoreEventBridge(std::function<void()> callback);
    /** Disconnect while borrowed EventSource objects are still alive. */
    ~CoreEventBridge();
    /** Subscribe once to a live source; the source does not own this bridge. */
    void attachTo(EventSource &source);
    /** Remove every subscription, without destroying engine objects. */
    void disconnect();
    /** Coalesce an event through the callback; never propagate callback errors. */
    void update(Event *event) override;
    /** GUI subscriptions must never be copied into a cloned workflow. */
    Observer *clone() override;
private:
    CoreEventBridge(const CoreEventBridge &) = delete;
    CoreEventBridge &operator=(const CoreEventBridge &) = delete;
    std::function<void()> callback;
    std::vector<EventSource *> sources;
};
}
#endif
