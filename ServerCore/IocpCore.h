#pragma once


class IocpObject : public enable_shared_from_this<IocpObject> // shared_ptr로 관리되는 객체 내부에서 shared_ptr로 this를 전달하게 해주는 클래스
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEventm, int32_t numOfBytes = 0) abstract; // GetqueuedIOCP로 받은 iocpEvent에서 호출해줄 함수(각각의 iocpObject역할에 맡에 구현)
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return iocpHandle; }

	bool Register(shared_ptr<IocpObject> iocpObject);
	bool Dispatch(uint32_t timeoutMs = INFINITE);

private:
	HANDLE iocpHandle;
};

