#ifndef CORE_THREAD_CONTEXT_HPP
#define CORE_THREAD_CONTEXT_HPP

#include <stack>
#include <boost/noncopyable.hpp>

/**/ class World;



struct IInitialiser
{
	virtual ~IInitialiser() = default;
	virtual bool Validate() = 0;
};


class ThreadContext final 
	: public boost::noncopyable
{
public:
	struct ScopeInit final
	{	//!^ RAII initialisation wrapper
		ScopeInit(IInitialiser& init)
		{ 
			ThreadContext::PushInitialiser(init);
		}
		~ScopeInit() 
		{ 
			ThreadContext::PopInitialiser(); 
		}
	};

	struct WorldContext final
	{	//!^ 
		World* world;
	};

public: //~~~~~~~~~~~~~~| interface
	static void SetWorldContext(WorldContext* newWorld);
	static World* GetWorld();

	static void PushInitialiser(IInitialiser& init);
	static void PopInitialiser();

	template<typename T>
	static T& TopInitialiser()
	{
		if (auto& inits = Get().inits; inits.size())
		{
			if (auto init = dynamic_cast<T*>(inits.top()))
			{
				return *init;
			}
			else throw std::runtime_error("incorrect initialiser type");
		}
		else throw std::runtime_error("no initialiser was found");
	}

	static size_t CountOfInitialisers();

private: //~~~~~~~~~~~~~~| internal
	WorldContext* world;
	std::stack<IInitialiser*> inits;

	static ThreadContext& Get();
};


#endif //!CORE_THREAD_CONTEXT_HPP
