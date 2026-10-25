/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rteles-f <rteles-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/29 19:33:25 by rteles-f          #+#    #+#             */
/*   Updated: 2023/05/29 12:27:25 by rteles-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "terminal_internal.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int	getwords(char const *s);
static int	fill_list(char const *s, char **list);
static char	*checkmalloc(int size, int members, char **list);

char	**ft_split_space(const char *string)
{
	int		foundwords;
	int		check;
	char	**list;

	if (!string)
		return (NULL);
	foundwords = getwords(string);
	list = malloc(sizeof(char *) * (foundwords + 1));
	if (!list)
		return (NULL);
	list[foundwords] = NULL;
	check = fill_list(string, list);
	if (!check)
		return (NULL);
	return (list);
}

static int	getwords(char const *s)
{
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (s[i] != '\0')
	{
		while (s[i] && isspace(s[i]))
			i++;
		if (s[i] && !isspace(s[i]))
			count++;
		while (s[i] && !isspace(s[i]))
			i++;
	}
	return (count);
}

static int	fill_list(char const *s, char **list)
{
	int		i;
	int		j;
	int		word;

	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		while (s[i] && isspace(s[i]))
			i++;
		word = 0;
		while (s[i + word] && !isspace(s[i + word]))
			word++;
		if (word > 0)
		{
			list[j] = checkmalloc((word + 1), j, list);
			if (!list[j])
				return (0);
			memcpy(list[j], &(s)[i], word);
			j++;
			i = i + word;
		}
	}
	return (1);
}

static char	*checkmalloc(int size, int members, char **list)
{
	char	*string;

	string = malloc(sizeof(char) * size);
	if (!string)
	{
		while (members > 0)
			free(list[--members]);
		free(list);
		return (NULL);
	}
	string[size - 1] = 0;
	return (string);
}
