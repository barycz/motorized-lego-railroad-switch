#pragma once

template<class T>
class ReplicatedVariable {
public:
	void SetLocal(T local) { Local = local; }
	const T& GetLocal() const { return Local; }

	void SetRemote(T remote) { Remote = remote; }
	const T& GetRemote() const { return Remote; }

	void SetBoth(T value) { Local = Remote = value; }

private:
	T Local;
	T Remote;
};