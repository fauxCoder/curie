#pragma once

<typename T>
struct Spring
{
    Spring(){}

    ~Spring(){}

    void set(T a_New)
    {
        m_New = a_New;
    }

    T get()
    {
        return m_Current;
    }

    T m_New;
    T m_Current;
};
