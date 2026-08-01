#pragma once

class CachedCommentsUpdatedEvent : public geode::Event<CachedCommentsUpdatedEvent, bool(), int> {
public:
    using Event::Event;
};
