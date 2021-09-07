#pragma once

/*
 * Super-class for objects that need to be inserted into a linked list.
 */
class Chain
{
private:
    Chain(const Chain &copy);

public:
    Chain() {}
    Chain* next;
};
