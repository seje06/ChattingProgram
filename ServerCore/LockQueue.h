#pragma once
#include <functional>
#include <queue>

template<typename T>													
class LockQueue												
{																		
public:																	
	void Push(T item)													
	{																	
		WriteLockGuard guard(_lock);									
		_items.push(item);												
	}																	
																		
	T Pop()																
	{																	
		WriteLockGuard guard(_lock);									
		if (_items.empty()) return T();									
																		
																		
		T ret = _items.front();
		_items.pop();													
		return ret;														
	}

	T Pop(OUT bool &isEmpty)
	{
		WriteLockGuard guard(_lock);
		if (isEmpty = _items.empty()) return T();


		T ret = _items.front();
		_items.pop();
		return ret;
	}
																		
	void PopAll(OUT vector<T>& items)									
	{																	
		WriteLockGuard guard(_lock);
		bool isEmpty;
		while (T item = Pop(OUT isEmpty))
		{				
			if (isEmpty) return;
			items.push_back(item);										
		}																
	}																	
																		
	void Clear()														
	{																	
		WriteLockGuard guard(_lock);									
		_items = queue<T>();
	}																	
																		
private:																
	Lock _lock;															
	queue<T> _items;													
};

template<typename T>
class PriorityLockQueue
{
public:
	void Push(T item)
	{
		WriteLockGuard guard(_lock);
		_items.push(item);
	}

	T Top()
	{
		WriteLockGuard guard(_lock);
		if (_items.empty()) return T();

		return _items.top();
	}

	T Pop()
	{
		WriteLockGuard guard(_lock);
		if (_items.empty()) return T();


		T ret = _items.top();
		_items.pop();
		return ret;
	}

	void PopAll(OUT vector<T>& items)
	{
		WriteLockGuard guard(_lock);
		while (T item = Pop())
		{
			items.push_back(item);
		}
	}

	void Clear()
	{
		WriteLockGuard guard(_lock);
		_items = priority_queue<T>();
	}

private:
	Lock _lock;
	priority_queue<T> _items;
};