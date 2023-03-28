#pragma once

class Page
{
    class Action
    {

    };

    class Request
    {

    };

    class Event
    {
    private:
        /* data */
    public:
        Event(/* args */);
        ~Event();
    };
    
public:
    Page();
    ~Page();

    void InitLVGL();
    void DestroyLVGL();
};
