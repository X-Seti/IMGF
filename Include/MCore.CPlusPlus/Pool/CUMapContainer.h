#ifndef CUMapContainer_H
#define CUMapContainer_H

#include "mcore.h"
#include "Type/Types.h"
#include <unordered_map>

template <class Key, class Value>
class mcore::CUMapContainer
{
public:
	bool									doesKeyExist(Key key);
	uint32									getEntryCount(void);
	std::unordered_map<Key, Value>&			getEntries(void) { return m_umapEntries; }
	void									setEntry(Key key, Value value);
	Value									getEntry(Key key);
	Key										getKeyByValue(Value value);
	void									removeEntry(Key key);
	void									removeAllEntries(void);
	void									swapEntries(Key key1, Key key2);

protected:
	std::unordered_map<Key, Value>			m_umapEntries;
};




template <class Key, class Value>
bool				mcore::CUMapContainer<Key, Value>::doesKeyExist(Key key)
{
	return m_umapEntries.count(key) != 0;
}

template <class Key, class Value>
uint32				mcore::CUMapContainer<Key, Value>::getEntryCount(void)
{
	return m_umapEntries.size();
}

template <class Key, class Value>
void				mcore::CUMapContainer<Key, Value>::setEntry(Key key, Value value)
{
	m_umapEntries[key] = value;
}

template <class Key, class Value>
Value				mcore::CUMapContainer<Key, Value>::getEntry(Key key)
{
	if (doesKeyExist(key))
	{
		return m_umapEntries[key];
	}
	return (Value) 0;
}

template <class Key, class Value>
Key					mcore::CUMapContainer<Key, Value>::getKeyByValue(Value value)
{
	for (auto it = m_umapEntries.begin(), it_end = m_umapEntries.end; it != it_end; it++)
	{
		if (it.second == value)
		{
			return key;
		}
	}
	return (Key) 0;
}

template <class Key, class Value>
void				mcore::CUMapContainer<Key, Value>::removeEntry(Key key)
{
	auto it = m_umapEntries.find(key);
	if(it == m_umapEntries.end())
	{
		return;
	}
	m_umapEntries.erase(it);
}

template <class Key, class Value>
void				mcore::CUMapContainer<Key, Value>::removeAllEntries(void)
{
	m_umapEntries.clear();
}

template <class Key, class Value>
void				mcore::CUMapContainer<Key, Value>::swapEntries(Key key1, Key key2)
{
	Value value1 = getEntry(key1);
	setEntry(key1, getEntry(key2));
	setEntry(key2, value1);
}

#endif