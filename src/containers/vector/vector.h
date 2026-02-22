#ifndef VECTOR_H
# define VECTOR_H

# include <unistd.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>
# include <stdio.h>
# include <stdbool.h>

# define VECTOR_CAPACITY 50
# define V (void*)

# define DEBUG \
	printf("DEBUG %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__)

typedef struct s_vector	t_vector;
typedef t_vector		t_uvector;
typedef t_vector		t_pvector;

struct s_hashpair {
	size_t	key;
	void	*content;
} ;

struct s_vector {
	void	*data;
	size_t	size;
	size_t	capacity;
	size_t	element_size;
	int		(*compare)(const void*, const void*, size_t);
	void	*(*copy)(void*, const void*, size_t);

	void	*(*push)(t_vector*, void*);
	void	*(*emplace)(t_vector*);
	void	(*push_batch)(t_vector*, void*, size_t);
	void	(*insert)(t_vector*, void*, size_t);
	void	(*for_each)(t_vector*, void*(*)(void*, size_t));
	void	(*remove_at)(t_vector*, size_t);
	void	(*remove_element)(t_vector*, void*);
	void	*(*find)(const t_vector *const, const void *const);
	int		(*get_index)(const t_vector *const, const void *const);
	void	*(*find_with)(const t_vector *const this, const void *const find, int (*comparator)(const void *, const void *));
	void	*(*at)(const t_vector *const, const size_t);
	void	*(*end)(const t_vector *const);
	void	(*destroy)(t_vector*);
} ;

//vector_setup.c
t_vector	init_vector(size_t size);
t_vector	*new_vector(size_t size);
t_uvector	*new_uvector(size_t size);
void		vector_expand(t_vector *this);
void		vector_destroy(t_vector *this);
void		delete_vector(t_vector *this);

//vector_get.c
void		*vector_end(const t_vector *const this);
void		*vector_at(const t_vector *const this, const size_t find);
void		*vector_search(const t_vector *const this, const void *const find);
void		*vector_search_with(const t_vector *const this,
								const void *const find,
								int (*comparator)(const void*, const void*));
int			vector_get_index(const t_vector *const this, const void *const find);

//vector_mod.c
void		*vector_push_back(t_vector *this, void *element);
void		vector_insert(t_vector *this, void *element, size_t position);
void		vector_remove_index(t_vector *this, size_t find);
void		vector_mix_remove_index(t_vector *this, size_t index);
void		vector_remove_element(t_vector *this, void *find);
void		vector_mix_remove_element(t_vector *this, void *element);
void		vector_for_each(t_vector *this, void *(*function)(void*, size_t));
void		*vector_emplace(t_vector *this);
void		vector_push_batch(t_vector *this, void *batch, size_t count);

void		vector_strigfy(t_vector *this);


#endif
