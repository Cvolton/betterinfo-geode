#pragma once

class PageNumberDelegate {
public:
    virtual void loadPage(unsigned int page) = 0;
    virtual int getPage() const = 0;

};