#ifndef INDEXMAP_H
# define INDEXMAP_H

# include "../vector/vector.h"
# include <stdbool.h>
# include <stdarg.h>

typedef struct s_indexmap	t_indexmap;
typedef struct s_indexmap	t_strmap;

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
	bool 		(*compare)(void*, void*, size_t);

	void*		(*set)(t_indexmap*, void*, void*);
	void*		(*get)(t_indexmap*, void*);
	void*		(*emplace)(t_indexmap*, void*);
	bool		(*remove)(t_indexmap*, void*);
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

t_strmap	*new_strmap(size_t value_size);


#endif