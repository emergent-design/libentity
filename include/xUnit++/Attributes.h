#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#include <string>
#include <utility>
#include <vector>

namespace xUnitpp { class AttributeCollection; }

namespace xUnitAttributes
{
    xUnitpp::AttributeCollection Attributes();
}

namespace xUnitpp
{

class AttributeCollection
{
public:
    typedef std::pair<std::string, std::string> Attribute;
    typedef std::vector<Attribute>::const_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> iterator_range;

    AttributeCollection();
    friend void swap(AttributeCollection &a, AttributeCollection &b);

    // !!!VS My kingdom for initialization lists!
    void insert(Attribute &&a);

    bool empty() const;

    const_iterator begin() const;
    const_iterator end() const;
    size_t size() const;
    const Attribute &operator[](size_t index) const;

    void sort();

    iterator_range find(const Attribute &key) const;

    const std::pair<bool, std::string> &Skipped() const;

private:
    std::vector<Attribute> sortedAttributes;

    // shortcut to searching
    std::pair<bool, std::string> skipped;
};

}

#endif
