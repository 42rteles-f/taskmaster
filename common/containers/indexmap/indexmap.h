#ifndef INDEXMAP_H
# define INDEXMAP_H

# include "vector.h"
# include <stdbool.h>
# include <stdarg.h>

typedef struct s_indexmap	t_indexmap;
typedef struct s_indexmap	t_strmap;
typedef t_vector_compare	t_indexmap_compare;

typedef struct s_pair {
	void	*first;
	void	*second;
} t_pair;

typedef struct s_process {
	int		a;
	long	b;
	char	c;
	void	*d;
} t_process;

struct s_indexmap {
	t_vector	keys;
	t_vector	values;

	void		(*set_compare)(t_indexmap*, t_indexmap_compare);
	void		(*custom_keys)(t_indexmap*, t_vector_copy, t_vector_compare,
					t_vector_destroy_element);
	void		(*custom_values)(t_indexmap*, t_vector_copy, t_vector_compare,
					t_vector_destroy_element);
	void*		(*set)(t_indexmap*, void*, void*);
	void*		(*get)(t_indexmap*, void*);
	void*		(*emplace)(t_indexmap*, void*);
	bool		(*remove)(t_indexmap*, void*);
	bool		(*remove_at)(t_indexmap*, size_t);
	bool		(*has)(const t_indexmap*, const void*);
	int			(*index_of)(const t_indexmap*, const void*);
	void*		(*key_at)(const t_indexmap*, size_t);
	void*		(*value_at)(const t_indexmap*, size_t);
	t_pair		(*at)(const t_indexmap*, size_t);
	void		(*destroy)(t_indexmap*);
} ;

//indexmap_setup.c
void		delete_indexmap(t_indexmap *this);
void		destroy_indexmap(t_indexmap *this);
t_indexmap	init_indexmap(size_t key_size, size_t value_size);
t_indexmap	*new_indexmap(size_t key_size, size_t value_size);
t_strmap	init_strmap(size_t value_size);

//indexmap.c
void	*indexmap_get(t_indexmap *this, void *key);
bool	indexmap_remove(t_indexmap *this, void *key);
void	*indexmap_emplace(t_indexmap *this, void *key);
void	*indexmap_set(t_indexmap *this, void *key, void *value);
void	indexmap_set_compare(t_indexmap *this, t_indexmap_compare compare);
void	indexmap_custom_keys(t_indexmap *this, t_vector_copy copy,
			t_vector_compare compare, t_vector_destroy_element destroy);
void	indexmap_custom_values(t_indexmap *this, t_vector_copy copy,
			t_vector_compare compare, t_vector_destroy_element destroy);
bool	indexmap_remove_at(t_indexmap *this, size_t index);
bool	indexmap_has(const t_indexmap *this, const void *key);
int		indexmap_index_of(const t_indexmap *this, const void *key);
void	*indexmap_key_at(const t_indexmap *this, size_t index);
void	*indexmap_value_at(const t_indexmap *this, size_t index);
t_pair	indexmap_at(const t_indexmap *this, size_t index);

t_strmap	*new_strmap(size_t value_size);


#endif
