#pragma once

class CachedCommentsUpdatedEvent : public geode::Event<CachedCommentsUpdatedEvent, void(), int> {
public:
    using Event::Event;
};
