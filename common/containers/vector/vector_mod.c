#include "vector.h"

void	*vector_push_back(t_vector *this, void *element)
{
	void	*end;

	if (this->size >= this->capacity)
		vector_expand(this);

	end = vector_end(this);
	this->copy(end, element, this->element_size);
	this->size++;
	return (end);
}

void	*vector_emplace(t_vector *this)
{
	void	*end;

	if (this->size >= this->capacity)
		vector_expand(this);

	end = vector_end(this);
	this->size++;
	return (end);
}

void	vector_remove_index(t_vector *this, size_t find)
{
	unsigned char	*position;

	if (find >= this->size)
		return ;
	position = vector_at(this, find);
	if (this->destroy_element)
		this->destroy_element(position);
	if (find == this->size - 1)
	{
		this->size--;
		return ;
	}

	memmove(
		position,
		position + this->element_size,
		(this->size - find - 1) * this->element_size
	);
	this->size--;
}

void	vector_remove_element(t_vector *this, void *find)
{
	unsigned char	*const element = vector_search(this, find);

	if (!element)
		return ;	
	vector_remove_index(this, (element - (unsigned char *)this->data)
		/ this->element_size);
}

void	vector_mix_remove_index(t_vector *this, size_t index)
{
	unsigned char	*found;

	found = vector_at(this, index);
	if (!found)
		return ;
	if (this->destroy_element)
		this->destroy_element(found);

	memmove(
		found,
		(unsigned char *)vector_end(this) - this->element_size,
		this->element_size
	);
	this->size--;
}

void	vector_mix_remove_element(t_vector *this, void *element)
{
	unsigned char	*const found = vector_search(this, element);

	if (!found)
		return ;
	vector_mix_remove_index(this, (found - (unsigned char *)this->data)
		/ this->element_size);
}

void	vector_insert(t_vector *this, void *element, size_t index)
{
	unsigned char	*position_ptr;

	if (index >= this->size)
	{
		vector_push_back(this, element);
		return ;
	}
	if (this->size >= this->capacity)
		vector_expand(this);

	position_ptr = vector_at(this, index);
	memmove(
		position_ptr + this->element_size,
		position_ptr,
		(this->size - index) * this->element_size
	);
	this->copy(position_ptr, element, this->element_size);
	this->size++;
}

void	vector_for_each(t_vector *this, void (*function)(void*, size_t))
{
	unsigned char	*const	end = vector_end(this);
	unsigned char	*it;
	size_t			index;

	it = this->data;
	index = 0;
	while (it < end)
	{
		function(it, index);
		it += this->element_size;
		index++;
	}
}

void	vector_push_batch(t_vector *this, void *batch, size_t count)
{
	if (count == 0 || !batch)
		return ;

	while (this->size + count >= this->capacity)
		vector_expand(this);

	memmove(vector_end(this), batch, this->element_size * count);
	this->size += count;
}
