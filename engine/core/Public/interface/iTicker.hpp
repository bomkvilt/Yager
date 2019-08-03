#ifndef CORE_ITICKER_HPP
#define CORE_ITICKER_HPP



struct ITicker
{
	virtual void Tick(float ds) {}

	virtual ~ITicker() = default;
};


#endif //!CORE_ITICKER_HPP
