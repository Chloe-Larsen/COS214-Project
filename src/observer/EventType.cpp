#include <string>

#include "../../include/observer/EventType.h"

std::string eventTypeToString(EventType type)
{
    switch (type)
    {
    case EventType::COMPLETED:
        return "COMPLETED";
    case EventType::APPROVED:
        return "APPROVED";
    case EventType::REJECTED:
        return "REJECTED";
    case EventType::ESCALATED:
        return "ESCALATED";
    case EventType::ASSIGNED:
        return "ASSIGNED";
    }
    return "UNKNOWN";
}