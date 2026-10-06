#include "indexmap.h"

int	indexmap_index_of(const t_indexmap *this, const void *key)
{
	return (vector_get_index(&this->keys, key));
}

void	indexmap_set_compare(t_indexmap *this, t_indexmap_compare compare)
{
	this->keys.compare = compare;
}

void	indexmap_custom_keys(t_indexmap *this, t_vector_copy copy,
		t_vector_compare compare, t_vector_destroy_element destroy)
{
	vector_custom(&this->keys, copy, compare, destroy);
}

void	indexmap_custom_values(t_indexmap *this, t_vector_copy copy,
		t_vector_compare compare, t_vector_destroy_element destroy)
{
	vector_custom(&this->values, copy, compare, destroy);
}

void	*indexmap_set(t_indexmap *this, void *key, void *value)
{
	const int	index = vector_get_index(&this->keys, key);
	void		*place;

	if (0 > index) {
		vector_push_back(&this->keys, key);
		return (vector_push_back(&this->values, value));
	}

	place = this->values.at(&this->values, index);
	if (place == value)
		return (place);

	if (this->values.destroy_element)
		this->values.destroy_element(place);
	this->values.copy(place, value, this->values.element_size);

	return (place);
}

void	*indexmap_emplace(t_indexmap *this, void *key)
{
	int	const index = vector_get_index(&this->keys, key);

	if (index >= 0)
		return (vector_at(&this->values, index));

	vector_push_back(&this->keys, key);
	return (vector_emplace(&this->values));
}

bool	indexmap_remove(t_indexmap *this, void *key)
{
	const int	index = vector_get_index(&this->keys, key);

	if (index < 0)
		return (false);
	return (this->remove_at(this, (size_t)index));
}

void	*indexmap_get(t_indexmap *this, void *key)
{
	const int	index = vector_get_index(&this->keys, key);

	return ((index < 0) ? NULL : vector_at(&this->values, index));
}

bool	indexmap_remove_at(t_indexmap *this, size_t index)
{
	if (index >= this->keys.size)
		return (false);
	vector_remove_index(&this->keys, index);
	vector_remove_index(&this->values, index);
	return (true);
}

bool	indexmap_has(const t_indexmap *this, const void *key)
{
	return (vector_get_index(&this->keys, key) >= 0);
}

void	*indexmap_key_at(const t_indexmap *this, size_t index)
{
	return (vector_at(&this->keys, index));
}

void	*indexmap_value_at(const t_indexmap *this, size_t index)
{
	return (vector_at(&this->values, index));
}

t_pair	indexmap_at(const t_indexmap *this, size_t index)
{
	return ((t_pair){this->key_at(this, index), this->value_at(this, index)});
}

// #define call(ptr, member, ...) \
//     ((ptr)->member(ptr, __VA_ARGS__))

// int main()  {
// 	t_indexmap	processes;
// 	t_indexmap	strct_map;
// 	t_process	*value;
// 	t_vector	*programs;

// 	programs = new_vector(sizeof(t_program));
// 	programs->compare = program_name_compare;
// 	programs->find(programs, "nginx");

// 	call(&processes, get, 5);
// 	processes = init_indexmap(sizeof(char*), sizeof(t_process));
// 	processes.keys.compare = (void*)vector_string_compare;

// 	processes.set(&processes, &(char*){"nginx"}, &(t_process){.d = "nginx"});
// 	printf("nginx set\n");
// 	processes.set(&processes, &(char*){"frontend"}, &(t_process){
// 		.a = 1,
// 		.b = 2048,
// 		.c = 'R',
// 		.d = "frontend"
// 	});
// 	printf("frontend set\n");
// 	processes.set(&processes, &(char*){"backend"}, &(t_process){});
// 	printf("backend set\n");
// 	processes.set(&processes, &(char*){"database"}, &(t_process){});
// 	printf("database set\n");

// 	char	*frontend = strdup("frontend");
// 	printf("pointers %p, %p\n", frontend, "frontend");
// 	value = processes.get(&processes, "frontend");
// 	printf("get frontend %p\n", value);
// 	value = processes.get(&processes, frontend);
// 	printf("get frontend2 %p\n", value);

// 	if (value)
// 		printf("frontend %i, %li, %c, %s\n", value->a, value->b, value->c, (char*)value->d);
// 	value = processes.get(&processes, "nginx");
// 	if (value)
// 		printf("nginx %i, %li, %c, %s\n", value->a, value->b, value->c, (char*)value->d);


// 	destroy_indexmap(&processes);
// 	return (0);
// }



// t_idxmap_pair	*indexmap_get_pair(t_indexmap *this, void *key)
// {
// 	size_t			i;
// 	t_idxmap_pair	*pair;

// 	i = 0;
// 	while (i < this->data.size)
// 	{
// 		pair = this->data.at(&this->data, i);
// 		if (pair->key == key)
// 			return (pair);
// 		i++;
// 	}
// 	return (NULL);
// }

// bool	indexmap_has(t_indexmap *this, void *key)
// {
// 	t_idxmap_pair	*pair;
// 	size_t			i;

// 	i = 0;
// 	while (i < this->data.size)
// 	{
// 		pair = this->data.at(&this->data, i);
// 		if (pair->key == key)
// 			return (true);
// 		i++;
// 	}
// 	return (false);
// }


// void	indexmap_evoque_each(t_indexmap *this, size_t offset, void *arg, ...) {
// 	size_t			i;
// 	void			*value;
// 	t_offset_func	method;
// 	va_list			original;
// 	va_list			arg_list;

// 	va_start(original, arg);
// 	i = 0;
// 	while (i < this->keys.size) {
// 		va_copy(arg_list, original);
// 		value = vector_at(&this->values, i);

// 		memmove(&method, (char *)value + offset, sizeof(void *));
// 		method(value, arg_list);

// 		va_end(arg_list);
// 		i++;
// 	}
// 	va_end(original);
// }

// t_pair indexmap_end(t_indexmap *this)
// {
// 	return ((t_pair){
// 		vector_end(&this->keys),
// 		vector_end(&this->values)
// 	});
// }
