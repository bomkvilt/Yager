#include "common/hit_timer.hpp"


HitTimer::HitTimer(size_t required)
	: required(required)
	, hits(0)
{}

size_t HitTimer::GetHits() const
{ return hits; }

size_t HitTimer::GetNorm() const
{ return required; }

void HitTimer::SetNorm(size_t required)
{ this->required = required; }

bool HitTimer::Hit(size_t count)
{
	if (hits += count; hits >= required)
	{
		hits -= required;
		return true;
	}
	else return false;
}
