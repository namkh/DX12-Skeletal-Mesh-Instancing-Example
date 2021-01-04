#pragma once

#include "Utils.h"

class CommandBase : public UniqueIdentifier
{
public:
	CommandBase()
		: m_isBinded(false)
	{

	}
	virtual void Exec() = 0;

	bool IsBinded() { return m_isBinded; }
	void SetBinded(bool binded) { m_isBinded = binded; }

	
protected:
	bool m_isBinded;
};

template <typename OwnerClassType>
class Command : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)();
public:
	Command()
		: m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}
	Command(OwnerClassType* owner, FuncPtr funcPtr)
		: m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}
	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_isBinded = true;
	}
	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)();
		}
	}
private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
};

template <typename OwnerClassType, typename Param0Type>
class CommandWithOneParam : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)(Param0Type);
public:
	CommandWithOneParam()
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{

	}

	CommandWithOneParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithOneParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		m_isBinded = true;
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;

		m_isBinded = true;
	}

	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0);
		}
	}

	void Exec(Param0Type param0)
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(param0);
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
};

template <typename OwnerClassType, typename Param0Type, typename Param1Type>
class CommandWithTwoParam : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)(Param0Type, Param1Type);
public:
	CommandWithTwoParam()
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{

	}

	CommandWithTwoParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithTwoParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1);
	}

	virtual void Exec(Param0Type param0) override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(param0);
		}
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		m_isBinded = true;
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;

		m_isBinded = true;
	}

	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0, m_param1);
		}
	}

	void Exec(Param0Type param0, Param1Type param1)
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(param0, param1);
		}
	}
private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
};

template <typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type>
class CommandWithThreeParam : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type);
public:
	CommandWithThreeParam()
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{

	}

	CommandWithThreeParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithThreeParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		m_isBinded = true;
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;

		m_isBinded = true;
	}

	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0, m_param1, m_param2);
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2)
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0, m_param1, m_param2);
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
};

template <typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type>
class CommandWithFourParam : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type, Param3Type);
public:
	CommandWithFourParam()
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{

	}

	CommandWithFourParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithFourParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2, param3);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		m_isBinded = true;
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;

		m_isBinded = true;
	}
	
	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0, m_param1, m_param2, m_param3);
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(param0, param1, param2, param3);
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
};

template <typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type>
class CommandWithFiveParam : public CommandBase
{
private:
	typedef void (OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type);
public:
	CommandWithFiveParam()
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{

	}

	CommandWithFiveParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithFiveParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
		: CommandBase()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2, param3, param4);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		m_isBinded = true;
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		m_param4 = param4;

		m_isBinded = true;
	}

	virtual void Exec() override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(m_param0, m_param1, m_param2, m_param3, m_param4);
		}
	}

	virtual void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4) override
	{
		if (m_owner != nullptr && m_funcPtr != nullptr)
		{
			(m_owner->*m_funcPtr)(param0, param1, param2, param3, param4);
		}
	}
private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
	Param4Type m_param4;
};

template <typename RetValType>
class CommandWithRetValBase : public CommandBase
{
public:
	CommandWithRetValBase()
		: CommandBase()
	{

	}
	RetValType GetRetValue() { return m_returnValue; }
	void SetRetValue(RetValType ret) { m_returnValue = ret; }
protected:
	RetValType m_returnValue;
};

template <typename RetValType, typename OwnerClassType>
class CommandWithRetVal : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)();
public:
	CommandWithRetVal()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}
	CommandWithRetVal(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}
	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}
	virtual void Exec()
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)());
		}
	}
private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
};

template <typename RetValType, typename OwnerClassType, typename Param0Type>
class CommandWithRetValAndOneParam : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)(Param0Type);
public:
	CommandWithRetValAndOneParam()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}

	CommandWithRetValAndOneParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithRetValAndOneParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;

		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(m_param0));
		}
	}

	void Exec(Param0Type param0)
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(param0));
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
};

template <typename RetValType, typename OwnerClassType, typename Param0Type, typename Param1Type>
class CommandWithRetValAndTwoParam : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)(Param0Type, Param1Type);
public:
	CommandWithRetValAndTwoParam()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}

	CommandWithRetValAndTwoParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithRetValAndTwoParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;

		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(m_param0, m_param1));
		}
	}

	void Exec(Param0Type param0, Param1Type param1)
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(param0, param1));
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
};

template <typename RetValType, typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type>
class CommandWithRetValAndThreeParam : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type);
public:
	CommandWithRetValAndThreeParam()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}

	CommandWithRetValAndThreeParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithRetValAndThreeParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;

		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(m_param0, m_param1, m_param2));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2)
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(param0, param1, param2));
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
};

template <typename RetValType, typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type>
class CommandWithRetValAndFourParam : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type, Param3Type);
public:
	CommandWithRetValAndFourParam()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}

	CommandWithRetValAndFourParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithRetValAndFourParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2, param3);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;

		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(m_param0, m_param1, m_param2, m_param3));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(param0, param1, param2, param3));
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
};

template <typename RetValType, typename OwnerClassType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type>
class CommandWithRetValAndFiveParam : public CommandWithRetValBase<RetValType>
{
private:
	typedef RetValType(OwnerClassType::* FuncPtr)(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type);
public:
	CommandWithRetValAndFiveParam()
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
	}

	CommandWithRetValAndFiveParam(OwnerClassType* owner, FuncPtr funcPtr)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr);
	}

	CommandWithRetValAndFiveParam(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
		: CommandWithRetValBase<RetValType>()
		, m_owner(nullptr)
		, m_funcPtr(nullptr)
	{
		Bind(owner, funcPtr, param0, param1, param2, param3, param4);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;

		CommandBase::SetBinded(true);
	}

	void Bind(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		m_owner = owner;
		m_funcPtr = funcPtr;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		m_param4 = param4;

		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(m_param0, m_param1, m_param2, m_param3, m_param4));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		if (CommandBase::IsBinded() && m_owner != nullptr && m_funcPtr != nullptr)
		{
			CommandWithRetValBase<RetValType>::SetRetValue((m_owner->*m_funcPtr)(param0, param1, param2, param3, param4));
		}
	}

private:
	OwnerClassType* m_owner;
	FuncPtr m_funcPtr;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
	Param4Type m_param4;
};

template <typename LambdaFunctionType>
class LambdaCommand : public CommandBase
{
public:
	LambdaCommand()
		: CommandBase()
	{
	}
	LambdaCommand(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}
	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}
	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda();
		}
	}
protected:
	LambdaFunctionType m_lambda;
};

template <typename LambdaFunctionType, typename Param0Type>
class LambdaCommandWithOneParam : public CommandBase
{
public:
	LambdaCommandWithOneParam()
		: CommandBase()
	{
	}

	LambdaCommandWithOneParam(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}

	LambdaCommandWithOneParam(LambdaFunctionType lambda, Param0Type param0)
		: CommandBase()
	{
		Bind(lambda, param0);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0)
	{
		m_lambda = func;
		m_param0 = param0;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(m_param0);
		}
	}

	void Exec(Param0Type param0)
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(param0);
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
};

template <typename LambdaFunctionType, typename Param0Type, typename Param1Type>
class LambdaCommandWithTwoParam : public CommandBase
{
public:
	LambdaCommandWithTwoParam()
		: CommandBase()
	{
	}

	LambdaCommandWithTwoParam(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}

	LambdaCommandWithTwoParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1)
		: CommandBase()
	{
		Bind(lambda, param0, param1);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(m_param0, m_param1);
		}
	}

	virtual void Exec(Param0Type param0, Param1Type param1) override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(param0, param1);
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
};

template <typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type>
class LambdaCommandWithThreeParam : public CommandBase
{
public:
	LambdaCommandWithThreeParam()
		: CommandBase()
	{
	}

	LambdaCommandWithThreeParam(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}

	LambdaCommandWithThreeParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2)
		: CommandBase()
	{
		Bind(lambda, param0, param1, param2);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(m_param0, m_param1, m_param2);
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2)
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(param0, param1, param2);
		}
	}

protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
};

template <typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type>
class LambdaCommandWithFourParam : public CommandBase
{
public:
	LambdaCommandWithFourParam()
		: CommandBase()
	{
	}

	LambdaCommandWithFourParam(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}

	LambdaCommandWithFourParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
		: CommandBase()
	{
		Bind(lambda, param0, param1, param2, param3);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(m_param0, m_param1, m_param2, m_param3);
		}
	}

	virtual void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3) override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(param0, param1, param2, param3);
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
};

template <typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type>
class LambdaCommandWithFiveParam : public CommandBase
{
public:
	LambdaCommandWithFiveParam()
		: CommandBase()
	{
	}

	LambdaCommandWithFiveParam(LambdaFunctionType lambda)
		: CommandBase()
	{
		Bind(lambda);
	}

	LambdaCommandWithFiveParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
		: CommandBase()
	{
		Bind(lambda, param0, param1, param2, param3, param4);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		m_param4 = param4;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(m_param0, m_param1, m_param2, m_param3, m_param4);
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		if (CommandBase::IsBinded())
		{
			m_lambda(param0, param1, param2, param3, param4);
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
	Param4Type m_param4;
};

template <typename RetValType>
class LambdaCommandWithRetValueBase : public CommandBase
{
public:
	LambdaCommandWithRetValueBase()
		: CommandBase()
	{

	}
	RetValType GetRetValue()
	{
		return m_returnValue;
	}
	void SetRetValue(RetValType retValue)
	{
		m_returnValue = retValue;
	}
protected:
	RetValType m_returnValue;
};


template <typename RetValType, typename LambdaFunctionType>
class LambdaCommandWithRetValue : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValue()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}
	LambdaCommandWithRetValue(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}
	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}
	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda());
		}
	}
private:
	LambdaFunctionType m_lambda;
};

template <typename RetValType, typename LambdaFunctionType, typename Param0Type>
class LambdaCommandWithRetValAndOneParam : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValAndOneParam()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}

	LambdaCommandWithRetValAndOneParam(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}

	LambdaCommandWithRetValAndOneParam(LambdaFunctionType lambda, Param0Type param0)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda, param0);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0)
	{
		m_lambda = func;
		m_param0 = param0;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(m_param0));
		}
	}

	void Exec(Param0Type param0)
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(param0));
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
};

template <typename RetValType, typename LambdaFunctionType, typename Param0Type, typename Param1Type>
class LambdaCommandWithRetValAndTwoParam : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValAndTwoParam()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}

	LambdaCommandWithRetValAndTwoParam(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}

	LambdaCommandWithRetValAndTwoParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda, param0, param1);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(m_param0, m_param1));
		}
	}

	void Exec(Param0Type param0, Param1Type param1)
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(param0, param1));
		}
	}

protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
};

template <typename RetValType, typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type>
class LambdaCommandWithRetValAndThreeParam : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValAndThreeParam()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}

	LambdaCommandWithRetValAndThreeParam(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}

	LambdaCommandWithRetValAndThreeParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda, param0, param1, param2);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		CommandBase::SetBinded(true);
	}
	
	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(m_param0, m_param1, m_param2));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2)
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(param0, param1, param2));
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
};

template <typename RetValType, typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type>
class LambdaCommandWithRetValAndFourParam : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValAndFourParam()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}

	LambdaCommandWithRetValAndFourParam(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}

	LambdaCommandWithRetValAndFourParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda, param0, param1, param2, param3);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(m_param0, m_param1, m_param2, m_param3));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3)
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(param0, param1, param2, param3));
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
};

template <typename RetValType, typename LambdaFunctionType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type, typename Param4Type>
class LambdaCommandWithRetValAndFiveParam : public LambdaCommandWithRetValueBase<RetValType>
{
public:
	LambdaCommandWithRetValAndFiveParam()
		: LambdaCommandWithRetValueBase<RetValType>()
	{
	}

	LambdaCommandWithRetValAndFiveParam(LambdaFunctionType lambda)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda);
	}

	LambdaCommandWithRetValAndFiveParam(LambdaFunctionType lambda, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
		: LambdaCommandWithRetValueBase<RetValType>()
	{
		Bind(lambda, param0, param1, param2, param3, param4);
	}

	void Bind(LambdaFunctionType func)
	{
		m_lambda = func;
		CommandBase::SetBinded(true);
	}

	void Bind(LambdaFunctionType func, Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		m_lambda = func;
		m_param0 = param0;
		m_param1 = param1;
		m_param2 = param2;
		m_param3 = param3;
		m_param4 = param4;
		CommandBase::SetBinded(true);
	}

	virtual void Exec() override
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(m_param0, m_param1, m_param2, m_param3, m_param4));
		}
	}

	void Exec(Param0Type param0, Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
	{
		if (CommandBase::IsBinded())
		{
			LambdaCommandWithRetValueBase<RetValType>::SetRetValue(m_lambda(param0, param1, param2, param3, param4));
		}
	}
protected:
	LambdaFunctionType m_lambda;
	Param0Type m_param0;
	Param1Type m_param1;
	Param2Type m_param2;
	Param3Type m_param3;
	Param4Type m_param4;
};

typedef UID CommandHandle;
template <typename OwnerClassType>
class FuncCommandList
{
private:
	typedef void (OwnerClassType::* FuncPtr)();
public:
	FuncCommandList(){}
	~FuncCommandList()
	{
		Clear();
	}
public:
	CommandHandle Add(OwnerClassType* owner, FuncPtr funcPtr)
	{
		Command<OwnerClassType>* cmd = new Command<OwnerClassType>(owner, funcPtr);
		CommandHandle handle = cmd->GetUID();
		m_funcCmdList.insert(std::make_pair(handle, cmd));
	}
	void Remove(CommandHandle handle)
	{
		auto iterFind = m_funcCmdList.find(handle);
		if (iterFind != m_funcCmdList.end())
		{
			m_funcCmdList.erase(iterFind);
		}
	}
	virtual void Exec()
	{
		for (auto& cur : m_funcCmdList)
		{
			if (cur.second != nullptr)
			{
				cur.second->Exec();
			}
		}
	}
	void Clear()
	{
		for (auto& cur : m_funcCmdList)
		{
			if (cur.second != nullptr)
			{
				delete cur.second;
			}
		}
		m_funcCmdList.clear();
	}
	uint32_t GetCommandCount() { return static_cast<uint32_t>(m_funcCmdList.size()); }
private:
	std::map<CommandHandle, Command<OwnerClassType>*> m_funcCmdList;
};

template <typename OwnerClassType, typename Param0Type>
class FuncCommandListWithOneParam
{
private:
	typedef void (OwnerClassType::* FuncPtr)();
public:
	FuncCommandListWithOneParam(){}
	~FuncCommandListWithOneParam()
	{
		Clear();
	}
public:
	CommandHandle Add(OwnerClassType* owner, FuncPtr funcPtr, Param0Type param)
	{
		CommandWithOneParam<OwnerClassType, Param0Type>* cmd = new CommandWithOneParam<OwnerClassType, Param0Type>(owner, funcPtr, param);
		CommandHandle handle = cmd->GetUID();
		m_funcCmdList.insert(std::make_pair(handle, cmd));
		return handle;
	}
	void Remove(CommandHandle handle)
	{
		auto iterFind = m_funcCmdList.find(handle);
		if (iterFind != m_funcCmdList.end())
		{
			m_funcCmdList.erase(iterFind);
		}
	}
	virtual void Exec()
	{
		for (auto& cur : m_funcCmdList)
		{
			if (cur.second != nullptr)
			{
				cur.second->Exec();
			}
		}
	}
	void Clear()
	{
		for (auto& cur : m_funcCmdList)
		{
			if (cur.second != nullptr)
			{
				delete cur.second;
			}
		}
		m_funcCmdList.clear();
	}
	uint32_t GetCommandCount() { return static_cast<uint32_t>(m_funcCmdList.size()); }
private:
	std::map<CommandHandle, CommandWithOneParam<OwnerClassType, Param0Type>*> m_funcCmdList;
};

template <typename LambdaFunctionType>
class LambdaCommandList
{
public:
	LambdaCommandList() {}
	~LambdaCommandList()
	{
		Clear();
	}

public:
	CommandHandle Add(LambdaFunctionType lambda)
	{
		LambdaCommand<LambdaFunctionType>* cmd = new LambdaCommand<LambdaFunctionType>(lambda);
		CommandHandle handle = cmd->GetUID();
		m_lambdaCmdList.insert(std::make_pair(handle, cmd));

		return handle;
	}
	void Remove(CommandHandle handle)
	{
		auto iterFind = m_lambdaCmdList.find(handle);
		if (iterFind != m_lambdaCmdList.end())
		{
			if (iterFind->second != nullptr)
			{
				delete iterFind->second;
			}

			m_lambdaCmdList.erase(iterFind);
		}
	}
	void Exec()
	{
		for (auto& cur : m_lambdaCmdList)
		{
			if (cur.second != nullptr)
			{
				cur.second->Exec();
			}
		}
	}
	void Clear()
	{
		for (auto& cur : m_lambdaCmdList)
		{
			if (cur.second != nullptr)
			{
				delete cur.second;
			}
		}
		m_lambdaCmdList.clear();
	}
	uint32_t GetCommandCount() { return static_cast<uint32_t>(m_lambdaCmdList.size()); }
private:
	std::map<CommandHandle, LambdaCommand<LambdaFunctionType>*> m_lambdaCmdList;
};

template <typename LambdaFunctionType, typename Param0Type>
class LambdaCommandListWithOneParam
{
public:
	LambdaCommandListWithOneParam(){}
	~LambdaCommandListWithOneParam()
	{
		Clear();
	}
public:
	CommandHandle Add(LambdaFunctionType lambda)
	{
		LambdaCommandWithOneParam<LambdaFunctionType, Param0Type>* cmd = new LambdaCommandWithOneParam<LambdaFunctionType, Param0Type>(lambda);
		CommandHandle handle = cmd->GetUID();
		m_lambdaCmdList.insert(std::make_pair(handle, cmd));

		return handle;
	}
	CommandHandle Add(LambdaFunctionType lambda, Param0Type param)
	{
		LambdaCommandWithOneParam<LambdaFunctionType, Param0Type>* cmd = new LambdaCommandWithOneParam<LambdaFunctionType, Param0Type>(lambda, param);
		CommandHandle handle = cmd->GetUID();
		m_lambdaCmdList.insert(std::make_pair(handle, cmd));

		return handle;
	}
	void Remove(CommandHandle handle)
	{
		auto iterFind = m_lambdaCmdList.find(handle);
		if (iterFind != m_lambdaCmdList.end())
		{
			if (iterFind->second != nullptr)
			{
				delete iterFind->second;
			}
			
			m_lambdaCmdList.erase(iterFind);
		}
	}
	void Exec()
	{
		for (auto& cur : m_lambdaCmdList)
		{
			if (cur.second != nullptr)
			{
				cur.second->Exec();
			}
		}
	}
	void Exec(Param0Type param)
	{
		for (auto& cur : m_lambdaCmdList)
		{
			if (cur.second != nullptr)
			{
				cur.second->Exec(param);
			}
		}
	}
	void Clear()
	{
		for (auto& cur : m_lambdaCmdList)
		{
			if (cur.second != nullptr)
			{
				delete cur.second;
			}
		}
		m_lambdaCmdList.clear();
	}
	uint32_t GetCommandCount() { return static_cast<uint32_t>(m_lambdaCmdList.size()); }
private:
	std::map<CommandHandle, LambdaCommandWithOneParam<LambdaFunctionType, Param0Type>*> m_lambdaCmdList = {};
};