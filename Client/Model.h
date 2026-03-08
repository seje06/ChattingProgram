#pragma once


template<typename T>
class Model
{
public:
	Model(T data)
	{
		_lockQueue.Push(data);
	}

	static T Pop(OUT bool& isEmpty)
	{
		return _lockQueue.Pop(OUT isEmpty);
	}

	static void PopAll(OUT vector<T>& datas)
	{
		_lockQueue.PopAll(OUT datas);
	}

	static void Clear()
	{
		_lockQueue.Clear();
	}
private:
	static LockQueue<T> _lockQueue;
};
template<typename T>
LockQueue<T> Model<T>::_lockQueue;

//////////// Auth /////////////
struct AuthData
{
	bool isSuccess;
	CString id;
	CString pasd;
};

class AuthModel : public Model<AuthData>
{
public:
	AuthModel(AuthData data) : Model(data)
	{

	}
};


//////////// Lobby /////////////
struct RoomCreateData
{
	CString id;
	CString roomName;
};

class RoomCreateModel : public Model<RoomCreateData>
{
public:
	RoomCreateModel(RoomCreateData data) : Model(data)
	{

	}
};
