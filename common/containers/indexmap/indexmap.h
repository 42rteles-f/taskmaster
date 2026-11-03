#ifndef INDEXMAP_H
# define INDEXMAP_H

// Custom Headers
# include "vector.h"

// Standard Headers
# include <stdbool.h>
# include <stddef.h>

typedef struct s_indexmap	t_indexmap;
typedef struct s_indexmap	t_strmap;
typedef void			(*t_indexmap_iter)(void *, void *, size_t);

typedef struct s_pair {
	void	*first;
	void	*second;
} t_pair;

struct s_indexmap {
	t_uovector	keys;
	t_uovector	values;

	void		(*set_compare)(t_indexmap*,
					int(*)(const void*, const void*, size_t));
	void		(*custom_keys)(t_indexmap*,
					void*(*)(void*, const void*, size_t),
					int(*)(const void*, const void*, size_t), void(*)(void*));
	void		(*custom_values)(t_indexmap*,
					void*(*)(void*, const void*, size_t),
					int(*)(const void*, const void*, size_t), void(*)(void*));
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
	void		(*for_each)(t_indexmap*, t_indexmap_iter);
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
void	indexmap_set_compare(t_indexmap *this,
			int (*compare)(const void*, const void*, size_t));
void	indexmap_custom_keys(t_indexmap *this,
			void *(*copy)(void*, const void*, size_t),
			int (*compare)(const void*, const void*, size_t),
			void (*destroy)(void*));
void	indexmap_custom_values(t_indexmap *this,
			void *(*copy)(void*, const void*, size_t),
			int (*compare)(const void*, const void*, size_t),
			void (*destroy)(void*));
bool	indexmap_remove_at(t_indexmap *this, size_t index);
bool	indexmap_has(const t_indexmap *this, const void *key);
int		indexmap_index_of(const t_indexmap *this, const void *key);
void	*indexmap_key_at(const t_indexmap *this, size_t index);
void	*indexmap_value_at(const t_indexmap *this, size_t index);
t_pair	indexmap_at(const t_indexmap *this, size_t index);
void	indexmap_for_each(t_indexmap *this, t_indexmap_iter function);

t_strmap	*new_strmap(size_t value_size);


#endif
