#ifndef EVENTTYPE_H
#define EVENTTYPE_H

enum class EventType
{
    COMPLETED,
    APPROVED,
    REJECTED,
    ESCALATED,
    ASSIGNED
};

std::string eventTypeToString(EventType type);

#endif